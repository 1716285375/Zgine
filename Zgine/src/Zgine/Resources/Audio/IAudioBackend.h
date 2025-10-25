#pragma once

#include "zgpch.h"
#include "../Core/IResourceBackend.h"
#include "../IResource.h"
#include "Zgine/Core/SmartPointers.h"
#include <miniaudio.h>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace Zgine {
namespace Resources {
namespace Audio {

    /**
     * @brief 音频资源类
     */
    class AudioResource : public IResource {
    public:
        AudioResource(const std::string& path, uint32_t id);
        virtual ~AudioResource();

        // IResource接口实现
        uint32_t GetID() const override { return m_ID; }
        const std::string& GetName() const override { return m_Name; }
        const std::string& GetPath() const override { return m_Path; }
        ResourceType GetType() const override { return ResourceType::Audio; }
        ResourceState GetState() const override { return m_State; }
        size_t GetSize() const override;
        uint32_t GetRefCount() const override { return m_RefCount.load(); }
        void AddRef() override { m_RefCount++; }
        void RemoveRef() override { m_RefCount--; }
        bool IsValid() const override;
        bool IsLoaded() const override { return m_State == ResourceState::Loaded; }
        void LoadAsync(std::function<void(bool)> callback = nullptr) override;
        bool LoadSync() override;
        void Unload() override;
        bool Reload() override;
        std::string GetMetadata(const std::string& key) const override;
        void SetMetadata(const std::string& key, const std::string& value) override;

        // 音频特定接口
        ma_format GetFormat() const { return m_Format; }
        uint32_t GetChannels() const { return m_Channels; }
        uint32_t GetSampleRate() const { return m_SampleRate; }
        float GetDuration() const { return m_Duration; }
        const std::vector<uint8_t>& GetAudioData() const { return m_AudioData; }

        // 设置资源状态（供后端使用）
        void SetState(ResourceState state) { m_State = state; }

    private:
        // 友元类声明
        friend class MiniaudioBackend;
        
        uint32_t m_ID;
        std::string m_Path;
        std::string m_Name;
        ResourceState m_State;
        std::atomic<uint32_t> m_RefCount{0};
        std::unordered_map<std::string, std::string> m_Metadata;
        
        // 音频数据
        ma_decoder m_Decoder;
        ma_format m_Format;
        uint32_t m_Channels;
        uint32_t m_SampleRate;
        float m_Duration;
        std::vector<uint8_t> m_AudioData;
        bool m_DecoderInitialized;
    };

    /**
     * @brief 音频后端抽象接口
     */
    class IAudioBackend : public Core::IResourceBackend {
    public:
        virtual ~IAudioBackend() = default;

        /**
         * @brief 播放音频
         * @param resource 音频资源
         * @param loop 是否循环播放
         * @param volume 音量 (0.0 - 1.0)
         * @return 播放ID，如果播放失败返回0
         */
        virtual uint32_t PlayAudio(ResourceRef resource, bool loop = false, float volume = 1.0f) = 0;

        /**
         * @brief 停止音频播放
         * @param playID 播放ID
         */
        virtual void StopAudio(uint32_t playID) = 0;

        /**
         * @brief 暂停音频播放
         * @param playID 播放ID
         */
        virtual void PauseAudio(uint32_t playID) = 0;

        /**
         * @brief 恢复音频播放
         * @param playID 播放ID
         */
        virtual void ResumeAudio(uint32_t playID) = 0;

        /**
         * @brief 设置音频音量
         * @param playID 播放ID
         * @param volume 音量 (0.0 - 1.0)
         */
        virtual void SetAudioVolume(uint32_t playID, float volume) = 0;

        /**
         * @brief 停止所有音频播放
         */
        virtual void StopAllAudio() = 0;

        /**
         * @brief 获取当前播放的音频实例列表
         * @return 播放ID列表
         */
        virtual std::vector<uint32_t> GetActivePlaybacks() const = 0;

        /**
         * @brief 获取音频引擎
         * @return miniaudio引擎指针
         */
        virtual ma_engine* GetEngine() = 0;
    };

    /**
     * @brief 音频后端智能指针类型
     */
    using AudioBackendRef = Ref<IAudioBackend>;

} // namespace Audio
} // namespace Resources
} // namespace Zgine