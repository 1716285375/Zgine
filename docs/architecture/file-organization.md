# Zgine 引擎文件组织结构详解

## 项目根目录结构

```
Zgine/
├── docs/                          # 文档目录
│   ├── architecture/              # 架构文档
│   ├── api/                      # API 文档
│   ├── guides/                   # 使用指南
│   └── examples/                 # 示例代码
├── Zgine/                        # 引擎核心库
├── Sandbox/                      # 测试应用
├── vendor/                       # 第三方库
├── bin/                          # 编译输出目录
├── bin-int/                      # 中间文件目录
├── premake5.lua                  # 构建配置
├── build.bat                     # 构建脚本
├── GenerateProject.bat           # 项目生成脚本
├── CleanProject.bat              # 清理脚本
└── README.md                     # 项目说明
```

## Zgine 引擎核心库结构

### 顶层目录 (Zgine/src/Zgine/)

```
Zgine/src/Zgine/
├── Core/                         # 核心功能模块
├── Events/                       # 事件系统
├── ImGui/                        # ImGui 集成
├── Renderer/                     # 渲染系统
├── Application.h/cpp             # 主应用程序类
├── Core.h                        # 核心定义和宏
├── EntryPoint.h                  # 应用程序入口点
├── Input.h                       # 输入抽象接口
├── KeyCodes.h                    # 键盘按键码定义
├── Layer.h/cpp                   # 层基类
├── LayerStack.h/cpp              # 层栈管理
├── Log.h/cpp                     # 日志系统
├── MouseButtonCodes.h            # 鼠标按键码定义
├── Window.h                      # 窗口抽象接口
└── Zgine.h                       # 主头文件
```

### 核心功能模块 (Core/)

```
Zgine/src/Zgine/Core/
├── SmartPointers.h               # 智能指针定义
└── (其他核心功能文件)
```

**功能说明:**
- `SmartPointers.h`: 定义了 `Ref<T>`、`Scope<T>` 等智能指针类型
- 提供内存管理和资源自动释放功能

### 事件系统 (Events/)

```
Zgine/src/Zgine/Events/
├── Event.h                       # 事件基类和分发器
├── ApplicationEvent.h            # 应用程序事件
├── KeyEvent.h                    # 键盘事件
└── MouseEvent.h                  # 鼠标事件
```

**功能说明:**
- `Event.h`: 定义事件基类、事件类型枚举、事件分发器
- `ApplicationEvent.h`: 窗口关闭、调整大小、焦点等应用级事件
- `KeyEvent.h`: 按键按下、释放、输入等键盘事件
- `MouseEvent.h`: 鼠标移动、点击、滚轮等鼠标事件

### ImGui 集成 (ImGui/)

```
Zgine/src/Zgine/ImGui/
├── ImGuiLayer.h/cpp              # ImGui 层实现
└── ImGuiBuild.cpp                # ImGui 构建配置
```

**功能说明:**
- `ImGuiLayer.h/cpp`: ImGui 调试界面的层实现
- `ImGuiBuild.cpp`: ImGui 库的构建配置

### 渲染系统 (Renderer/)

```
Zgine/src/Zgine/Renderer/
├── AdvancedRendering.h/cpp      # 高级渲染功能
├── BatchRenderer2D.h/cpp        # 2D 批处理渲染器
├── BatchRenderer3D.h/cpp        # 3D 批处理渲染器
├── Buffer.h/cpp                  # 缓冲区抽象
├── CullingSystem.h/cpp           # 剔除系统
├── GraphicsContext.h             # 图形上下文抽象
├── HDRSystem.h/cpp               # HDR 渲染系统
├── Lighting.h/cpp                # 光照系统
├── Material.h/cpp                # 材质系统
├── MemoryAnalyzer.h              # 内存分析器
├── MemoryManager.h/cpp           # 内存管理器
├── ObjectPool.h/cpp             # 对象池
├── OrthographicCamera.h/cpp      # 正交相机
├── ParticleSystem.h/cpp         # 粒子系统
├── PerformanceBenchmark.h/cpp    # 性能基准测试
├── PerformanceMonitorUI.h/cpp   # 性能监控 UI
├── PerformanceProfiler.h/cpp     # 性能分析器
├── PerspectiveCamera.h/cpp       # 透视相机
├── PostProcessing.h/cpp          # 后处理效果
├── RenderCommand.h/cpp           # 渲染命令
├── Renderer.h/cpp                # 高级渲染接口
├── RendererAPI.h/cpp             # 渲染 API 抽象
├── RendererManager.h             # 渲染管理器
├── ResourceManager.h/cpp         # 资源管理器
├── Shader.h/cpp                  # 着色器管理
├── ShadowMapping.h/cpp          # 阴影映射
├── Texture.h/cpp                 # 纹理抽象
└── VertexArray.h/cpp             # 顶点数组抽象
```

**功能说明:**

#### 核心渲染组件
- `Renderer.h/cpp`: 高级渲染接口，管理场景渲染
- `RenderCommand.h/cpp`: 渲染命令抽象，提供平台无关的渲染操作
- `RendererAPI.h/cpp`: 渲染 API 抽象基类
- `GraphicsContext.h`: 图形上下文抽象接口

#### 批处理渲染器
- `BatchRenderer2D.h/cpp`: 高性能 2D 批处理渲染器
  - 支持四边形、圆形、线条等 2D 图元
  - 动态缓冲区管理和内存池优化
  - 纹理批处理和缓存
- `BatchRenderer3D.h/cpp`: 3D 批处理渲染器
  - 支持立方体等 3D 图元
  - 3D 变换和投影

#### 相机系统
- `OrthographicCamera.h/cpp`: 正交投影相机
- `PerspectiveCamera.h/cpp`: 透视投影相机，支持 FPS 风格控制

#### 着色器和纹理
- `Shader.h/cpp`: 着色器程序管理，支持 uniform 上传
- `Texture.h/cpp`: 纹理抽象接口
- `Material.h/cpp`: 材质系统

#### 高级渲染功能
- `AdvancedRendering.h/cpp`: 高级渲染技术
- `HDRSystem.h/cpp`: HDR (高动态范围) 渲染
- `PostProcessing.h/cpp`: 后处理效果
- `ShadowMapping.h/cpp`: 阴影映射
- `Lighting.h/cpp`: 光照系统
- `ParticleSystem.h/cpp`: 粒子系统

#### 性能优化
- `CullingSystem.h/cpp`: 视锥剔除系统
- `MemoryManager.h/cpp`: 内存管理器
- `ObjectPool.h/cpp`: 对象池
- `PerformanceProfiler.h/cpp`: 性能分析器
- `PerformanceBenchmark.h/cpp`: 性能基准测试
- `PerformanceMonitorUI.h/cpp`: 性能监控 UI

#### 资源管理
- `ResourceManager.h/cpp`: 资源管理器
- `MemoryAnalyzer.h`: 内存分析工具

## 平台抽象层结构 (Platform/)

```
Zgine/src/Platform/
├── OpenGL/                       # OpenGL 实现
│   ├── OpenGLBuffer.h/cpp        # OpenGL 缓冲区
│   ├── OpenGLContext.h/cpp       # OpenGL 上下文
│   ├── OpenGLRendererAPI.h/cpp   # OpenGL 渲染 API
│   ├── OpenGLTexture.h/cpp       # OpenGL 纹理
│   └── OpenGLVertexArray.h/cpp   # OpenGL 顶点数组
├── Windows/                      # Windows 平台实现
│   ├── WindowsInput.h/cpp        # Windows 输入处理
│   └── WindowsWindow.h/cpp       # Windows 窗口实现
└── Direct3D/                     # Direct3D 实现 (预留)
    └── Direct3DBuffer.h          # Direct3D 缓冲区 (预留)
```

**功能说明:**

### OpenGL 实现
- `OpenGLBuffer.h/cpp`: OpenGL 缓冲区实现
- `OpenGLContext.h/cpp`: OpenGL 图形上下文实现
- `OpenGLRendererAPI.h/cpp`: OpenGL 渲染 API 实现
- `OpenGLTexture.h/cpp`: OpenGL 纹理实现
- `OpenGLVertexArray.h/cpp`: OpenGL 顶点数组实现

### Windows 平台
- `WindowsInput.h/cpp`: Windows 平台输入处理
- `WindowsWindow.h/cpp`: Windows 平台窗口实现

### Direct3D 实现 (预留)
- `Direct3DBuffer.h`: Direct3D 缓冲区接口 (预留扩展)

## Sandbox 测试应用结构

```
Sandbox/src/
├── SandboxApp.cpp                # 应用程序入口
├── MainControlLayer.h/cpp        # 主控制层
├── Test2DLayer.h/cpp             # 2D 测试层
├── Test3DLayer.h/cpp             # 3D 测试层
├── UI/                           # UI 管理模块
│   ├── UIManager.h/cpp           # UI 管理器
├── Rendering/                    # 渲染管理模块
│   ├── RenderManager.h/cpp       # 渲染管理器
├── Scene/                        # 场景管理模块
│   ├── SceneManager.h/cpp       # 场景管理器
├── Settings/                     # 设置管理模块
│   ├── SettingsManager.h/cpp    # 设置管理器
└── Testing/                      # 测试模块
    ├── Test2DModule.h/cpp        # 2D 测试模块
    └── Test3DModule.h/cpp        # 3D 测试模块
```

**功能说明:**

### 应用层
- `SandboxApp.cpp`: 继承自 `Application` 的测试应用
- `MainControlLayer.h/cpp`: 主控制层，协调各个管理器

### 测试层
- `Test2DLayer.h/cpp`: 2D 渲染测试层
- `Test3DLayer.h/cpp`: 3D 渲染测试层

### 管理模块
- `UI/UIManager.h/cpp`: UI 管理器，处理 ImGui 界面
- `Rendering/RenderManager.h/cpp`: 渲染管理器
- `Scene/SceneManager.h/cpp`: 场景管理器
- `Settings/SettingsManager.h/cpp`: 设置管理器

### 测试模块
- `Testing/Test2DModule.h/cpp`: 2D 测试功能模块
- `Testing/Test3DModule.h/cpp`: 3D 测试功能模块

## 第三方库结构 (vendor/)

```
Zgine/vendor/
├── bin/                          # 第三方库二进制文件
├── glad/                         # OpenGL 加载器
│   ├── glad.h                    # GLAD 头文件
│   ├── glad.c                    # GLAD 实现
│   └── glad.lua                   # GLAD 构建配置
├── glfw/                         # GLFW 窗口库
│   ├── include/                   # GLFW 头文件
│   ├── src/                       # GLFW 源文件
│   └── (其他 GLFW 文件)
├── glm/                          # GLM 数学库
│   ├── glm/                      # GLM 头文件
│   └── (其他 GLM 文件)
├── imgui/                        # ImGui UI 库
│   ├── imgui.h                   # ImGui 头文件
│   ├── imgui.cpp                 # ImGui 实现
│   └── (其他 ImGui 文件)
├── spdlog/                       # spdlog 日志库
│   ├── include/                   # spdlog 头文件
│   └── (其他 spdlog 文件)
└── stb_image/                    # stb_image 图像加载库
    ├── stb_image.h               # stb_image 头文件
    └── stb_image.cpp             # stb_image 实现
```

**功能说明:**

### 核心库
- `glad/`: OpenGL 函数加载器，提供 OpenGL API 访问
- `glfw/`: 跨平台窗口和输入管理库
- `glm/`: OpenGL 数学库，提供向量、矩阵等数学运算

### UI 和工具库
- `imgui/`: 即时模式 GUI 库，用于调试界面
- `spdlog/`: 高性能日志库
- `stb_image/`: 图像加载库，支持多种图像格式

## 构建和配置文件

### 构建配置
- `premake5.lua`: Premake5 构建配置文件
- `build.bat`: Windows 构建脚本
- `GenerateProject.bat`: 生成 Visual Studio 项目文件
- `CleanProject.bat`: 清理构建文件

### 项目文件
- `Zgine.sln`: Visual Studio 解决方案文件
- `Zgine/Zgine.vcxproj`: Zgine 库项目文件
- `Sandbox/Sandbox.vcxproj`: Sandbox 应用项目文件

### 配置文件
- `settings.json`: 应用程序设置文件
- `imgui.ini`: ImGui 配置和布局文件

## 编译输出目录

### bin/ 目录
```
bin/Debug-windows-x86_64/
├── GLAD/                         # GLAD 库输出
├── GLFW/                         # GLFW 库输出
├── ImGui/                        # ImGui 库输出
├── Sandbox/                      # Sandbox 应用输出
│   ├── Sandbox.exe              # 可执行文件
│   ├── imgui.ini                # ImGui 配置
│   ├── performance_data.csv     # 性能数据
│   └── settings.json            # 应用设置
└── Zgine/                        # Zgine 库输出
```

### bin-int/ 目录
```
bin-int/Debug-windows-x86_64/
├── GLAD/                         # GLAD 中间文件
├── GLFW/                         # GLFW 中间文件
├── ImGui/                        # ImGui 中间文件
├── Sandbox/                      # Sandbox 中间文件
└── Zgine/                        # Zgine 中间文件
```

## 文件命名规范

### 头文件
- 使用 `.h` 扩展名
- 类名使用 PascalCase (如 `Application.h`)
- 接口文件使用描述性名称 (如 `Window.h`)

### 源文件
- 使用 `.cpp` 扩展名
- 与对应头文件同名 (如 `Application.cpp`)

### 目录命名
- 使用 PascalCase (如 `Renderer/`, `Events/`)
- 平台特定目录使用平台名 (如 `Windows/`, `OpenGL/`)

### 类命名
- 使用 PascalCase (如 `Application`, `BatchRenderer2D`)
- 接口类通常不加前缀 (如 `Window`, `RendererAPI`)
- 实现类使用平台前缀 (如 `WindowsWindow`, `OpenGLRendererAPI`)

## 依赖关系

### 核心依赖
- Zgine 核心库依赖第三方库 (GLFW, GLAD, GLM, spdlog)
- 平台抽象层依赖 Zgine 核心
- 渲染系统依赖平台抽象层
- Sandbox 应用依赖 Zgine 引擎

### 编译顺序
1. 第三方库 (GLAD, GLFW, ImGui, spdlog)
2. Zgine 核心库
3. Sandbox 应用

这种文件组织结构确保了代码的模块化、可维护性和可扩展性，每个模块都有明确的职责和清晰的接口。
