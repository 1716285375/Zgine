#include "zgpch.h"
#include "ShaderManager.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <fstream>

namespace Zgine {
namespace Resources {

    // ============================================================================
    // ShaderResource Implementation
    // ============================================================================

    ShaderResource::ShaderResource(const std::string& path, uint32_t id)
        : m_ID(id), m_Path(path), m_State(ResourceState::Unloaded) {
        m_Name = std::filesystem::path(path).stem().string();
        ZG_CORE_TRACE("ShaderResource created: {} (ID: {})", m_Name, m_ID);
    }

    ShaderResource::~ShaderResource() {
        Unload();
        ZG_CORE_TRACE("ShaderResource destroyed: {}", m_Name);
    }

    size_t ShaderResource::GetSize() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Shader ? m_Shader->GetSize() : 0;
    }

    void ShaderResource::AddRef() {
        m_RefCount++;
        ZG_CORE_TRACE("ShaderResource {} ref count: {}", m_Name, m_RefCount.load());
    }

    void ShaderResource::RemoveRef() {
        uint32_t count = m_RefCount--;
        ZG_CORE_TRACE("ShaderResource {} ref count: {}", m_Name, count - 1);
        
        if (count == 1) {
            ZG_CORE_INFO("ShaderResource {} has no more references, marking for cleanup", m_Name);
        }
    }

    bool ShaderResource::IsValid() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State != ResourceState::Error && m_Shader != nullptr;
    }

    bool ShaderResource::IsLoaded() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State == ResourceState::Loaded;
    }

    void ShaderResource::LoadAsync(std::function<void(bool)> callback) {
        std::thread([this, callback]() {
            bool success = LoadSync();
            if (callback) {
                callback(success);
            }
        }).detach();
    }

    bool ShaderResource::LoadSync() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Loaded) {
            return true;
        }
        
        SetState(ResourceState::Loading);
        
        try {
            std::string source = ReadShaderSource();
            if (source.empty()) {
                ZG_CORE_ERROR("Failed to read shader source: {}", m_Path);
                SetState(ResourceState::Error);
                return false;
            }
            
            // 创建着色器对象
            m_Shader = Zgine::Shader::Create(source); // 使用单个源码创建着色器
            if (!m_Shader) {
                ZG_CORE_ERROR("Failed to create shader: {}", m_Name);
                SetState(ResourceState::Error);
                return false;
            }
            
            // 编译着色器
            if (!CompileShader()) {
                ZG_CORE_ERROR("Failed to compile shader: {}", m_Name);
                SetState(ResourceState::Error);
                return false;
            }
            
            SetState(ResourceState::Loaded);
            ZG_CORE_INFO("Shader loaded successfully: {}", m_Name);
            return true;
            
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Failed to load shader {}: {}", m_Name, e.what());
            SetState(ResourceState::Error);
            return false;
        }
    }

    void ShaderResource::Unload() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Unloaded) {
            return;
        }
        
        m_Shader.reset();
        m_CompileError.clear();
        SetState(ResourceState::Unloaded);
        ZG_CORE_INFO("Shader unloaded: {}", m_Name);
    }

    bool ShaderResource::Reload() {
        Unload();
        return LoadSync();
    }

    std::string ShaderResource::GetMetadata(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Metadata.find(key);
        return it != m_Metadata.end() ? it->second : "";
    }

    void ShaderResource::SetMetadata(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Metadata[key] = value;
    }

    std::string ShaderResource::GetShaderType() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::string extension = std::filesystem::path(m_Path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".vert") return "vertex";
        if (extension == ".frag") return "fragment";
        if (extension == ".comp") return "compute";
        if (extension == ".geom") return "geometry";
        if (extension == ".tesc") return "tessellation_control";
        if (extension == ".tese") return "tessellation_evaluation";
        
        return "unknown";
    }

    bool ShaderResource::IsCompiled() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Shader != nullptr && m_State == ResourceState::Loaded;
    }

    void ShaderResource::SetState(ResourceState newState) {
        ResourceState oldState = m_State;
        m_State = newState;
        ZG_CORE_TRACE("ShaderResource {} state changed: {} -> {}", 
                     m_Name, static_cast<int>(oldState), static_cast<int>(newState));
    }

    bool ShaderResource::CompileShader() {
        if (!m_Shader) {
            return false;
        }
        
        // 这里应该调用实际的着色器编译逻辑
        // 由于Zgine::Shader的具体实现可能不同，这里简化处理
        return true;
    }

    std::string ShaderResource::ReadShaderSource() {
        std::ifstream file(m_Path);
        if (!file.is_open()) {
            ZG_CORE_ERROR("Failed to open shader file: {}", m_Path);
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return buffer.str();
    }

    // ============================================================================
    // ShaderManager Implementation
    // ============================================================================

    ShaderManager::ShaderManager() {
        ZG_CORE_INFO("ShaderManager created");
    }

    ShaderManager::~ShaderManager() {
        ZG_CORE_INFO("ShaderManager destroyed");
    }

    ResourceRef ShaderManager::LoadShader(const std::string& path, bool async) {
        if (path.empty()) {
            ZG_CORE_WARN("Attempted to load shader with empty path");
            return nullptr;
        }
        
        ResourceRef existing = GetResource(path);
        if (existing) {
            ZG_CORE_TRACE("Shader already loaded: {}", path);
            return existing;
        }
        
        if (!ValidateResourceFile(path, ResourceType::Shader)) {
            ZG_CORE_ERROR("Invalid shader file: {}", path);
            return nullptr;
        }
        
        if (async) {
            return LoadAsync(path, ResourceType::Shader);
        } else {
            return LoadSync(path, ResourceType::Shader);
        }
    }

    ResourceRef ShaderManager::CreateShader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& name) {
        std::string path = "generated://" + name;
        
        ResourceRef existing = GetResource(path);
        if (existing) {
            return existing;
        }
        
        ResourceRef resource = CreateResource(path, ResourceType::Shader);
        if (!resource) {
            return nullptr;
        }
        
        auto shaderResource = std::static_pointer_cast<ShaderResource>(resource);
        
        // 创建着色器对象
        shaderResource->m_Shader = Zgine::Shader::Create(vertexSrc, fragmentSrc);
        if (shaderResource->m_Shader) {
            shaderResource->SetState(ResourceState::Loaded);
        } else {
            shaderResource->SetState(ResourceState::Error);
        }
        
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        m_ResourceCache[path] = resource;
        
        ZG_CORE_INFO("Created shader resource: {}", name);
        return resource;
    }

    ResourceRef ShaderManager::GetShaderResource(const std::string& path) {
        return GetResource(path);
    }

    Ref<Zgine::Shader> ShaderManager::GetShader(const std::string& path) {
        ResourceRef resource = GetResource(path);
        if (!resource) {
            return nullptr;
        }
        
        auto shaderResource = std::static_pointer_cast<ShaderResource>(resource);
        return shaderResource->GetShader();
    }

    std::string ShaderManager::ExtractNameFromPath(const std::string& path) const {
        std::filesystem::path filePath(path);
        return filePath.stem().string();
    }

    bool ShaderManager::UnloadShader(const std::string& path) {
        return UnloadResource(path);
    }

    std::vector<ResourceRef> ShaderManager::GetAllShaders() const {
        return GetAllResources();
    }

    std::string ShaderManager::GetStatistics() const {
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
        ss << "ShaderManager Statistics:\n";
        ss << "  Total Shaders: " << m_ResourceCache.size() << "\n";
        ss << "  Loaded: " << loadedCount << "\n";
        ss << "  Errors: " << errorCount << "\n";
        ss << "  Total Size: " << (totalSize / 1024) << " KB";
        
        return ss.str();
    }

    uint32_t ShaderManager::CleanupUnusedShaders() {
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
            ZG_CORE_INFO("Cleaned up {} unused shaders", cleanedCount);
        }
        
        return cleanedCount;
    }

    void ShaderManager::Clear() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t count = static_cast<uint32_t>(m_ResourceCache.size());
        m_ResourceCache.clear();
        
        ZG_CORE_INFO("Cleared all shaders: {}", count);
    }

    ResourceRef ShaderManager::CreateResource(const std::string& path, ResourceType type) {
        uint32_t id = GenerateResourceID(path);
        return CreateScope<ShaderResource>(path, id);
    }

    bool ShaderManager::ValidateResourceFile(const std::string& path, ResourceType type) {
        if (path.find("generated://") == 0) {
            return true;
        }
        
        if (std::filesystem::exists(path)) {
            std::string extension = std::filesystem::path(path).extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            return extension == ".glsl" || extension == ".vert" || extension == ".frag" ||
                   extension == ".comp" || extension == ".geom" || extension == ".tesc" ||
                   extension == ".tese";
        }
        
        return false;
    }

} // namespace Resources
} // namespace Zgine