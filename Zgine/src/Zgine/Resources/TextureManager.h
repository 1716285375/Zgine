#pragma once

#include "ResourceManager.h"
#include "Zgine/Renderer/Texture.h"
#include <unordered_map>

namespace Zgine {
namespace Resources {

    /**
     * @brief 纹理资源类
     */
    class TextureResource : public IResource {
        friend class TextureManager;
    public:
        TextureResource(const std::string& path, uint32_t id);
        virtual ~TextureResource();

        // IResource interface
        uint32_t GetID() const override { return m_ID; }
        const std::string& GetName() const override { return m_Name; }
        const std::string& GetPath() const override { return m_Path; }
        ResourceType GetType() const override { return ResourceType::Texture; }
        ResourceState GetState() const override { return m_State; }
        size_t GetSize() const override;
        uint32_t GetRefCount() const override { return m_RefCount; }
        void AddRef() override;
        void RemoveRef() override;
        bool IsValid() const override;
        bool IsLoaded() const override;
        void LoadAsync(std::function<void(bool)> callback = nullptr) override;
        bool LoadSync() override;
        void Unload() override;
        bool Reload() override;
        std::string GetMetadata(const std::string& key) const override;
        void SetMetadata(const std::string& key, const std::string& value) override;

        /**
         * @brief 获取纹理对象
         * @return 纹理引用
         */
        Ref<Zgine::Texture2D> GetTexture() const { return m_Texture; }

        /**
         * @brief 获取纹理宽度
         * @return 纹理宽度
         */
        uint32_t GetWidth() const;

        /**
         * @brief 获取纹理高度
         * @return 纹理高度
         */
        uint32_t GetHeight() const;

        /**
         * @brief 获取纹理格式
         * @return 纹理格式
         */
        uint32_t GetFormat() const;

        /**
         * @brief 设置资源状态（供TextureManager使用）
         * @param newState 新状态
         */
        void SetState(ResourceState newState);

    private:
        uint32_t m_ID;
        std::string m_Name;
        std::string m_Path;
        ResourceState m_State;
        std::atomic<uint32_t> m_RefCount{0};
        
        Ref<Zgine::Texture2D> m_Texture;
        std::unordered_map<std::string, std::string> m_Metadata;
        
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief 纹理管理器
     * @details 管理所有纹理资源的加载、缓存和生命周期
     */
    class TextureManager : public ResourceManager {
    public:
        TextureManager();
        virtual ~TextureManager() = default;

        /**
         * @brief 加载纹理
         * @param path 纹理文件路径
         * @param async 是否异步加载
         * @return 纹理资源引用
         */
        ResourceRef LoadTexture(const std::string& path, bool async = true);

        /**
         * @brief 创建纹理
         * @param width 纹理宽度
         * @param height 纹理高度
         * @param data 纹理数据
         * @param name 纹理名称
         * @return 纹理资源引用
         */
        ResourceRef CreateTexture(uint32_t width, uint32_t height, 
                                const void* data = nullptr,
                                const std::string& name = "");

        /**
         * @brief 获取纹理资源
         * @param path 纹理路径
         * @return 纹理资源引用
         */
        ResourceRef GetTextureResource(const std::string& path);

        /**
         * @brief 获取纹理对象
         * @param path 纹理路径
         * @return 纹理引用
         */
        Ref<Zgine::Texture2D> GetTexture(const std::string& path);

        /**
         * @brief 获取默认纹理
         * @return 默认纹理引用
         */
        Ref<Zgine::Texture2D> GetDefaultTexture();

        /**
         * @brief 获取错误纹理
         * @return 错误纹理引用
         */
        Ref<Zgine::Texture2D> GetErrorTexture();

        /**
         * @brief 预加载常用纹理
         */
        void PreloadCommonTextures();

        /**
         * @brief 获取支持的纹理格式
         * @return 支持的格式列表
         */
        std::vector<std::string> GetSupportedFormats() const;

        /**
         * @brief 检查格式是否支持
         * @param extension 文件扩展名
         * @return true如果支持，false否则
         */
        bool IsFormatSupported(const std::string& extension) const;

        /**
         * @brief 卸载纹理
         * @param path 纹理路径
         * @return true如果卸载成功
         */
        bool UnloadTexture(const std::string& path);

        /**
         * @brief 获取所有已加载的纹理
         * @return 纹理资源列表
         */
        std::vector<ResourceRef> GetAllTextures() const;

        /**
         * @brief 获取纹理统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 清理未使用的纹理
         * @return 清理的纹理数量
         */
        uint32_t CleanupUnusedTextures();

        /**
         * @brief 清空所有纹理
         */
        void Clear();

    protected:
        ResourceRef CreateResource(const std::string& path, ResourceType type) override;

    private:
        void CreateDefaultTextures();
        std::string ExtractNameFromPath(const std::string& path) const;
        
        Ref<Zgine::Texture2D> m_DefaultTexture;
        Ref<Zgine::Texture2D> m_ErrorTexture;
        
        std::vector<std::string> m_SupportedFormats;
    };

} // namespace Resources
} // namespace Zgine
