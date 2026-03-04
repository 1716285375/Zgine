#pragma once

#include <Zgine/Core/Base/Platform.h>

// ============================================================================
// Debug Break Macros
// ============================================================================
// Platform-specific debugger break points for development builds.

#ifdef ZGINE_DEBUG
    #if defined(ZGINE_PLATFORM_WINDOWS)
        #define ZGINE_DEBUGBREAK() __debugbreak()
    #elif defined(ZGINE_PLATFORM_LINUX) || defined(ZGINE_PLATFORM_MACOS)
        #include <signal.h>
        #define ZGINE_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define ZGINE_ENABLE_ASSERTS
#else
    #define ZGINE_DEBUGBREAK()
#endif
