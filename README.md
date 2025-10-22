# Zgine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/yourusername/Zgine)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

A modern, lightweight 2D/3D game engine built with C++17, OpenGL, and ImGui.

**中文文档**: [README_CN.md](README_CN.md)

## 🚀 Features

### Core Engine
- **Cross-platform**: Windows support with planned Linux/macOS support
- **Modern C++17**: Leveraging modern C++ features for clean, maintainable code
- **Modular Architecture**: Clean separation between engine core and application code
- **Event System**: Robust event-driven architecture with type-safe event dispatching

### Rendering System
- **2D Batch Renderer**: High-performance 2D rendering with texture batching
- **OpenGL Backend**: Modern OpenGL 3.3+ rendering pipeline
- **Shader System**: Flexible shader management with hot-reloading support
- **Texture Management**: Efficient texture loading and caching
- **Camera System**: Orthographic and perspective camera support

### Developer Tools
- **ImGui Integration**: Built-in debug UI and editor tools
- **Layer System**: Flexible application layer architecture
- **Input System**: Cross-platform keyboard and mouse input handling
- **Logging System**: Comprehensive logging with multiple severity levels

## 📁 Project Structure

```
Zgine/
├── Zgine/                    # Engine core library
│   ├── src/
│   │   ├── Zgine/           # Engine source code
│   │   │   ├── Core/        # Core utilities and macros
│   │   │   ├── Events/      # Event system
│   │   │   ├── Renderer/    # Rendering system
│   │   │   ├── Platform/    # Platform-specific implementations
│   │   │   └── ...
│   │   └── vendor/          # Third-party dependencies
│   └── Zgine.vcxproj
├── Sandbox/                  # Example application
│   ├── src/
│   │   └── SandboxApp.cpp   # Sandbox application implementation
│   └── Sandbox.vcxproj
├── vendor/                   # External dependencies
│   ├── glfw/                # Window management
│   ├── glad/                # OpenGL loader
│   ├── imgui/               # Immediate mode GUI
│   ├── glm/                 # Mathematics library
│   └── spdlog/              # Logging library
├── premake5.lua             # Build configuration
└── README.md
```

## 🛠️ Building

### Prerequisites
- **Visual Studio 2022** (Windows)
- **Premake5** (included in vendor/bin/premake/)

### Build Steps

1. **Generate Project Files**
   ```bash
   # Run the premake script to generate Visual Studio solution
   vendor/bin/premake/premake5.exe vs2022
   ```

2. **Build the Engine**
   ```bash
   # Open Zgine.sln in Visual Studio and build
   # Or use command line:
   msbuild Zgine.sln /p:Configuration=Debug /p:Platform=x64
   ```

3. **Run Sandbox**
   ```bash
   # The Sandbox application will be built to:
   bin/Debug-windows-x86_64/Sandbox/Sandbox.exe
   ```

## 🎮 Usage

### Creating a Simple Application

```cpp
#include <Zgine.h>

class MyApp : public Zgine::Application
{
public:
    MyApp()
    {
        // Add your custom layers here
        PushLayer(new MyGameLayer());
    }
};

// Entry point
Zgine::Application* Zgine::CreateApplication()
{
    return new MyApp();
}
```

### Basic 2D Rendering

```cpp
// In your layer's OnUpdate method
Zgine::BatchRenderer2D::BeginScene(camera);

// Draw colored quad
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f}, 
    {1.0f, 1.0f}, 
    {1.0f, 0.0f, 0.0f, 1.0f}  // Red color
);

// Draw textured quad
Zgine::BatchRenderer2D::DrawQuad(
    {1.0f, 1.0f, 0.0f}, 
    {0.5f, 0.5f}, 
    texture, 
    {1.0f, 1.0f, 1.0f, 1.0f}  // White tint
);

Zgine::BatchRenderer2D::EndScene();
```

## 🎯 Roadmap

### Phase 1: Core Foundation ✅
- [x] Basic application framework
- [x] Event system
- [x] Input handling
- [x] 2D batch renderer
- [x] Shader system
- [x] Texture management

### Phase 2: Enhanced Rendering 🚧
- [ ] Line and circle rendering
- [ ] 3D rendering support
- [ ] Advanced camera system
- [ ] Material system improvements

### Phase 3: Game Systems 📋
- [ ] Entity Component System (ECS)
- [ ] Resource management
- [ ] Scene management
- [ ] Physics integration

### Phase 4: Developer Tools 📋
- [ ] Scene editor
- [ ] Asset pipeline
- [ ] Scripting support
- [ ] Performance profiling

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **GLFW** - Window management
- **GLAD** - OpenGL loader
- **ImGui** - Immediate mode GUI
- **GLM** - Mathematics library
- **spdlog** - Logging library

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/Zgine/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/Zgine/discussions)
- **Documentation**: [Wiki](https://github.com/yourusername/Zgine/wiki)

---

**Zgine** - *Building games, one frame at a time* 🎮