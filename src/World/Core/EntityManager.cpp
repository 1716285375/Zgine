#include <Zgine/World/Core/EntityManager.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Components/Components.h>
#include "WorldRegistryAccess.h"
#include <algorithm>
#include <vector>

namespace Zgine {

EntityManager::EntityManager(World& world)
    : m_World(world)
{}

EntityHandle EntityManager::Create(const std::string& name) {
    auto& registry = Internal::GetRegistry(m_World);

    // Create the entity in the registry
    entt::entity handle = registry.create();
    EntityHandle entityHandle = Internal::FromEnTT(handle);

    // Add default components
    registry.emplace<IDComponent>(handle);
    registry.emplace<TagComponent>(handle, name.empty() ? "Entity" : name);
    registry.emplace<TransformComponent>(handle);
    registry.emplace<RelationshipComponent>(handle);

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

    auto& registry = Internal::GetRegistry(m_World);
    entt::entity entity = Internal::ToEnTT(handle);

    // Handle relationship cleanup
    if (registry.all_of<RelationshipComponent>(entity)) {
        auto& rel = registry.get<RelationshipComponent>(entity);

        // Remove from parent's children list
        if (rel.Parent && registry.valid(Internal::ToEnTT(rel.Parent))) {
            auto& parentRel = registry.get<RelationshipComponent>(Internal::ToEnTT(rel.Parent));
            parentRel.Children.erase(
                std::remove(parentRel.Children.begin(), parentRel.Children.end(), handle),
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
    registry.destroy(entity);
    return true;
}

bool EntityManager::IsValid(EntityHandle handle) const {
    if (!handle) {
        return false;
    }
    return Internal::GetRegistry(m_World).valid(Internal::ToEnTT(handle));
}

size_t EntityManager::GetEntityCount() const {
    return Internal::GetRegistry(m_World).storage<IDComponent>().size();
}

void EntityManager::Clear() {
    auto& registry = Internal::GetRegistry(m_World);

    std::vector<EntityHandle> roots;
    auto view = registry.view<IDComponent, RelationshipComponent>();
    for (auto entity : view) {
        const auto& rel = view.get<RelationshipComponent>(entity);
        if (!rel.Parent || !registry.valid(Internal::ToEnTT(rel.Parent))) {
            roots.push_back(Internal::FromEnTT(entity));
        }
    }

    for (EntityHandle root : roots) {
        Destroy(root);
    }

    // Components should be gone through Destroy(), but keep a final guard for
    // malformed relationship graphs or entities created outside EntityManager.
    registry.clear();
}

} // namespace Zgine
