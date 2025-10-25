#pragma once

#include "ResourceManager.h"
#include <miniaudio.h>
#include <unordered_map>
#include <vector>

namespace Zgine {
namespace Resources {

    /**
     * @brief 音频资源类
     * @details 管理音频文件资源，包括WAV、MP3、OGG等格式
     */
    class AudioResource : public IResource {
        friend class AudioManager;
    public:
        AudioResource(const std::string& path, uint32_t id);
        virtual ~AudioResource();

        // IResource interface
        uint32_t GetID() const override { return m_ID; }
        const std::string& GetName() const override { return m_Name; }
        const std::string& GetPath() const override { return m_Path; }
        ResourceType GetType() const override { return ResourceType::Audio; }
        ResourceState GetState() const override { return m_State; }
        size_t GetSize() const override;
        uint32_t GetRefCount() const override { return m_RefCount; }
        void AddRef() override;
        void RemoveRef() override;
        bool IsValid() const override;
        bool IsLoaded() const override;
        void LoadAsync(std::function<void(bool)> callback = nullptr) override;
        bool LoadSync() override;
        void Unload() override;
        bool Reload() override;
        std::string GetMetadata(const std::string& key) const override;
        void SetMetadata(const std::string& key, const std::string& value) override;

        /**
         * @brief 获取音频时长（秒）
         * @return 音频时长
         */
        float GetDuration() const;

        /**
         * @brief 获取采样率
         * @return 采样率
         */
        uint32_t GetSampleRate() const;

        /**
         * @brief 获取声道数
         * @return 声道数
         */
        uint32_t GetChannels() const;

        /**
         * @brief 获取音频数据
         * @return 音频数据指针
         */
        const void* GetAudioData() const;

        /**
         * @brief 获取音频数据大小
         * @return 数据大小（字节）
         */
        size_t GetAudioDataSize() const;

        /**
         * @brief 设置资源状态（供AudioManager使用）
         * @param newState 新状态
         */
        void SetState(ResourceState newState);

    private:
        uint32_t m_ID;
        std::string m_Name;
        std::string m_Path;
        ResourceState m_State;
        std::atomic<uint32_t> m_RefCount{0};
        
        // 音频数据
        std::vector<uint8_t> m_AudioData;
        ma_decoder m_Decoder;
        ma_format m_Format;
        uint32_t m_Channels;
        uint32_t m_SampleRate;
        float m_Duration;
        
        std::unordered_map<std::string, std::string> m_Metadata;
        
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief 音频管理器
     * @details 管理所有音频资源的加载、缓存和生命周期
     */
    class AudioManager : public ResourceManager {
    public:
        AudioManager();
        virtual ~AudioManager();

        /**
         * @brief 初始化音频系统
         * @return true如果初始化成功
         */
        bool Initialize();

        /**
         * @brief 关闭音频系统
         */
        void Shutdown();

        /**
         * @brief 加载音频文件
         * @param path 音频文件路径
         * @param async 是否异步加载
         * @return 音频资源引用
         */
        ResourceRef LoadAudio(const std::string& path, bool async = true);

        /**
         * @brief 创建音频资源
         * @param data 音频数据
         * @param size 数据大小
         * @param format 音频格式
         * @param channels 声道数
         * @param sampleRate 采样率
         * @param name 资源名称
         * @return 音频资源引用
         */
        ResourceRef CreateAudio(const void* data, size_t size, 
                               ma_format format, uint32_t channels, 
                               uint32_t sampleRate, const std::string& name);

        /**
         * @brief 获取音频资源
         * @param path 音频路径
         * @return 音频资源引用
         */
        ResourceRef GetAudio(const std::string& path);

        /**
         * @brief 卸载音频
         * @param path 音频路径
         * @return true如果卸载成功
         */
        bool UnloadAudio(const std::string& path);

        /**
         * @brief 获取所有已加载的音频
         * @return 音频资源列表
         */
        std::vector<ResourceRef> GetAllAudio() const;

        /**
         * @brief 获取音频统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 清理未使用的音频
         * @return 清理的音频数量
         */
        uint32_t CleanupUnusedAudio();

        /**
         * @brief 播放音频
         * @param path 音频路径
         * @param volume 音量 (0.0 - 1.0)
         * @param loop 是否循环播放
         * @return 播放ID
         */
        uint32_t PlayAudio(const std::string& path, float volume = 1.0f, bool loop = false);

        /**
         * @brief 停止音频播放
         * @param playID 播放ID
         */
        void StopAudio(uint32_t playID);

        /**
         * @brief 暂停音频播放
         * @param playID 播放ID
         */
        void PauseAudio(uint32_t playID);

        /**
         * @brief 恢复音频播放
         * @param playID 播放ID
         */
        void ResumeAudio(uint32_t playID);

        /**
         * @brief 设置音频音量
         * @param playID 播放ID
         * @param volume 音量 (0.0 - 1.0)
         */
        void SetAudioVolume(uint32_t playID, float volume);

        /**
         * @brief 停止所有音频播放
         */
        void StopAllAudio();

        /**
         * @brief 清空所有音频
         */
        void Clear();

    protected:
        /**
         * @brief 创建音频资源
         * @param path 音频路径
         * @param type 资源类型
         * @return 音频资源引用
         */
        virtual ResourceRef CreateResource(const std::string& path, ResourceType type) override;

        /**
         * @brief 验证音频文件
         * @param path 文件路径
         * @param type 资源类型
         * @return true如果文件有效
         */
        virtual bool ValidateResourceFile(const std::string& path, ResourceType type) override;

    private:
        /**
         * @brief 从路径提取音频名称
         * @param path 文件路径
         * @return 音频名称
         */
        std::string ExtractNameFromPath(const std::string& path) const;

        /**
         * @brief 音频播放回调
         * @param pDevice 音频设备
         * @param pOutput 输出缓冲区
         * @param frameCount 帧数
         * @param pUserData 用户数据
         */
        static void AudioCallback(ma_device* pDevice, void* pOutput, 
                                 const void* pInput, ma_uint32 frameCount);

        // 音频系统
        ma_context m_Context;
        ma_device m_Device;
        bool m_Initialized;
        
        // 播放管理
        struct AudioPlayback {
            uint32_t id;
            ResourceRef audioResource;
            ma_sound sound;
            float volume;
            bool loop;
            bool playing;
        };
        
        std::unordered_map<uint32_t, AudioPlayback> m_ActivePlaybacks;
        uint32_t m_NextPlayID;
        std::mutex m_PlaybackMutex;
    };

} // namespace Resources
} // namespace Zgine
