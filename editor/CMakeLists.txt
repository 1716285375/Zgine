cmake_minimum_required(VERSION 3.20)

# ZgineEditor application
# Editor is a separate executable that links to ZgineRuntime
# Runtime does NOT depend on Editor, ensuring one-way dependency
project(ZgineEditor VERSION 1.0.0 LANGUAGES CXX)

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

# Editor core source files
file(GLOB_RECURSE EDITOR_CORE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/Zgine/Editor/*.h"
)

add_library(ZgineEditorCore STATIC ${EDITOR_CORE_SOURCES})

target_link_libraries(ZgineEditorCore
    PUBLIC ZgineRuntime
    PRIVATE ImGuizmo::ImGuizmo
)

target_include_directories(ZgineEditorCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(ZgineEditorCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# ImGuizmo is PRIVATE to ZgineRuntime, so editor core needs its include path.
FetchContent_GetProperties(imguizmo)
if(imguizmo_POPULATED)
    target_include_directories(ZgineEditorCore PRIVATE ${imguizmo_SOURCE_DIR})
endif()

set_target_properties(ZgineEditorCore PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set_project_warnings(ZgineEditorCore)

if(ZGINE_USE_PCH AND TARGET ZgineRuntime)
    target_precompile_headers(ZgineEditorCore REUSE_FROM ZgineRuntime)
endif()

# Editor executable
set(_editor_sources "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
if(WIN32)
    # Add resource file if exists
    set(_editor_rc "${CMAKE_CURRENT_SOURCE_DIR}/ZgineEditor.rc")
    if(EXISTS "${_editor_rc}")
        list(APPEND _editor_sources ${_editor_rc})
    endif()
endif()

if(APPLE)
    set(_editor_icon "${CMAKE_SOURCE_DIR}/assets/icons/app/ZgineEditor.icns")
    if(EXISTS "${_editor_icon}")
        add_executable(ZgineEditor MACOSX_BUNDLE ${_editor_sources} ${_editor_icon})
        set_source_files_properties(${_editor_icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set_target_properties(ZgineEditor PROPERTIES MACOSX_BUNDLE_ICON_FILE "ZgineEditor.icns")
    else()
        add_executable(ZgineEditor ${_editor_sources})
    endif()
else()
    add_executable(ZgineEditor ${_editor_sources})
endif()

# Link to editor core - Editor depends on Runtime through ZgineEditorCore, not vice versa.
# main.cpp also uses ImGuizmo directly for editor input focus checks.
target_link_libraries(ZgineEditor PRIVATE ZgineEditorCore ImGuizmo::ImGuizmo)

# Include Editor headers
target_include_directories(ZgineEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# C++ standard
set_target_properties(ZgineEditor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Apply compiler warnings
set_project_warnings(ZgineEditor)

# Reuse PCH from ZgineRuntime if enabled
if(ZGINE_USE_PCH AND TARGET ZgineRuntime)
    target_precompile_headers(ZgineEditor REUSE_FROM ZgineRuntime)
endif()

# Install rules (optional)
if(UNIX AND NOT APPLE)
    install(TARGETS ZgineEditor RUNTIME DESTINATION bin)
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/ZgineEditor.desktop")
        install(FILES ${CMAKE_SOURCE_DIR}/cmake/ZgineEditor.desktop DESTINATION share/applications)
    endif()
    # Install icons if they exist
    foreach(_size 16 32 64 128 256)
        set(_icon "${CMAKE_SOURCE_DIR}/assets/icons/app/zgine-editor-${_size}.png")
        if(EXISTS "${_icon}")
            install(FILES ${_icon} DESTINATION share/icons/hicolor/${_size}x${_size}/apps RENAME zgine-editor.png)
        endif()
    endforeach()
endif()

# ============================================================================
# Post-Build: Copy assets to output directory
# ============================================================================
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../assets)
if(EXISTS ${ASSETS_DIR})
    add_custom_command(TARGET ZgineEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -DASSETS_SOURCE=${ASSETS_DIR}
            -DASSETS_DEST=$<TARGET_FILE_DIR:ZgineEditor>/assets
            -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CopyAssetsIfMissing.cmake
        COMMENT "Ensuring assets are available in editor output directory..."
    )
endif()
