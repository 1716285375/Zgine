# ============================================================================
# Unified Dependency Management
# System libraries preferred, with FetchContent fallback and version locking
# ============================================================================

include(FetchContent)
include(CMakePackageConfigHelpers)

# Dependency version lock file
set(ZGINE_DEPS_VERSION_FILE "${CMAKE_SOURCE_DIR}/cmake/deps_versions.cmake")

# Load version lock if exists
if(EXISTS "${ZGINE_DEPS_VERSION_FILE}")
    include("${ZGINE_DEPS_VERSION_FILE}")
endif()

# ============================================================================
# Helper Function: Find or Fetch Dependency
# ============================================================================
function(zgine_find_or_fetch_dependency dep_name)
    set(options REQUIRED)
    set(oneValueArgs VERSION GIT_REPOSITORY GIT_TAG FIND_PACKAGE_NAME SOURCE_SUBDIR)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(ZGINE_DEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Default values
    if(NOT ZGINE_DEP_FIND_PACKAGE_NAME)
        set(ZGINE_DEP_FIND_PACKAGE_NAME ${dep_name})
    endif()

    # Try system package first (but default to OFF for consistent builds)
    if(NOT DEFINED ZGINE_USE_SYSTEM_DEPS)
        set(ZGINE_USE_SYSTEM_DEPS OFF CACHE BOOL "Use system-installed dependencies when available")
    endif()

    set(_found_system FALSE)
    if(ZGINE_USE_SYSTEM_DEPS)
        find_package(${ZGINE_DEP_FIND_PACKAGE_NAME} ${ZGINE_DEP_VERSION} QUIET ${ZGINE_DEP_COMPONENTS})
        if(${ZGINE_DEP_FIND_PACKAGE_NAME}_FOUND)
            set(_found_system TRUE)
            message(STATUS "Found system ${dep_name}: ${${ZGINE_DEP_FIND_PACKAGE_NAME}_VERSION}")
        endif()
    endif()

    # Fallback to FetchContent
    if(NOT _found_system)
        if(NOT ZGINE_DEP_GIT_REPOSITORY)
            message(FATAL_ERROR "No system ${dep_name} found and no GIT_REPOSITORY provided")
        endif()

        # Use version lock if available
        if(DEFINED ZGINE_DEPS_${dep_name}_GIT_TAG)
            set(ZGINE_DEP_GIT_TAG ${ZGINE_DEPS_${dep_name}_GIT_TAG})
            message(STATUS "Using locked version for ${dep_name}: ${ZGINE_DEP_GIT_TAG}")
        endif()

        # Build FetchContent_Declare arguments
        set(_fc_args
            GIT_REPOSITORY ${ZGINE_DEP_GIT_REPOSITORY}
            GIT_TAG ${ZGINE_DEP_GIT_TAG}
            GIT_SHALLOW TRUE
        )
        if(ZGINE_DEP_SOURCE_SUBDIR)
            list(APPEND _fc_args SOURCE_SUBDIR ${ZGINE_DEP_SOURCE_SUBDIR})
        endif()

        FetchContent_Declare(${dep_name} ${_fc_args})
        FetchContent_MakeAvailable(${dep_name})
        message(STATUS "Fetched ${dep_name} from ${ZGINE_DEP_GIT_REPOSITORY} (${ZGINE_DEP_GIT_TAG})")
    endif()
endfunction()

# ============================================================================
# Dependency Declarations with Version Locking
# ============================================================================

# spdlog
zgine_find_or_fetch_dependency(spdlog
    VERSION 1.14.0
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.0
)
if(TARGET spdlog)
    set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
endif()

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

zgine_find_or_fetch_dependency(glfw
    VERSION 3.4
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

# GLM
set(_zgine_saved_warn_deprecated "${CMAKE_WARN_DEPRECATED}")
set(CMAKE_WARN_DEPRECATED OFF)
zgine_find_or_fetch_dependency(glm
    VERSION 1.0.1
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
if(_zgine_saved_warn_deprecated)
    set(CMAKE_WARN_DEPRECATED "${_zgine_saved_warn_deprecated}")
else()
    unset(CMAKE_WARN_DEPRECATED)
endif()

# EnTT
zgine_find_or_fetch_dependency(entt
    VERSION 3.13.2
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
)

# Assimp
set(_zgine_saved_message_log_level "${CMAKE_MESSAGE_LOG_LEVEL}")
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
zgine_find_or_fetch_dependency(assimp
    VERSION 5.4.3
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
)
if(TARGET assimp)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
endif()
if(_zgine_saved_message_log_level)
    set(CMAKE_MESSAGE_LOG_LEVEL "${_zgine_saved_message_log_level}")
else()
    unset(CMAKE_MESSAGE_LOG_LEVEL)
endif()

# Jolt Physics
# Jolt requires special handling with SOURCE_SUBDIR
# IMPORTANT: These cache variables must be set BEFORE FetchContent to take effect
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(JPH_FLOATING_POINT_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
zgine_find_or_fetch_dependency(jolt
    VERSION 4.0.2
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG v4.0.2
    SOURCE_SUBDIR Build
    FIND_PACKAGE_NAME JoltPhysics
)

# ImGui
zgine_find_or_fetch_dependency(imgui
    VERSION 1.92.5
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.5-docking
)
if(TARGET imgui)
    set(IMGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif()

# ImGuizmo
zgine_find_or_fetch_dependency(imguizmo
    VERSION master
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
)

# miniaudio
zgine_find_or_fetch_dependency(miniaudio
    VERSION 0.11.23
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.23
)

# nlohmann/json
zgine_find_or_fetch_dependency(json
    VERSION 3.11.3
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    FIND_PACKAGE_NAME nlohmann_json
)

# Lua
zgine_find_or_fetch_dependency(lua
    VERSION 5.4.6
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.4.6
)

# Sol2
zgine_find_or_fetch_dependency(sol2
    VERSION 3.5.0
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.5.0
)

# stduuid (Header-only)
FetchContent_Declare(stduuid
    GIT_REPOSITORY https://github.com/mariusbancila/stduuid.git
    GIT_TAG v1.2.3
)
FetchContent_MakeAvailable(stduuid)

# Nuklear
zgine_find_or_fetch_dependency(nuklear
    VERSION 4.12.8
    GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
    GIT_TAG 4.12.8
)
if(NOT TARGET nuklear)
    FetchContent_GetProperties(nuklear)
    if(nuklear_POPULATED)
        add_library(nuklear INTERFACE)
        target_include_directories(nuklear INTERFACE
            $<BUILD_INTERFACE:${nuklear_SOURCE_DIR}>
            $<BUILD_INTERFACE:${nuklear_SOURCE_DIR}/demo>
        )
        if(NOT TARGET nuklear::nuklear)
            add_library(nuklear::nuklear ALIAS nuklear)
        endif()
    endif()
endif()

# RuntimeCompiledCPlusPlus (optional)
# Enables runtime C++ compilation for rapid development iteration
if(ZGINE_ENABLE_RCC)
    message(STATUS "RuntimeCompiledCPlusPlus enabled")

    # RCC++ has specific requirements and uses CMake
    set(RCC_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RCC_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    zgine_find_or_fetch_dependency(rcc
        GIT_REPOSITORY https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git
        GIT_TAG master
        FIND_PACKAGE_NAME RuntimeCompiledCPlusPlus
    )

    # Create alias if needed
    if(TARGET RuntimeCompiler AND NOT TARGET RCC::RuntimeCompiler)
        add_library(RCC::RuntimeCompiler ALIAS RuntimeCompiler)
    endif()
    if(TARGET RuntimeObjectSystem AND NOT TARGET RCC::RuntimeObjectSystem)
        add_library(RCC::RuntimeObjectSystem ALIAS RuntimeObjectSystem)
    endif()
else()
    message(STATUS "RuntimeCompiledCPlusPlus disabled (use -DZGINE_ENABLE_RCC=ON to enable)")
endif()

# TinyEXR (header-only)
FetchContent_Declare(tinyexr
    GIT_REPOSITORY https://github.com/syoyo/tinyexr.git
    GIT_TAG v1.0.8
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(tinyexr)
if(NOT tinyexr_POPULATED)
    FetchContent_Populate(tinyexr)
endif()

# PhysicsFS - Virtual File System
# Disable all optional features to avoid conflicts
set(PHYSFS_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(PHYSFS_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(PHYSFS_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(physfs
    GIT_REPOSITORY https://github.com/icculus/physfs.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(physfs)

# GoogleTest (tests only)
if(ZGINE_BUILD_TESTS)
    zgine_find_or_fetch_dependency(googletest
        VERSION 1.14.0
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        FIND_PACKAGE_NAME GTest
    )
    if(TARGET gtest)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
endif()

# ============================================================================
# Manual Library Setup (ImGui, ImGuizmo, Lua, miniaudio)
# ============================================================================

# ImGui manual setup
# ImGui doesn't provide CMake targets, so we need to create them manually
FetchContent_GetProperties(imgui)
if(imgui_POPULATED)
    if(NOT TARGET imgui)
        add_library(imgui STATIC
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
        target_include_directories(imgui PUBLIC
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
        )
        target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
    endif()
    if(TARGET imgui AND NOT TARGET imgui::imgui)
        add_library(imgui::imgui ALIAS imgui)
    endif()
endif()

# ImGuizmo manual setup
FetchContent_GetProperties(imguizmo)
if(imguizmo_POPULATED)
    if(NOT TARGET ImGuizmo)
        add_library(ImGuizmo STATIC
            ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
        )
        target_include_directories(ImGuizmo PUBLIC
            $<BUILD_INTERFACE:${imguizmo_SOURCE_DIR}>
        )
        # Define IMGUI_DEFINE_MATH_OPERATORS for ImGuizmo
        # Note: CaptureMouseFromApp was removed in newer ImGui versions
        # We define IMGUI_DISABLE_OBSOLETE_FUNCTIONS=0 to keep compatibility
        target_compile_definitions(ImGuizmo PRIVATE
            IMGUI_DEFINE_MATH_OPERATORS
            IMGUI_DISABLE_OBSOLETE_FUNCTIONS=0
        )
        target_link_libraries(ImGuizmo PUBLIC imgui::imgui)
    endif()
    if(TARGET ImGuizmo AND NOT TARGET ImGuizmo::ImGuizmo)
        add_library(ImGuizmo::ImGuizmo ALIAS ImGuizmo)
    endif()
endif()

# miniaudio manual setup
if(NOT TARGET miniaudio::miniaudio)
    if(TARGET miniaudio AND DEFINED miniaudio_SOURCE_DIR)
        target_include_directories(miniaudio PUBLIC
            $<BUILD_INTERFACE:${miniaudio_SOURCE_DIR}>
        )
        add_library(miniaudio::miniaudio ALIAS miniaudio)
    elseif(DEFINED miniaudio_SOURCE_DIR)
        add_library(miniaudio STATIC ${miniaudio_SOURCE_DIR}/miniaudio.c)
        target_include_directories(miniaudio PUBLIC
            $<BUILD_INTERFACE:${miniaudio_SOURCE_DIR}>
        )
        add_library(miniaudio::miniaudio ALIAS miniaudio)
    endif()
endif()

# Lua manual setup
FetchContent_GetProperties(lua)
if(lua_POPULATED)
    if(NOT TARGET lua::lua)
        if(EXISTS "${lua_SOURCE_DIR}/src/lapi.c")
            set(_zgine_lua_src_dir "${lua_SOURCE_DIR}/src")
        else()
            set(_zgine_lua_src_dir "${lua_SOURCE_DIR}")
        endif()

        if(NOT TARGET lua)
            add_library(lua STATIC
                ${_zgine_lua_src_dir}/lapi.c
                ${_zgine_lua_src_dir}/lauxlib.c
                ${_zgine_lua_src_dir}/lbaselib.c
                ${_zgine_lua_src_dir}/lcode.c
                ${_zgine_lua_src_dir}/lcorolib.c
                ${_zgine_lua_src_dir}/lctype.c
                ${_zgine_lua_src_dir}/ldblib.c
                ${_zgine_lua_src_dir}/ldebug.c
                ${_zgine_lua_src_dir}/ldo.c
                ${_zgine_lua_src_dir}/ldump.c
                ${_zgine_lua_src_dir}/lfunc.c
                ${_zgine_lua_src_dir}/lgc.c
                ${_zgine_lua_src_dir}/linit.c
                ${_zgine_lua_src_dir}/liolib.c
                ${_zgine_lua_src_dir}/llex.c
                ${_zgine_lua_src_dir}/lmathlib.c
                ${_zgine_lua_src_dir}/lmem.c
                ${_zgine_lua_src_dir}/loadlib.c
                ${_zgine_lua_src_dir}/lobject.c
                ${_zgine_lua_src_dir}/lopcodes.c
                ${_zgine_lua_src_dir}/loslib.c
                ${_zgine_lua_src_dir}/lparser.c
                ${_zgine_lua_src_dir}/lstate.c
                ${_zgine_lua_src_dir}/lstring.c
                ${_zgine_lua_src_dir}/lstrlib.c
                ${_zgine_lua_src_dir}/ltable.c
                ${_zgine_lua_src_dir}/ltablib.c
                ${_zgine_lua_src_dir}/ltm.c
                ${_zgine_lua_src_dir}/lundump.c
                ${_zgine_lua_src_dir}/lutf8lib.c
                ${_zgine_lua_src_dir}/lvm.c
                ${_zgine_lua_src_dir}/lzio.c
            )
            target_include_directories(lua PUBLIC
                $<BUILD_INTERFACE:${_zgine_lua_src_dir}>
            )
        endif()
        if(TARGET lua AND NOT TARGET lua::lua)
            add_library(lua::lua ALIAS lua)
        endif()
    endif()
endif()

# ============================================================================
# Vendor Libraries (GLAD, stb)
# ============================================================================
set(ZGINE_VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

# GLAD
add_library(glad STATIC ${ZGINE_VENDOR_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC
    $<BUILD_INTERFACE:${ZGINE_VENDOR_DIR}/glad/include>
)
find_package(OpenGL REQUIRED)
target_link_libraries(glad PUBLIC OpenGL::GL)

# stb libraries (header-only)
# stb is a collection of single-file public domain libraries
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

# Create interface targets for commonly used stb libraries
# stb_image - image loader
if(NOT TARGET stb_image)
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    )
endif()

# stb_image_write - image writer
if(NOT TARGET stb_image_write)
    add_library(stb_image_write INTERFACE)
    target_include_directories(stb_image_write INTERFACE
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    )
endif()

# stb_truetype - font text rasterizer
if(NOT TARGET stb_truetype)
    add_library(stb_truetype INTERFACE)
    target_include_directories(stb_truetype INTERFACE
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    )
endif()

# stb_image_resize2 - image resizer
if(NOT TARGET stb_image_resize2)
    add_library(stb_image_resize2 INTERFACE)
    target_include_directories(stb_image_resize2 INTERFACE
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    )
endif()


