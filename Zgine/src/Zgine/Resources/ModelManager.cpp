#include "zgpch.h"
#include "ModelManager.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace Zgine {
namespace Resources {

    // ============================================================================
    // ModelResource Implementation
    // ============================================================================

    ModelResource::ModelResource(const std::string& path, uint32_t id)
        : m_ID(id), m_Path(path), m_State(ResourceState::Unloaded) {
        m_Name = std::filesystem::path(path).stem().string();
        ZG_CORE_TRACE("ModelResource created: {} (ID: {})", m_Name, m_ID);
    }

    ModelResource::~ModelResource() {
        Unload();
        ZG_CORE_TRACE("ModelResource destroyed: {}", m_Name);
    }

    size_t ModelResource::GetSize() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        size_t size = 0;
        
        // 计算顶点数据大小
        size += m_Vertices.size() * sizeof(float);
        
        // 计算索引数据大小
        size += m_Indices.size() * sizeof(uint32_t);
        
        // 计算纹理大小（简化计算）
        size += m_Textures.size() * 1024; // 假设每个纹理约1KB元数据
        
        return size;
    }

    void ModelResource::AddRef() {
        m_RefCount++;
        ZG_CORE_TRACE("ModelResource {} ref count: {}", m_Name, m_RefCount.load());
    }

    void ModelResource::RemoveRef() {
        uint32_t count = m_RefCount--;
        ZG_CORE_TRACE("ModelResource {} ref count: {}", m_Name, count - 1);
        
        if (count == 1) {
            ZG_CORE_INFO("ModelResource {} has no more references, marking for cleanup", m_Name);
        }
    }

    bool ModelResource::IsValid() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State != ResourceState::Error && !m_Vertices.empty();
    }

    bool ModelResource::IsLoaded() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State == ResourceState::Loaded;
    }

    void ModelResource::LoadAsync(std::function<void(bool)> callback) {
        std::thread([this, callback]() {
            bool success = LoadSync();
            if (callback) {
                callback(success);
            }
        }).detach();
    }

    bool ModelResource::LoadSync() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Loaded) {
            return true;
        }
        
        SetState(ResourceState::Loading);
        
        try {
            // 简化的模型加载逻辑
            if (m_Path.find("cube") != std::string::npos) {
                // 创建立方体顶点数据
                m_Vertices = {
                    // 前面
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                    // 后面
                    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f
                };
                
                m_Indices = {
                    0, 1, 2,  2, 3, 0,  // 前面
                    4, 5, 6,  6, 7, 4,  // 后面
                    8, 9, 10, 10, 11, 8, // 左面
                    12, 13, 14, 14, 15, 12, // 右面
                    16, 17, 18, 18, 19, 16, // 上面
                    20, 21, 22, 22, 23, 20  // 下面
                };
            } else if (m_Path.find("sphere") != std::string::npos) {
                // 创建球体顶点数据（简化版本）
                const int segments = 32;
                const int rings = 16;
                
                for (int ring = 0; ring <= rings; ++ring) {
                    float v = (float)ring / rings;
                    float phi = v * glm::pi<float>();
                    
                    for (int seg = 0; seg <= segments; ++seg) {
                        float u = (float)seg / segments;
                        float theta = u * 2.0f * glm::pi<float>();
                        
                        float x = cos(theta) * sin(phi);
                        float y = cos(phi);
                        float z = sin(theta) * sin(phi);
                        
                        m_Vertices.push_back(x * 0.5f);
                        m_Vertices.push_back(y * 0.5f);
                        m_Vertices.push_back(z * 0.5f);
                        m_Vertices.push_back(u);
                        m_Vertices.push_back(v);
                    }
                }
                
                // 生成索引
                for (int ring = 0; ring < rings; ++ring) {
                    for (int seg = 0; seg < segments; ++seg) {
                        int current = ring * (segments + 1) + seg;
                        int next = current + segments + 1;
                        
                        m_Indices.push_back(current);
                        m_Indices.push_back(next);
                        m_Indices.push_back(current + 1);
                        
                        m_Indices.push_back(current + 1);
                        m_Indices.push_back(next);
                        m_Indices.push_back(next + 1);
                    }
                }
            } else if (m_Path.find("plane") != std::string::npos) {
                // 创建平面顶点数据
                m_Vertices = {
                    -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
                     0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
                     0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
                    -0.5f, 0.0f,  0.5f,  0.0f, 1.0f
                };
                
                m_Indices = {
                    0, 1, 2,  2, 3, 0
                };
            }
            
            SetState(ResourceState::Loaded);
            ZG_CORE_INFO("Model loaded successfully: {}", m_Name);
            return true;
            
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Failed to load model {}: {}", m_Name, e.what());
            SetState(ResourceState::Error);
            return false;
        }
    }

    void ModelResource::Unload() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Unloaded) {
            return;
        }
        
        m_Vertices.clear();
        m_Indices.clear();
        m_Textures.clear();
        
        SetState(ResourceState::Unloaded);
        ZG_CORE_INFO("Model unloaded: {}", m_Name);
    }

    bool ModelResource::Reload() {
        Unload();
        return LoadSync();
    }

    std::string ModelResource::GetMetadata(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Metadata.find(key);
        return it != m_Metadata.end() ? it->second : "";
    }

    void ModelResource::SetMetadata(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Metadata[key] = value;
    }

    uint32_t ModelResource::GetMeshCount() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Indices.empty() ? 0 : 1; // 简化：每个模型只有一个网格
    }

    uint32_t ModelResource::GetMaterialCount() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return 1; // 简化：每个模型只有一个材质
    }

    uint32_t ModelResource::GetTextureCount() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return static_cast<uint32_t>(m_Textures.size());
    }

    void ModelResource::SetState(ResourceState newState) {
        ResourceState oldState = m_State;
        m_State = newState;
        ZG_CORE_TRACE("ModelResource {} state changed: {} -> {}", 
                     m_Name, static_cast<int>(oldState), static_cast<int>(newState));
    }

    // ============================================================================
    // ModelManager Implementation
    // ============================================================================

    ModelManager::ModelManager() {
        ZG_CORE_INFO("ModelManager initialized");
    }

    ModelManager::~ModelManager() {
        ZG_CORE_INFO("ModelManager destroyed");
    }

    ResourceRef ModelManager::LoadModel(const std::string& path, bool async) {
        if (path.empty()) {
            ZG_CORE_WARN("Attempted to load model with empty path");
            return nullptr;
        }
        
        // 检查是否已加载
        ResourceRef existing = GetResource(path);
        if (existing) {
            ZG_CORE_TRACE("Model already loaded: {}", path);
            return existing;
        }
        
        // 验证文件
        if (!ValidateResourceFile(path, ResourceType::Model)) {
            ZG_CORE_ERROR("Invalid model file: {}", path);
            return nullptr;
        }
        
        if (async) {
            return LoadAsync(path, ResourceType::Model);
        } else {
            return LoadSync(path, ResourceType::Model);
        }
    }

    ResourceRef ModelManager::CreatePrimitiveModel(const std::string& type, const std::string& name) {
        std::string resourceName = name.empty() ? "Primitive_" + type : name;
        std::string path = "primitive://" + resourceName;
        
        // 检查是否已存在
        ResourceRef existing = GetResource(path);
        if (existing) {
            return existing;
        }
        
        ResourceRef resource = CreateResource(path, ResourceType::Model);
        if (!resource) {
            return nullptr;
        }
        
        auto modelResource = std::static_pointer_cast<ModelResource>(resource);
        
        // 根据类型创建不同的几何体
        if (type == "cube") {
            return CreateCubeModel(resourceName);
        } else if (type == "sphere") {
            return CreateSphereModel(resourceName);
        } else if (type == "plane") {
            return CreatePlaneModel(resourceName);
        } else {
            ZG_CORE_WARN("Unknown primitive type: {}", type);
            return nullptr;
        }
    }

    ResourceRef ModelManager::GetModel(const std::string& path) {
        return GetResource(path);
    }

    bool ModelManager::UnloadModel(const std::string& path) {
        return UnloadResource(path);
    }

    std::vector<ResourceRef> ModelManager::GetAllModels() const {
        return GetAllResources();
    }

    std::string ModelManager::GetStatistics() const {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        size_t totalSize = 0;
        uint32_t loadedCount = 0;
        uint32_t errorCount = 0;
        
        for (const auto& [path, resource] : m_ResourceCache) {
            totalSize += resource->GetSize();
            if (resource->IsLoaded()) {
                loadedCount++;
            } else if (resource->GetState() == ResourceState::Error) {
                errorCount++;
            }
        }
        
        std::stringstream ss;
        ss << "ModelManager Statistics:\n";
        ss << "  Total Models: " << m_ResourceCache.size() << "\n";
        ss << "  Loaded: " << loadedCount << "\n";
        ss << "  Errors: " << errorCount << "\n";
        ss << "  Total Size: " << (totalSize / 1024) << " KB";
        
        return ss.str();
    }

    uint32_t ModelManager::CleanupUnusedModels() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t cleanedCount = 0;
        auto it = m_ResourceCache.begin();
        
        while (it != m_ResourceCache.end()) {
            if (it->second->GetRefCount() == 0) {
                it->second->Unload();
                it = m_ResourceCache.erase(it);
                cleanedCount++;
            } else {
                ++it;
            }
        }
        
        if (cleanedCount > 0) {
            ZG_CORE_INFO("Cleaned up {} unused models", cleanedCount);
        }
        
        return cleanedCount;
    }

    ResourceRef ModelManager::CreateResource(const std::string& path, ResourceType type) {
        uint32_t id = GenerateResourceID(path);
        return CreateScope<ModelResource>(path, id);
    }

    bool ModelManager::ValidateResourceFile(const std::string& path, ResourceType type) {
        // 对于原始几何体，总是有效的
        if (path.find("primitive://") == 0) {
            return true;
        }
        
        // 对于文件路径，检查文件是否存在
        if (std::filesystem::exists(path)) {
            std::string extension = std::filesystem::path(path).extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            return extension == ".obj" || extension == ".fbx" || extension == ".gltf";
        }
        
        return false;
    }

    ResourceRef ModelManager::LoadOBJFile(const std::string& path) {
        // 简化的OBJ文件加载（实际项目中需要更复杂的解析）
        ZG_CORE_INFO("Loading OBJ file: {}", path);
        
        ResourceRef resource = CreateResource(path, ResourceType::Model);
        if (!resource) {
            return nullptr;
        }
        
        auto modelResource = std::static_pointer_cast<ModelResource>(resource);
        
        // 这里应该实现真正的OBJ文件解析
        // 现在只是创建一个简单的立方体作为占位符
        return CreateCubeModel(std::filesystem::path(path).stem().string());
    }

    ResourceRef ModelManager::CreateCubeModel(const std::string& name) {
        std::string path = "primitive://cube_" + name;
        
        ResourceRef resource = CreateResource(path, ResourceType::Model);
        if (!resource) {
            return nullptr;
        }
        
        auto modelResource = std::static_pointer_cast<ModelResource>(resource);
        modelResource->m_Path = path;
        modelResource->m_Name = name;
        
        if (modelResource->LoadSync()) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
            return resource;
        }
        
        return nullptr;
    }

    ResourceRef ModelManager::CreateSphereModel(const std::string& name) {
        std::string path = "primitive://sphere_" + name;
        
        ResourceRef resource = CreateResource(path, ResourceType::Model);
        if (!resource) {
            return nullptr;
        }
        
        auto modelResource = std::static_pointer_cast<ModelResource>(resource);
        modelResource->m_Path = path;
        modelResource->m_Name = name;
        
        if (modelResource->LoadSync()) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
            return resource;
        }
        
        return nullptr;
    }

    ResourceRef ModelManager::CreatePlaneModel(const std::string& name) {
        std::string path = "primitive://plane_" + name;
        
        ResourceRef resource = CreateResource(path, ResourceType::Model);
        if (!resource) {
            return nullptr;
        }
        
        auto modelResource = std::static_pointer_cast<ModelResource>(resource);
        modelResource->m_Path = path;
        modelResource->m_Name = name;
        
        if (modelResource->LoadSync()) {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
            return resource;
        }
        
        return nullptr;
    }

    std::string ModelManager::ExtractNameFromPath(const std::string& path) const {
        std::filesystem::path filePath(path);
        return filePath.stem().string();
    }

    void ModelManager::Clear() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t count = static_cast<uint32_t>(m_ResourceCache.size());
        m_ResourceCache.clear();
        
        ZG_CORE_INFO("Cleared all models: {}", count);
    }

} // namespace Resources
} // namespace Zgine
