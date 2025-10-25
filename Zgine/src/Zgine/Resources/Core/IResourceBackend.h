#pragma once

#include "zgpch.h"
#include "../ResourceTypes.h"
#include "../IResource.h"
#include "Zgine/Core/SmartPointers.h"
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Zgine {
namespace Resources {
namespace Core {

    /**
     * @brief 资源加载回调函数类型
     * @param resource 加载的资源
     * @param success 是否成功
     */
    using ResourceLoadCallback = std::function<void(ResourceRef resource, bool success)>;

    /**
     * @brief 资源后端抽象接口
     * 定义资源加载的具体实现接口，支持多种后端实现
     */
    class IResourceBackend {
    public:
        virtual ~IResourceBackend() = default;

        /**
         * @brief 获取后端名称
         * @return 后端名称
         */
        virtual std::string GetBackendName() const = 0;

        /**
         * @brief 获取后端版本
         * @return 后端版本字符串
         */
        virtual std::string GetBackendVersion() const = 0;

        /**
         * @brief 检查是否支持指定的资源类型
         * @param type 资源类型
         * @return true如果支持，false否则
         */
        virtual bool SupportsResourceType(ResourceType type) const = 0;

        /**
         * @brief 检查是否支持指定的文件格式
         * @param extension 文件扩展名
         * @return true如果支持，false否则
         */
        virtual bool SupportsFileFormat(const std::string& extension) const = 0;

        /**
         * @brief 初始化后端
         * @return true如果初始化成功，false否则
         */
        virtual bool Initialize() = 0;

        /**
         * @brief 关闭后端
         */
        virtual void Shutdown() = 0;

        /**
         * @brief 同步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param config 加载配置
         * @return 资源引用，如果加载失败返回nullptr
         */
        virtual ResourceRef LoadSync(const std::string& path, ResourceType type, 
                                   const ResourceLoadConfig& config = {}) = 0;

        /**
         * @brief 异步加载资源
         * @param path 资源路径
         * @param type 资源类型
         * @param config 加载配置
         * @param callback 加载完成回调
         * @return 资源引用，如果资源已存在则直接返回
         */
        virtual ResourceRef LoadAsync(const std::string& path, ResourceType type,
                                    const ResourceLoadConfig& config,
                                    ResourceLoadCallback callback) = 0;

        /**
         * @brief 验证资源文件
         * @param path 文件路径
         * @param type 资源类型
         * @return true如果文件有效，false否则
         */
        virtual bool ValidateResourceFile(const std::string& path, ResourceType type) const = 0;

        /**
         * @brief 获取支持的资源类型列表
         * @return 支持的资源类型列表
         */
        virtual std::vector<ResourceType> GetSupportedResourceTypes() const = 0;

        /**
         * @brief 获取支持的文件格式列表
         * @return 支持的文件格式列表
         */
        virtual std::vector<std::string> GetSupportedFileFormats() const = 0;

        /**
         * @brief 获取后端统计信息
         * @return 统计信息
         */
        virtual ResourceStatistics GetStatistics() const = 0;
    };

    /**
     * @brief 资源后端智能指针类型
     */
    using ResourceBackendRef = Ref<IResourceBackend>;

} // namespace Core
} // namespace Resources
} // namespace Zgine
