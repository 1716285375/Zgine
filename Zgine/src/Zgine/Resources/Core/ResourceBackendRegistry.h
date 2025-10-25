#pragma once

#include "zgpch.h"
#include "IResourceBackend.h"
#include "Zgine/Logging/Log.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <functional>

namespace Zgine {
namespace Resources {
namespace Core {

    /**
     * @brief 资源后端工厂函数类型
     */
    using ResourceBackendFactory = std::function<ResourceBackendRef()>;

    /**
     * @brief 资源后端注册器
     * 管理所有可用的资源后端，支持动态注册和热拔插
     */
    class ResourceBackendRegistry {
    public:
        /**
         * @brief 获取单例实例
         * @return 注册器实例
         */
        static ResourceBackendRegistry& GetInstance();

        /**
         * @brief 注册资源后端
         * @param name 后端名称
         * @param factory 后端工厂函数
         * @param priority 优先级（数字越小优先级越高）
         * @return true如果注册成功，false否则
         */
        bool RegisterBackend(const std::string& name, ResourceBackendFactory factory, int priority = 0);

        /**
         * @brief 注销资源后端
         * @param name 后端名称
         * @return true如果注销成功，false否则
         */
        bool UnregisterBackend(const std::string& name);

        /**
         * @brief 创建资源后端实例
         * @param name 后端名称
         * @return 后端实例，如果创建失败返回nullptr
         */
        ResourceBackendRef CreateBackend(const std::string& name);

        /**
         * @brief 获取所有已注册的后端名称
         * @return 后端名称列表
         */
        std::vector<std::string> GetRegisteredBackends() const;

        /**
         * @brief 获取支持指定资源类型的后端
         * @param type 资源类型
         * @return 支持的后端名称列表
         */
        std::vector<std::string> GetBackendsForResourceType(ResourceType type) const;

        /**
         * @brief 获取支持指定文件格式的后端
         * @param extension 文件扩展名
         * @return 支持的后端名称列表
         */
        std::vector<std::string> GetBackendsForFileFormat(const std::string& extension) const;

        /**
         * @brief 检查后端是否已注册
         * @param name 后端名称
         * @return true如果已注册，false否则
         */
        bool IsBackendRegistered(const std::string& name) const;

        /**
         * @brief 清空所有注册的后端
         */
        void Clear();

    private:
        ResourceBackendRegistry() = default;
        ~ResourceBackendRegistry() = default;
        ResourceBackendRegistry(const ResourceBackendRegistry&) = delete;
        ResourceBackendRegistry& operator=(const ResourceBackendRegistry&) = delete;

        struct BackendInfo {
            std::string name;
            ResourceBackendFactory factory;
            int priority;
        };

        mutable std::mutex m_Mutex;
        std::unordered_map<std::string, BackendInfo> m_Backends;
        std::vector<std::string> m_BackendOrder; // 按优先级排序的后端名称
    };

    /**
     * @brief 资源后端注册宏
     * 简化后端注册过程
     */
    #define REGISTER_RESOURCE_BACKEND(BackendClass, name, priority) \
        namespace { \
            struct BackendClass##Registrar { \
                BackendClass##Registrar() { \
                    Zgine::Resources::Core::ResourceBackendRegistry::GetInstance().RegisterBackend( \
                        name, \
                        []() -> Zgine::Resources::Core::ResourceBackendRef { \
                            return Zgine::CreateRef<BackendClass>(); \
                        }, \
                        priority \
                    ); \
                } \
            }; \
            static BackendClass##Registrar s_##BackendClass##Registrar; \
        }

} // namespace Core
} // namespace Resources
} // namespace Zgine
