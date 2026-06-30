#pragma once

#include <type_traits>
#include <utility>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/EntityHandle.h>

namespace Zgine {

/**
 * @brief Entity is a smart reference to an entity in a World
 *
 * Unlike EntityHandle, Entity holds a World pointer for convenient component access.
 * Always check IsValid() before using the entity to ensure it still exists.
 */
class Entity {
public:
    Entity() = default;

    Entity(EntityHandle handle, World* World)
        : m_Handle(handle)
        , m_Scene(World)
    {}

    Entity(uint32_t handle, World* World)
        : m_Handle(EntityHandle(handle))
        , m_Scene(World)
    {}

    /**
     * @brief Check if this entity is valid in the World
     * @return true if entity exists in the World's registry
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

    World* GetWorld() const noexcept {
        return m_Scene;
    }

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
        T component(std::forward<Args>(args)...);
        return m_Scene->AddComponentFromValue<T>(m_Handle, std::move(component));
    }

    template<typename T>
    T& GetComponent() {
        return m_Scene->GetComponent<T>(m_Handle);
    }

    template<typename T>
    const T& GetComponent() const {
        return m_Scene->GetComponent<T>(m_Handle);
    }

    template<typename T>
    bool HasComponent() const {
        return m_Scene->HasComponent<T>(m_Handle);
    }

    template<typename T>
    void RemoveComponent() {
        m_Scene->RemoveComponent<T>(m_Handle);
    }

    // Operators
    operator bool() const {
        return static_cast<bool>(m_Handle);
    }

    bool operator==(const Entity& other) const {
        return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }

private:
    EntityHandle m_Handle;
    World* m_Scene = nullptr;
};

}

