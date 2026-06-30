#pragma once

#include <Zgine/World/Core/EntityHandle.h>
#include <Zgine/World/Systems/SystemManager.h>
#include <string>
#include <memory>
#include <vector>

namespace Zgine {

class Entity;
class EntityManager;
namespace Internal { struct WorldRegistryAccess; }

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
    [[nodiscard]] std::unique_ptr<World> CloneForRuntime() const;

    // System Updates
    void OnUpdate(float deltaTime);
    void OnRender();

    // System Management
    SystemManager& GetSystemManager() { return m_SystemManager; }
    const SystemManager& GetSystemManager() const { return m_SystemManager; }

    std::vector<Entity> GetAllEntities() const;
    std::vector<Entity> GetRootEntities() const;
    std::vector<Entity> GetChildren(Entity entity) const;

    size_t GetEntityCount() const;

    // Entity validation
    bool IsEntityValid(Entity entity) const;

    EntityManager& GetEntityManager() { return *m_EntityManager; }
    const EntityManager& GetEntityManager() const { return *m_EntityManager; }

private:
    struct Storage;

    template<typename T>
    T& AddComponentFromValue(EntityHandle handle, T&& component);

    template<typename T>
    T& GetComponent(EntityHandle handle);

    template<typename T>
    const T& GetComponent(EntityHandle handle) const;

    template<typename T>
    bool HasComponent(EntityHandle handle) const;

    template<typename T>
    void RemoveComponent(EntityHandle handle);

    std::unique_ptr<Storage> m_Storage;
    std::unique_ptr<EntityManager> m_EntityManager;
    SystemManager m_SystemManager;

    friend class Entity;
    friend struct Internal::WorldRegistryAccess;
};

}

