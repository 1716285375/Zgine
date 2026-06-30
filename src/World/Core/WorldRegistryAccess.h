#pragma once

#include <Zgine/World/Core/World.h>
#include <entt/entt.hpp>

namespace Zgine {

struct World::Storage {
    entt::registry Registry;
};

namespace Internal {

inline entt::entity ToEnTT(EntityHandle handle) {
    if (!handle) {
        return entt::null;
    }
    return static_cast<entt::entity>(handle.GetValue());
}

inline EntityHandle FromEnTT(entt::entity entity) {
    if (entity == entt::null) {
        return EntityHandle();
    }
    return EntityHandle::FromValue(static_cast<uint32_t>(entity));
}

struct WorldRegistryAccess {
    static entt::registry& GetRegistry(World& world) {
        return world.m_Storage->Registry;
    }

    static const entt::registry& GetRegistry(const World& world) {
        return world.m_Storage->Registry;
    }
};

inline entt::registry& GetRegistry(World& world) {
    return WorldRegistryAccess::GetRegistry(world);
}

inline const entt::registry& GetRegistry(const World& world) {
    return WorldRegistryAccess::GetRegistry(world);
}

} // namespace Internal
} // namespace Zgine
