# Zgine Third-Party Dependencies

All dependencies are managed via CMake FetchContent unless otherwise noted.

## Core Libraries
- https://github.com/skypjack/entt.git - Entity Component System
- https://github.com/g-truc/glm.git - OpenGL Mathematics
- https://github.com/gabime/spdlog.git - Logging library

## Graphics & Rendering
- https://github.com/ocornut/imgui.git - Immediate Mode GUI
- https://github.com/CedricGuillemet/ImGuizmo.git - 3D gizmo for ImGui
- https://github.com/assimp/assimp.git - Asset Import Library
- https://github.com/nothings/stb - Single-file public domain libraries (image loading, etc.)

## Physics & Audio
- https://github.com/jrouwe/JoltPhysics.git - Physics Engine
- https://github.com/mackron/miniaudio.git - Audio Library

## Scripting
- https://github.com/ThePhD/sol2.git - Lua C++ binding

## Utilities
- https://github.com/nlohmann/json.git - JSON library
- https://github.com/mariusbancila/stduuid.git - UUID library
- https://github.com/icculus/physfs.git - Virtual File System
- https://github.com/syoyo/tinyexr.git - EXR image format support

## Development Tools (Optional)
- https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus.git - Runtime C++ compilation (via ZGINE_ENABLE_RCC option)
- https://github.com/google/googletest.git - Testing framework (when ZGINE_BUILD_TESTS=ON)

## Vendor (Manual Integration)
- GLAD - OpenGL loader (custom generated, located in vendor/glad)

## Notes
- All GitHub dependencies are fetched automatically via CMake FetchContent
- Version locking is managed in `cmake/deps_versions.cmake`
- Use `ZGINE_USE_SYSTEM_DEPS=ON` to prefer system-installed libraries
