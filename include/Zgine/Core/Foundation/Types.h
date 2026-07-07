#pragma once

#include <memory>
#include <concepts>

// ============================================================================
// Smart Pointer Type Aliases
// ============================================================================
// Convenient type aliases for smart pointers used throughout the engine.

namespace Zgine {

    // Unique pointer (exclusive ownership)
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
        requires std::constructible_from<T, Args...>
    [[nodiscard]] constexpr Scope<T> CreateScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Shared pointer (shared ownership)
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
        requires std::constructible_from<T, Args...>
    [[nodiscard]] constexpr Ref<T> CreateRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace Zgine
