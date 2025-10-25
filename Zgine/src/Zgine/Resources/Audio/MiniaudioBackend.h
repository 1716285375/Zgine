#pragma once

#include "zgpch.h"
#include "IAudioBackend.h"
#include "../Core/IResourceBackend.h"
#include "Zgine/Logging/Log.h"
#include <miniaudio.h>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace Zgine {
namespace Resources {
namespace Audio {

    /**
     * @brief 音频播放实例
     */
    struct AudioPlayback {
        uint32_t id;
        ResourceRef audioResource;
        ma_sound sound;  // miniaudio声音对象
        bool isPlaying = false;
        bool isLooping = false;
        float volume = 1.0f;
        std::atomic<bool> initialized{false};
        
        AudioPlayback() {
            // 不在这里初始化ma_sound，在PlayAudio中初始化
        }
        
        ~AudioPlayback() {
            // 清理ma_sound
            if (initialized.load()) {
                ma_sound_uninit(&sound);
            }
        }
    };

    /**
     * @brief Miniaudio音频后端实现
     */
    class MiniaudioBackend : public IAudioBackend {
    public:
        MiniaudioBackend();
        virtual ~MiniaudioBackend();

        // IResourceBackend接口实现
        std::string GetBackendName() const override;
        std::string GetBackendVersion() const override;
        bool SupportsResourceType(ResourceType type) const override;
        bool SupportsFileFormat(const std::string& extension) const override;
        bool Initialize() override;
        void Shutdown() override;
        ResourceRef LoadSync(const std::string& path, ResourceType type, 
                           const ResourceLoadConfig& config = {}) override;
        ResourceRef LoadAsync(const std::string& path, ResourceType type,
                            const ResourceLoadConfig& config,
                            ResourceLoadCallback callback) override;
        bool ValidateResourceFile(const std::string& path, ResourceType type) const override;
        std::vector<ResourceType> GetSupportedResourceTypes() const override;
        std::vector<std::string> GetSupportedFileFormats() const override;
        ResourceStatistics GetStatistics() const override;

        // IAudioBackend接口实现
        uint32_t PlayAudio(ResourceRef resource, bool loop = false, float volume = 1.0f) override;
        void StopAudio(uint32_t playID) override;
        void PauseAudio(uint32_t playID) override;
        void ResumeAudio(uint32_t playID) override;
        void SetAudioVolume(uint32_t playID, float volume) override;
        void StopAllAudio() override;
        std::vector<uint32_t> GetActivePlaybacks() const override;
        ma_engine* GetEngine() override { return const_cast<ma_engine*>(&m_Engine); }

        // 更新音频系统
        void Update(float deltaTime);

    private:
        ma_engine m_Engine;
        bool m_EngineInitialized = false;

        std::unordered_map<uint32_t, Scope<AudioPlayback>> m_ActivePlaybacks;
        std::atomic<uint32_t> m_NextPlayID{1};
        std::atomic<uint32_t> m_NextResourceID{1};
        mutable std::mutex m_PlaybackMutex;

        // 统计信息
        std::atomic<uint32_t> m_TotalLoads{0};
        std::atomic<uint32_t> m_FailedLoads{0};
        std::atomic<uint32_t> m_TotalPlaybacks{0};

        // 支持的文件格式
        std::vector<std::string> m_SupportedFormats;

        // 内部方法
        ResourceRef CreateAudioResource(const std::string& path);
        void CleanupFinishedPlaybacks();
    };

} // namespace Audio
} // namespace Resources
} // namespace Zgine