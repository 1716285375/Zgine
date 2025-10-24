# Zgine 引擎架构文档

## 项目概述

Zgine 是一个基于 C++ 的现代游戏引擎，采用模块化设计，支持 2D 和 3D 渲染。引擎使用 OpenGL 作为底层渲染 API，并提供了完整的平台抽象层。

## 整体架构

### 核心设计原则
- **分层架构**: 采用分层设计，每层职责明确
- **平台无关**: 通过抽象层实现跨平台支持
- **事件驱动**: 基于事件系统的松耦合设计
- **模块化**: 各功能模块独立，便于维护和扩展

### 主要模块

1. **核心层 (Core Layer)**
   - 基础类型定义和宏
   - 内存管理
   - 日志系统
   - 智能指针

2. **平台抽象层 (Platform Abstraction Layer)**
   - 窗口管理
   - 输入处理
   - 图形上下文
   - 渲染 API 抽象

3. **渲染层 (Renderer Layer)**
   - 高级渲染接口
   - 批处理渲染器
   - 着色器管理
   - 纹理和材质系统

4. **应用层 (Application Layer)**
   - 主应用程序类
   - 层栈管理
   - 事件分发
   - ImGui 集成

5. **Sandbox 层 (Sandbox Layer)**
   - 测试和演示应用
   - UI 管理
   - 场景管理
   - 设置管理

## 文件组织结构

```
Zgine/
├── src/
│   ├── Zgine/                    # 引擎核心代码
│   │   ├── Core/                 # 核心功能
│   │   ├── Events/               # 事件系统
│   │   ├── ImGui/                # ImGui 集成
│   │   ├── Renderer/             # 渲染系统
│   │   ├── Application.h/cpp     # 主应用程序
│   │   ├── Layer.h/cpp          # 层基类
│   │   ├── LayerStack.h/cpp     # 层栈管理
│   │   ├── Window.h             # 窗口抽象
│   │   ├── Input.h              # 输入抽象
│   │   └── Log.h/cpp            # 日志系统
│   ├── Platform/                 # 平台特定实现
│   │   ├── OpenGL/              # OpenGL 实现
│   │   ├── Windows/             # Windows 平台
│   │   └── Direct3D/            # Direct3D 实现
│   └── vendor/                   # 第三方库
│       ├── glfw/                # GLFW 窗口库
│       ├── glad/                # OpenGL 加载器
│       ├── imgui/               # ImGui UI 库
│       ├── glm/                 # 数学库
│       └── spdlog/              # 日志库
└── Sandbox/                      # 测试应用
    └── src/
        ├── SandboxApp.cpp       # 应用入口
        ├── MainControlLayer.h/cpp # 主控制层
        ├── Test2DLayer.h/cpp    # 2D 测试层
        ├── Test3DLayer.h/cpp    # 3D 测试层
        ├── UI/                  # UI 管理
        ├── Rendering/           # 渲染管理
        ├── Scene/               # 场景管理
        ├── Settings/            # 设置管理
        └── Testing/             # 测试模块
```

## 核心类关系

### 应用程序生命周期
- `Application`: 主应用程序类，管理整个引擎生命周期
- `LayerStack`: 管理应用层的栈结构
- `Layer`: 所有应用层的基类
- `ImGuiLayer`: ImGui 调试界面层

### 渲染系统
- `Renderer`: 高级渲染接口
- `RenderCommand`: 渲染命令抽象
- `RendererAPI`: 渲染 API 抽象基类
- `OpenGLRendererAPI`: OpenGL 实现
- `BatchRenderer2D`: 2D 批处理渲染器
- `BatchRenderer3D`: 3D 批处理渲染器

### 平台抽象
- `Window`: 窗口抽象接口
- `WindowsWindow`: Windows 平台窗口实现
- `GraphicsContext`: 图形上下文抽象
- `OpenGLContext`: OpenGL 上下文实现

### 事件系统
- `Event`: 事件基类
- `EventDispatcher`: 事件分发器
- `WindowCloseEvent`, `KeyPressedEvent` 等: 具体事件类型

## 设计模式

1. **单例模式**: `Application` 类使用单例模式
2. **工厂模式**: `Window::Create()` 使用工厂模式创建平台特定窗口
3. **观察者模式**: 事件系统使用观察者模式
4. **策略模式**: 渲染 API 使用策略模式
5. **命令模式**: `RenderCommand` 使用命令模式

## 性能优化特性

1. **批处理渲染**: 2D 和 3D 批处理渲染器减少绘制调用
2. **内存池**: 使用内存池优化内存分配
3. **纹理缓存**: 纹理索引缓存减少状态切换
4. **性能监控**: 内置性能监控和基准测试
5. **剔除系统**: 视锥剔除优化渲染性能

## 扩展性

- **插件系统**: 通过层系统支持插件
- **多渲染 API**: 支持 OpenGL，预留 Direct3D 接口
- **跨平台**: 支持 Windows，可扩展至 Linux/macOS
- **模块化**: 各模块独立，便于替换和扩展

## 技术栈

- **语言**: C++17
- **构建系统**: Premake5
- **渲染 API**: OpenGL 3.3+
- **窗口管理**: GLFW
- **UI 框架**: ImGui
- **数学库**: GLM
- **日志系统**: spdlog
- **图像加载**: stb_image