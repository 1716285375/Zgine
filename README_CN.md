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
- **内存管理**: 先进的内存池和优化系统
- **性能监控**: 内置性能分析和基准测试

### 渲染系统
- **2D 批处理渲染器**: 高性能 2D 渲染，支持纹理批处理
  - 四边形、线条、圆形、三角形、椭圆、弧形和渐变渲染
  - 最多支持 32 个纹理槽的纹理批处理
  - 线框和点渲染模式
  - 动画形状和性能压力测试
- **3D 批处理渲染器**: 现代 3D 渲染，支持高级功能
  - 立方体、球体和平面图元
  - 3D 变换和旋转
  - 深度测试和 3D 相机控制
  - 环境光和方向光的光照系统
- **高级渲染功能**:
  - **HDR 渲染**: 高动态范围渲染，支持色调映射 (ACES, Reinhard, Uncharted)
  - **后处理**: 泛光、模糊、暗角、色差、胶片颗粒、运动模糊、景深、SSAO
  - **抗锯齿**: FXAA、SMAA、TAA 支持
  - **阴影映射**: 软阴影和级联阴影贴图
  - **视锥剔除**: 自动对象剔除以提升性能
- **双相机系统**: 正交（2D）和透视（3D）相机支持
- **OpenGL 后端**: 现代 OpenGL 4.2+ 渲染管线
- **着色器系统**: 灵活的着色器管理，支持 uniform 上传
- **纹理管理**: 高效的纹理加载和缓存
- **光照系统**: 支持材质的高级光照

### 开发工具
- **ImGui 集成**: 全面的调试 UI 和编辑器工具
- **层系统**: 灵活的应用层架构
- **输入系统**: 跨平台键盘和鼠标输入处理
- **日志系统**: 多级别日志记录
- **性能分析器**: 实时性能监控和优化建议
- **内存分析器**: GPU 和 CPU 内存使用跟踪
- **质量评估**: 自动性能评分和推荐

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

// 绘制高级形状
Zgine::BatchRenderer2D::DrawCircle({0.0f, 0.0f, 0.0f}, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f});
Zgine::BatchRenderer2D::DrawLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, 0.05f);

Zgine::BatchRenderer2D::EndScene();
```

### 基础 3D 渲染

```cpp
// 在你的层的 OnUpdate 方法中
Zgine::BatchRenderer3D::BeginScene(camera);

// 绘制 3D 立方体
Zgine::BatchRenderer3D::DrawCube(
    {0.0f, 0.0f, 0.0f}, 
    {1.0f, 1.0f, 1.0f}, 
    {1.0f, 0.0f, 0.0f, 1.0f}  // 红色立方体
);

// 绘制 3D 球体
Zgine::BatchRenderer3D::DrawSphere(
    {2.0f, 0.0f, 0.0f}, 
    0.5f, 
    {0.0f, 1.0f, 0.0f, 1.0f}  // 绿色球体
);

// 绘制平面
Zgine::BatchRenderer3D::DrawPlane(
    {0.0f, -1.0f, 0.0f}, 
    {10.0f, 10.0f}, 
    {0.5f, 0.5f, 0.5f, 1.0f}  // 灰色地面平面
);

Zgine::BatchRenderer3D::EndScene();
```

### 高级渲染功能

```cpp
// 启用 HDR 渲染
Zgine::AdvancedRenderingManager::SetConfig({
    .enableHDR = true,
    .enablePostProcessing = true,
    .enableBloom = true,
    .enableShadowMapping = true,
    .enableFrustumCulling = true
});

// 开始高级场景渲染
Zgine::AdvancedRenderingManager::BeginScene(camera);
// ... 你的渲染代码 ...
Zgine::AdvancedRenderingManager::EndScene();
```

## 🎯 开发路线图

### 第一阶段：核心基础 ✅
- [x] 基础应用框架
- [x] 事件系统
- [x] 输入处理
- [x] 2D 批处理渲染器
- [x] 着色器系统
- [x] 纹理管理
- [x] 内存管理
- [x] 性能监控

### 第二阶段：增强渲染 ✅
- [x] 线条和圆形渲染
- [x] 3D 渲染支持
- [x] 高级相机系统
- [x] HDR 渲染系统
- [x] 后处理管线
- [x] 阴影映射
- [x] 抗锯齿支持
- [x] 视锥剔除

### 第三阶段：高级功能 ✅
- [x] 高级渲染功能 (HDR、后处理、阴影)
- [x] 性能优化系统
- [x] 内存分析和优化
- [x] 质量评估工具
- [x] 带全面测试的 Sandbox 应用

### 第四阶段：游戏系统 📋
- [ ] 实体组件系统 (ECS)
- [ ] 资源管理
- [ ] 场景管理
- [ ] 物理集成
- [ ] 音频系统

### 第五阶段：开发工具 📋
- [ ] 场景编辑器
- [ ] 资源管线
- [ ] 脚本支持
- [ ] 高级分析工具

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
