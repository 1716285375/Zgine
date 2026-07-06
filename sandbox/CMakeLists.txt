cmake_minimum_required(VERSION 3.20)

# Sandbox application that uses Zgine library
project(ZgineSandbox VERSION 1.0.0 LANGUAGES CXX)

# Find or use parent's ZgineRuntime target
if(NOT TARGET ZgineRuntime)
    # If built standalone, find installed Zgine
    find_package(Zgine REQUIRED)
    # ZgineRuntime is exported as Zgine::ZgineRuntime
    set(ZgineRuntime Zgine::ZgineRuntime)
else()
    # If built as part of Zgine project, use the target directly
    message(STATUS "Using ZgineRuntime from parent project")
endif()

# Sandbox executable
set(_sandbox_sources main.cpp)
if(WIN32)
    list(APPEND _sandbox_sources ZgineSandbox.rc)
endif()

if(APPLE)
    set(_sandbox_icon "${CMAKE_SOURCE_DIR}/assets/icons/app/ZgineSandbox.icns")
    if(EXISTS "${_sandbox_icon}")
        add_executable(ZgineSandbox MACOSX_BUNDLE ${_sandbox_sources} ${_sandbox_icon})
        set_source_files_properties(${_sandbox_icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set_target_properties(ZgineSandbox PROPERTIES MACOSX_BUNDLE_ICON_FILE "ZgineSandbox.icns")
    else()
        add_executable(ZgineSandbox ${_sandbox_sources})
    endif()
else()
    add_executable(ZgineSandbox ${_sandbox_sources})
endif()

# The sandbox renders directly to the OpenGL default framebuffer for teaching
# purposes, so it owns its explicit glad dependency instead of receiving it
# through the runtime target.
target_link_libraries(ZgineSandbox PRIVATE ZgineRuntime glad)

# C++ standard
set_target_properties(ZgineSandbox PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install rules (optional)
if(UNIX AND NOT APPLE)
    install(TARGETS ZgineSandbox RUNTIME DESTINATION bin)
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/ZgineSandbox.desktop")
        install(FILES ${CMAKE_SOURCE_DIR}/cmake/ZgineSandbox.desktop DESTINATION share/applications)
    endif()
    # Install icons if they exist
    foreach(_size 16 32 64 128 256)
        set(_icon "${CMAKE_SOURCE_DIR}/assets/icons/app/zgine-${_size}.png")
        if(EXISTS "${_icon}")
            install(FILES ${_icon} DESTINATION share/icons/hicolor/${_size}x${_size}/apps RENAME zgine.png)
        endif()
    endforeach()
endif()

# ============================================================================
# Post-Build: Copy assets to output directory
# ============================================================================
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
if(EXISTS ${ASSETS_DIR})
    add_custom_command(TARGET ZgineSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DASSETS_SOURCE=${ASSETS_DIR}
            -DASSETS_DEST=$<TARGET_FILE_DIR:ZgineSandbox>/assets
            -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CopyAssetsIfMissing.cmake
        COMMENT "Ensuring assets are available in sandbox output directory..."
    )
endif()
