# CMake Export/Install Fixes

## Issues Fixed

### 1. INTERFACE_INCLUDE_DIRECTORIES with Source Directory Paths

**Problem**: CMake doesn't allow source directory paths in INTERFACE_INCLUDE_DIRECTORIES when exporting targets.

**Solution**: Use generator expressions to differentiate between build and install interfaces:

```cmake
target_include_directories(Zgine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
```

### 2. Dependencies Not in Export Set

**Problem**: CMake's `install(EXPORT)` checks all linked targets, including PRIVATE dependencies.

**Solution**: 
- Mark implementation dependencies as PRIVATE (not PUBLIC)
- Only expose what downstream projects actually need (EnTT, glm)
- PRIVATE dependencies are not exported and must be available via `find_package()` or `FetchContent` in downstream projects

```cmake
# PUBLIC dependencies - exposed to downstream projects
target_link_libraries(Zgine PUBLIC
    EnTT::EnTT
    glm::glm
)

# PRIVATE dependencies - implementation details, not exported
target_link_libraries(Zgine PRIVATE
    spdlog::spdlog
    glfw
    glad
    stb_image
    assimp
    Jolt
    imgui::imgui
    ImGuizmo::ImGuizmo
    miniaudio::miniaudio
    nlohmann_json::nlohmann_json
    lua::lua
)
```

### 3. ImGui Target Not Found

**Problem**: ImGui doesn't provide CMake targets, so we need to create them manually after FetchContent.

**Solution**: Check if imgui was populated and create the target if it doesn't exist:

```cmake
FetchContent_GetProperties(imgui)
if(imgui_POPULATED)
    if(NOT TARGET imgui)
        add_library(imgui STATIC ...)
        # ... configure imgui target
    endif()
    if(TARGET imgui AND NOT TARGET imgui::imgui)
        add_library(imgui::imgui ALIAS imgui)
    endif()
endif()
```

## Key Principles

1. **PUBLIC vs PRIVATE**: Only mark dependencies as PUBLIC if downstream projects need direct access to them
2. **Generator Expressions**: Use `$<BUILD_INTERFACE:...>` and `$<INSTALL_INTERFACE:...>` for paths that differ between build and install
3. **Export Sets**: Only targets explicitly installed are exported. PRIVATE dependencies are not exported
4. **Manual Targets**: For libraries without CMake support, create targets after FetchContent population

## Testing

After these fixes, the build should:
- Configure without errors about INTERFACE_INCLUDE_DIRECTORIES
- Export only Zgine target (not its PRIVATE dependencies)
- Successfully create imgui and other manual targets
