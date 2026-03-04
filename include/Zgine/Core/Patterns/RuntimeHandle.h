#pragma once

#include <cstdint>
#include <type_traits>

namespace Zgine {

/**
 * @brief Type-safe wrapper for runtime handles
 *
 * Provides type safety for void* or integer handles used by runtime systems.
 * Zero-overhead abstraction - compiles to same code as raw pointer/integer.
 *
 * @tparam T Tag type for type safety (need not be defined)
 * @tparam HandleType Underlying handle type (void*, uint32_t, etc.)
 */
template<typename T, typename HandleType = void*>
class RuntimeHandle {
public:
    RuntimeHandle() : m_Handle(GetNullValue()) {}

    explicit RuntimeHandle(HandleType handle) : m_Handle(handle) {}

    /**
     * @brief Check if handle is valid (non-null)
     */
    bool IsValid() const {
        return m_Handle != GetNullValue();
    }

    /**
     * @brief Get the raw handle value
     */
    HandleType Get() const {
        return m_Handle;
    }

    /**
     * @brief Set the handle value
     */
    void Set(HandleType handle) {
        m_Handle = handle;
    }

    /**
     * @brief Reset handle to null
     */
    void Reset() {
        m_Handle = GetNullValue();
    }

    /**
     * @brief Bool conversion for null checking
     */
    explicit operator bool() const {
        return IsValid();
    }

    /**
     * @brief Equality comparison
     */
    bool operator==(const RuntimeHandle& other) const {
        return m_Handle == other.m_Handle;
    }

    bool operator!=(const RuntimeHandle& other) const {
        return m_Handle != other.m_Handle;
    }

private:
    HandleType m_Handle;

    /**
     * @brief Get null value for the handle type
     */
    static constexpr HandleType GetNullValue() {
        if constexpr (std::is_pointer_v<HandleType>) {
            return nullptr;
        } else {
            return HandleType(0);
        }
    }
};

// Specialized handle types for engine systems
class PhysicsBodyTag;
class AudioSourceTag;
class ScriptInstanceTag;

using PhysicsHandle = RuntimeHandle<PhysicsBodyTag, void*>;
using AudioHandle = RuntimeHandle<AudioSourceTag, void*>;
using ScriptHandle = RuntimeHandle<ScriptInstanceTag, uint32_t>;

} // namespace Zgine
