#pragma once

#include "IResource.h"
#include "Zgine/Core/SmartPointers.h"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <future>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

namespace Zgine {
namespace Resources {

    /**
     * @brief 资源加载任务
     */
    struct LoadTask {
        std::string path;
        ResourceType type;
        ResourceLoadCallback callback;
        std::shared_ptr<std::promise<bool>> promise;
        
        LoadTask(const std::string& p, ResourceType t, ResourceLoadCallback cb = nullptr)
            : path(p), type(t), callback(cb), promise(std::make_shared<std::promise<bool>>()) {}
    };

    /**
     * @brief 资源管理器基类
     * @details 提供资源管理的通用功能，包括缓存、异步加载、引用计数等
     */
    class ResourceManager {
    public:
        ResourceManager();
        virtual ~ResourceManager();

        // 禁用拷贝构造和赋值
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        /**
         * @brief 初始化资源管理器
         * @return true如果初始化成功，false否则
         */
        virtual bool Initialize();

        /**
         * @brief 关闭资源管理器
         */
        virtual void Shutdown();

        /**
         * @brief 更新资源管理器
         * @param deltaTime 时间间隔
         */
        virtual void Update(float deltaTime);

        /**
         * @brief 异步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param callback 加载完成回调
         * @return 资源引用，如果资源已存在则直接返回
         */
        virtual ResourceRef LoadAsync(const std::string& path, ResourceType type, 
                                    ResourceLoadCallback callback = nullptr);

        /**
         * @brief 同步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @return 资源引用，如果加载失败返回nullptr
         */
        virtual ResourceRef LoadSync(const std::string& path, ResourceType type);

        /**
         * @brief 获取已加载的资源
         * @param path 资源路径
         * @return 资源引用，如果不存在返回nullptr
         */
        virtual ResourceRef GetResource(const std::string& path);

        /**
         * @brief 检查资源是否已加载
         * @param path 资源路径
         * @return true如果已加载，false否则
         */
        virtual bool IsLoaded(const std::string& path);

        /**
         * @brief 卸载资源
         * @param path 资源路径
         * @return true如果卸载成功，false否则
         */
        virtual bool Unload(const std::string& path);

        /**
         * @brief 卸载所有资源
         */
        virtual void UnloadAll();

        /**
         * @brief 重新加载资源
         * @param path 资源路径
         * @return true如果重新加载成功，false否则
         */
        virtual bool Reload(const std::string& path);

        /**
         * @brief 重新加载所有资源
         */
        virtual void ReloadAll();

        /**
         * @brief 获取资源统计信息
         * @return 包含资源数量、内存使用等信息的字符串
         */
        virtual std::string GetStats() const;

        /**
         * @brief 设置资源状态变化回调
         * @param callback 状态变化回调
         */
        virtual void SetStateCallback(ResourceStateCallback callback);

        /**
         * @brief 设置最大缓存大小（字节）
         * @param maxSize 最大缓存大小
         */
        virtual void SetMaxCacheSize(size_t maxSize);

        /**
         * @brief 获取当前缓存大小（字节）
         * @return 当前缓存大小
         */
        virtual size_t GetCurrentCacheSize() const;

        /**
         * @brief 清理未使用的资源
         * @return 清理的资源数量
         */
        virtual size_t CleanupUnusedResources();

        /**
         * @brief 获取所有已加载资源的路径
         * @return 资源路径列表
         */
        virtual std::vector<std::string> GetLoadedResourcePaths() const;

        /**
         * @brief 卸载资源
         * @param path 资源路径
         * @return true如果卸载成功
         */
        virtual bool UnloadResource(const std::string& path);

        /**
         * @brief 获取所有资源
         * @return 资源列表
         */
        virtual std::vector<ResourceRef> GetAllResources() const;

    protected:
        // Friend classes for accessing protected members
        friend class TextureManager;
        friend class ShaderManager;
        
        /**
         * @brief 创建资源对象
         * @param path 资源路径
         * @param type 资源类型
         * @return 资源引用
         */
        virtual ResourceRef CreateResource(const std::string& path, ResourceType type) = 0;

        /**
         * @brief 验证资源文件
         * @param path 资源路径
         * @param type 资源类型
         * @return true如果文件有效，false否则
         */
        virtual bool ValidateResourceFile(const std::string& path, ResourceType type);

        /**
         * @brief 获取资源文件大小
         * @param path 资源路径
         * @return 文件大小（字节）
         */
        virtual size_t GetResourceFileSize(const std::string& path);

        /**
         * @brief 生成资源ID
         * @param path 资源路径
         * @return 唯一资源ID
         */
        virtual uint32_t GenerateResourceID(const std::string& path);

        /**
         * @brief 工作线程函数
         */
        void WorkerThread();

        /**
         * @brief 处理加载任务
         * @param task 加载任务
         */
        void ProcessLoadTask(const LoadTask& task);

        /**
         * @brief 通知状态变化
         * @param resource 资源引用
         * @param oldState 旧状态
         * @param newState 新状态
         */
        void NotifyStateChange(ResourceRef resource, ResourceState oldState, ResourceState newState);

    protected:
        // 资源缓存（供子类访问）
        std::unordered_map<std::string, ResourceRef> m_ResourceCache;
        mutable std::mutex m_CacheMutex;

    private:
        // 异步加载
        std::queue<LoadTask> m_LoadQueue;
        std::mutex m_QueueMutex;
        std::condition_variable m_QueueCondition;
        std::vector<std::thread> m_WorkerThreads;
        std::atomic<bool> m_ShouldStop{false};

        // 配置
        size_t m_MaxCacheSize{1024 * 1024 * 1024}; // 1GB
        std::atomic<size_t> m_CurrentCacheSize{0};

        // 回调
        ResourceStateCallback m_StateCallback;

        // 统计
        std::atomic<uint32_t> m_TotalLoads{0};
        std::atomic<uint32_t> m_FailedLoads{0};
        std::atomic<uint32_t> m_CacheHits{0};
        std::atomic<uint32_t> m_CacheMisses{0};
    };

} // namespace Resources
} // namespace Zgine
