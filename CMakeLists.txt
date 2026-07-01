cmake_minimum_required(VERSION 3.20)

# Disable package dependency export to avoid install(EXPORT) checking PRIVATE dependencies
# This tells CMake not to check dependencies when exporting targets
set(CMAKE_EXPORT_PACKAGE_DEPENDENCIES OFF CACHE BOOL "Export package dependencies" FORCE)

# ============================================================================
# Project Configuration
# ============================================================================
project(Zgine
    VERSION 1.0.0
    DESCRIPTION "Modern Data-Oriented 3D Game Engine"
    LANGUAGES CXX C
)

# Silence CMake developer warnings from third-party projects.
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

# ============================================================================
# Build Options
# ============================================================================
option(ZGINE_BUILD_TESTS "Build unit tests" ON)
option(ZGINE_BUILD_EXAMPLES "Build example projects" OFF)
option(ZGINE_ENABLE_ASSERTIONS "Enable runtime assertions" ON)
option(ZGINE_ENABLE_PROFILING "Enable profiling markers" OFF)
option(ZGINE_USE_PCH "Use precompiled headers" ON)
option(ZGINE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ZGINE_BUILD_SHARED "Build Zgine as a shared library" OFF)
option(ZGINE_BUILD_SANDBOX "Build the sandbox app" ON)
option(ZGINE_BUILD_EDITOR "Build the editor application" OFF)
option(ZGINE_ENABLE_LTO "Enable Link Time Optimization" OFF)
option(ZGINE_ENABLE_SANITIZER_ADDRESS "Enable AddressSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_UNDEFINED "Enable UndefinedBehaviorSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_THREAD "Enable ThreadSanitizer" OFF)
option(ZGINE_ENABLE_SANITIZER_MEMORY "Enable MemorySanitizer" OFF)
set(ZGINE_WARNING_LEVEL "high" CACHE STRING "Warning level: off, medium, high, error")
option(ZGINE_GUI_NUKLEAR "Use Nuklear GUI backend instead of ImGui" OFF)
option(ZGINE_ENABLE_RCC "Enable Runtime Compiled C++ for rapid development" OFF)
set(ZGINE_RENDERER_BACKEND "OpenGL" CACHE STRING "Default renderer backend: OpenGL, DirectX12, Vulkan, None")
set_property(CACHE ZGINE_RENDERER_BACKEND PROPERTY STRINGS OpenGL DirectX12 Vulkan None)

if(NOT ZGINE_RENDERER_BACKEND MATCHES "^(OpenGL|DirectX12|Vulkan|None)$")
    message(FATAL_ERROR "ZGINE_RENDERER_BACKEND must be one of: OpenGL, DirectX12, Vulkan, None")
endif()

if(ZGINE_GUI_NUKLEAR)
    add_compile_definitions(ZGINE_GUI_NUKLEAR)
endif()

# ============================================================================
# C++ Standard & Global Settings
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32 AND ZGINE_BUILD_SHARED)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# ============================================================================
# Output Directories
# ============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Compiler Options & Toolchain
# ============================================================================
include(cmake/CompilerOptions.cmake)

# ============================================================================
# Dependencies Management (System libraries preferred, FetchContent fallback)
# ============================================================================
include(cmake/Dependencies.cmake)

# Fix third-party library include directories for install/export
include(cmake/FixThirdPartyIncludes.cmake)

find_package(Vulkan QUIET)
set(ZGINE_HAS_VULKAN OFF)
if(Vulkan_FOUND)
    set(ZGINE_HAS_VULKAN ON)
    message(STATUS "Found Vulkan SDK: ${Vulkan_VERSION}")
elseif(ZGINE_RENDERER_BACKEND STREQUAL "Vulkan")
    message(FATAL_ERROR "ZGINE_RENDERER_BACKEND=Vulkan requires the Vulkan SDK. Set VULKAN_SDK or install the Vulkan SDK.")
else()
    message(STATUS "Vulkan SDK not found. Vulkan backend sources will be excluded.")
endif()

# ============================================================================
# Source Files (Runtime only - excludes Editor)
# ============================================================================
# Exclude Editor sources from Runtime to maintain strict one-way dependency:
# Runtime does NOT depend on Editor, ensuring game builds are minimal and portable
file(GLOB_RECURSE ZGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/include/Zgine/*.h"
)

# Keep editor implementation out of the runtime target. Editor code is built by
# ZgineEditorCore in editor/CMakeLists.txt.
list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/Editor/")
list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/include/Zgine/Editor/")

# Exclude non-GLM math backend implementations to avoid duplicate definitions
# We use GLM as the backend, so generic implementations should not be compiled
list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/Core/Math/Vector[234]\\.cpp$")
list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/Core/Math/Matrix[34]\\.cpp$")
list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/Core/Math/Math\\.cpp$")

if(NOT ZGINE_HAS_VULKAN)
    list(FILTER ZGINE_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/Renderer/Backend/Vulkan/")
endif()

# ============================================================================
# ZgineRuntime Target (Core Engine Library)
# ============================================================================
set(_zgine_engine_type STATIC)
if(ZGINE_BUILD_SHARED)
    set(_zgine_engine_type SHARED)
endif()

add_library(ZgineRuntime ${_zgine_engine_type} ${ZGINE_SOURCES})

# 包含路径配置
# Use generator expressions to handle build vs install interface
target_include_directories(ZgineRuntime PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)



# Jolt include path is an implementation detail hidden behind PhysicsSystem.
if(DEFINED jolt_SOURCE_DIR)
    target_include_directories(ZgineRuntime PRIVATE $<BUILD_INTERFACE:${jolt_SOURCE_DIR}>)
endif()

# Sol2 include path is an implementation detail hidden behind ScriptSystem.
if(DEFINED sol2_SOURCE_DIR)
    target_include_directories(ZgineRuntime PRIVATE $<BUILD_INTERFACE:${sol2_SOURCE_DIR}/include>)
endif()

# TinyEXR include paths (if fetched)
if(DEFINED tinyexr_SOURCE_DIR)
    target_include_directories(ZgineRuntime PUBLIC $<BUILD_INTERFACE:${tinyexr_SOURCE_DIR}>)
    if(EXISTS "${tinyexr_SOURCE_DIR}/deps/miniz")
        target_include_directories(ZgineRuntime PUBLIC $<BUILD_INTERFACE:${tinyexr_SOURCE_DIR}/deps/miniz>)
    endif()
endif()


# 链接第三方库
# Link third-party libraries
# PUBLIC: Used in public API headers (include/ and src/ headers exposed to users)
# EnTT and GLM types still appear in the ECS public API, so they remain PUBLIC.
# glad appears in include/Zgine/Renderer/GL.h
target_link_libraries(ZgineRuntime PUBLIC
    glm::glm
    EnTT::EnTT
    glad
    spdlog::spdlog
    stduuid
    glfw
    nlohmann_json::nlohmann_json
    assimp
)

# PRIVATE: Implementation details not exposed in public API
target_link_libraries(ZgineRuntime PRIVATE
    stb_image
    Jolt
    lua::lua
    sol2
    imgui::imgui
    ImGuizmo::ImGuizmo
    miniaudio::miniaudio
    physfs-static
)

if(ZGINE_HAS_VULKAN)
    target_link_libraries(ZgineRuntime PRIVATE Vulkan::Vulkan)
endif()

# RuntimeCompiledCPlusPlus (optional)
if(ZGINE_ENABLE_RCC AND TARGET RuntimeCompiler)
    target_link_libraries(ZgineRuntime PUBLIC
        RuntimeCompiler
        RuntimeObjectSystem
    )
    target_compile_definitions(ZgineRuntime PUBLIC ZGINE_RCC_ENABLED)
endif()


# Note: All dependencies are PRIVATE and built via FetchContent.
# CMake 3.20+ should automatically exclude PRIVATE dependencies from export checks,
# but if errors persist, we may need to handle them differently.

target_compile_definitions(ZgineRuntime PUBLIC
    GLM_ENABLE_EXPERIMENTAL
    ZGINE_DEFAULT_RENDERER_BACKEND="${ZGINE_RENDERER_BACKEND}"
    ZGINE_HAS_VULKAN=$<BOOL:${ZGINE_HAS_VULKAN}>
)

if(ZGINE_BUILD_EDITOR)
    target_compile_definitions(ZgineRuntime PUBLIC ZGINE_ENABLE_EDITOR)
endif()

# Jolt 专用宏定义 (防止 ABI 冲突)
if(TARGET Jolt)
    target_compile_definitions(ZgineRuntime PRIVATE
        JPH_FLOATING_POINT_EXCEPTIONS_ENABLED=0
        JPH_DEBUG_RENDERER
    )
endif()

# Enable assertions if enabled
if(ZGINE_ENABLE_ASSERTIONS)
    target_compile_definitions(ZgineRuntime PUBLIC ZGINE_ENABLE_ASSERTIONS)
endif()

set_project_warnings(ZgineRuntime)

# ============================================================================
# Precompiled Headers (加快编译速度)
# ============================================================================
if(ZGINE_USE_PCH)
    target_precompile_headers(ZgineRuntime PRIVATE
        ${CMAKE_SOURCE_DIR}/include/Zgine/Core/PCH/PCH.h
    )
endif()

if(ZGINE_HAS_VULKAN)
    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/src/Renderer/Backend/Vulkan/VulkanRendererAPI.cpp
        ${CMAKE_SOURCE_DIR}/src/Platform/VulkanSurface.cpp
        PROPERTIES SKIP_PRECOMPILE_HEADERS ON
    )
endif()



# ============================================================================
# Applications: Sandbox, Editor, and Tests
# All are separate executables that link to ZgineRuntime
# Runtime and Editor have strict one-way dependency: Runtime does NOT depend on Editor
# ============================================================================
if(ZGINE_BUILD_SANDBOX OR ZGINE_BUILD_EXAMPLES)
    add_subdirectory(sandbox)
endif()

if(ZGINE_BUILD_EDITOR)
    add_subdirectory(editor)
endif()

if(ZGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Asset copying is now handled in sandbox/CMakeLists.txt

# ============================================================================
# Install & Export Configuration
# ============================================================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install ZgineRuntime target
# Note: Third-party dependencies are linked as PRIVATE, so they won't be exported
install(TARGETS ZgineRuntime
    EXPORT ZgineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT ZgineRuntime
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install CMake config files
set(ZGINE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Zgine)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/ZgineConfig.cmake.in
    ${CMAKE_BINARY_DIR}/ZgineConfig.cmake
    INSTALL_DESTINATION ${ZGINE_CONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/ZgineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_BINARY_DIR}/ZgineConfig.cmake
    ${CMAKE_BINARY_DIR}/ZgineConfigVersion.cmake
    DESTINATION ${ZGINE_CONFIG_INSTALL_DIR}
)

# Export ZgineRuntime target
# Note: Only ZgineRuntime is exported. Its PRIVATE dependencies are not exported
# and must be available via find_package() or FetchContent in downstream projects.
#
# All dependencies are built via FetchContent and are not in any export set.
# Workaround: Install all PRIVATE dependencies to the same export set to satisfy CMake's checks.
# These will be exported but downstream projects don't need to use them directly.
set(_zgine_private_deps
    spdlog
    glfw
    glad
    glm
    glm-header-only  # GLM internal target
    EnTT
    stb_image
    stb_image_write
    stb_truetype
    stb_image_resize2
    assimp
    zlibstatic  # assimp dependency
    Jolt
    imgui
    imguizmo
    ImGuizmo
    miniaudio
    physfs-static  # PhysicsFS
    nlohmann_json
    lua
    sol2  # Lua wrapper (header-only)
    stduuid  # UUID library (header-only)
    nuklear
)

# Add RCC++ libraries if enabled
if(ZGINE_ENABLE_RCC)
    list(APPEND _zgine_private_deps RuntimeCompiler RuntimeObjectSystem)
endif()

foreach(_dep IN LISTS _zgine_private_deps)
    if(TARGET ${_dep})
        # Install to the same export set to satisfy CMake's checks
        install(TARGETS ${_dep} EXPORT ZgineTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT ZgineRuntime
        )
    endif()
endforeach()

# Export ZgineRuntime with namespace Zgine::
# Downstream projects use: target_link_libraries(MyApp PRIVATE Zgine::ZgineRuntime)
install(EXPORT ZgineTargets
    FILE ZgineTargets.cmake
    NAMESPACE Zgine::
    DESTINATION ${ZGINE_CONFIG_INSTALL_DIR}
    COMPONENT ZgineRuntime
)

# ============================================================================
# CPack Configuration
# ============================================================================
set(CPACK_PACKAGE_NAME "Zgine")
set(CPACK_PACKAGE_VENDOR "Zgine Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern Data-Oriented 3D Game Engine")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# Optional resource files
if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
endif()

# Platform-specific settings
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "Zgine ${PROJECT_VERSION}")
    set(CPACK_NSIS_DISPLAY_NAME "Zgine ${PROJECT_VERSION}")
    set(CPACK_NSIS_HELP_LINK "https://github.com/yourusername/zgine")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/yourusername/zgine")
    set(CPACK_NSIS_CONTACT "your-email@example.com")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/assets/icons/app/ZgineSandbox.icns")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Zgine Project")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Summary Output
message(STATUS "-------------------------------------------------------")
message(STATUS "  Zgine Build Configuration")
message(STATUS "-------------------------------------------------------")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Shared: ${ZGINE_BUILD_SHARED}")
message(STATUS "  Build Tests: ${ZGINE_BUILD_TESTS}")
message(STATUS "  Build Sandbox: ${ZGINE_BUILD_SANDBOX}")
message(STATUS "  Build Editor: ${ZGINE_BUILD_EDITOR}")
message(STATUS "  Renderer Backend: ${ZGINE_RENDERER_BACKEND}")
message(STATUS "  Vulkan SDK: ${ZGINE_HAS_VULKAN}")
message(STATUS "  Use PCH: ${ZGINE_USE_PCH}")
message(STATUS "  Warnings as Errors: ${ZGINE_WARNINGS_AS_ERRORS}")
message(STATUS "  Enable LTO: ${ZGINE_ENABLE_LTO}")
message(STATUS "  Enable Sanitizers: ${ZGINE_ENABLE_SANITIZER_ADDRESS}")
message(STATUS "  Use System Dependencies: ${ZGINE_USE_SYSTEM_DEPS}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "-------------------------------------------------------")
