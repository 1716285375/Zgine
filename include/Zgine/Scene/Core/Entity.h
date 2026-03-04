#pragma once

#include <entt/entt.hpp>
#include <type_traits>
#include <utility>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Core/EntityHandle.h>

namespace Zgine {

/**
 * @brief Entity is a smart reference to an entity in a scene
 *
 * Unlike EntityHandle, Entity holds a Scene pointer for convenient component access.
 * Always check IsValid() before using the entity to ensure it still exists.
 */
class Entity {
public:
    Entity() = default;

    Entity(EntityHandle handle, Scene* scene)
        : m_Handle(handle)
        , m_Scene(scene)
    {}

    // Legacy constructor for compatibility
    Entity(entt::entity handle, Scene* scene)
        : m_Handle(EntityHandle(handle))
        , m_Scene(scene)
    {}

    /**
     * @brief Check if this entity is valid in the scene
     * @return true if entity exists in the scene's registry
     */
    bool IsValid() const {
        return m_Handle.IsValid(m_Scene);
    }

    /**
     * @brief Get the entity handle
     */
    EntityHandle GetHandle() const {
        return m_Handle;
    }

    template<typename T, typename... Args>
    decltype(auto) AddComponent(Args&&... args) {
        if constexpr (std::is_empty_v<T>) {
            m_Scene->m_Registry.emplace<T>(m_Handle.GetHandle(), std::forward<Args>(args)...);
        } else {
            return m_Scene->m_Registry.emplace<T>(m_Handle.GetHandle(), std::forward<Args>(args)...);
        }
    }

    template<typename T>
    T& GetComponent() {
        return m_Scene->m_Registry.get<T>(m_Handle.GetHandle());
    }

    template<typename T>
    bool HasComponent() {
        return m_Scene->m_Registry.all_of<T>(m_Handle.GetHandle());
    }

    template<typename T>
    void RemoveComponent() {
        m_Scene->m_Registry.remove<T>(m_Handle.GetHandle());
    }

    // Operators
    operator bool() const {
        return static_cast<bool>(m_Handle);
    }

    operator entt::entity() const {
        return m_Handle.GetHandle();
    }

    bool operator==(const Entity& other) const {
        return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }

private:
    EntityHandle m_Handle;
    Scene* m_Scene = nullptr;
};

}

