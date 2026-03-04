#pragma once

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

// ============================================================================
// Assertion Macros
// ============================================================================

#ifdef ZGINE_ENABLE_ASSERTS
    #define ZGINE_CORE_ASSERT(x, ...) \
        do { \
            if (!(x)) { \
                ZGINE_CORE_ERROR("Assertion Failed: {}", __VA_ARGS__); \
                ZGINE_DEBUGBREAK(); \
            } \
        } while(0)

    #define ZGINE_ASSERT(x, ...) \
        do { \
            if (!(x)) { \
                ZGINE_ERROR("Assertion Failed: {}", __VA_ARGS__); \
                ZGINE_DEBUGBREAK(); \
            } \
        } while(0)
#else
    #define ZGINE_CORE_ASSERT(x, ...) ZGINE_UNUSED(x)
    #define ZGINE_ASSERT(x, ...) ZGINE_UNUSED(x)
#endif

} // namespace Zgine
