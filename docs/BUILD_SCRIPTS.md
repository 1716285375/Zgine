# Zgine Build Scripts

## Overview

Zgine 提供了多个构建脚本，用于构建不同的组件：

- **build-runtime.bat**: 仅构建 ZgineRuntime（核心引擎库）
- **build-editor.bat**: 构建 ZgineRuntime + ZgineEditor（编辑器）
- **build-sandbox.bat**: 构建 ZgineRuntime + ZgineSandbox（示例程序）
- **build-tests.bat**: 构建 ZgineRuntime + ZgineTests（测试套件）
- **build-all.bat**: 构建所有组件
- **build.bat**: 主入口脚本，可指定组件

## Usage

### 方式 1: 使用专用脚本

```batch
REM 仅构建 Runtime
build-runtime.bat [debug|release|debug-vs|release-vs] [ON|OFF]

REM 构建 Editor
build-editor.bat [debug|release|debug-vs|release-vs] [ON|OFF]

REM 构建 Sandbox
build-sandbox.bat [debug|release|debug-vs|release-vs] [ON|OFF]

REM 构建 Tests
build-tests.bat [debug|release|debug-vs|release-vs] [ON|OFF]

REM 构建所有组件
build-all.bat [debug|release|debug-vs|release-vs] [ON|OFF]
```

### 方式 2: 使用主入口脚本

```batch
REM 构建指定组件
build.bat [component] [build_type] [pch]

REM 示例
build.bat runtime release ON
build.bat editor debug OFF
build.bat sandbox release
build.bat tests debug
build.bat all release
```

## Arguments

### Build Type

- `debug`: Debug 构建（使用 Ninja，如果可用）
- `release`: Release 构建（使用 Ninja，如果可用）
- `debug-vs`: Debug 构建（强制使用 Visual Studio）
- `release-vs`: Release 构建（强制使用 Visual Studio）

### PCH (Precompiled Headers)

- `ON`: 启用预编译头（默认）
- `OFF`: 禁用预编译头

## Examples

### 构建 Runtime（Release）

```batch
build-runtime.bat release
```

### 构建 Editor（Debug，禁用 PCH）

```batch
build-editor.bat debug OFF
```

### 构建 Sandbox（Release，使用 VS）

```batch
build-sandbox.bat release-vs
```

### 构建 Tests（Debug）

```batch
build-tests.bat debug
```

### 构建所有组件（Release）

```batch
build-all.bat release
```

## Output Locations

构建产物位于 `build/[preset]/` 目录：

- **Runtime**: `build/[preset]/lib/ZgineRuntime.lib` (或 `.dll`)
- **Editor**: `build/[preset]/bin/ZgineEditor.exe`
- **Sandbox**: `build/[preset]/bin/ZgineSandbox.exe`
- **Tests**: `build/[preset]/bin/ZgineTests.exe`

## Notes

1. **依赖关系**: Editor、Sandbox、Tests 都依赖 Runtime，构建时会自动构建 Runtime
2. **默认行为**: `build.bat` 不带参数时默认构建 Sandbox
3. **测试运行**: `build-tests.bat` 构建完成后会询问是否运行测试
4. **可执行文件**: Editor 和 Sandbox 构建完成后会询问是否运行

## Requirements

- Visual Studio 2022 (17.0+) with "Desktop development with C++"
- CMake 3.20+
- Ninja (可选，用于更快的构建)
