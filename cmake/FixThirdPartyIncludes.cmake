# ============================================================================
# Post-FetchContent Fixes for Third-Party Libraries
# Fix include directories to use BUILD_INTERFACE for libraries we don't control
# ============================================================================

# Fix Jolt include directories (it's created by FetchContent with absolute paths)
if(TARGET Jolt)
    get_target_property(_jolt_includes Jolt INTERFACE_INCLUDE_DIRECTORIES)
    if(_jolt_includes)
        set(_jolt_build_includes)
        foreach(_inc IN LISTS _jolt_includes)
            list(APPEND _jolt_build_includes "$<BUILD_INTERFACE:${_inc}>")
        endforeach()
        set_target_properties(Jolt PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_jolt_build_includes}"
        )
    endif()
endif()

# Fix miniaudio include directories if it was created by FetchContent
if(TARGET miniaudio)
    get_target_property(_miniaudio_includes miniaudio INTERFACE_INCLUDE_DIRECTORIES)
    if(_miniaudio_includes)
        set(_miniaudio_build_includes)
        foreach(_inc IN LISTS _miniaudio_includes)
            list(APPEND _miniaudio_build_includes "$<BUILD_INTERFACE:${_inc}>")
        endforeach()
        set_target_properties(miniaudio PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_miniaudio_build_includes}"
        )
    endif()
endif()
