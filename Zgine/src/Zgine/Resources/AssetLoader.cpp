#include "zgpch.h"
#include "AssetLoader.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <future>
#include <chrono>

namespace Zgine {
namespace Resources {

    AssetLoader::AssetLoader() 
        : m_NextTaskID(1) {
        ZG_CORE_INFO("AssetLoader created");
    }

    AssetLoader::~AssetLoader() {
        Shutdown();
        ZG_CORE_INFO("AssetLoader destroyed");
    }

    bool AssetLoader::Initialize() {
        try {
            // 初始化各个资源管理器
            m_TextureManager = CreateScope<TextureManager>();
            m_ShaderManager = CreateScope<ShaderManager>();
            m_ModelManager = CreateScope<ModelManager>();
            m_AudioManager = CreateScope<AudioManager>();

            // 初始化音频管理器
            if (!m_AudioManager->Initialize()) {
                ZG_CORE_ERROR("Failed to initialize audio manager");
                return false;
            }

            ZG_CORE_INFO("AssetLoader initialized successfully");
            return true;
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Failed to initialize AssetLoader: {}", e.what());
            return false;
        }
    }

    void AssetLoader::Shutdown() {
        // 取消所有活跃任务
        {
            std::lock_guard<std::mutex> lock(m_TaskMutex);
            for (auto& [id, task] : m_LoadTasks) {
                if (task.future.valid()) {
                    // 等待任务完成或超时
                    task.future.wait_for(std::chrono::milliseconds(100));
                }
            }
            m_LoadTasks.clear();
        }

        // 关闭资源管理器
        if (m_AudioManager) {
            m_AudioManager->Shutdown();
        }

        // 清理资源缓存
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_AssetCache.clear();
        }

        ZG_CORE_INFO("AssetLoader shutdown");
    }

    // ============================================================================
    // 同步加载接口
    // ============================================================================

    ResourceRef AssetLoader::LoadTexture(const std::string& path) {
        if (!m_TextureManager) {
            ZG_CORE_ERROR("TextureManager not initialized");
            return nullptr;
        }

        ResourceRef resource = m_TextureManager->LoadTexture(path, false);
        if (resource) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_AssetCache[path] = resource;
        }
        return resource;
    }

    ResourceRef AssetLoader::LoadShader(const std::string& path) {
        if (!m_ShaderManager) {
            ZG_CORE_ERROR("ShaderManager not initialized");
            return nullptr;
        }

        ResourceRef resource = m_ShaderManager->LoadShader(path, false);
        if (resource) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_AssetCache[path] = resource;
        }
        return resource;
    }

    ResourceRef AssetLoader::LoadModel(const std::string& path) {
        if (!m_ModelManager) {
            ZG_CORE_ERROR("ModelManager not initialized");
            return nullptr;
        }

        ResourceRef resource = m_ModelManager->LoadModel(path, false);
        if (resource) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_AssetCache[path] = resource;
        }
        return resource;
    }

    ResourceRef AssetLoader::LoadAudio(const std::string& path) {
        if (!m_AudioManager) {
            ZG_CORE_ERROR("AudioManager not initialized");
            return nullptr;
        }

        ResourceRef resource = m_AudioManager->LoadAudio(path, false);
        if (resource) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_AssetCache[path] = resource;
        }
        return resource;
    }

    ResourceRef AssetLoader::LoadAsset(const std::string& path) {
        ResourceType type = DetectResourceType(path);
        
        switch (type) {
            case ResourceType::Texture:
                return LoadTexture(path);
            case ResourceType::Shader:
                return LoadShader(path);
            case ResourceType::Model:
                return LoadModel(path);
            case ResourceType::Audio:
                return LoadAudio(path);
            default:
                ZG_CORE_WARN("Unknown resource type for path: {}", path);
                return nullptr;
        }
    }

    // ============================================================================
    // 异步加载接口
    // ============================================================================

    uint32_t AssetLoader::LoadTextureAsync(const std::string& path, std::function<void(ResourceRef)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.path = path;
        task.type = ResourceType::Texture;
        task.callback = callback;
        task.completed = false;
        
        // 启动异步任务
        task.future = std::async(std::launch::async, [this, path]() -> ResourceRef {
            return LoadTexture(path);
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started async texture loading task: {} (ID: {})", path, taskID);
        return taskID;
    }

    uint32_t AssetLoader::LoadShaderAsync(const std::string& path, std::function<void(ResourceRef)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.path = path;
        task.type = ResourceType::Shader;
        task.callback = callback;
        task.completed = false;
        
        task.future = std::async(std::launch::async, [this, path]() -> ResourceRef {
            return LoadShader(path);
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started async shader loading task: {} (ID: {})", path, taskID);
        return taskID;
    }

    uint32_t AssetLoader::LoadModelAsync(const std::string& path, std::function<void(ResourceRef)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.path = path;
        task.type = ResourceType::Model;
        task.callback = callback;
        task.completed = false;
        
        task.future = std::async(std::launch::async, [this, path]() -> ResourceRef {
            return LoadModel(path);
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started async model loading task: {} (ID: {})", path, taskID);
        return taskID;
    }

    uint32_t AssetLoader::LoadAudioAsync(const std::string& path, std::function<void(ResourceRef)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.path = path;
        task.type = ResourceType::Audio;
        task.callback = callback;
        task.completed = false;
        
        task.future = std::async(std::launch::async, [this, path]() -> ResourceRef {
            return LoadAudio(path);
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started async audio loading task: {} (ID: {})", path, taskID);
        return taskID;
    }

    uint32_t AssetLoader::LoadAssetAsync(const std::string& path, std::function<void(ResourceRef)> callback) {
        ResourceType type = DetectResourceType(path);
        
        switch (type) {
            case ResourceType::Texture:
                return LoadTextureAsync(path, callback);
            case ResourceType::Shader:
                return LoadShaderAsync(path, callback);
            case ResourceType::Model:
                return LoadModelAsync(path, callback);
            case ResourceType::Audio:
                return LoadAudioAsync(path, callback);
            default:
                ZG_CORE_WARN("Unknown resource type for async loading: {}", path);
                return 0;
        }
    }

    // ============================================================================
    // 批量加载接口
    // ============================================================================

    uint32_t AssetLoader::LoadAssetsBatch(const std::vector<std::string>& paths, 
                                         std::function<void(const std::vector<ResourceRef>&)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.type = ResourceType::Unknown; // 批量任务
        task.completed = false;
        
        task.future = std::async(std::launch::async, [this, paths, callback]() -> ResourceRef {
            std::vector<ResourceRef> resources;
            resources.reserve(paths.size());
            
            for (const auto& path : paths) {
                ResourceRef resource = LoadAsset(path);
                resources.push_back(resource);
            }
            
            if (callback) {
                callback(resources);
            }
            
            return nullptr; // 批量任务不返回单个资源
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started batch loading task with {} assets (ID: {})", paths.size(), taskID);
        return taskID;
    }

    uint32_t AssetLoader::LoadAssetsFromConfig(const std::string& configPath, 
                                              std::function<void(const std::vector<ResourceRef>&)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        LoadTask task;
        task.id = taskID;
        task.path = configPath;
        task.type = ResourceType::Unknown;
        task.completed = false;
        
        task.future = std::async(std::launch::async, [this, configPath, callback]() -> ResourceRef {
            std::vector<ResourceRef> resources;
            
            try {
                std::ifstream file(configPath);
                if (!file.is_open()) {
                    ZG_CORE_ERROR("Failed to open config file: {}", configPath);
                    if (callback) callback(resources);
                    return nullptr;
                }
                
                std::string line;
                while (std::getline(file, line)) {
                    // 跳过空行和注释
                    if (line.empty() || line[0] == '#') {
                        continue;
                    }
                    
                    // 简单的配置文件格式：每行一个资源路径
                    ResourceRef resource = LoadAsset(line);
                    if (resource) {
                        resources.push_back(resource);
                    }
                }
                
                file.close();
                ZG_CORE_INFO("Loaded {} assets from config: {}", resources.size(), configPath);
                
            } catch (const std::exception& e) {
                ZG_CORE_ERROR("Failed to load assets from config {}: {}", configPath, e.what());
            }
            
            if (callback) {
                callback(resources);
            }
            
            return nullptr;
        });
        
        m_LoadTasks[taskID] = std::move(task);
        
        ZG_CORE_TRACE("Started config loading task: {} (ID: {})", configPath, taskID);
        return taskID;
    }

    // ============================================================================
    // 资源管理接口
    // ============================================================================

    ResourceRef AssetLoader::GetAsset(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_AssetCache.find(path);
        return it != m_AssetCache.end() ? it->second : nullptr;
    }

    bool AssetLoader::UnloadAsset(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        auto it = m_AssetCache.find(path);
        if (it != m_AssetCache.end()) {
            ResourceType type = DetectResourceType(path);
            
            bool success = false;
            switch (type) {
                case ResourceType::Texture:
                    success = m_TextureManager ? m_TextureManager->UnloadTexture(path) : false;
                    break;
                case ResourceType::Shader:
                    success = m_ShaderManager ? m_ShaderManager->UnloadShader(path) : false;
                    break;
                case ResourceType::Model:
                    success = m_ModelManager ? m_ModelManager->UnloadModel(path) : false;
                    break;
                case ResourceType::Audio:
                    success = m_AudioManager ? m_AudioManager->UnloadAudio(path) : false;
                    break;
                default:
                    success = false;
            }
            
            if (success) {
                m_AssetCache.erase(it);
            }
            
            return success;
        }
        
        return false;
    }

    void AssetLoader::UnloadAllAssets() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t count = static_cast<uint32_t>(m_AssetCache.size());
        
        if (m_TextureManager) m_TextureManager->Clear();
        if (m_ShaderManager) m_ShaderManager->Clear();
        if (m_ModelManager) m_ModelManager->Clear();
        if (m_AudioManager) m_AudioManager->Clear();
        
        m_AssetCache.clear();
        
        ZG_CORE_INFO("Unloaded all assets: {}", count);
    }

    std::vector<ResourceRef> AssetLoader::GetAllAssets() const {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        std::vector<ResourceRef> assets;
        assets.reserve(m_AssetCache.size());
        
        for (const auto& [path, resource] : m_AssetCache) {
            assets.push_back(resource);
        }
        
        return assets;
    }

    std::string AssetLoader::GetStatistics() const {
        std::stringstream ss;
        
        ss << "AssetLoader Statistics:\n";
        ss << "  Cached Assets: " << m_AssetCache.size() << "\n";
        ss << "  Active Tasks: " << GetActiveTaskCount() << "\n";
        
        if (m_TextureManager) {
            ss << "\n" << m_TextureManager->GetStatistics();
        }
        if (m_ShaderManager) {
            ss << "\n" << m_ShaderManager->GetStatistics();
        }
        if (m_ModelManager) {
            ss << "\n" << m_ModelManager->GetStatistics();
        }
        if (m_AudioManager) {
            ss << "\n" << m_AudioManager->GetStatistics();
        }
        
        return ss.str();
    }

    uint32_t AssetLoader::CleanupUnusedAssets() {
        uint32_t totalCleaned = 0;
        
        if (m_TextureManager) totalCleaned += m_TextureManager->CleanupUnusedTextures();
        if (m_ShaderManager) totalCleaned += m_ShaderManager->CleanupUnusedShaders();
        if (m_ModelManager) totalCleaned += m_ModelManager->CleanupUnusedModels();
        if (m_AudioManager) totalCleaned += m_AudioManager->CleanupUnusedAudio();
        
        // 清理缓存中的无效引用
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_AssetCache.begin();
        while (it != m_AssetCache.end()) {
            if (!it->second || it->second->GetRefCount() == 0) {
                it = m_AssetCache.erase(it);
                totalCleaned++;
            } else {
                ++it;
            }
        }
        
        if (totalCleaned > 0) {
            ZG_CORE_INFO("Cleaned up {} unused assets", totalCleaned);
        }
        
        return totalCleaned;
    }

    // ============================================================================
    // 任务管理接口
    // ============================================================================

    bool AssetLoader::CancelTask(uint32_t taskID) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        auto it = m_LoadTasks.find(taskID);
        if (it != m_LoadTasks.end()) {
            // 等待任务完成或超时
            if (it->second.future.valid()) {
                it->second.future.wait_for(std::chrono::milliseconds(100));
            }
            
            m_LoadTasks.erase(it);
            ZG_CORE_INFO("Cancelled task: {}", taskID);
            return true;
        }
        
        return false;
    }

    bool AssetLoader::IsTaskCompleted(uint32_t taskID) const {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        auto it = m_LoadTasks.find(taskID);
        if (it != m_LoadTasks.end()) {
            if (it->second.future.valid()) {
                auto status = it->second.future.wait_for(std::chrono::seconds(0));
                return status == std::future_status::ready;
            }
        }
        
        return true; // 任务不存在或已完成
    }

    bool AssetLoader::WaitForTask(uint32_t taskID, uint32_t timeoutMs) const {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        auto it = m_LoadTasks.find(taskID);
        if (it != m_LoadTasks.end() && it->second.future.valid()) {
            auto timeout = timeoutMs > 0 ? 
                std::chrono::milliseconds(timeoutMs) : 
                std::chrono::seconds(30); // 默认30秒超时
            
            auto status = it->second.future.wait_for(timeout);
            return status == std::future_status::ready;
        }
        
        return true;
    }

    uint32_t AssetLoader::GetActiveTaskCount() const {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t count = 0;
        for (const auto& [id, task] : m_LoadTasks) {
            if (task.future.valid()) {
                auto status = task.future.wait_for(std::chrono::seconds(0));
                if (status != std::future_status::ready) {
                    count++;
                }
            }
        }
        
        return count;
    }

    ResourceType AssetLoader::DetectResourceType(const std::string& path) const {
        std::string extension = std::filesystem::path(path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // 纹理格式
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || 
            extension == ".bmp" || extension == ".tga" || extension == ".gif" ||
            extension == ".hdr" || extension == ".exr") {
            return ResourceType::Texture;
        }
        
        // 着色器格式
        if (extension == ".glsl" || extension == ".vert" || extension == ".frag" ||
            extension == ".comp" || extension == ".geom" || extension == ".tesc" ||
            extension == ".tese") {
            return ResourceType::Shader;
        }
        
        // 模型格式
        if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" ||
            extension == ".glb" || extension == ".dae" || extension == ".3ds" ||
            extension == ".blend") {
            return ResourceType::Model;
        }
        
        // 音频格式
        if (extension == ".wav" || extension == ".mp3" || extension == ".ogg" ||
            extension == ".flac" || extension == ".m4a" || extension == ".aac") {
            return ResourceType::Audio;
        }
        
        // 配置文件格式
        if (extension == ".json" || extension == ".xml" || extension == ".yaml" ||
            extension == ".yml" || extension == ".ini" || extension == ".cfg") {
            return ResourceType::Unknown;
        }
        
        return ResourceType::Unknown;
    }

    uint32_t AssetLoader::LoadAsync(const std::string& path, ResourceType type, std::function<void(ResourceRef)> callback) {
        std::lock_guard<std::mutex> lock(m_TaskMutex);
        
        uint32_t taskID = m_NextTaskID++;
        
        // 创建加载任务
        LoadTask task;
        task.id = taskID;
        task.path = path;
        task.type = type;
        task.callback = callback;
        task.completed = false;
        
        // 启动异步加载
        task.future = std::async(std::launch::async, [this, taskID, path = task.path, type = task.type]() -> ResourceRef {
            try {
                ResourceRef resource;
                
                switch (type) {
                    case ResourceType::Texture:
                        resource = m_TextureManager->LoadTexture(path);
                        break;
                    case ResourceType::Shader:
                        resource = m_ShaderManager->LoadShader(path);
                        break;
                    case ResourceType::Model:
                        resource = m_ModelManager->LoadModel(path);
                        break;
                    case ResourceType::Audio:
                        resource = m_AudioManager->LoadAudio(path);
                        break;
                    default:
                        ZG_CORE_ERROR("Unsupported resource type for async loading: {}", static_cast<int>(type));
                        return nullptr;
                }
                
                // 标记任务完成并调用回调
                {
                    std::lock_guard<std::mutex> lock(m_TaskMutex);
                    auto it = m_LoadTasks.find(taskID);
                    if (it != m_LoadTasks.end()) {
                        it->second.completed = true;
                        // 调用回调函数
                        if (it->second.callback) {
                            ZG_CORE_INFO("Calling callback for task {}", taskID);
                            it->second.callback(resource);
                        }
                    }
                }
                
                return resource;
            } catch (const std::exception& e) {
                ZG_CORE_ERROR("Async loading failed for {}: {}", path, e.what());
                
                // 即使失败也要调用回调
                {
                    std::lock_guard<std::mutex> lock(m_TaskMutex);
                    auto it = m_LoadTasks.find(taskID);
                    if (it != m_LoadTasks.end() && it->second.callback) {
                        it->second.callback(nullptr);
                    }
                }
                
                return nullptr;
            }
        });
        
        // 存储任务（使用移动语义）
        m_LoadTasks.emplace(taskID, std::move(task));
        
        ZG_CORE_INFO("Started async loading task {} for {}", taskID, path);
        return taskID;
    }

} // namespace Resources
} // namespace Zgine
