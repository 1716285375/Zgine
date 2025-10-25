#pragma once

#include "Zgine/Core/SmartPointers.h"
#include "ResourceTypes.h"
#include <string>
#include <memory>
#include <functional>

namespace Zgine {
namespace Resources {

    /**
     * @brief 基础资源接口
     * @details 所有资源类型都必须实现此接口
     */
    class IResource {
    public:
        virtual ~IResource() = default;

        /**
         * @brief 获取资源ID
         * @return 资源唯一标识符
         */
        virtual uint32_t GetID() const = 0;

        /**
         * @brief 获取资源名称
         * @return 资源名称
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief 获取资源路径
         * @return 资源文件路径
         */
        virtual const std::string& GetPath() const = 0;

        /**
         * @brief 获取资源类型
         * @return 资源类型枚举
         */
        virtual ResourceType GetType() const = 0;

        /**
         * @brief 获取资源状态
         * @return 资源状态枚举
         */
        virtual ResourceState GetState() const = 0;

        /**
         * @brief 获取资源大小
         * @return 资源大小（字节）
         */
        virtual size_t GetSize() const = 0;

        /**
         * @brief 获取引用计数
         * @return 当前引用计数
         */
        virtual uint32_t GetRefCount() const = 0;

        /**
         * @brief 增加引用计数
         */
        virtual void AddRef() = 0;

        /**
         * @brief 减少引用计数
         */
        virtual void RemoveRef() = 0;

        /**
         * @brief 检查资源是否有效
         * @return true如果资源有效，false否则
         */
        virtual bool IsValid() const = 0;

        /**
         * @brief 检查资源是否已加载
         * @return true如果已加载，false否则
         */
        virtual bool IsLoaded() const = 0;

        /**
         * @brief 异步加载资源
         * @param callback 加载完成回调
         */
        virtual void LoadAsync(std::function<void(bool)> callback = nullptr) = 0;

        /**
         * @brief 同步加载资源
         * @return true如果加载成功，false否则
         */
        virtual bool LoadSync() = 0;

        /**
         * @brief 卸载资源
         */
        virtual void Unload() = 0;

        /**
         * @brief 重新加载资源
         * @return true如果重新加载成功，false否则
         */
        virtual bool Reload() = 0;

        /**
         * @brief 获取资源元数据
         * @param key 元数据键
         * @return 元数据值，如果不存在返回空字符串
         */
        virtual std::string GetMetadata(const std::string& key) const = 0;

        /**
         * @brief 设置资源元数据
         * @param key 元数据键
         * @param value 元数据值
         */
        virtual void SetMetadata(const std::string& key, const std::string& value) = 0;
    };

    /**
     * @brief 资源智能指针类型
     */
    using ResourceRef = Ref<IResource>;

    /**
     * @brief 资源加载回调类型
     */
    using ResourceLoadCallback = std::function<void(ResourceRef, bool)>;

    /**
     * @brief 资源状态变化回调类型
     */
    using ResourceStateCallback = std::function<void(ResourceRef, ResourceState)>;

} // namespace Resources
} // namespace Zgine