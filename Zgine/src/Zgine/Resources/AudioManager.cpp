#include "zgpch.h"
#include "AudioManager.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <fstream>

namespace Zgine {
namespace Resources {

    // ============================================================================
    // AudioResource Implementation
    // ============================================================================

    AudioResource::AudioResource(const std::string& path, uint32_t id)
        : m_ID(id), m_Path(path), m_State(ResourceState::Unloaded),
          m_Format(ma_format_unknown), m_Channels(0), m_SampleRate(0), m_Duration(0.0f) {
        m_Name = std::filesystem::path(path).stem().string();
        ZG_CORE_TRACE("AudioResource created: {} (ID: {})", m_Name, m_ID);
    }

    AudioResource::~AudioResource() {
        Unload();
        ZG_CORE_TRACE("AudioResource destroyed: {}", m_Name);
    }

    size_t AudioResource::GetSize() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_AudioData.size();
    }

    void AudioResource::AddRef() {
        m_RefCount++;
        ZG_CORE_TRACE("AudioResource {} ref count: {}", m_Name, m_RefCount.load());
    }

    void AudioResource::RemoveRef() {
        uint32_t count = m_RefCount--;
        ZG_CORE_TRACE("AudioResource {} ref count: {}", m_Name, count - 1);
        
        if (count == 1) {
            ZG_CORE_INFO("AudioResource {} has no more references, marking for cleanup", m_Name);
        }
    }

    bool AudioResource::IsValid() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State != ResourceState::Error && !m_AudioData.empty();
    }

    bool AudioResource::IsLoaded() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State == ResourceState::Loaded;
    }

    void AudioResource::LoadAsync(std::function<void(bool)> callback) {
        std::thread([this, callback]() {
            bool success = LoadSync();
            if (callback) {
                callback(success);
            }
        }).detach();
    }

    bool AudioResource::LoadSync() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Loaded) {
            return true;
        }
        
        SetState(ResourceState::Loading);
        
        try {
            // 初始化解码器
            ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 0, 0);
            
            ma_result result = ma_decoder_init_file(m_Path.c_str(), &decoderConfig, &m_Decoder);
            if (result != MA_SUCCESS) {
                ZG_CORE_ERROR("Failed to initialize decoder for {}: {}", m_Path, static_cast<int>(result));
                SetState(ResourceState::Error);
                return false;
            }
            
            // 获取音频信息
            m_Format = m_Decoder.outputFormat;
            m_Channels = m_Decoder.outputChannels;
            m_SampleRate = m_Decoder.outputSampleRate;
            
            // 计算时长
            ma_uint64 frameCount;
            result = ma_decoder_get_length_in_pcm_frames(&m_Decoder, &frameCount);
            if (result == MA_SUCCESS) {
                m_Duration = (float)frameCount / m_SampleRate;
            } else {
                m_Duration = 0.0f;
            }
            
            // 读取音频数据
            const size_t bufferSize = frameCount * m_Channels * ma_get_bytes_per_sample(m_Format);
            m_AudioData.resize(bufferSize);
            
            ma_uint64 framesRead;
            result = ma_decoder_read_pcm_frames(&m_Decoder, m_AudioData.data(), frameCount, &framesRead);
            
            if (result != MA_SUCCESS) {
                ZG_CORE_ERROR("Failed to read audio data for {}: {}", m_Path, static_cast<int>(result));
                ma_decoder_uninit(&m_Decoder);
                SetState(ResourceState::Error);
                return false;
            }
            
            // 调整数据大小到实际读取的大小
            m_AudioData.resize(framesRead * m_Channels * ma_get_bytes_per_sample(m_Format));
            
            SetState(ResourceState::Loaded);
            ZG_CORE_INFO("Audio loaded successfully: {} (Duration: {:.2f}s, Channels: {}, SampleRate: {})", 
                         m_Name, m_Duration, m_Channels, m_SampleRate);
            return true;
            
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Failed to load audio {}: {}", m_Name, e.what());
            SetState(ResourceState::Error);
            return false;
        }
    }

    void AudioResource::Unload() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Unloaded) {
            return;
        }
        
        if (m_State == ResourceState::Loaded) {
            ma_decoder_uninit(&m_Decoder);
        }
        
        m_AudioData.clear();
        m_Format = ma_format_unknown;
        m_Channels = 0;
        m_SampleRate = 0;
        m_Duration = 0.0f;
        
        SetState(ResourceState::Unloaded);
        ZG_CORE_INFO("Audio unloaded: {}", m_Name);
    }

    bool AudioResource::Reload() {
        Unload();
        return LoadSync();
    }

    std::string AudioResource::GetMetadata(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Metadata.find(key);
        return it != m_Metadata.end() ? it->second : "";
    }

    void AudioResource::SetMetadata(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Metadata[key] = value;
    }

    float AudioResource::GetDuration() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Duration;
    }

    uint32_t AudioResource::GetSampleRate() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_SampleRate;
    }

    uint32_t AudioResource::GetChannels() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Channels;
    }

    const void* AudioResource::GetAudioData() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_AudioData.empty() ? nullptr : m_AudioData.data();
    }

    size_t AudioResource::GetAudioDataSize() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_AudioData.size();
    }

    void AudioResource::SetState(ResourceState newState) {
        ResourceState oldState = m_State;
        m_State = newState;
        ZG_CORE_TRACE("AudioResource {} state changed: {} -> {}", 
                     m_Name, static_cast<int>(oldState), static_cast<int>(newState));
    }

    // ============================================================================
    // AudioManager Implementation
    // ============================================================================

    AudioManager::AudioManager() 
        : m_Initialized(false), m_NextPlayID(1) {
        ZG_CORE_INFO("AudioManager created");
    }

    AudioManager::~AudioManager() {
        Shutdown();
        ZG_CORE_INFO("AudioManager destroyed");
    }

    bool AudioManager::Initialize() {
        if (m_Initialized) {
            return true;
        }
        
        // 初始化miniaudio上下文
        ma_result result = ma_context_init(nullptr, 0, nullptr, &m_Context);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to initialize audio context: {}", static_cast<int>(result));
            return false;
        }
        
        // 初始化音频设备
        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = ma_format_f32;
        deviceConfig.playback.channels = 2;
        deviceConfig.sampleRate = 44100;
        deviceConfig.dataCallback = AudioCallback;
        deviceConfig.pUserData = this;
        
        result = ma_device_init(&m_Context, &deviceConfig, &m_Device);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to initialize audio device: {}", static_cast<int>(result));
            ma_context_uninit(&m_Context);
            return false;
        }
        
        // 启动音频设备
        result = ma_device_start(&m_Device);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to start audio device: {}", static_cast<int>(result));
            ma_device_uninit(&m_Device);
            ma_context_uninit(&m_Context);
            return false;
        }
        
        m_Initialized = true;
        ZG_CORE_INFO("AudioManager initialized successfully");
        return true;
    }

    void AudioManager::Shutdown() {
        if (!m_Initialized) {
            return;
        }
        
        // 停止所有播放
        StopAllAudio();
        
        // 关闭音频设备
        ma_device_uninit(&m_Device);
        ma_context_uninit(&m_Context);
        
        m_Initialized = false;
        ZG_CORE_INFO("AudioManager shutdown");
    }

    ResourceRef AudioManager::LoadAudio(const std::string& path, bool async) {
        if (path.empty()) {
            ZG_CORE_WARN("Attempted to load audio with empty path");
            return nullptr;
        }
        
        // 检查是否已加载
        ResourceRef existing = GetResource(path);
        if (existing) {
            ZG_CORE_TRACE("Audio already loaded: {}", path);
            return existing;
        }
        
        // 验证文件
        if (!ValidateResourceFile(path, ResourceType::Audio)) {
            ZG_CORE_ERROR("Invalid audio file: {}", path);
            return nullptr;
        }
        
        if (async) {
            return LoadAsync(path, ResourceType::Audio);
        } else {
            return LoadSync(path, ResourceType::Audio);
        }
    }

    ResourceRef AudioManager::CreateAudio(const void* data, size_t size, 
                                         ma_format format, uint32_t channels, 
                                         uint32_t sampleRate, const std::string& name) {
        std::string path = "generated://" + name;
        
        // 检查是否已存在
        ResourceRef existing = GetResource(path);
        if (existing) {
            return existing;
        }
        
        ResourceRef resource = CreateResource(path, ResourceType::Audio);
        if (!resource) {
            return nullptr;
        }
        
        auto audioResource = std::static_pointer_cast<AudioResource>(resource);
        
        // 设置音频数据
        audioResource->m_AudioData.resize(size);
        std::memcpy(audioResource->m_AudioData.data(), data, size);
        
        audioResource->m_Format = format;
        audioResource->m_Channels = channels;
        audioResource->m_SampleRate = sampleRate;
        audioResource->m_Duration = (float)size / (channels * ma_get_bytes_per_sample(format) * sampleRate);
        
        audioResource->SetState(ResourceState::Loaded);
        
        // 添加到缓存
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        m_ResourceCache[path] = resource;
        
        ZG_CORE_INFO("Created audio resource: {}", name);
        return resource;
    }

    ResourceRef AudioManager::GetAudio(const std::string& path) {
        return GetResource(path);
    }

    bool AudioManager::UnloadAudio(const std::string& path) {
        return UnloadResource(path);
    }

    std::vector<ResourceRef> AudioManager::GetAllAudio() const {
        return GetAllResources();
    }

    std::string AudioManager::GetStatistics() const {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        size_t totalSize = 0;
        uint32_t loadedCount = 0;
        uint32_t errorCount = 0;
        
        for (const auto& [path, resource] : m_ResourceCache) {
            totalSize += resource->GetSize();
            if (resource->IsLoaded()) {
                loadedCount++;
            } else if (resource->GetState() == ResourceState::Error) {
                errorCount++;
            }
        }
        
        std::stringstream ss;
        ss << "AudioManager Statistics:\n";
        ss << "  Total Audio: " << m_ResourceCache.size() << "\n";
        ss << "  Loaded: " << loadedCount << "\n";
        ss << "  Errors: " << errorCount << "\n";
        ss << "  Total Size: " << (totalSize / 1024) << " KB\n";
        ss << "  Active Playbacks: " << m_ActivePlaybacks.size();
        
        return ss.str();
    }

    uint32_t AudioManager::CleanupUnusedAudio() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t cleanedCount = 0;
        auto it = m_ResourceCache.begin();
        
        while (it != m_ResourceCache.end()) {
            if (it->second->GetRefCount() == 0) {
                it->second->Unload();
                it = m_ResourceCache.erase(it);
                cleanedCount++;
            } else {
                ++it;
            }
        }
        
        if (cleanedCount > 0) {
            ZG_CORE_INFO("Cleaned up {} unused audio resources", cleanedCount);
        }
        
        return cleanedCount;
    }

    uint32_t AudioManager::PlayAudio(const std::string& path, float volume, bool loop) {
        if (!m_Initialized) {
            ZG_CORE_WARN("AudioManager not initialized");
            return 0;
        }
        
        ResourceRef audioResource = GetAudio(path);
        if (!audioResource) {
            ZG_CORE_WARN("Audio not found: {}", path);
            return 0;
        }
        
        auto audio = std::static_pointer_cast<AudioResource>(audioResource);
        if (!audio->IsLoaded()) {
            ZG_CORE_WARN("Audio not loaded: {}", path);
            return 0;
        }
        
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        uint32_t playID = m_NextPlayID++;
        
        AudioPlayback playback;
        playback.id = playID;
        playback.audioResource = audioResource;
        playback.volume = volume;
        playback.loop = loop;
        playback.playing = true;
        
        // 初始化ma_sound（简化版本，实际需要更复杂的实现）
        // 这里只是示例，实际实现需要根据miniaudio的API
        
        m_ActivePlaybacks[playID] = playback;
        
        ZG_CORE_INFO("Started playing audio: {} (ID: {})", path, playID);
        return playID;
    }

    void AudioManager::StopAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            it->second.playing = false;
            m_ActivePlaybacks.erase(it);
            ZG_CORE_INFO("Stopped audio playback: {}", playID);
        }
    }

    void AudioManager::PauseAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            it->second.playing = false;
            ZG_CORE_INFO("Paused audio playback: {}", playID);
        }
    }

    void AudioManager::ResumeAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            it->second.playing = true;
            ZG_CORE_INFO("Resumed audio playback: {}", playID);
        }
    }

    void AudioManager::SetAudioVolume(uint32_t playID, float volume) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            it->second.volume = glm::clamp(volume, 0.0f, 1.0f);
        }
    }

    void AudioManager::StopAllAudio() {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        uint32_t count = static_cast<uint32_t>(m_ActivePlaybacks.size());
        m_ActivePlaybacks.clear();
        
        if (count > 0) {
            ZG_CORE_INFO("Stopped all audio playbacks: {}", count);
        }
    }

    void AudioManager::Clear() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t count = static_cast<uint32_t>(m_ResourceCache.size());
        m_ResourceCache.clear();
        
        ZG_CORE_INFO("Cleared all audio resources: {}", count);
    }

    ResourceRef AudioManager::CreateResource(const std::string& path, ResourceType type) {
        uint32_t id = GenerateResourceID(path);
        return CreateScope<AudioResource>(path, id);
    }

    bool AudioManager::ValidateResourceFile(const std::string& path, ResourceType type) {
        // 对于生成的音频，总是有效的
        if (path.find("generated://") == 0) {
            return true;
        }
        
        // 对于文件路径，检查文件是否存在
        if (std::filesystem::exists(path)) {
            std::string extension = std::filesystem::path(path).extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            return extension == ".wav" || extension == ".mp3" || extension == ".ogg" || 
                   extension == ".flac" || extension == ".m4a";
        }
        
        return false;
    }

    std::string AudioManager::ExtractNameFromPath(const std::string& path) const {
        std::filesystem::path filePath(path);
        return filePath.stem().string();
    }

    void AudioManager::AudioCallback(ma_device* pDevice, void* pOutput, 
                                    const void* pInput, ma_uint32 frameCount) {
        AudioManager* manager = static_cast<AudioManager*>(pDevice->pUserData);
        
        // 简化的音频回调实现
        // 实际实现需要根据miniaudio的API进行混音和播放
        std::memset(pOutput, 0, frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format));
    }

} // namespace Resources
} // namespace Zgine
