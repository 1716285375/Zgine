# Zgine Architecture

## Overview

Zgine 采用模块化架构，核心产物为 **ZgineRuntime**（静态/动态库），对外提供稳定的 C++ API。

## Components

### ZgineRuntime (Core Engine Library)

- **类型**: 静态库（默认）或动态库
- **位置**: `src/` (排除 `src/Editor/`)
- **目标名**: `ZgineRuntime`
- **导出名**: `Zgine::ZgineRuntime`
- **职责**: 
  - 提供核心引擎功能（渲染、物理、音频、脚本等）
  - 提供稳定的 C++ API
  - 不依赖任何应用程序（Editor、Sandbox、Tests）

### ZgineEditor (Editor Application)

- **类型**: 可执行文件
- **位置**: `src/Editor/` + `editor/CMakeLists.txt`
- **目标名**: `ZgineEditor`
- **依赖**: 链接 `ZgineRuntime`
- **职责**: 
  - 提供可视化编辑器界面
  - 场景编辑、资源管理、调试工具等
  - **严格单向依赖**: Editor 依赖 Runtime，Runtime 不依赖 Editor

### ZgineSandbox (Example/Demo Application)

- **类型**: 可执行文件
- **位置**: `sandbox/`
- **目标名**: `ZgineSandbox`
- **依赖**: 链接 `ZgineRuntime`
- **职责**: 
  - 示例程序和演示
  - 展示引擎功能

### ZgineTests (Test Suite)

- **类型**: 可执行文件
- **位置**: `tests/`
- **目标名**: `ZgineTests`
- **依赖**: 链接 `ZgineRuntime` + GoogleTest
- **职责**: 
  - 单元测试和集成测试

## Dependency Graph

```
ZgineRuntime (Core Library)
    ↑
    ├── ZgineEditor (Editor App)
    ├── ZgineSandbox (Example App)
    └── ZgineTests (Test Suite)
```

**关键原则**: Runtime 与 Editor 严格单向依赖
- ✅ Editor 可以依赖 Runtime
- ❌ Runtime **不能**依赖 Editor
- 这确保了游戏发布产物可裁剪、可移植并降低依赖泄漏

## Build Options

- `ZGINE_BUILD_SHARED`: 构建 Runtime 为动态库（默认: OFF，静态库）
- `ZGINE_BUILD_SANDBOX`: 构建 Sandbox 应用（默认: ON）
- `ZGINE_BUILD_EDITOR`: 构建 Editor 应用（默认: OFF）
- `ZGINE_BUILD_TESTS`: 构建测试套件（默认: ON）

## Build Scripts

Zgine 提供了多个构建脚本用于构建不同组件：

- `build-runtime.bat`: 仅构建 ZgineRuntime
- `build-editor.bat`: 构建 ZgineRuntime + ZgineEditor
- `build-sandbox.bat`: 构建 ZgineRuntime + ZgineSandbox
- `build-tests.bat`: 构建 ZgineRuntime + ZgineTests
- `build-all.bat`: 构建所有组件
- `build.bat`: 主入口脚本

详细用法请参考 [BUILD_SCRIPTS.md](BUILD_SCRIPTS.md)

## Usage

### In Your Project

```cmake
find_package(Zgine REQUIRED)
target_link_libraries(MyGame PRIVATE Zgine::ZgineRuntime)
```

### Building Zgine

```bash
# Build Runtime only
cmake --preset windows-release -DZGINE_BUILD_SANDBOX=OFF -DZGINE_BUILD_EDITOR=OFF

# Build Runtime + Editor
cmake --preset windows-release -DZGINE_BUILD_EDITOR=ON

# Build Runtime + Sandbox + Tests
cmake --preset windows-release -DZGINE_BUILD_SANDBOX=ON -DZGINE_BUILD_TESTS=ON
```

## Benefits

1. **可裁剪**: 游戏发布时只需 Runtime，不需要 Editor
2. **可移植**: Runtime 不依赖编辑器特定代码
3. **低依赖泄漏**: Runtime API 清晰，不暴露编辑器实现细节
4. **模块化**: 各组件独立构建和测试
