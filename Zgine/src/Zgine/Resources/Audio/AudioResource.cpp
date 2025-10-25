#include "zgpch.h"
#include "IAudioBackend.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>

namespace Zgine {
namespace Resources {
namespace Audio {

    // ==================== AudioResource Implementation ====================

    AudioResource::AudioResource(const std::string& path, uint32_t id)
        : m_ID(id)
        , m_Path(path)
        , m_Name(std::filesystem::path(path).stem().string())
        , m_State(ResourceState::Unloaded)
        , m_Format(ma_format_unknown)
        , m_Channels(0)
        , m_SampleRate(0)
        , m_Duration(0.0f)
        , m_DecoderInitialized(false) {
        
        // 零初始化ma_decoder结构体
        memset(&m_Decoder, 0, sizeof(m_Decoder));
        
        ZG_CORE_TRACE("Created AudioResource: {} (ID: {})", m_Name, m_ID);
    }

    AudioResource::~AudioResource() {
        Unload();
    }

    size_t AudioResource::GetSize() const {
        return m_AudioData.size();
    }

    bool AudioResource::IsValid() const {
        return m_DecoderInitialized && m_State != ResourceState::Error;
    }

    void AudioResource::LoadAsync(std::function<void(bool)> callback) {
        // 简化实现：直接调用LoadSync
        bool success = LoadSync();
        if (callback) {
            callback(success);
        }
    }

    bool AudioResource::LoadSync() {
        if (m_State == ResourceState::Loaded) {
            return true;
        }

        ZG_CORE_INFO("Loading audio resource: {}", m_Path);
        
        // 检查文件是否存在
        if (!std::filesystem::exists(m_Path)) {
            ZG_CORE_ERROR("Audio file does not exist: {}", m_Path);
            m_State = ResourceState::Error;
            return false;
        }
        
        auto fileSize = std::filesystem::file_size(m_Path);
        ZG_CORE_INFO("Audio file exists, size: {} bytes", fileSize);
        
        if (fileSize == 0) {
            ZG_CORE_ERROR("Audio file is empty: {}", m_Path);
            m_State = ResourceState::Error;
            return false;
        }
        
        m_State = ResourceState::Loading;

        try {
            // 使用ma_decoder获取音频信息，让miniaudio自动检测格式
            ma_result result;
            ma_decoder_config decoderConfig = ma_decoder_config_init_default();
            
            // 尝试初始化解码器
            result = ma_decoder_init_file(m_Path.c_str(), &decoderConfig, &m_Decoder);

            if (result != MA_SUCCESS) {
                ZG_CORE_ERROR("Failed to initialize audio decoder for {}: {} ({})", 
                             m_Path, static_cast<int>(result), ma_result_description(result));
                m_State = ResourceState::Error;
                return false;
            }
            m_DecoderInitialized = true;

            // 获取音频信息
            m_SampleRate = m_Decoder.outputSampleRate;
            m_Channels = m_Decoder.outputChannels;
            m_Format = m_Decoder.outputFormat;
            
            // 获取音频长度
            ma_uint64 totalPcmFrames;
            result = ma_decoder_get_length_in_pcm_frames(&m_Decoder, &totalPcmFrames);
            if (result == MA_SUCCESS && totalPcmFrames > 0) {
                m_Duration = static_cast<float>(totalPcmFrames) / m_SampleRate;
            } else {
                // 如果无法获取长度，设置为未知
                m_Duration = -1.0f;
                ZG_CORE_WARN("Could not determine audio duration for {}", m_Path);
            }

            ZG_CORE_INFO("Loaded audio: {} ({} channels, {}Hz, {} format, {:.2f}s)", 
                        m_Path, m_Channels, m_SampleRate, 
                        ma_get_format_name(m_Format), m_Duration);

            m_State = ResourceState::Loaded;
            return true;

        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Exception loading audio {}: {}", m_Path, e.what());
            m_State = ResourceState::Error;
            return false;
        }
    }

    void AudioResource::Unload() {
        if (m_DecoderInitialized) {
            ma_decoder_uninit(&m_Decoder);
            m_DecoderInitialized = false;
        }
        
        m_AudioData.clear();
        m_State = ResourceState::Unloaded;
        ZG_CORE_INFO("Unloaded audio resource: {}", m_Name);
    }

    bool AudioResource::Reload() {
        Unload();
        return LoadSync();
    }

    std::string AudioResource::GetMetadata(const std::string& key) const {
        auto it = m_Metadata.find(key);
        return it != m_Metadata.end() ? it->second : "";
    }

    void AudioResource::SetMetadata(const std::string& key, const std::string& value) {
        m_Metadata[key] = value;
    }

} // namespace Audio
} // namespace Resources
} // namespace Zgine
