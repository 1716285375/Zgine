#pragma once

#include <entt/entt.hpp>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/EntityHandle.h>
#include <Zgine/World/Systems/SystemManager.h>
#include <memory>

namespace Zgine {

class Entity;
class EntityManager;

/**
 * @brief World represents a collection of entities and systems (Model in MVVM)
 *
 * World manages the game world state without UI concerns.
 * It delegates entity lifecycle to EntityManager for proper separation.
 */
class World {
public:
    World();
    ~World();

    // Entity Lifecycle (Model operations)
    Entity CreateEntity(const std::string& name = std::string());
    Entity CreateEntity(const std::string& name, Entity parent);
    void DestroyEntity(Entity entity);
    void Clear();

    // Hierarchy Management
    void SetParent(Entity child, Entity parent);
    void ClearParent(Entity child);
    Entity DuplicateEntity(Entity source);

    // System Updates
    void OnUpdate(float deltaTime);
    void OnRender();

    // System Management
    SystemManager& GetSystemManager() { return m_SystemManager; }
    const SystemManager& GetSystemManager() const { return m_SystemManager; }

    // Query API (for ViewModel read access)
    template<typename... Components>
    auto GetEntitiesWith() {
        return m_Registry.view<Components...>();
    }

    template<typename... Components>
    auto GetEntitiesWith() const {
        return m_Registry.view<Components...>();
    }

    size_t GetEntityCount() const;

    // Entity validation
    bool IsEntityValid(Entity entity) const;

    // Internal access (package-private)
    entt::registry& GetRegistry() { return m_Registry; }
    const entt::registry& GetRegistry() const { return m_Registry; }

    EntityManager& GetEntityManager() { return *m_EntityManager; }
    const EntityManager& GetEntityManager() const { return *m_EntityManager; }

private:
    std::unique_ptr<EntityManager> m_EntityManager;
    entt::registry m_Registry;
    SystemManager m_SystemManager;

    friend class Entity;
    friend class PhysicsSystem;
    friend class WorldSerializer;
};

}

