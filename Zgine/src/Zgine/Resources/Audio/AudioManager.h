#pragma once

#include "zgpch.h"
#include "IAudioBackend.h"
#include "../ResourceManager.h"
#include "Zgine/Logging/Log.h"

namespace Zgine {
namespace Resources {
namespace Audio {

    /**
     * @brief 音频管理器
     * 提供高级音频管理功能，基于新的资源管理系统
     */
    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        /**
         * @brief 初始化音频管理器
         * @return true如果初始化成功
         */
        bool Initialize();

        /**
         * @brief 关闭音频管理器
         */
        void Shutdown();

        /**
         * @brief 更新音频管理器
         * @param deltaTime 时间间隔
         */
        void Update(float deltaTime);

        /**
         * @brief 播放音频
         * @param path 音频文件路径
         * @param loop 是否循环播放
         * @param volume 音量 (0.0 - 1.0)
         * @return 播放ID，如果播放失败返回0
         */
        uint32_t PlayAudio(const std::string& path, bool loop = false, float volume = 1.0f);

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
         * @brief 获取当前播放的音频实例列表
         * @return 播放ID列表
         */
        std::vector<uint32_t> GetActivePlaybacks() const;

        /**
         * @brief 设置主音量
         * @param volume 主音量 (0.0 - 1.0)
         */
        void SetMasterVolume(float volume);

        /**
         * @brief 获取主音量
         * @return 主音量
         */
        float GetMasterVolume() const;

        /**
         * @brief 获取音频后端
         * @return 音频后端引用
         */
        AudioBackendRef GetAudioBackend() const;

        /**
         * @brief 获取资源管理器
         * @return 资源管理器引用
         */
        ResourceManager* GetResourceManager() const;

        /**
         * @brief 获取统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

    private:
        ResourceManager* m_ResourceManager;
        AudioBackendRef m_AudioBackend;
        float m_MasterVolume;
        std::string m_BackendName;

        /**
         * @brief 注册音频后端
         * @return true如果注册成功
         */
        bool RegisterAudioBackend();
    };

} // namespace Audio
} // namespace Resources
} // namespace Zgine