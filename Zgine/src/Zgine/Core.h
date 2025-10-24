#pragma once

#include "Zgine/Core/SmartPointers.h"
#include "Zgine/Timestep.h"

/**
 * @file Core.h
 * @brief Core definitions and macros for the Zgine engine
 * @details This header contains essential type definitions, compiler detection,
 *          platform detection, and utility macros used throughout the engine
 */

/**
 * @brief Global application shutdown flag
 * @details This flag is set to true when the application is shutting down
 *          to prevent operations on destroyed resources
 */
extern bool g_ApplicationShuttingDown;

namespace Zgine {
	// Timestep is now defined in Timestep.h
}

/*  Compiler Detection  */
#if defined(_MSC_VER)                       // Microsoft Visual C++
#   define ZG_COMPILER_MSVC
#   define ZG_FORCEINLINE          __forceinline
#   define ZG_DEBUGBREAK()         __debugbreak()

#elif defined(__clang__)                    // Clang
#   define ZG_COMPILER_CLANG
#   define ZG_FORCEINLINE          inline __attribute__((always_inline))
#   define ZG_DEBUGBREAK()         __builtin_debugtrap()

#elif defined(__GNUC__)                     // GCC
#   define ZG_COMPILER_GCC
#   define ZG_FORCEINLINE          inline __attribute__((always_inline))
#   define ZG_DEBUGBREAK()         __builtin_trap()

#else
#   error "[Zgine] Unsupported compiler!"
#endif


/*  Platform Detection  */
#if defined(_WIN32)
#   ifndef ZG_PLATFORM_WINDOWS
#   define ZG_PLATFORM_WINDOWS
#   endif
#elif defined(__linux__)
#   define ZG_PLATFORM_LINUX
#elif defined(__APPLE__)
#   define ZG_PLATFORM_MACOS
#else
#   error "[Zgine] Unsupported platform!"
#endif


/*  Windows DLL import / export  */
#if defined(ZG_PLATFORM_WINDOWS) && defined(ZG_DYMATIC_LINK)
#   ifdef ZG_BUILD_DLL
#       define ZG_API      __declspec(dllexport)
#   else
#       define ZG_API      __declspec(dllimport)
#   endif
#else
#   define ZG_API
#endif


/*  Runtime Assertion  */
#ifdef ZG_ENABLE_ASSERTS
/**
 * @brief Client assertion macro
 * @param x The condition to assert
 * @param ... Additional arguments for error message formatting
 * @details Triggers a debug break if the condition is false
 */
#   define ZG_ASSERT(x, ...)                                                       \
        do {                                                                       \
            if (!(x)) {                                                            \
                ZG_ERROR("Assertion Failed: {0}", __VA_ARGS__);                    \
                ZG_DEBUGBREAK();                                                   \
            }                                                                      \
        } while (0)

/**
 * @brief Core assertion macro
 * @param x The condition to assert
 * @param ... Additional arguments for error message formatting
 * @details Triggers a debug break if the condition is false
 */
#   define ZG_CORE_ASSERT(x, ...)                                                  \
        do {                                                                       \
            if (!(x)) {                                                            \
                ZG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);               \
                ZG_DEBUGBREAK();                                                   \
            }                                                                      \
        } while (0)
#else
#   define ZG_ASSERT(x, ...)
#   define ZG_CORE_ASSERT(x, ...)
#endif


/*  Helper Macros  */
/**
 * @brief Bind event function macro
 * @param fn The function to bind
 * @details Creates a std::bind wrapper for event handling functions
 */
#define BIND_EVENT_FN(fn)    std::bind(&fn, this, std::placeholders::_1)

/**
 * @brief Bit manipulation macro
 * @param x The bit position
 * @return uint32_t A value with the specified bit set
 * @details Creates a bitmask with a single bit set at the specified position
 */
#define BIT(x)               (1u << (x))