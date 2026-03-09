#pragma once

#include <tuple>

// ============================================================================
// Common Utility Macros
// ============================================================================
// Note: Macros are defined at global scope, not within namespace Zgine

// Bit manipulation
#define BIT(x) (1 << (x))

// Event binding helper for member functions
#define ZGINE_BIND_EVENT_FN(fn) \
    [this](auto&&... args) -> decltype(auto) { \
        return this->fn(std::forward<decltype(args)>(args)...); \
    }

// Disable copy constructor and assignment operator
#define ZGINE_NON_COPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

// Disable move constructor and assignment operator
#define ZGINE_NON_MOVABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

// Disable all copy and move operations
#define ZGINE_NON_COPYABLE_NON_MOVABLE(ClassName) \
    ZGINE_NON_COPYABLE(ClassName) \
    ZGINE_NON_MOVABLE(ClassName)

// Array size helper
#define ZGINE_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Stringify macro
#define ZGINE_STRINGIFY(x) #x
#define ZGINE_STRINGIFY_MACRO(x) ZGINE_STRINGIFY(x)

// Concatenation macro
#define ZGINE_CONCAT(a, b) a##b
#define ZGINE_CONCAT_MACRO(a, b) ZGINE_CONCAT(a, b)

// Unused parameter/variable suppression
#define ZGINE_UNUSED(x) (void)(x)

#define ZGINE_IGNORE_RESULT(expr) std::ignore = (expr)

// Force inline hints
#if defined(_MSC_VER)
    #define ZGINE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define ZGINE_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define ZGINE_FORCE_INLINE inline
#endif

// No inline hints
#if defined(_MSC_VER)
    #define ZGINE_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
    #define ZGINE_NO_INLINE __attribute__((noinline))
#else
    #define ZGINE_NO_INLINE
#endif

// Alignment macros
#if defined(_MSC_VER)
    #define ZGINE_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
    #define ZGINE_ALIGN(x) __attribute__((aligned(x)))
#else
    #define ZGINE_ALIGN(x)
#endif

// Likely/Unlikely branch prediction hints
#if defined(__GNUC__) || defined(__clang__)
    #define ZGINE_LIKELY(x) __builtin_expect(!!(x), 1)
    #define ZGINE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define ZGINE_LIKELY(x) (x)
    #define ZGINE_UNLIKELY(x) (x)
#endif
