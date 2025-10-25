#include "zgpch.h"
#include "TextureManager.h"
#include "Zgine/Renderer/Renderer.h"
#include "Zgine/Logging/Log.h"
#include <filesystem>
#include <algorithm>

namespace Zgine {
namespace Resources {

    // TextureResource Implementation
    TextureResource::TextureResource(const std::string& path, uint32_t id)
        : m_ID(id), m_Path(path), m_State(ResourceState::Unloaded) {
        
        // 从路径提取名称
        std::filesystem::path filePath(path);
        m_Name = filePath.stem().string();
        
        ZG_CORE_TRACE("Created TextureResource: {} (ID: {})", m_Name, m_ID);
    }

    TextureResource::~TextureResource() {
        if (m_Texture) {
            Unload();
        }
    }

    size_t TextureResource::GetSize() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Texture) {
            // 估算纹理内存大小
            uint32_t width = m_Texture->GetWidth();
            uint32_t height = m_Texture->GetHeight();
            uint32_t channels = 4; // 假设RGBA
            
            return width * height * channels;
        }
        return 0;
    }

    void TextureResource::AddRef() {
        m_RefCount++;
        ZG_CORE_TRACE("TextureResource {} ref count: {}", m_Name, m_RefCount.load());
    }

    void TextureResource::RemoveRef() {
        uint32_t count = m_RefCount--;
        ZG_CORE_TRACE("TextureResource {} ref count: {}", m_Name, count - 1);
    }

    bool TextureResource::IsValid() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_State == ResourceState::Loaded && m_Texture != nullptr;
    }

    bool TextureResource::IsLoaded() const {
        return m_State == ResourceState::Loaded;
    }

    void TextureResource::LoadAsync(std::function<void(bool)> callback) {
        // 对于纹理，异步加载实际上就是同步加载
        // 因为纹理加载通常很快
        bool success = LoadSync();
        if (callback) {
            callback(success);
        }
    }

    bool TextureResource::LoadSync() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Loaded) {
            return true;
        }
        
        SetState(ResourceState::Loading);
        
        try {
            m_Texture = Zgine::Texture2D::Create(m_Path);
            if (m_Texture) {
                SetState(ResourceState::Loaded);
                ZG_CORE_INFO("Successfully loaded texture: {}", m_Path);
                return true;
            } else {
                SetState(ResourceState::Failed);
                ZG_CORE_ERROR("Failed to create texture from: {}", m_Path);
                return false;
            }
        } catch (const std::exception& e) {
            SetState(ResourceState::Failed);
            ZG_CORE_ERROR("Exception while loading texture {}: {}", m_Path, e.what());
            return false;
        }
    }

    void TextureResource::Unload() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_State == ResourceState::Loaded) {
            SetState(ResourceState::Unloading);
            m_Texture.reset();
            SetState(ResourceState::Unloaded);
            ZG_CORE_TRACE("Unloaded texture: {}", m_Path);
        }
    }

    bool TextureResource::Reload() {
        Unload();
        return LoadSync();
    }

    std::string TextureResource::GetMetadata(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto it = m_Metadata.find(key);
        return it != m_Metadata.end() ? it->second : "";
    }

    void TextureResource::SetMetadata(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Metadata[key] = value;
    }

    uint32_t TextureResource::GetWidth() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Texture ? m_Texture->GetWidth() : 0;
    }

    uint32_t TextureResource::GetHeight() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Texture ? m_Texture->GetHeight() : 0;
    }

    uint32_t TextureResource::GetFormat() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        // 返回一个默认格式，因为Texture类没有GetFormat方法
        return 0; // GL_RGBA
    }

    void TextureResource::SetState(ResourceState newState) {
        ResourceState oldState = m_State;
        m_State = newState;
        ZG_CORE_TRACE("TextureResource {} state changed: {} -> {}", 
                     m_Name, (int)oldState, (int)newState);
    }

    // TextureManager Implementation
    TextureManager::TextureManager() {
        ZG_CORE_INFO("TextureManager created");
        
        // 初始化支持的格式
        m_SupportedFormats = {
            ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds", ".hdr", ".ktx"
        };
    }

    ResourceRef TextureManager::LoadTexture(const std::string& path, bool async) {
        if (async) {
            return LoadAsync(path, ResourceType::Texture);
        } else {
            return LoadSync(path, ResourceType::Texture);
        }
    }

    ResourceRef TextureManager::CreateTexture(uint32_t width, uint32_t height, 
                                            const void* data,
                                            const std::string& name) {
        std::string resourceName = name.empty() ? "GeneratedTexture" : name;
        std::string path = "generated://" + resourceName;
        
        // 检查是否已存在
        ResourceRef existing = GetResource(path);
        if (existing) {
            return existing;
        }
        
        // 创建资源
        ResourceRef resource = CreateResource(path, ResourceType::Texture);
        if (!resource) {
            return nullptr;
        }
        
        // 创建纹理
        auto textureResource = std::static_pointer_cast<TextureResource>(resource);
        textureResource->m_Texture = Zgine::Texture2D::Create(width, height);
        
        if (textureResource->m_Texture) {
            if (data) {
                textureResource->m_Texture->SetData(const_cast<void*>(data), width * height * 4);
            }
            textureResource->SetState(ResourceState::Loaded);
            
            // 添加到缓存
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_ResourceCache[path] = resource;
            
            ZG_CORE_INFO("Created texture: {} ({}x{})", resourceName, width, height);
            return resource;
        } else {
            textureResource->SetState(ResourceState::Failed);
            ZG_CORE_ERROR("Failed to create texture: {}", resourceName);
            return nullptr;
        }
    }

    ResourceRef TextureManager::GetTextureResource(const std::string& path) {
        return GetResource(path);
    }

    Ref<Zgine::Texture2D> TextureManager::GetTexture(const std::string& path) {
        ResourceRef resource = GetResource(path);
        if (resource) {
            auto textureResource = std::static_pointer_cast<TextureResource>(resource);
            return textureResource->GetTexture();
        }
        return nullptr;
    }

    Ref<Zgine::Texture2D> TextureManager::GetDefaultTexture() {
        if (!m_DefaultTexture) {
            CreateDefaultTextures();
        }
        return m_DefaultTexture;
    }

    Ref<Zgine::Texture2D> TextureManager::GetErrorTexture() {
        if (!m_ErrorTexture) {
            CreateDefaultTextures();
        }
        return m_ErrorTexture;
    }

    void TextureManager::PreloadCommonTextures() {
        ZG_CORE_INFO("Preloading common textures...");
        
        // 这里可以预加载一些常用纹理
        // 比如UI纹理、默认材质纹理等
        
        ZG_CORE_INFO("Common textures preloaded");
    }

    std::vector<std::string> TextureManager::GetSupportedFormats() const {
        return m_SupportedFormats;
    }

    bool TextureManager::IsFormatSupported(const std::string& extension) const {
        std::string lowerExt = extension;
        std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
        
        return std::find(m_SupportedFormats.begin(), m_SupportedFormats.end(), lowerExt) 
               != m_SupportedFormats.end();
    }

    ResourceRef TextureManager::CreateResource(const std::string& path, ResourceType type) {
        if (type != ResourceType::Texture) {
            ZG_CORE_ERROR("Invalid resource type for TextureManager");
            return nullptr;
        }
        
        uint32_t id = GenerateResourceID(path);
        return CreateScope<TextureResource>(path, id);
    }

    void TextureManager::CreateDefaultTextures() {
        // 创建默认纹理（白色1x1像素）
        uint32_t whitePixel = 0xFFFFFFFF;
        m_DefaultTexture = Zgine::Texture2D::Create(1, 1);
        if (m_DefaultTexture) {
            m_DefaultTexture->SetData(&whitePixel, 4);
        }
        
        // 创建错误纹理（紫色棋盘格）
        uint32_t errorPixels[4] = {
            0xFF00FFFF, 0xFFFF00FF,  // 紫黄
            0xFFFF00FF, 0xFF00FFFF   // 黄紫
        };
        m_ErrorTexture = Zgine::Texture2D::Create(2, 2);
        if (m_ErrorTexture) {
            m_ErrorTexture->SetData(errorPixels, 16);
        }
        
        ZG_CORE_INFO("Created default textures");
    }

    std::string TextureManager::ExtractNameFromPath(const std::string& path) const {
        std::filesystem::path filePath(path);
        return filePath.stem().string();
    }

    bool TextureManager::UnloadTexture(const std::string& path) {
        return UnloadResource(path);
    }

    std::vector<ResourceRef> TextureManager::GetAllTextures() const {
        return GetAllResources();
    }

    std::string TextureManager::GetStatistics() const {
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
        ss << "TextureManager Statistics:\n";
        ss << "  Total Textures: " << m_ResourceCache.size() << "\n";
        ss << "  Loaded: " << loadedCount << "\n";
        ss << "  Errors: " << errorCount << "\n";
        ss << "  Total Size: " << (totalSize / 1024) << " KB";
        
        return ss.str();
    }

    uint32_t TextureManager::CleanupUnusedTextures() {
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
            ZG_CORE_INFO("Cleaned up {} unused textures", cleanedCount);
        }
        
        return cleanedCount;
    }

    void TextureManager::Clear() {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        
        uint32_t count = static_cast<uint32_t>(m_ResourceCache.size());
        m_ResourceCache.clear();
        
        ZG_CORE_INFO("Cleared all textures: {}", count);
    }

} // namespace Resources
} // namespace Zgine
