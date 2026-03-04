#pragma once

#include <entt/entt.hpp>
#include <cstdint>

namespace Zgine {

class Scene;

/**
 * @brief Lightweight entity handle with version tracking for validity checking
 *
 * EntityHandle is a value type that represents an entity reference.
 * Unlike Entity, it doesn't hold a Scene pointer, making it safe for serialization
 * and storage across scene reloads.
 */
class EntityHandle {
public:
    EntityHandle() = default;

    explicit EntityHandle(entt::entity handle)
        : m_Handle(handle)
        , m_Version(static_cast<uint32_t>(handle))
    {}

    /**
     * @brief Check if this handle is valid in the given scene
     * @param scene Scene to check validity against
     * @return true if entity exists and is valid in the scene's registry
     */
    bool IsValid(const Scene* scene) const;

    /**
     * @brief Check if handle is not null (doesn't check registry validity)
     * @return true if handle != entt::null
     */
    explicit operator bool() const {
        return m_Handle != entt::null;
    }

    /**
     * @brief Convert to entt::entity for registry operations
     */
    operator entt::entity() const {
        return m_Handle;
    }

    /**
     * @brief Get the raw entt entity handle
     */
    entt::entity GetHandle() const {
        return m_Handle;
    }

    /**
     * @brief Get version number for validity checking
     */
    uint32_t GetVersion() const {
        return m_Version;
    }

    // Comparison operators
    bool operator==(const EntityHandle& other) const {
        return m_Handle == other.m_Handle;
    }

    bool operator!=(const EntityHandle& other) const {
        return m_Handle != other.m_Handle;
    }

    bool operator<(const EntityHandle& other) const {
        return m_Handle < other.m_Handle;
    }

private:
    entt::entity m_Handle{ entt::null };
    uint32_t m_Version{ 0 };
};

} // namespace Zgine
