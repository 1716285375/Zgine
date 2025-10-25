#pragma once

#include "ResourceManager.h"
#include "Zgine/Renderer/Texture.h"
#include <unordered_map>

namespace Zgine {
namespace Resources {

    /**
     * @brief 模型资源类
     * @details 管理3D模型资源，包括网格、材质、纹理等
     */
    class ModelResource : public IResource {
        friend class ModelManager;
    public:
        ModelResource(const std::string& path, uint32_t id);
        virtual ~ModelResource();

        // IResource interface
        uint32_t GetID() const override { return m_ID; }
        const std::string& GetName() const override { return m_Name; }
        const std::string& GetPath() const override { return m_Path; }
        ResourceType GetType() const override { return ResourceType::Model; }
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
         * @brief 获取网格数量
         * @return 网格数量
         */
        uint32_t GetMeshCount() const;

        /**
         * @brief 获取材质数量
         * @return 材质数量
         */
        uint32_t GetMaterialCount() const;

        /**
         * @brief 获取纹理数量
         * @return 纹理数量
         */
        uint32_t GetTextureCount() const;

        /**
         * @brief 设置资源状态（供ModelManager使用）
         * @param newState 新状态
         */
        void SetState(ResourceState newState);

    private:
        uint32_t m_ID;
        std::string m_Name;
        std::string m_Path;
        ResourceState m_State;
        std::atomic<uint32_t> m_RefCount{0};
        
        // 模型数据（简化版本，实际项目中需要更复杂的网格数据结构）
        std::vector<float> m_Vertices;
        std::vector<uint32_t> m_Indices;
        std::vector<Ref<Texture2D>> m_Textures;
        
        std::unordered_map<std::string, std::string> m_Metadata;
        
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief 模型管理器
     * @details 管理所有3D模型资源的加载、缓存和生命周期
     */
    class ModelManager : public ResourceManager {
    public:
        ModelManager();
        virtual ~ModelManager();

        /**
         * @brief 加载模型
         * @param path 模型文件路径
         * @param async 是否异步加载
         * @return 模型资源引用
         */
        ResourceRef LoadModel(const std::string& path, bool async = true);

        /**
         * @brief 创建简单几何体模型
         * @param type 几何体类型 ("cube", "sphere", "plane")
         * @param name 模型名称
         * @return 模型资源引用
         */
        ResourceRef CreatePrimitiveModel(const std::string& type, const std::string& name = "");

        /**
         * @brief 获取模型资源
         * @param path 模型路径
         * @return 模型资源引用
         */
        ResourceRef GetModel(const std::string& path);

        /**
         * @brief 卸载模型
         * @param path 模型路径
         * @return true如果卸载成功
         */
        bool UnloadModel(const std::string& path);

        /**
         * @brief 获取所有已加载的模型
         * @return 模型资源列表
         */
        std::vector<ResourceRef> GetAllModels() const;

        /**
         * @brief 获取模型统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 清理未使用的模型
         * @return 清理的模型数量
         */
        uint32_t CleanupUnusedModels();

        /**
         * @brief 清空所有模型
         */
        void Clear();

    protected:
        /**
         * @brief 创建模型资源
         * @param path 模型路径
         * @param type 资源类型
         * @return 模型资源引用
         */
        virtual ResourceRef CreateResource(const std::string& path, ResourceType type) override;

        /**
         * @brief 验证模型文件
         * @param path 文件路径
         * @param type 资源类型
         * @return true如果文件有效
         */
        virtual bool ValidateResourceFile(const std::string& path, ResourceType type) override;

    private:
        /**
         * @brief 加载OBJ文件
         * @param path OBJ文件路径
         * @return 模型资源引用
         */
        ResourceRef LoadOBJFile(const std::string& path);

        /**
         * @brief 创建立方体模型
         * @param name 模型名称
         * @return 模型资源引用
         */
        ResourceRef CreateCubeModel(const std::string& name);

        /**
         * @brief 创建球体模型
         * @param name 模型名称
         * @return 模型资源引用
         */
        ResourceRef CreateSphereModel(const std::string& name);

        /**
         * @brief 创建平面模型
         * @param name 模型名称
         * @return 模型资源引用
         */
        ResourceRef CreatePlaneModel(const std::string& name);

        /**
         * @brief 从路径提取模型名称
         * @param path 文件路径
         * @return 模型名称
         */
        std::string ExtractNameFromPath(const std::string& path) const;

        // 默认模型
        ResourceRef m_DefaultCube;
        ResourceRef m_DefaultSphere;
        ResourceRef m_DefaultPlane;
    };

} // namespace Resources
} // namespace Zgine
