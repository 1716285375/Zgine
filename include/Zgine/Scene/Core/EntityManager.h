#pragma once

#include <Zgine/Scene/Core/EntityHandle.h>
#include <entt/entt.hpp>
#include <string>
#include <functional>

namespace Zgine {

class Scene;
class Entity;

/**
 * @brief Manages entity lifecycle with proper validation and event integration
 *
 * EntityManager is the single source of truth for entity creation and destruction.
 * It ensures all operations are validated and events are properly published.
 */
class EntityManager {
public:
    using EntityCreatedCallback = std::function<void(EntityHandle)>;
    using EntityDestroyedCallback = std::function<void(EntityHandle)>;

    explicit EntityManager(entt::registry& registry);
    ~EntityManager() = default;

    /**
     * @brief Create a new entity with the given name
     * @param name Entity name (defaults to "Entity" if empty)
     * @return Handle to the created entity
     */
    EntityHandle Create(const std::string& name = "");

    /**
     * @brief Destroy an entity and all its children
     * @param handle Handle to entity to destroy
     * @return true if entity was destroyed, false if invalid
     */
    bool Destroy(EntityHandle handle);

    /**
     * @brief Check if an entity handle is valid
     * @param handle Handle to check
     * @return true if entity exists in registry
     */
    bool IsValid(EntityHandle handle) const;

    /**
     * @brief Get the number of living entities
     */
    size_t GetEntityCount() const;

    /**
     * @brief Clear all entities
     */
    void Clear();

    /**
     * @brief Set callback for entity creation events
     */
    void SetEntityCreatedCallback(EntityCreatedCallback callback) {
        m_OnEntityCreated = std::move(callback);
    }

    /**
     * @brief Set callback for entity destruction events
     */
    void SetEntityDestroyedCallback(EntityDestroyedCallback callback) {
        m_OnEntityDestroyed = std::move(callback);
    }

    /**
     * @brief Get the underlying registry (for Scene/System access only)
     */
    entt::registry& GetRegistry() { return m_Registry; }
    const entt::registry& GetRegistry() const { return m_Registry; }

private:
    entt::registry& m_Registry;
    EntityCreatedCallback m_OnEntityCreated;
    EntityDestroyedCallback m_OnEntityDestroyed;
};

} // namespace Zgine
