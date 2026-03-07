#pragma once

#include <Zgine/Core/Patterns/RuntimeHandle.h>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <concepts>

namespace Zgine {

/**
 * @brief Simple service locator for dependency injection
 *
 * Provides type-safe registration and retrieval of service instances.
 * Services are registered as pointers (not owned by ServiceLocator).
 *
 * Usage:
 * @code
 *   ServiceLocator::Register<PhysicsSystem>(&physicsSystem);
 *   auto* physics = ServiceLocator::Get<PhysicsSystem>();
 * @endcode
 */
class ServiceLocator {
public:
    /**
     * @brief Register a service instance
     * @tparam T Service type
     * @param service Pointer to service (ServiceLocator does NOT take ownership)
     */
    template<typename T>
        requires std::is_class_v<T>
    static void Register(T* service) {
        s_Services[std::type_index(typeid(T))] = static_cast<void*>(service);
    }

    /**
     * @brief Get a registered service
     * @tparam T Service type
     * @return Pointer to service, or nullptr if not registered
     */
    template<typename T>
    static T* Get() {
        auto it = s_Services.find(std::type_index(typeid(T)));
        if (it != s_Services.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }

    /**
     * @brief Check if a service is registered
     * @tparam T Service type
     * @return True if service is registered
     */
    template<typename T>
        requires std::is_class_v<T>
    [[nodiscard]] static bool Has() {
        return s_Services.find(std::type_index(typeid(T))) != s_Services.end();
    }

    /**
     * @brief Unregister a service
     * @tparam T Service type
     */
    template<typename T>
        requires std::is_class_v<T>
    static void Unregister() {
        s_Services.erase(std::type_index(typeid(T)));
    }

    /**
     * @brief Clear all registered services
     */
    static void Clear() {
        s_Services.clear();
    }

private:
    static std::unordered_map<std::type_index, void*> s_Services;
};

} // namespace Zgine
