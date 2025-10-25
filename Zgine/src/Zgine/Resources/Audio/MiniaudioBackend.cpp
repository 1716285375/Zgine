#include "zgpch.h"
#include "MiniaudioBackend.h"
#include "../Core/IResourceBackend.h"
#include <glm/glm.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

namespace Zgine {
namespace Resources {
namespace Audio {

    // ==================== MiniaudioBackend Implementation ====================

    MiniaudioBackend::MiniaudioBackend() {
        ZG_CORE_INFO("MiniaudioBackend created");
    }

    MiniaudioBackend::~MiniaudioBackend() {
        // 在析构函数中避免使用日志系统，因为此时日志系统可能已经被销毁
        // ZG_CORE_INFO("MiniaudioBackend destroyed");
        
        // 确保在析构时正确关闭
        if (m_EngineInitialized) {
            // 直接调用清理逻辑，避免日志调用
            StopAllAudio();
            ma_engine_uninit(&m_Engine);
            m_EngineInitialized = false;
        }
    }

    bool MiniaudioBackend::Initialize() {
        ZG_CORE_INFO("Initializing MiniaudioBackend...");
        
        // 使用最保守的配置，避免重采样器问题
        ma_engine_config engineConfig = ma_engine_config_init();
        
        // 设置明确的采样率和通道数，避免重采样器状态不一致
        engineConfig.sampleRate = 44100;  // 使用标准CD质量
        engineConfig.channels = 2;        // 立体声
        
        ma_result result = ma_engine_init(&engineConfig, &m_Engine);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to initialize miniaudio engine: {} ({})", 
                         static_cast<int>(result), ma_result_description(result));
            return false;
        }
        m_EngineInitialized = true;
        
        ZG_CORE_INFO("MiniaudioBackend initialized successfully (44100Hz, 2 channels)");
        return true;
    }

    void MiniaudioBackend::Shutdown() {
        ZG_CORE_INFO("Shutting down MiniaudioBackend...");
        
        // 停止所有音频播放
        StopAllAudio();
        
        // 关闭miniaudio引擎
        if (m_EngineInitialized) {
            ma_engine_uninit(&m_Engine);
            m_EngineInitialized = false;
        }
        
        ZG_CORE_INFO("MiniaudioBackend shutdown complete");
    }

    ResourceRef MiniaudioBackend::LoadSync(const std::string& path, ResourceType type, const ResourceLoadConfig& config) {
        if (type != ResourceType::Audio) {
            ZG_CORE_ERROR("MiniaudioBackend can only load audio resources");
            return nullptr;
        }

        ZG_CORE_INFO("Loading audio resource: {}", path);
        
        // 创建AudioResource实例
        uint32_t id = m_NextResourceID++;
        Ref<AudioResource> audioResource = CreateRef<AudioResource>(path, id);
        
        if (!audioResource->LoadSync()) {
            ZG_CORE_ERROR("Failed to load audio resource: {}", path);
            return nullptr;
        }

        return audioResource;
    }

    ResourceRef MiniaudioBackend::LoadAsync(const std::string& path, ResourceType type, const ResourceLoadConfig& config, ResourceLoadCallback callback) {
        if (type != ResourceType::Audio) {
            ZG_CORE_ERROR("MiniaudioBackend can only load audio resources asynchronously");
            if (callback) callback(nullptr, false);
            return nullptr;
        }

        ZG_CORE_INFO("Loading audio resource asynchronously: {}", path);
        
        // 创建AudioResource实例
        uint32_t id = m_NextResourceID++;
        Ref<AudioResource> audioResource = CreateRef<AudioResource>(path, id);
        
        // 异步加载
        audioResource->LoadAsync([audioResource, callback](bool success) {
            if (callback) {
                callback(audioResource, success);
            }
        });

        return audioResource;
    }

    bool MiniaudioBackend::ValidateResourceFile(const std::string& path, ResourceType type) const {
        if (type != ResourceType::Audio) {
            return false;
        }
        
        // 检查文件是否存在
        std::ifstream file(path);
        return file.good();
    }

    bool MiniaudioBackend::SupportsResourceType(ResourceType type) const {
        return type == ResourceType::Audio;
    }

    bool MiniaudioBackend::SupportsFileFormat(const std::string& extension) const {
        return extension == ".wav" || extension == ".mp3" || extension == ".ogg" ||
               extension == ".flac" || extension == ".aiff" || extension == ".opus";
    }

    uint32_t MiniaudioBackend::PlayAudio(ResourceRef resource, bool loop, float volume) {
        if (!resource || resource->GetType() != ResourceType::Audio) {
            ZG_CORE_ERROR("Invalid audio resource for playback");
            return 0;
        }

        Ref<AudioResource> audioResource = std::static_pointer_cast<AudioResource>(resource);
        if (!audioResource->IsLoaded()) {
            ZG_CORE_ERROR("Audio resource is not loaded");
            return 0;
        }

        // 创建播放实例
        uint32_t playID = m_NextPlayID++;
        Scope<AudioPlayback> playback = CreateScope<AudioPlayback>();
        playback->id = playID;
        playback->audioResource = audioResource;
        playback->isLooping = loop;
        playback->volume = glm::clamp(volume, 0.0f, 1.0f);

        // 使用ma_sound_init_from_file创建声音对象
        ma_result result = ma_sound_init_from_file(&m_Engine, audioResource->GetPath().c_str(), 
                                                  MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, 
                                                  nullptr, nullptr, &playback->sound);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to initialize sound for {}: {}", 
                         audioResource->GetPath(), static_cast<int>(result));
            return 0;
        }

        // 设置循环和音量
        ma_sound_set_looping(&playback->sound, loop);
        ma_sound_set_volume(&playback->sound, playback->volume);

        // 开始播放
        result = ma_sound_start(&playback->sound);
        if (result != MA_SUCCESS) {
            ZG_CORE_ERROR("Failed to start sound for {}: {}", 
                         audioResource->GetPath(), static_cast<int>(result));
            ma_sound_uninit(&playback->sound);
            return 0;
        }
        
        // 标记为已初始化和播放
        playback->initialized = true;
        playback->isPlaying = true;

        // 存储播放实例
        {
            std::lock_guard<std::mutex> lock(m_PlaybackMutex);
            m_ActivePlaybacks[playID] = std::move(playback);
        }
        
        ZG_CORE_INFO("Started playing audio: {} (ID: {}, Loop: {}, Volume: {})", 
                    audioResource->GetPath(), playID, loop, volume);
        return playID;
    }

    void MiniaudioBackend::StopAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            // 停止声音播放
            ma_sound_stop(&it->second->sound);
            it->second->isPlaying = false;
            it->second->initialized = false;
            m_ActivePlaybacks.erase(it);
            ZG_CORE_INFO("Stopped audio playback: {}", playID);
        } else {
            ZG_CORE_WARN("Audio playback not found: {}", playID);
        }
    }

    void MiniaudioBackend::PauseAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            // 暂停声音播放
            ma_sound_stop(&it->second->sound);
            it->second->isPlaying = false;
            ZG_CORE_INFO("Paused audio playback: {}", playID);
        } else {
            ZG_CORE_WARN("Audio playback not found: {}", playID);
        }
    }

    void MiniaudioBackend::ResumeAudio(uint32_t playID) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            // 恢复声音播放
            ma_result result = ma_sound_start(&it->second->sound);
            if (result == MA_SUCCESS) {
                it->second->isPlaying = true;
                ZG_CORE_INFO("Resumed audio playback: {}", playID);
            } else {
                ZG_CORE_ERROR("Failed to resume audio playback: {} - {}", playID, static_cast<int>(result));
            }
        } else {
            ZG_CORE_WARN("Audio playback not found: {}", playID);
        }
    }

    void MiniaudioBackend::SetAudioVolume(uint32_t playID, float volume) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        auto it = m_ActivePlaybacks.find(playID);
        if (it != m_ActivePlaybacks.end()) {
            // 设置声音音量
            float clampedVolume = glm::clamp(volume, 0.0f, 1.0f);
            ma_sound_set_volume(&it->second->sound, clampedVolume);
            it->second->volume = clampedVolume;
            ZG_CORE_INFO("Set audio volume: {} to {}", playID, clampedVolume);
        } else {
            ZG_CORE_WARN("Audio playback not found: {}", playID);
        }
    }

    void MiniaudioBackend::StopAllAudio() {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        // 停止所有声音播放
        for (auto& [id, playback] : m_ActivePlaybacks) {
            if (playback->initialized.load()) {
                ma_sound_stop(&playback->sound);
            }
        }
        
        // 清空所有播放实例
        m_ActivePlaybacks.clear();
    }

    std::vector<uint32_t> MiniaudioBackend::GetActivePlaybacks() const {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        std::vector<uint32_t> activeIDs;
        activeIDs.reserve(m_ActivePlaybacks.size());
        
        for (const auto& pair : m_ActivePlaybacks) {
            if (pair.second->isPlaying) {
                activeIDs.push_back(pair.first);
            }
        }
        
        return activeIDs;
    }

    void MiniaudioBackend::Update(float deltaTime) {
        std::lock_guard<std::mutex> lock(m_PlaybackMutex);
        
        // 检查播放状态并清理已完成的播放
        auto it = m_ActivePlaybacks.begin();
        while (it != m_ActivePlaybacks.end()) {
            auto& playback = it->second;
            
            // 检查声音是否还在播放
            if (playback->initialized.load() && playback->isPlaying) {
                // 检查ma_sound的播放状态
                if (ma_sound_at_end(&playback->sound)) {
                    // 声音播放完毕
                    playback->isPlaying = false;
                    playback->initialized = false;
                    ZG_CORE_TRACE("Audio playback {} finished", playback->id);
                }
            }
            
            // 清理已完成的播放
            if (!playback->isPlaying) {
                it = m_ActivePlaybacks.erase(it);
            } else {
                ++it;
            }
        }
    }

    // ==================== IResourceBackend Implementation ====================

    std::string MiniaudioBackend::GetBackendName() const {
        return "Miniaudio";
    }

    std::string MiniaudioBackend::GetBackendVersion() const {
        return "1.0.0";
    }

    std::vector<ResourceType> MiniaudioBackend::GetSupportedResourceTypes() const {
        return { ResourceType::Audio };
    }

    std::vector<std::string> MiniaudioBackend::GetSupportedFileFormats() const {
        return { ".wav", ".mp3", ".flac", ".ogg", ".m4a", ".aac" };
    }

    ResourceStatistics MiniaudioBackend::GetStatistics() const {
        ResourceStatistics stats;
        stats.totalResources = m_TotalLoads.load();
        stats.failedResources = m_FailedLoads.load();
        stats.loadedResources = stats.totalResources - stats.failedResources;
        stats.cachedResources = static_cast<uint32_t>(m_ActivePlaybacks.size());
        return stats;
    }

} // namespace Audio
} // namespace Resources
} // namespace Zgine