#pragma once

#include <cstdint>
#include <limits>

namespace Zgine {

class World;

/**
 * @brief Lightweight entity handle with version tracking for validity checking
 *
 * EntityHandle is a value type that represents an entity reference.
 * Unlike Entity, it doesn't hold a World pointer, making it safe for serialization
 * and storage across World reloads.
 */
class EntityHandle {
public:
    static constexpr uint32_t NullValue = std::numeric_limits<uint32_t>::max();

    EntityHandle() = default;

    explicit EntityHandle(uint32_t value)
        : m_Value(value)
    {}

    static EntityHandle FromValue(uint32_t value) {
        return EntityHandle(value);
    }

    /**
     * @brief Check if this handle is valid in the given World
     * @param World World to check validity against
     * @return true if entity exists and is valid in the World's registry
     */
    bool IsValid(const World* World) const;

    /**
     * @brief Check if handle is not null (doesn't check registry validity)
     * @return true if the handle is not null
     */
    explicit operator bool() const {
        return m_Value != NullValue;
    }

    /**
     * @brief Get the engine-level raw handle value
     */
    uint32_t GetValue() const {
        return m_Value;
    }

    /**
     * @brief Get version number for validity checking
     */
    uint32_t GetVersion() const {
        return m_Value;
    }

    // Comparison operators
    bool operator==(const EntityHandle& other) const {
        return m_Value == other.m_Value;
    }

    bool operator!=(const EntityHandle& other) const {
        return m_Value != other.m_Value;
    }

    bool operator<(const EntityHandle& other) const {
        return m_Value < other.m_Value;
    }

private:
    uint32_t m_Value{ NullValue };
};

} // namespace Zgine
