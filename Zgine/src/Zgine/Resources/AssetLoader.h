#pragma once

#include "Zgine/Core/SmartPointers.h"
#include "Zgine/Resources/IResource.h"
#include "Zgine/Resources/TextureManager.h"
#include "Zgine/Resources/ShaderManager.h"
#include "Zgine/Resources/ModelManager.h"
#include "Zgine/Resources/AudioManager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>

namespace Zgine {
namespace Resources {

    /**
     * @brief 资源加载器
     * @details 统一的资源加载接口，管理所有类型的资源加载
     */
    class AssetLoader {
    public:
        AssetLoader();
        ~AssetLoader();

        /**
         * @brief 初始化资源加载器
         * @return true如果初始化成功
         */
        bool Initialize();

        /**
         * @brief 关闭资源加载器
         */
        void Shutdown();

        // ============================================================================
        // 同步加载接口
        // ============================================================================

        /**
         * @brief 加载纹理
         * @param path 纹理路径
         * @return 纹理资源引用
         */
        ResourceRef LoadTexture(const std::string& path);

        /**
         * @brief 加载着色器
         * @param path 着色器路径
         * @return 着色器资源引用
         */
        ResourceRef LoadShader(const std::string& path);

        /**
         * @brief 加载模型
         * @param path 模型路径
         * @return 模型资源引用
         */
        ResourceRef LoadModel(const std::string& path);

        /**
         * @brief 加载音频
         * @param path 音频路径
         * @return 音频资源引用
         */
        ResourceRef LoadAudio(const std::string& path);

        // ============================================================================
        // 异步加载接口
        // ============================================================================

        /**
         * @brief 异步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param callback 加载完成回调
         * @return 任务ID
         */
        uint32_t LoadAsync(const std::string& path, ResourceType type, std::function<void(ResourceRef)> callback);

        /**
         * @brief 加载资源（自动检测类型）
         * @param path 资源路径
         * @return 资源引用
         */
        ResourceRef LoadAsset(const std::string& path);

        // ============================================================================
        // 异步加载接口
        // ============================================================================

        /**
         * @brief 异步加载纹理
         * @param path 纹理路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadTextureAsync(const std::string& path, 
                                 std::function<void(ResourceRef)> callback = nullptr);

        /**
         * @brief 异步加载着色器
         * @param path 着色器路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadShaderAsync(const std::string& path, 
                                std::function<void(ResourceRef)> callback = nullptr);

        /**
         * @brief 异步加载模型
         * @param path 模型路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadModelAsync(const std::string& path, 
                               std::function<void(ResourceRef)> callback = nullptr);

        /**
         * @brief 异步加载音频
         * @param path 音频路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadAudioAsync(const std::string& path, 
                               std::function<void(ResourceRef)> callback = nullptr);

        /**
         * @brief 异步加载资源（自动检测类型）
         * @param path 资源路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadAssetAsync(const std::string& path, 
                               std::function<void(ResourceRef)> callback = nullptr);

        // ============================================================================
        // 批量加载接口
        // ============================================================================

        /**
         * @brief 批量加载资源
         * @param paths 资源路径列表
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadAssetsBatch(const std::vector<std::string>& paths, 
                                std::function<void(const std::vector<ResourceRef>&)> callback = nullptr);

        /**
         * @brief 从配置文件加载资源
         * @param configPath 配置文件路径
         * @param callback 完成回调
         * @return 加载任务ID
         */
        uint32_t LoadAssetsFromConfig(const std::string& configPath, 
                                     std::function<void(const std::vector<ResourceRef>&)> callback = nullptr);

        // ============================================================================
        // 资源管理接口
        // ============================================================================

        /**
         * @brief 获取资源
         * @param path 资源路径
         * @return 资源引用
         */
        ResourceRef GetAsset(const std::string& path);

        /**
         * @brief 卸载资源
         * @param path 资源路径
         * @return true如果卸载成功
         */
        bool UnloadAsset(const std::string& path);

        /**
         * @brief 卸载所有资源
         */
        void UnloadAllAssets();

        /**
         * @brief 获取所有已加载的资源
         * @return 资源列表
         */
        std::vector<ResourceRef> GetAllAssets() const;

        /**
         * @brief 获取资源统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 清理未使用的资源
         * @return 清理的资源数量
         */
        uint32_t CleanupUnusedAssets();

        // ============================================================================
        // 任务管理接口
        // ============================================================================

        /**
         * @brief 取消加载任务
         * @param taskID 任务ID
         * @return true如果取消成功
         */
        bool CancelTask(uint32_t taskID);

        /**
         * @brief 获取任务状态
         * @param taskID 任务ID
         * @return 任务状态
         */
        bool IsTaskCompleted(uint32_t taskID) const;

        /**
         * @brief 等待任务完成
         * @param taskID 任务ID
         * @param timeoutMs 超时时间（毫秒）
         * @return true如果任务完成
         */
        bool WaitForTask(uint32_t taskID, uint32_t timeoutMs = 0) const;

        /**
         * @brief 获取活跃任务数量
         * @return 活跃任务数量
         */
        uint32_t GetActiveTaskCount() const;

        // ============================================================================
        // 管理器访问接口
        // ============================================================================

        /**
         * @brief 获取纹理管理器
         * @return 纹理管理器指针
         */
        TextureManager* GetTextureManager() const { return m_TextureManager.get(); }

        /**
         * @brief 获取着色器管理器
         * @return 着色器管理器指针
         */
        ShaderManager* GetShaderManager() const { return m_ShaderManager.get(); }

        /**
         * @brief 获取模型管理器
         * @return 模型管理器指针
         */
        ModelManager* GetModelManager() const { return m_ModelManager.get(); }

        /**
         * @brief 获取音频管理器
         * @return 音频管理器指针
         */
        AudioManager* GetAudioManager() const { return m_AudioManager.get(); }

    private:
        /**
         * @brief 检测资源类型
         * @param path 资源路径
         * @return 资源类型
         */
        ResourceType DetectResourceType(const std::string& path) const;

        /**
         * @brief 加载任务结构
         */
        struct LoadTask {
            uint32_t id = 0;
            std::string path;
            ResourceType type = ResourceType::Unknown;
            std::function<void(ResourceRef)> callback;
            std::future<ResourceRef> future;
            bool completed = false;
            
            // 禁用复制构造函数和赋值操作符，因为std::future不可复制
            LoadTask() = default;
            LoadTask(const LoadTask&) = delete;
            LoadTask& operator=(const LoadTask&) = delete;
            
            // 启用移动构造函数和赋值操作符
            LoadTask(LoadTask&&) = default;
            LoadTask& operator=(LoadTask&&) = default;
        };

        // 资源管理器
        Scope<TextureManager> m_TextureManager;
        Scope<ShaderManager> m_ShaderManager;
        Scope<ModelManager> m_ModelManager;
        Scope<AudioManager> m_AudioManager;

        // 任务管理
        std::unordered_map<uint32_t, LoadTask> m_LoadTasks;
        uint32_t m_NextTaskID;
        mutable std::mutex m_TaskMutex;

        // 资源缓存
        std::unordered_map<std::string, ResourceRef> m_AssetCache;
        mutable std::mutex m_CacheMutex;
    };

} // namespace Resources
} // namespace Zgine
