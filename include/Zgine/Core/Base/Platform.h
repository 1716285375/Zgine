#pragma once

// ============================================================================
// Platform Detection Macros
// ============================================================================
// Detects the target platform and defines appropriate macros.
// These macros are used throughout the engine for platform-specific code.

#ifdef _WIN32
    /* Windows x64/x86 */
    #ifdef _WIN64
        /* Windows x64  */
        #define ZGINE_PLATFORM_WINDOWS
    #else
        /* Windows x86 */
        #define ZGINE_PLATFORM_WINDOWS
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    /* TARGET_OS_MAC includes iOS, iPadOS, watchOS, tvOS, and macOS.
       We need to distinguish between them. */
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define ZGINE_PLATFORM_IOS
        #error "IOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define ZGINE_PLATFORM_MACOS
    #else
        #error "Unknown Apple platform!"
    #endif
#elif defined(__linux__)
    #define ZGINE_PLATFORM_LINUX
#else
    /* Unrecognized platform */
    #error "Unknown platform!"
#endif
