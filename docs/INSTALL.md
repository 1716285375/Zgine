# Zgine Installation & Usage Guide

## Installation

### Building from Source

```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Install (optional)
cmake --build build --target install
```

### Using CMake Presets

```bash
# List available presets
cmake --list-presets

# Configure with preset
cmake --preset windows-release

# Build
cmake --build build

# Run tests
ctest --preset test-ci
```

## Using Zgine in Your Project

After installing Zgine, you can use it in your CMake project:

```cmake
find_package(Zgine REQUIRED)
target_link_libraries(YourTarget PRIVATE Zgine::ZgineRuntime)
```

Or with version requirements:

```cmake
find_package(Zgine 1.0.0 REQUIRED)
target_link_libraries(YourTarget PRIVATE Zgine::ZgineRuntime)
```

**Note**: Zgine's dependencies are built via FetchContent. If you have system-installed versions of these dependencies, they will be used automatically. Otherwise, you may need to use FetchContent in your project to provide them.

## Build Options

### Core Options
- `ZGINE_BUILD_TESTS`: Build unit tests (default: ON)
- `ZGINE_BUILD_SANDBOX`: Build the sandbox app (default: ON)
- `ZGINE_BUILD_SHARED`: Build as shared library (default: OFF)
- `ZGINE_USE_PCH`: Use precompiled headers (default: ON)

### Quality Options
- `ZGINE_WARNINGS_AS_ERRORS`: Treat warnings as errors (default: OFF)
- `ZGINE_WARNING_LEVEL`: Warning level: off, medium, high, error (default: high)
- `ZGINE_ENABLE_ASSERTIONS`: Enable runtime assertions (default: ON)

### Toolchain Options
- `ZGINE_ENABLE_LTO`: Enable Link Time Optimization (default: OFF)
- `ZGINE_ENABLE_SANITIZER_ADDRESS`: Enable AddressSanitizer (default: OFF)
- `ZGINE_ENABLE_SANITIZER_UNDEFINED`: Enable UndefinedBehaviorSanitizer (default: OFF)
- `ZGINE_ENABLE_SANITIZER_THREAD`: Enable ThreadSanitizer (default: OFF)
- `ZGINE_ENABLE_SANITIZER_MEMORY`: Enable MemorySanitizer (default: OFF, Clang only)

### Dependency Options
- `ZGINE_USE_SYSTEM_DEPS`: Use system-installed dependencies when available (default: ON)

## CMake Presets

### Development Presets
- `dev`: Development build with assertions and profiling
- `windows-debug`, `linux-debug`, `macos-debug`: Platform-specific debug builds
- `windows-release`, `linux-release`, `macos-release`: Platform-specific release builds

### CI Presets
- `ci`: Generic CI build (Release, tests enabled, warnings as errors)
- `ci-linux`, `ci-windows`, `ci-macos`: Platform-specific CI builds

### Toolchain Presets
- `sanitizer-address`: Debug build with AddressSanitizer
- `sanitizer-undefined`: Debug build with UndefinedBehaviorSanitizer
- `release-lto`: Release build with Link Time Optimization
- `release-optimized`: Release build with LTO and PCH

## Packaging

Zgine supports CPack for creating distribution packages:

```bash
# Configure with CPack
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Create package
cmake --build build --target package
```

Supported package formats:
- Windows: ZIP, NSIS installer
- macOS: TGZ, DMG
- Linux: TGZ, DEB, RPM

## Dependency Management

Zgine uses a unified dependency management system that:
1. **Prefers system libraries**: Tries to find dependencies via `find_package()` first
2. **Falls back to FetchContent**: Downloads and builds dependencies if not found
3. **Version locking**: Supports version locking via `cmake/deps_versions.cmake`

### Locking Dependency Versions

Edit `cmake/deps_versions.cmake` to lock specific versions:

```cmake
set(ZGINE_DEPS_spdlog_GIT_TAG "v1.14.0")
set(ZGINE_DEPS_glfw_GIT_TAG "3.4")
```

## Examples

### Debug Build with Sanitizers

```bash
cmake --preset sanitizer-address
cmake --build build
```

### Release Build with LTO

```bash
cmake --preset release-lto
cmake --build build
```

### CI Build

```bash
cmake --preset ci-linux
cmake --build build
ctest --preset test-ci
```

## Troubleshooting

### Dependency Issues

If you encounter dependency issues:
1. Ensure system dependencies are installed (if `ZGINE_USE_SYSTEM_DEPS=ON`)
2. Check `cmake/deps_versions.cmake` for version locks
3. Clear CMake cache: `rm -rf build`

### Installation Issues

If `find_package(Zgine)` fails:
1. Ensure Zgine is installed: `cmake --build build --target install`
2. Set `CMAKE_PREFIX_PATH` to installation directory
3. Check that `ZgineConfig.cmake` exists in `lib/cmake/Zgine/`

