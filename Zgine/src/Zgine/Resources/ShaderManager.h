#pragma once

#include "ResourceManager.h"
#include "Zgine/Renderer/Shader.h"
#include <unordered_map>
#include <vector>

namespace Zgine {
namespace Resources {

    /**
     * @brief 着色器资源类
     */
    class ShaderResource : public IResource {
        friend class ShaderManager;
    public:
        ShaderResource(const std::string& path, uint32_t id);
        virtual ~ShaderResource();

        // IResource interface
        uint32_t GetID() const override { return m_ID; }
        const std::string& GetName() const override { return m_Name; }
        const std::string& GetPath() const override { return m_Path; }
        ResourceType GetType() const override { return ResourceType::Shader; }
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
         * @brief 获取着色器对象
         * @return 着色器引用
         */
        Ref<Zgine::Shader> GetShader() const { return m_Shader; }

        /**
         * @brief 获取着色器类型
         * @return 着色器类型字符串
         */
        std::string GetShaderType() const;

        /**
         * @brief 检查着色器是否编译成功
         * @return true如果编译成功，false否则
         */
        bool IsCompiled() const;

        /**
         * @brief 获取编译错误信息
         * @return 编译错误信息
         */
        const std::string& GetCompileError() const { return m_CompileError; }

    private:
        void SetState(ResourceState newState);
        bool CompileShader();
        std::string ReadShaderSource();

        uint32_t m_ID;
        std::string m_Name;
        std::string m_Path;
        ResourceState m_State;
        std::atomic<uint32_t> m_RefCount{0};
        
        Ref<Zgine::Shader> m_Shader;
        std::string m_CompileError;
        std::unordered_map<std::string, std::string> m_Metadata;
        
        mutable std::mutex m_Mutex;
    };

    /**
     * @brief 着色器管理器
     * @details 管理所有着色器资源的加载、编译和缓存
     */
    class ShaderManager : public ResourceManager {
    public:
        ShaderManager();
        virtual ~ShaderManager();

        /**
         * @brief 加载着色器
         * @param path 着色器文件路径
         * @param async 是否异步加载
         * @return 着色器资源引用
         */
        ResourceRef LoadShader(const std::string& path, bool async = true);

        /**
         * @brief 创建着色器
         * @param vertexSrc 顶点着色器源码
         * @param fragmentSrc 片段着色器源码
         * @param name 着色器名称
         * @return 着色器资源引用
         */
        ResourceRef CreateShader(const std::string& vertexSrc, 
                               const std::string& fragmentSrc,
                               const std::string& name);

        /**
         * @brief 创建着色器
         * @param computeSrc 计算着色器源码
         * @param name 着色器名称
         * @return 着色器资源引用
         */
        ResourceRef CreateComputeShader(const std::string& computeSrc,
                                      const std::string& name);

        /**
         * @brief 获取着色器资源
         * @param path 着色器路径
         * @return 着色器资源引用
         */
        ResourceRef GetShaderResource(const std::string& path);

        /**
         * @brief 获取着色器对象
         * @param path 着色器路径
         * @return 着色器引用
         */
        Ref<Zgine::Shader> GetShader(const std::string& path);

        /**
         * @brief 获取默认着色器
         * @return 默认着色器引用
         */
        Ref<Zgine::Shader> GetDefaultShader();

        /**
         * @brief 获取错误着色器
         * @return 错误着色器引用
         */
        Ref<Zgine::Shader> GetErrorShader();

        /**
         * @brief 预编译常用着色器
         */
        void PrecompileCommonShaders();

        /**
         * @brief 重新编译所有着色器
         * @return 成功编译的着色器数量
         */
        size_t RecompileAllShaders();

        /**
         * @brief 获取着色器统计信息
         * @return 着色器统计信息
         */
        std::string GetShaderStats() const;

        /**
         * @brief 检查着色器文件是否有效
         * @param path 着色器文件路径
         * @return true如果有效，false否则
         */
        bool ValidateShaderFile(const std::string& path);

        /**
         * @brief 获取着色器依赖
         * @param path 着色器路径
         * @return 依赖的着色器路径列表
         */
        std::vector<std::string> GetShaderDependencies(const std::string& path);

        /**
         * @brief 卸载着色器
         * @param path 着色器路径
         * @return true如果卸载成功
         */
        bool UnloadShader(const std::string& path);

        /**
         * @brief 获取所有已加载的着色器
         * @return 着色器资源列表
         */
        std::vector<ResourceRef> GetAllShaders() const;

        /**
         * @brief 获取着色器统计信息
         * @return 统计信息字符串
         */
        std::string GetStatistics() const;

        /**
         * @brief 清理未使用的着色器
         * @return 清理的着色器数量
         */
        uint32_t CleanupUnusedShaders();

        /**
         * @brief 清空所有着色器
         */
        void Clear();

    protected:
        ResourceRef CreateResource(const std::string& path, ResourceType type) override;
        bool ValidateResourceFile(const std::string& path, ResourceType type) override;

    private:
        void CreateDefaultShaders();
        std::string ExtractNameFromPath(const std::string& path) const;
        std::string GetShaderSource(const std::string& path);
        std::vector<std::string> ParseIncludes(const std::string& source, const std::string& basePath);
        
        Ref<Zgine::Shader> m_DefaultShader;
        Ref<Zgine::Shader> m_ErrorShader;
        
        std::atomic<size_t> m_CompiledShaders{0};
        std::atomic<size_t> m_FailedCompilations{0};
    };

} // namespace Resources
} // namespace Zgine
