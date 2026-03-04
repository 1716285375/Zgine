# ============================================================================
# Compiler Options & Toolchain Configuration
# Sanitizers, LTO, Warning Levels, etc.
# ============================================================================

include(CheckCXXCompilerFlag)

# ============================================================================
# Warning Levels
# ============================================================================
function(set_warning_level target level)
    if(MSVC)
        if(level STREQUAL "error")
            target_compile_options(${target} PRIVATE /W4 /WX)
        elseif(level STREQUAL "high")
            target_compile_options(${target} PRIVATE /W4)
        elseif(level STREQUAL "medium")
            target_compile_options(${target} PRIVATE /W3)
        else()
            target_compile_options(${target} PRIVATE /W1)
        endif()
    else()
        if(level STREQUAL "error")
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
        elseif(level STREQUAL "high")
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        elseif(level STREQUAL "medium")
            target_compile_options(${target} PRIVATE -Wall)
        else()
            target_compile_options(${target} PRIVATE -w)
        endif()
    endif()
endfunction()

# ============================================================================
# Sanitizers
# ============================================================================
option(ZGINE_ENABLE_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_UNDEFINED "Enable UndefinedBehaviorSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_THREAD "Enable ThreadSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_MEMORY "Enable MemorySanitizer" OFF)

function(apply_sanitizers target)
    if(MSVC)
        # MSVC sanitizers
        if(ZGINE_ENABLE_SANITIZER_ADDRESS)
            target_compile_options(${target} PRIVATE /fsanitize=address)
            target_link_options(${target} PRIVATE /fsanitize=address)
        endif()
    else()
        # GCC/Clang sanitizers
        if(ZGINE_ENABLE_SANITIZER_ADDRESS)
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address)
        endif()

        if(ZGINE_ENABLE_SANITIZER_UNDEFINED)
            target_compile_options(${target} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=undefined)
        endif()

        if(ZGINE_ENABLE_SANITIZER_THREAD)
            if(ZGINE_ENABLE_SANITIZER_ADDRESS OR ZGINE_ENABLE_SANITIZER_UNDEFINED)
                message(WARNING "ThreadSanitizer cannot be used with AddressSanitizer or UndefinedBehaviorSanitizer")
            else()
                target_compile_options(${target} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
                target_link_options(${target} PRIVATE -fsanitize=thread)
            endif()
        endif()

        if(ZGINE_ENABLE_SANITIZER_MEMORY)
            if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                target_compile_options(${target} PRIVATE -fsanitize=memory -fno-omit-frame-pointer)
                target_link_options(${target} PRIVATE -fsanitize=memory)
            else()
                message(WARNING "MemorySanitizer is only available with Clang")
            endif()
        endif()
    endif()
endfunction()

# ============================================================================
# Link Time Optimization (LTO)
# ============================================================================
option(ZGINE_ENABLE_LTO "Enable Link Time Optimization" OFF)

function(apply_lto target)
    if(ZGINE_ENABLE_LTO)
        if(MSVC)
            target_compile_options(${target} PRIVATE /GL)
            target_link_options(${target} PRIVATE /LTCG)
        else()
            check_cxx_compiler_flag(-flto LTO_AVAILABLE)
            if(LTO_AVAILABLE)
                target_compile_options(${target} PRIVATE -flto)
                target_link_options(${target} PRIVATE -flto)
            else()
                message(WARNING "LTO not available with current compiler")
            endif()
        endif()
    endif()
endfunction()

# ============================================================================
# Standard Project Warnings Function (Enhanced)
# ============================================================================
function(set_project_warnings target)
    # Get warning level from option
    if(ZGINE_WARNINGS_AS_ERRORS)
        set(_warn_level "error")
    elseif(DEFINED ZGINE_WARNING_LEVEL)
        set(_warn_level ${ZGINE_WARNING_LEVEL})
    else()
        set(_warn_level "high")
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /utf-8 /Zc:u8EscapeEncoding
            /permissive- /Zc:__cplusplus /Zc:inline /Zc:preprocessor /EHsc /MP
            $<$<CONFIG:Release>:/O2 /Ob2 /DNDEBUG>
            $<$<CONFIG:Debug>:/Od /Zi /RTC1>
            /wd4251 /wd4275 # Disable DLL warnings
        )
        target_compile_definitions(${target} PRIVATE
            _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS
            NOMINMAX WIN32_LEAN_AND_MEAN
        )
        set_warning_level(${target} ${_warn_level})
    else()
        set_warning_level(${target} ${_warn_level})
        target_compile_options(${target} PRIVATE
            -fvisibility=hidden
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
            $<$<CONFIG:Debug>:-g -O0>
        )
    endif()

    # Apply sanitizers if enabled
    apply_sanitizers(${target})

    # Apply LTO if enabled
    apply_lto(${target})
endfunction()

