# Zgine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/yourusername/Zgine)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

一个使用 C++17、OpenGL 和 ImGui 构建的现代轻量级 2D/3D 游戏引擎。

**English Documentation**: [README.md](README.md)  
**📚 文档**: [docs/README.md](docs/README.md)

## 🚀 功能特性

### 核心引擎
- **跨平台**: 支持 Windows，计划支持 Linux/macOS
- **现代 C++17**: 利用现代 C++ 特性编写清晰、可维护的代码
- **模块化架构**: 引擎核心与应用代码的清晰分离
- **事件系统**: 强大的事件驱动架构，支持类型安全的事件分发

### 渲染系统
- **2D 批处理渲染器**: 高性能 2D 渲染，支持纹理批处理
- **OpenGL 后端**: 现代 OpenGL 3.3+ 渲染管线
- **着色器系统**: 灵活的着色器管理，支持热重载
- **纹理管理**: 高效的纹理加载和缓存
- **相机系统**: 正交和透视相机支持

### 开发工具
- **ImGui 集成**: 内置调试 UI 和编辑器工具
- **层系统**: 灵活的应用层架构
- **输入系统**: 跨平台键盘和鼠标输入处理
- **日志系统**: 多级别日志记录

## 📁 项目结构

```
Zgine/
├── Zgine/                    # 引擎核心库
│   ├── src/
│   │   ├── Zgine/           # 引擎源代码
│   │   │   ├── Core/        # 核心工具和宏
│   │   │   ├── Events/      # 事件系统
│   │   │   ├── Renderer/    # 渲染系统
│   │   │   ├── Platform/    # 平台特定实现
│   │   │   └── ...
│   │   └── vendor/          # 第三方依赖
│   └── Zgine.vcxproj
├── Sandbox/                  # 示例应用
│   ├── src/
│   │   └── SandboxApp.cpp   # Sandbox 应用实现
│   └── Sandbox.vcxproj
├── vendor/                   # 外部依赖
│   ├── glfw/                # 窗口管理
│   ├── glad/                # OpenGL 加载器
│   ├── imgui/               # 即时模式 GUI
│   ├── glm/                 # 数学库
│   └── spdlog/              # 日志库
├── premake5.lua             # 构建配置
└── README.md
```

## 🛠️ 构建指南

### 前置要求
- **Visual Studio 2022** (Windows)
- **Premake5** (包含在 vendor/bin/premake/ 中)

### 构建步骤

1. **生成项目文件**
   ```bash
   # 运行 premake 脚本生成 Visual Studio 解决方案
   vendor/bin/premake/premake5.exe vs2022
   ```

2. **构建引擎**
   ```bash
   # 在 Visual Studio 中打开 Zgine.sln 并构建
   # 或使用命令行：
   msbuild Zgine.sln /p:Configuration=Debug /p:Platform=x64
   ```

3. **运行 Sandbox**
   ```bash
   # Sandbox 应用将构建到：
   bin/Debug-windows-x86_64/Sandbox/Sandbox.exe
   ```

## 🎮 使用方法

### 创建简单应用

```cpp
#include <Zgine.h>

class MyApp : public Zgine::Application
{
public:
    MyApp()
    {
        // 在这里添加你的自定义层
        PushLayer(new MyGameLayer());
    }
};

// 入口点
Zgine::Application* Zgine::CreateApplication()
{
    return new MyApp();
}
```

### 基础 2D 渲染

```cpp
// 在你的层的 OnUpdate 方法中
Zgine::BatchRenderer2D::BeginScene(camera);

// 绘制彩色四边形
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f}, 
    {1.0f, 1.0f}, 
    {1.0f, 0.0f, 0.0f, 1.0f}  // 红色
);

// 绘制带纹理的四边形
Zgine::BatchRenderer2D::DrawQuad(
    {1.0f, 1.0f, 0.0f}, 
    {0.5f, 0.5f}, 
    texture, 
    {1.0f, 1.0f, 1.0f, 1.0f}  // 白色着色
);

Zgine::BatchRenderer2D::EndScene();
```

## 🎯 开发路线图

### 第一阶段：核心基础 ✅
- [x] 基础应用框架
- [x] 事件系统
- [x] 输入处理
- [x] 2D 批处理渲染器
- [x] 着色器系统
- [x] 纹理管理

### 第二阶段：增强渲染 🚧
- [ ] 线条和圆形渲染
- [ ] 3D 渲染支持
- [ ] 高级相机系统
- [ ] 材质系统改进

### 第三阶段：游戏系统 📋
- [ ] 实体组件系统 (ECS)
- [ ] 资源管理
- [ ] 场景管理
- [ ] 物理集成

### 第四阶段：开发工具 📋
- [ ] 场景编辑器
- [ ] 资源管线
- [ ] 脚本支持
- [ ] 性能分析

## 🤝 贡献指南

我们欢迎贡献！请查看我们的[贡献指南](CONTRIBUTING.md)了解详情。

### 开发环境设置
1. Fork 仓库
2. 创建功能分支
3. 进行更改
4. 如适用，添加测试
5. 提交拉取请求

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- **GLFW** - 窗口管理
- **GLAD** - OpenGL 加载器
- **ImGui** - 即时模式 GUI
- **GLM** - 数学库
- **spdlog** - 日志库

## 📞 支持

- **问题反馈**: [GitHub Issues](https://github.com/yourusername/Zgine/issues)
- **讨论交流**: [GitHub Discussions](https://github.com/yourusername/Zgine/discussions)
- **文档**: [Wiki](https://github.com/yourusername/Zgine/wiki)

---

**Zgine** - *逐帧构建游戏* 🎮
