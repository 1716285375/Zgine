#include <Zgine/Scene/Core/EntityManager.h>
#include <Zgine/Scene/Core/Entity.h>
#include <Zgine/Scene/Components/Components.h>
#include <algorithm>

namespace Zgine {

EntityManager::EntityManager(entt::registry& registry)
    : m_Registry(registry)
{}

EntityHandle EntityManager::Create(const std::string& name) {
    // Create the entity in the registry
    entt::entity handle = m_Registry.create();
    EntityHandle entityHandle(handle);

    // Add default components
    m_Registry.emplace<IDComponent>(handle);
    m_Registry.emplace<TagComponent>(handle, name.empty() ? "Entity" : name);
    m_Registry.emplace<TransformComponent>(handle);
    m_Registry.emplace<RelationshipComponent>(handle);

    // Notify listeners
    if (m_OnEntityCreated) {
        m_OnEntityCreated(entityHandle);
    }

    return entityHandle;
}

bool EntityManager::Destroy(EntityHandle handle) {
    if (!IsValid(handle)) {
        return false;
    }

    entt::entity entity = handle.GetHandle();

    // Handle relationship cleanup
    if (m_Registry.all_of<RelationshipComponent>(entity)) {
        auto& rel = m_Registry.get<RelationshipComponent>(entity);

        // Remove from parent's children list
        if (rel.Parent != entt::null && m_Registry.valid(rel.Parent)) {
            auto& parentRel = m_Registry.get<RelationshipComponent>(rel.Parent);
            parentRel.Children.erase(
                std::remove(parentRel.Children.begin(), parentRel.Children.end(), entity),
                parentRel.Children.end());
        }

        // Destroy all children recursively
        auto children = rel.Children; // Copy to avoid iterator invalidation
        for (auto child : children) {
            Destroy(EntityHandle(child));
        }
    }

    // Notify listeners before destruction
    if (m_OnEntityDestroyed) {
        m_OnEntityDestroyed(handle);
    }

    // Actually destroy the entity
    m_Registry.destroy(entity);
    return true;
}

bool EntityManager::IsValid(EntityHandle handle) const {
    if (!handle) {
        return false;
    }
    return m_Registry.valid(handle.GetHandle());
}

size_t EntityManager::GetEntityCount() const {
    return m_Registry.storage<entt::entity>().size();
}

void EntityManager::Clear() {
    m_Registry.clear();
}

} // namespace Zgine
