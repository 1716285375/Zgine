#include "zgpch.h"
#include "ResourceManager.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <future>

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
        
        // 启动工作线程
        m_ShouldStop = false;
        size_t threadCount = std::thread::hardware_concurrency();
        if (threadCount == 0) threadCount = 2; // 至少2个线程
        
        for (size_t i = 0; i < threadCount; ++i) {
            m_WorkerThreads.emplace_back(&ResourceManager::WorkerThread, this);
        }
        
        ZG_CORE_INFO("ResourceManager initialized with {} worker threads", threadCount);
        return true;
    }

    void ResourceManager::Shutdown() {
        ZG_CORE_INFO("Shutting down ResourceManager...");
        
        // 停止工作线程
        m_ShouldStop = true;
        m_QueueCondition.notify_all();
        
        for (auto& thread : m_WorkerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_WorkerThreads.clear();
        
        // 卸载所有资源
        UnloadAll();
        
        ZG_CORE_INFO("ResourceManager shutdown complete");
    }

    void ResourceManager::Update(float deltaTime) {
        // 定期清理未使用的资源
        static float cleanupTimer = 0.0f;
        cleanupTimer += deltaTime;
        
        if (cleanupTimer >= 30.0f) { // 每30秒清理一次
            size_t cleaned = CleanupUnusedResources();
            if (cleaned > 0) {
                ZG_CORE_TRACE("Cleaned up {} unused resources", cleaned);
            }
            cleanupTimer = 0.0f;
        }
    }

    ResourceRef ResourceManager::LoadAsync(const std::string& path, ResourceType type, 
                                         ResourceLoadCallback callback) {
        // 检查资源是否已存在
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
        
        m_CacheMisses++;
        
        // 验证资源文件
        if (!ValidateResourceFile(path, type)) {
            ZG_CORE_ERROR("Invalid resource file: {}", path);
            if (callback) {
                callback(nullptr, false);
            }
            return nullptr;
        }
        
        // 创建加载任务
        LoadTask task(path, type, callback);
        
        // 添加到加载队列
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_LoadQueue.push(task);
        }
        m_QueueCondition.notify_one();
        
        // 创建资源对象（状态为Loading）
        ResourceRef resource = CreateResource(path, type);
        if (resource) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
        }
        
        return resource;
    }

    ResourceRef ResourceManager::LoadSync(const std::string& path, ResourceType type) {
        // 检查资源是否已存在
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            auto it = m_ResourceCache.find(path);
            if (it != m_ResourceCache.end()) {
                m_CacheHits++;
                return it->second;
            }
        }
        
        m_CacheMisses++;
        
        // 验证资源文件
        if (!ValidateResourceFile(path, type)) {
            ZG_CORE_ERROR("Invalid resource file: {}", path);
            return nullptr;
        }
        
        // 创建资源对象
        ResourceRef resource = CreateResource(path, type);
        if (!resource) {
            return nullptr;
        }
        
        // 同步加载
        if (resource->LoadSync()) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
            m_TotalLoads++;
            return resource;
        } else {
            m_FailedLoads++;
            return nullptr;
        }
    }

    ResourceRef ResourceManager::GetResource(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            m_CacheHits++;
            return it->second;
        }
        m_CacheMisses++;
        return nullptr;
    }

    bool ResourceManager::IsLoaded(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        return it != m_ResourceCache.end() && it->second->IsLoaded();
    }

    bool ResourceManager::Unload(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            it->second->Unload();
            m_ResourceCache.erase(it);
            return true;
        }
        return false;
    }

    void ResourceManager::UnloadAll() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        for (auto& pair : m_ResourceCache) {
            pair.second->Unload();
        }
        m_ResourceCache.clear();
        m_CurrentCacheSize = 0;
    }

    bool ResourceManager::Reload(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            return it->second->Reload();
        }
        return false;
    }

    void ResourceManager::ReloadAll() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        for (auto& pair : m_ResourceCache) {
            pair.second->Reload();
        }
    }

    std::string ResourceManager::GetStats() const {
        std::stringstream ss;
        ss << "ResourceManager Stats:\n";
        ss << "  Loaded Resources: " << m_ResourceCache.size() << "\n";
        ss << "  Cache Size: " << (m_CurrentCacheSize.load() / 1024 / 1024) << " MB\n";
        ss << "  Max Cache Size: " << (m_MaxCacheSize / 1024 / 1024) << " MB\n";
        ss << "  Total Loads: " << m_TotalLoads.load() << "\n";
        ss << "  Failed Loads: " << m_FailedLoads.load() << "\n";
        ss << "  Cache Hits: " << m_CacheHits.load() << "\n";
        ss << "  Cache Misses: " << m_CacheMisses.load() << "\n";
        
        float hitRate = 0.0f;
        uint32_t totalRequests = m_CacheHits.load() + m_CacheMisses.load();
        if (totalRequests > 0) {
            hitRate = (float)m_CacheHits.load() / totalRequests * 100.0f;
        }
        ss << "  Cache Hit Rate: " << std::fixed << std::setprecision(2) << hitRate << "%\n";
        
        return ss.str();
    }

    void ResourceManager::SetStateCallback(ResourceStateCallback callback) {
        m_StateCallback = callback;
    }

    void ResourceManager::SetMaxCacheSize(size_t maxSize) {
        m_MaxCacheSize = maxSize;
    }

    size_t ResourceManager::GetCurrentCacheSize() const {
        return m_CurrentCacheSize.load();
    }

    size_t ResourceManager::CleanupUnusedResources() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        size_t cleaned = 0;
        
        auto it = m_ResourceCache.begin();
        while (it != m_ResourceCache.end()) {
            if (it->second->GetRefCount() == 0) {
                it->second->Unload();
                it = m_ResourceCache.erase(it);
                cleaned++;
            } else {
                ++it;
            }
        }
        
        return cleaned;
    }

    std::vector<std::string> ResourceManager::GetLoadedResourcePaths() const {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        std::vector<std::string> paths;
        paths.reserve(m_ResourceCache.size());
        
        for (const auto& pair : m_ResourceCache) {
            paths.push_back(pair.first);
        }
        
        return paths;
    }

    bool ResourceManager::ValidateResourceFile(const std::string& path, ResourceType type) {
        if (path.empty()) {
            return false;
        }
        
        std::filesystem::path filePath(path);
        if (!std::filesystem::exists(filePath)) {
            ZG_CORE_ERROR("Resource file does not exist: {}", path);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filePath)) {
            ZG_CORE_ERROR("Resource path is not a file: {}", path);
            return false;
        }
        
        // 检查文件大小
        auto fileSize = std::filesystem::file_size(filePath);
        if (fileSize == 0) {
            ZG_CORE_ERROR("Resource file is empty: {}", path);
            return false;
        }
        
        return true;
    }

    size_t ResourceManager::GetResourceFileSize(const std::string& path) {
        try {
            return std::filesystem::file_size(path);
        } catch (const std::filesystem::filesystem_error& e) {
            ZG_CORE_ERROR("Failed to get file size for {}: {}", path, e.what());
            return 0;
        }
    }

    uint32_t ResourceManager::GenerateResourceID(const std::string& path) {
        static std::hash<std::string> hasher;
        return static_cast<uint32_t>(hasher(path));
    }

    void ResourceManager::WorkerThread() {
        while (!m_ShouldStop) {
            LoadTask task("", ResourceType::Unknown);
            
            // 等待任务
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                m_QueueCondition.wait(lock, [this] { return !m_LoadQueue.empty() || m_ShouldStop; });
                
                if (m_ShouldStop) {
                    break;
                }
                
                task = m_LoadQueue.front();
                m_LoadQueue.pop();
            }
            
            // 处理任务
            ProcessLoadTask(task);
        }
    }

    void ResourceManager::ProcessLoadTask(const LoadTask& task) {
        // 获取资源对象
        ResourceRef resource;
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            auto it = m_ResourceCache.find(task.path);
            if (it != m_ResourceCache.end()) {
                resource = it->second;
            }
        }
        
        if (!resource) {
            ZG_CORE_ERROR("Resource not found for async loading: {}", task.path);
            if (task.callback) {
                task.callback(nullptr, false);
            }
            task.promise->set_value(false);
            return;
        }
        
        // 异步加载
        bool success = resource->LoadSync();
        
        if (success) {
            m_TotalLoads++;
            ZG_CORE_TRACE("Successfully loaded resource: {}", task.path);
        } else {
            m_FailedLoads++;
            ZG_CORE_ERROR("Failed to load resource: {}", task.path);
        }
        
        // 调用回调
        if (task.callback) {
            task.callback(resource, success);
        }
        
        // 设置promise
        task.promise->set_value(success);
    }

    void ResourceManager::NotifyStateChange(ResourceRef resource, ResourceState oldState, ResourceState newState) {
        if (m_StateCallback) {
            m_StateCallback(resource, oldState, newState);
        }
    }

    bool ResourceManager::UnloadResource(const std::string& path)
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        auto it = m_ResourceCache.find(path);
        if (it != m_ResourceCache.end()) {
            it->second->Unload();
            m_ResourceCache.erase(it);
            return true;
        }
        
        return false;
    }

    std::vector<ResourceRef> ResourceManager::GetAllResources() const
    {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        std::vector<ResourceRef> resources;
        resources.reserve(m_ResourceCache.size());
        
        for (const auto& [path, resource] : m_ResourceCache) {
            resources.push_back(resource);
        }
        
        return resources;
    }

    void ResourceManager::Shutdown() {
        ZG_CORE_INFO("Shutting down ResourceManager...");
        
        // 停止工作线程
        m_ShouldStop = true;
        m_QueueCondition.notify_all();
        
        for (auto& thread : m_WorkerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_WorkerThreads.clear();
        
        // 卸载所有资源
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache.clear();
        }
        
        ZG_CORE_INFO("ResourceManager shutdown complete");
    }

} // namespace Resources
} // namespace Zgine
