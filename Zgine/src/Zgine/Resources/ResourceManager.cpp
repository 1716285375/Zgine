#include "zgpch.h"
#include "ResourceManager.h"
#include "Core/IResourceBackend.h"
#include "Core/ResourceBackendRegistry.h"
#include <algorithm>
#include <filesystem>

namespace Zgine {
namespace Resources {

    ResourceManager::ResourceManager() {
        ZG_CORE_INFO("ResourceManager created");
    }

    ResourceManager::~ResourceManager() {
        Shutdown();
    }

    bool ResourceManager::Initialize() {
        ZG_CORE_INFO("Initializing ResourceManager...");

        // 从ResourceBackendRegistry加载已注册的后端
        auto& registry = Core::ResourceBackendRegistry::GetInstance();
        auto registeredBackendNames = registry.GetRegisteredBackends();
        
        for (const auto& backendName : registeredBackendNames) {
            ZG_CORE_INFO("Loading registered backend: {}", backendName);
            
            auto backend = registry.CreateBackend(backendName);
            if (backend && backend->Initialize()) {
                m_ActiveBackends[backendName] = backend;
                ZG_CORE_INFO("Successfully loaded backend: {}", backendName);
                
                // 设置第一个后端为默认后端
                if (m_DefaultBackend.empty()) {
                    m_DefaultBackend = backendName;
                    ZG_CORE_INFO("Set default backend to: {}", m_DefaultBackend);
                }
            } else {
                ZG_CORE_ERROR("Failed to initialize backend: {}", backendName);
            }
        }

        // 启动工作线程
        m_Shutdown = false;
        uint32_t threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 2; // 至少2个线程

        for (uint32_t i = 0; i < threadCount; ++i) {
            m_WorkerThreads.emplace_back(&ResourceManager::LoadWorkerThread, this);
        }

        ZG_CORE_INFO("ResourceManager initialized with {} worker threads", threadCount);
        return true;
    }

    void ResourceManager::Shutdown() {
        ZG_CORE_INFO("Shutting down ResourceManager...");

        // 停止工作线程
        m_Shutdown = true;
        m_QueueCondition.notify_all();

        for (auto& thread : m_WorkerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_WorkerThreads.clear();

        // 关闭所有后端
        {
            std::lock_guard<std::mutex> lock(m_BackendMutex);
            for (auto& pair : m_ActiveBackends) {
                pair.second->Shutdown();
            }
            m_ActiveBackends.clear();
        }

        // 清理资源缓存
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache.clear();
        }

        ZG_CORE_INFO("ResourceManager shutdown complete");
    }

    void ResourceManager::Update(float deltaTime) {
        // 更新所有后端
        {
            std::lock_guard<std::mutex> lock(m_BackendMutex);
            for (auto& pair : m_ActiveBackends) {
                // 如果后端支持Update方法，可以在这里调用
                // pair.second->Update(deltaTime);
            }
        }
    }

    bool ResourceManager::RegisterBackend(const std::string& name, Core::ResourceBackendRef backend, int priority) {
        if (!backend) {
            ZG_CORE_ERROR("Cannot register null backend: {}", name);
            return false;
        }

        std::lock_guard<std::mutex> lock(m_BackendMutex);
        
        if (m_ActiveBackends.find(name) != m_ActiveBackends.end()) {
            ZG_CORE_WARN("Backend '{}' is already registered", name);
            return false;
        }

        if (!backend->Initialize()) {
            ZG_CORE_ERROR("Failed to initialize backend: {}", name);
            return false;
        }

        m_ActiveBackends[name] = backend;
        
        // 如果是第一个后端，设为默认后端
        if (m_DefaultBackend.empty()) {
            m_DefaultBackend = name;
        }

        ZG_CORE_INFO("Registered backend: {} (priority: {})", name, priority);
        return true;
    }

    bool ResourceManager::UnregisterBackend(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_BackendMutex);
        
        auto it = m_ActiveBackends.find(name);
        if (it == m_ActiveBackends.end()) {
            ZG_CORE_WARN("Backend '{}' is not registered", name);
            return false;
        }

        it->second->Shutdown();
        m_ActiveBackends.erase(it);

        // 如果这是默认后端，选择新的默认后端
        if (m_DefaultBackend == name) {
            m_DefaultBackend = m_ActiveBackends.empty() ? "" : m_ActiveBackends.begin()->first;
        }

        ZG_CORE_INFO("Unregistered backend: {}", name);
        return true;
    }

    bool ResourceManager::SetDefaultBackend(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_BackendMutex);
        
        if (m_ActiveBackends.find(name) == m_ActiveBackends.end()) {
            ZG_CORE_ERROR("Backend '{}' is not registered", name);
            return false;
        }

        m_DefaultBackend = name;
        ZG_CORE_INFO("Set default backend to: {}", name);
        return true;
    }

    ResourceRef ResourceManager::LoadSync(const std::string& path, ResourceType type, 
                                       const ResourceLoadConfig& config) {
        // 检查缓存
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            auto it = m_ResourceCache.find(path);
            if (it != m_ResourceCache.end()) {
                m_CacheHits++;
                return it->second;
            }
        }

        // 选择后端
        std::string backendName = SelectBestBackend(path, type);
        if (backendName.empty()) {
            ZG_CORE_ERROR("No backend available for resource: {} (type: {})", path, static_cast<int>(type));
            m_FailedLoads++;
            return nullptr;
        }

        Core::ResourceBackendRef backend;
        {
            std::lock_guard<std::mutex> lock(m_BackendMutex);
            auto it = m_ActiveBackends.find(backendName);
            if (it == m_ActiveBackends.end()) {
                ZG_CORE_ERROR("Backend '{}' is not available", backendName);
                m_FailedLoads++;
                return nullptr;
            }
            backend = it->second;
        }

        // 加载资源
        ResourceRef resource = backend->LoadSync(path, type, config);
        if (!resource) {
            m_FailedLoads++;
            return nullptr;
        }

        // 添加到缓存
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
        }

        m_TotalLoads++;
        ZG_CORE_TRACE("Loaded resource: {} using backend: {}", path, backendName);
        return resource;
    }

    ResourceRef ResourceManager::LoadAsync(const std::string& path, ResourceType type,
                                         const ResourceLoadConfig& config,
                                         ResourceLoadCallback callback) {
        // 检查缓存
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            auto it = m_ResourceCache.find(path);
            if (it != m_ResourceCache.end()) {
                m_CacheHits++;
                if (callback) {
                    callback(it->second, true);
                }
                return it->second;
            }
        }

        // 创建加载任务
        auto task = std::make_shared<LoadTask>(path, type, config, callback);
        
        // 添加到队列
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_LoadQueue.push(task);
        }
        
        m_QueueCondition.notify_one();
        m_AsyncLoads++;
        
        ZG_CORE_TRACE("Queued async load task: {}", path);
        return nullptr; // 异步加载不立即返回资源
    }

    ResourceRef ResourceManager::GetResource(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        return it != m_ResourceCache.end() ? it->second : nullptr;
    }

    bool ResourceManager::IsLoaded(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        return m_ResourceCache.find(path) != m_ResourceCache.end();
    }

    bool ResourceManager::UnloadResource(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            m_ResourceCache.erase(it);
            ZG_CORE_TRACE("Unloaded resource: {}", path);
            return true;
        }
        return false;
    }

    std::vector<ResourceRef> ResourceManager::GetAllResources() const {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        std::vector<ResourceRef> resources;
        resources.reserve(m_ResourceCache.size());
        
        for (const auto& pair : m_ResourceCache) {
            resources.push_back(pair.second);
        }
        
        return resources;
    }

    uint32_t ResourceManager::CleanupUnusedResources() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t cleaned = 0;
        auto it = m_ResourceCache.begin();
        while (it != m_ResourceCache.end()) {
            // 检查资源是否只被缓存引用（引用计数为1）
            if (it->second.use_count() == 1) {
                ZG_CORE_TRACE("Cleaning up unused resource: {}", it->first);
                it = m_ResourceCache.erase(it);
                cleaned++;
            } else {
                ++it;
            }
        }
        
        return cleaned;
    }

    std::string ResourceManager::GetStatistics() const {
        std::lock_guard<std::mutex> cacheLock(m_CacheMutex);
        std::lock_guard<std::mutex> backendLock(m_BackendMutex);
        
        std::ostringstream stats;
        stats << "ResourceManager Statistics:\n";
        stats << "  Total Loads: " << m_TotalLoads.load() << "\n";
        stats << "  Failed Loads: " << m_FailedLoads.load() << "\n";
        stats << "  Async Loads: " << m_AsyncLoads.load() << "\n";
        stats << "  Cache Hits: " << m_CacheHits.load() << "\n";
        stats << "  Cached Resources: " << m_ResourceCache.size() << "\n";
        stats << "  Active Backends: " << m_ActiveBackends.size() << "\n";
        stats << "  Default Backend: " << m_DefaultBackend << "\n";
        
        return stats.str();
    }

    Core::ResourceBackendRef ResourceManager::GetBackend(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_BackendMutex);
        auto it = m_ActiveBackends.find(name);
        return it != m_ActiveBackends.end() ? it->second : nullptr;
    }

    std::vector<std::string> ResourceManager::GetRegisteredBackends() const {
        std::lock_guard<std::mutex> lock(m_BackendMutex);
        
        std::vector<std::string> backends;
        backends.reserve(m_ActiveBackends.size());
        
        for (const auto& pair : m_ActiveBackends) {
            backends.push_back(pair.first);
        }
        
        return backends;
    }

    void ResourceManager::LoadWorkerThread() {
        while (!m_Shutdown) {
            std::shared_ptr<LoadTask> task;
            
            // 获取任务
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                m_QueueCondition.wait(lock, [this] { return !m_LoadQueue.empty() || m_Shutdown; });
                
                if (m_Shutdown) break;
                
                task = m_LoadQueue.front();
                m_LoadQueue.pop();
            }
            
            // 处理任务
            ProcessLoadTask(task);
        }
    }

    void ResourceManager::ProcessLoadTask(std::shared_ptr<LoadTask> task) {
        // 选择后端
        std::string backendName = SelectBestBackend(task->path, task->type);
        if (backendName.empty()) {
            ZG_CORE_ERROR("No backend available for async load: {} (type: {})", task->path, static_cast<int>(task->type));
            task->success = false;
            task->completed = true;
            if (task->callback) {
                task->callback(nullptr, false);
            }
            return;
        }

        Core::ResourceBackendRef backend;
        {
            std::lock_guard<std::mutex> lock(m_BackendMutex);
            auto it = m_ActiveBackends.find(backendName);
            if (it == m_ActiveBackends.end()) {
                ZG_CORE_ERROR("Backend '{}' is not available for async load", backendName);
                task->success = false;
                task->completed = true;
                if (task->callback) {
                    task->callback(nullptr, false);
                }
                return;
            }
            backend = it->second;
        }

        // 加载资源
        ResourceRef resource = backend->LoadSync(task->path, task->type, task->config);
        task->resource = resource;
        task->success = resource != nullptr;
        task->completed = true;

        if (task->success) {
            // 添加到缓存
            {
                std::lock_guard<std::mutex> lock(m_CacheMutex);
                m_ResourceCache[task->path] = resource;
            }
            m_TotalLoads++;
            ZG_CORE_TRACE("Async loaded resource: {} using backend: {}", task->path, backendName);
        } else {
            m_FailedLoads++;
        }

        // 调用回调
        if (task->callback) {
            task->callback(resource, task->success);
        }
    }

    std::string ResourceManager::SelectBestBackend(const std::string& path, ResourceType type) {
        std::lock_guard<std::mutex> lock(m_BackendMutex);
        
        // 首先尝试默认后端
        if (!m_DefaultBackend.empty()) {
            auto it = m_ActiveBackends.find(m_DefaultBackend);
            if (it != m_ActiveBackends.end() && it->second->SupportsResourceType(type)) {
                return m_DefaultBackend;
            }
        }

        // 查找支持该资源类型的后端
        for (const auto& pair : m_ActiveBackends) {
            if (pair.second->SupportsResourceType(type)) {
                return pair.first;
            }
        }

        return "";
    }

} // namespace Resources
} // namespace Zgine
