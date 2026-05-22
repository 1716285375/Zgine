# Zgine Build Instructions

## Prerequisites

### All Platforms
- **CMake** 3.20 or higher
- **C++20 compatible compiler**
  - Windows: MSVC 2022+ or Clang
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+ (AppleClang)
- **Git** (for fetching dependencies)

### Platform-Specific Requirements

#### Windows
- Visual Studio 2019 or 2022 (Community Edition or higher)
- Windows SDK 10.0.18362.0 or higher
- Optional: Ninja build system (recommended)

Verified local toolchain on this machine:

```text
Visual Studio root:
  C:\Program Files\Microsoft Visual Studio\2022\Community

MSVC toolset root:
  C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207

cl.exe:
  C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe

link.exe:
  C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\link.exe

Windows SDK tools:
  C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64

rc.exe:
  C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\rc.exe

mt.exe:
  C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\mt.exe

Visual Studio environment bootstrap:
  C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat
```

Important: absolute compiler paths are not enough by themselves. MSVC also needs the
Visual Studio and Windows SDK environment variables (`INCLUDE`, `LIB`, `PATH`) so the
linker can find libraries such as `kernel32.lib`. Run CMake from `VsDevCmd.bat` or a
Developer PowerShell/Command Prompt, then pass absolute tools if you want deterministic
tool selection.

Working local command used for configure, build, and test verification:

```powershell
$vsdev = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
$build = 'build\architecture-check-msvc-devcmd'

cmd.exe /S /C "`"$vsdev`" -arch=x64 -host_arch=x64 && cmake -S . -B $build -G Ninja -DCMAKE_C_COMPILER=`"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe`" -DCMAKE_CXX_COMPILER=`"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe`" -DCMAKE_LINKER=`"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\link.exe`" -DCMAKE_RC_COMPILER=`"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\rc.exe`" -DCMAKE_MT=`"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\mt.exe`" && cmake --build $build --target ZgineEditor ZgineSandbox ZgineTests ZgineEditorTests -j 8 && ctest --test-dir $build --output-on-failure"
```

Expected verified result on 2026-05-22:

```text
ZgineEditor, ZgineSandbox, ZgineTests, and ZgineEditorTests build successfully.
100% tests passed, 0 tests failed out of 50
```

#### Linux
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake ninja-build \
    libgl1-mesa-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxi-dev libx11-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake ninja-build \
    mesa-libGL-devel libXrandr-devel libXinerama-devel \
    libXcursor-devel libXi-devel libX11-devel

# Arch Linux
sudo pacman -S base-devel cmake ninja \
    mesa libxrandr libxinerama libxcursor libxi
```

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake and Ninja (using Homebrew)
brew install cmake ninja
```

---

## Vendor Dependencies Setup

Before building, you need to set up vendor libraries that are not fetched automatically:

### 1. GLAD (OpenGL Function Loader)

**Option A: Generate from Web Service (Recommended)**
1. Visit https://glad.dav1d.de/
2. Configure:
   - Language: **C/C++**
   - Specification: **OpenGL**
   - API gl: **Version 4.5** (or 4.6)
   - Profile: **Core**
   - Options: Check **Generate a loader**
3. Click **Generate** and download `glad.zip`
4. Extract to `vendor/glad/`:
   ```
   vendor/glad/
   ├── include/
   │   ├── glad/
   │   │   └── glad.h
   │   └── KHR/
   │       └── khrplatform.h
   └── src/
       └── glad.c
   ```

**Option B: Use Provided Files**
```bash
# If provided in the repository
cp -r deps/glad vendor/
```

### 2. stb_image (Image Loader)

Download the single-header library:

```bash
# Create directory
mkdir -p vendor/stb

# Download stb_image.h
curl -o vendor/stb/stb_image.h \
    https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
```

Or manually:
1. Visit https://github.com/nothings/stb
2. Download `stb_image.h`
3. Place in `vendor/stb/stb_image.h`

---

## Build Instructions

### Method 1: Using CMake Presets (Recommended)

#### Quick Start
```bash
# Configure for your platform (automatically detects OS)
cmake --preset=dev

# Build
cmake --build --preset=dev

# Run
./build/dev/bin/Zgine
```

#### Platform-Specific Builds

**Windows:**
```powershell
# Debug build
cmake --preset=windows-debug
cmake --build --preset=windows-debug

# Release build
cmake --preset=windows-release
cmake --build --preset=windows-release
```

**Linux:**
```bash
# Debug build
cmake --preset=linux-debug
cmake --build --preset=linux-debug

# Release build
cmake --preset=linux-release
cmake --build --preset=linux-release
```

**macOS:**
```bash
# Debug build
cmake --preset=macos-debug
cmake --build --preset=macos-debug

# Release build
cmake --preset=macos-release
cmake --build --preset=macos-release
```

### Method 2: Manual CMake Configuration

```bash
# Create build directory
mkdir build && cd build

# Configure (Debug)
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DZGINE_ENABLE_ASSERTIONS=ON \
         -DZGINE_USE_PCH=ON

# Build
cmake --build . --config Debug -j 8

# Run
./bin/Zgine
```

### Method 3: Visual Studio Integration (Windows)

1. Open Visual Studio
2. Select **Open a local folder**
3. Navigate to the Zgine project root
4. VS will automatically detect `CMakePresets.json`
5. Select configuration from dropdown (e.g., "windows-debug")
6. Build using **Build > Build All** (Ctrl+Shift+B)

---

## Build Options

Configure using CMake options:

```bash
cmake .. \
  -DZGINE_BUILD_TESTS=ON \
  -DZGINE_BUILD_EXAMPLES=ON \
  -DZGINE_ENABLE_ASSERTIONS=ON \
  -DZGINE_ENABLE_PROFILING=ON \
  -DZGINE_USE_PCH=ON \
  -DZGINE_WARNINGS_AS_ERRORS=OFF \
  -DZGINE_RENDERER_BACKEND=OpenGL
```

| Option | Description | Default |
|--------|-------------|---------|
| `ZGINE_BUILD_TESTS` | Build unit tests | ON |
| `ZGINE_BUILD_EXAMPLES` | Build example projects | OFF |
| `ZGINE_ENABLE_ASSERTIONS` | Enable runtime assertions | ON |
| `ZGINE_ENABLE_PROFILING` | Enable profiling markers | OFF |
| `ZGINE_USE_PCH` | Use precompiled headers | ON |
| `ZGINE_WARNINGS_AS_ERRORS` | Treat warnings as errors | OFF |
| `ZGINE_RENDERER_BACKEND` | Default renderer backend: `OpenGL`, `DirectX12`, `Vulkan`, or `None` | OpenGL |

Renderer backend status:

- `OpenGL`: implemented and used by sandbox/editor.
- `DirectX12`: selectable teaching stub; window can be created without an OpenGL context, but rendering resources are not implemented yet.
- `Vulkan`: initial teaching backend; requires the Vulkan SDK and creates instance, validation debug messenger, surface, device, queues, swapchain, and image views. RHI resources, command buffers, pipelines, and presentation are not implemented yet.
- `None`: headless/no-rendering selection for future tests and tools.

See [docs/rendering-backends.md](docs/rendering-backends.md) for the implementation roadmap.

---

## Build Output Structure

```
build/
├── bin/              # Executables
│   ├── Zgine         # Main executable
│   └── assets/       # Copied assets
└── lib/              # Static libraries
```

---

## Troubleshooting

### Common Issues

#### 1. GLAD Not Found
**Error:** `GLAD not found in vendor directory`
**Solution:** Follow the GLAD setup instructions above.

#### 2. OpenGL Not Found (Linux)
**Error:** `Could not find OpenGL`
**Solution:** Install Mesa development libraries:
```bash
sudo apt install libgl1-mesa-dev
```

#### 3. Missing X11 Libraries (Linux)
**Error:** `Could not find X11`
**Solution:** Install X11 development packages:
```bash
sudo apt install libx11-dev libxrandr-dev libxinerama-dev
```

#### 4. Ninja Not Found
**Error:** `Could not find Ninja`
**Solution:** 
- Windows: Download from https://ninja-build.org/ or use `choco install ninja`
- Linux: `sudo apt install ninja-build`
- macOS: `brew install ninja`

#### 5. Slow Dependency Downloads
**Solution:** The first build will download several GB of dependencies. Subsequent builds will use cached versions.

#### 6. Assimp Build Warnings (Safe to Ignore)
Assimp may generate numerous warnings during compilation. These are suppressed in the CMake configuration but don't affect functionality.

---

## Clean Build

To perform a clean rebuild:

```bash
# Remove build directory
rm -rf build/

# Or use Git clean (CAUTION: removes all untracked files)
git clean -xfd

# Reconfigure and build
cmake --preset=dev
cmake --build --preset=dev
```

---

## IDE Setup

### Visual Studio Code

1. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - CMake (twxs)

2. Configure settings (`.vscode/settings.json`):
```json
{
  "cmake.configureOnOpen": true,
  "cmake.buildDirectory": "${workspaceFolder}/build/${buildType}",
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

### CLion

CLion automatically detects `CMakePresets.json`. Simply:
1. Open project
2. Select preset from toolbar
3. Build (Ctrl+F9)

### Visual Studio (Windows)

Open folder in Visual Studio 2019/2022 for automatic CMake integration.

---

## Advanced Configuration

### Custom Compiler

```bash
# GCC
cmake .. -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12

# Clang
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```

### Build Type

```bash
# Available types: Debug, Release, RelWithDebInfo, MinSizeRel
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Parallel Builds

```bash
# Use all available cores
cmake --build . -j

# Specify core count
cmake --build . -j 8
```

### Generator Selection

```bash
# Ninja (fast, recommended)
cmake .. -G Ninja

# Unix Makefiles
cmake .. -G "Unix Makefiles"

# Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64
```

---

## Performance Tips

1. **Use Ninja generator** for faster builds
2. **Enable PCH** (`ZGINE_USE_PCH=ON`) to reduce compilation time
3. **Parallel builds** with `-j` flag
4. **ccache** for C++ caching (Linux/macOS):
   ```bash
   sudo apt install ccache  # Linux
   brew install ccache      # macOS
   
   # Configure CMake to use ccache
   cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
   ```

---

## Next Steps

After successful build:
1. Run the engine: `./build/dev/bin/Zgine`
2. Check logs for initialization messages
3. See `docs/` for API documentation
4. Explore `assets/` for sample resources

For development workflow, see [CONTRIBUTING.md](CONTRIBUTING.md)
