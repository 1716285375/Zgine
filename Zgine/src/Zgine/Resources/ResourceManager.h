#pragma once

#include "zgpch.h"
#include "Core/IResourceBackend.h"
#include "Core/ResourceBackendRegistry.h"
#include "ResourceTypes.h"
#include "IResource.h"
#include "Zgine/Logging/Log.h"
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>

namespace Zgine {
namespace Resources {

    /**
     * @brief 资源加载任务
     */
    struct LoadTask {
        std::string path;
        ResourceType type;
        ResourceLoadConfig config;
        ResourceLoadCallback callback;
        std::atomic<bool> completed{false};
        std::atomic<bool> success{false};
        ResourceRef resource{nullptr};
        
        LoadTask(const std::string& p, ResourceType t, const ResourceLoadConfig& cfg, ResourceLoadCallback cb)
            : path(p), type(t), config(cfg), callback(std::move(cb)) {}
    };

    /**
     * @brief 资源管理器
     * 支持多后端、热拔插的模块化资源管理系统
     */
    class ResourceManager {
    public:
        ResourceManager();
        virtual ~ResourceManager();

        /**
         * @brief 初始化资源管理器
         * @return true如果初始化成功
         */
        bool Initialize();

        /**
         * @brief 关闭资源管理器
         */
        void Shutdown();

        /**
         * @brief 更新资源管理器
         * @param deltaTime 时间间隔
         */
        void Update(float deltaTime);

        /**
         * @brief 注册资源后端
         * @param name 后端名称
         * @param backend 后端实例
         * @param priority 优先级
         * @return true如果注册成功
         */
        bool RegisterBackend(const std::string& name, Core::ResourceBackendRef backend, int priority = 0);

        /**
         * @brief 注销资源后端
         * @param name 后端名称
         * @return true如果注销成功
         */
        bool UnregisterBackend(const std::string& name);

        /**
         * @brief 设置默认后端
         * @param name 后端名称
         * @return true如果设置成功
         */
        bool SetDefaultBackend(const std::string& name);

        /**
         * @brief 同步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param config 加载配置
         * @return 资源引用
         */
        ResourceRef LoadSync(const std::string& path, ResourceType type, 
                           const ResourceLoadConfig& config = {});

        /**
         * @brief 异步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param config 加载配置
         * @param callback 加载完成回调
         * @return 资源引用
         */
        ResourceRef LoadAsync(const std::string& path, ResourceType type,
                             const ResourceLoadConfig& config,
                             ResourceLoadCallback callback = nullptr);

        /**
         * @brief 获取已加载的资源
         * @param path 资源路径
         * @return 资源引用
         */
        ResourceRef GetResource(const std::string& path);

        /**
         * @brief 检查资源是否已加载
         * @param path 资源路径
         * @return true如果已加载
         */
        bool IsLoaded(const std::string& path);

        /**
         * @brief 卸载资源
         * @param path 资源路径
         * @return true如果卸载成功
         */
        bool UnloadResource(const std::string& path);

        /**
         * @brief 获取所有已加载的资源
         * @return 资源列表
         */
        std::vector<ResourceRef> GetAllResources() const;

        /**
         * @brief 清理未使用的资源
         * @return 清理的资源数量
         */
        uint32_t CleanupUnusedResources();

        /**
         * @brief 获取统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 获取指定类型的后端
         * @param name 后端名称
         * @return 后端引用
         */
        Core::ResourceBackendRef GetBackend(const std::string& name);

        /**
         * @brief 获取所有已注册的后端
         * @return 后端名称列表
         */
        std::vector<std::string> GetRegisteredBackends() const;

    private:
        /**
         * @brief 异步加载工作线程
         */
        void LoadWorkerThread();

        /**
         * @brief 处理加载任务
         * @param task 加载任务
         */
        void ProcessLoadTask(std::shared_ptr<LoadTask> task);

        /**
         * @brief 选择最佳后端
         * @param path 资源路径
         * @param type 资源类型
         * @return 后端名称
         */
        std::string SelectBestBackend(const std::string& path, ResourceType type);

        // 资源缓存
        std::unordered_map<std::string, ResourceRef> m_ResourceCache;
        mutable std::mutex m_CacheMutex;

        // 后端管理
        std::unordered_map<std::string, Core::ResourceBackendRef> m_ActiveBackends;
        std::string m_DefaultBackend;
        mutable std::mutex m_BackendMutex;

        // 异步加载
        std::queue<std::shared_ptr<LoadTask>> m_LoadQueue;
        std::mutex m_QueueMutex;
        std::condition_variable m_QueueCondition;
        std::vector<std::thread> m_WorkerThreads;
        std::atomic<bool> m_Shutdown{false};

        // 统计信息
        std::atomic<uint32_t> m_TotalLoads{0};
        std::atomic<uint32_t> m_FailedLoads{0};
        std::atomic<uint32_t> m_AsyncLoads{0};
        std::atomic<uint32_t> m_CacheHits{0};
    };

} // namespace Resources
} // namespace Zgine
