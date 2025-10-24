# Zgine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/yourusername/Zgine)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

A modern, lightweight 2D/3D game engine built with C++17, OpenGL, and ImGui.

**中文文档**: [README_CN.md](README_CN.md)  
**📚 Documentation**: [docs/README.md](docs/README.md)

## 🚀 Features

### Core Engine
- **Cross-platform**: Windows support with planned Linux/macOS support
- **Modern C++17**: Leveraging modern C++ features for clean, maintainable code
- **Modular Architecture**: Clean separation between engine core and application code
- **Event System**: Robust event-driven architecture with type-safe event dispatching
- **Memory Management**: Advanced memory pooling and optimization systems
- **Performance Monitoring**: Built-in performance profiling and benchmarking

### Rendering System
- **2D Batch Renderer**: High-performance 2D rendering with texture batching
  - Quad, line, circle, triangle, ellipse, arc, and gradient rendering
  - Texture batching with up to 32 texture slots
  - Wireframe and point rendering modes
  - Animated shapes and performance stress testing
- **3D Batch Renderer**: Modern 3D rendering with advanced features
  - Cube, sphere, and plane primitives
  - 3D transformations and rotations
  - Depth testing and 3D camera controls
  - Lighting system with ambient and directional lighting
- **Advanced Rendering Features**:
  - **HDR Rendering**: High Dynamic Range with tone mapping (ACES, Reinhard, Uncharted)
  - **Post-Processing**: Bloom, blur, vignette, chromatic aberration, film grain, motion blur, depth of field, SSAO
  - **Anti-Aliasing**: FXAA, SMAA, TAA support
  - **Shadow Mapping**: Soft shadows and cascaded shadow maps
  - **Frustum Culling**: Automatic object culling for performance
- **Dual Camera System**: Orthographic (2D) and perspective (3D) camera support
- **OpenGL Backend**: Modern OpenGL 4.2+ rendering pipeline
- **Shader System**: Flexible shader management with uniform uploading
- **Texture Management**: Efficient texture loading and caching
- **Lighting System**: Advanced lighting with material support

### Developer Tools
- **ImGui Integration**: Comprehensive debug UI and editor tools
- **Layer System**: Flexible application layer architecture
- **Input System**: Cross-platform keyboard and mouse input handling
- **Logging System**: Comprehensive logging with multiple severity levels
- **Performance Profiler**: Real-time performance monitoring and optimization suggestions
- **Memory Profiler**: GPU and CPU memory usage tracking
- **Quality Assessment**: Automatic performance scoring and recommendations

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

// Draw advanced shapes
Zgine::BatchRenderer2D::DrawCircle({0.0f, 0.0f, 0.0f}, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f});
Zgine::BatchRenderer2D::DrawLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, 0.05f);

Zgine::BatchRenderer2D::EndScene();
```

### Basic 3D Rendering

```cpp
// In your layer's OnUpdate method
Zgine::BatchRenderer3D::BeginScene(camera);

// Draw 3D cubes
Zgine::BatchRenderer3D::DrawCube(
    {0.0f, 0.0f, 0.0f}, 
    {1.0f, 1.0f, 1.0f}, 
    {1.0f, 0.0f, 0.0f, 1.0f}  // Red cube
);

// Draw 3D spheres
Zgine::BatchRenderer3D::DrawSphere(
    {2.0f, 0.0f, 0.0f}, 
    0.5f, 
    {0.0f, 1.0f, 0.0f, 1.0f}  // Green sphere
);

// Draw planes
Zgine::BatchRenderer3D::DrawPlane(
    {0.0f, -1.0f, 0.0f}, 
    {10.0f, 10.0f}, 
    {0.5f, 0.5f, 0.5f, 1.0f}  // Gray ground plane
);

Zgine::BatchRenderer3D::EndScene();
```

### Advanced Rendering Features

```cpp
// Enable HDR rendering
Zgine::AdvancedRenderingManager::SetConfig({
    .enableHDR = true,
    .enablePostProcessing = true,
    .enableBloom = true,
    .enableShadowMapping = true,
    .enableFrustumCulling = true
});

// Begin advanced scene rendering
Zgine::AdvancedRenderingManager::BeginScene(camera);
// ... your rendering code ...
Zgine::AdvancedRenderingManager::EndScene();
```

## 🎯 Roadmap

### Phase 1: Core Foundation ✅
- [x] Basic application framework
- [x] Event system
- [x] Input handling
- [x] 2D batch renderer
- [x] Shader system
- [x] Texture management
- [x] Memory management
- [x] Performance monitoring

### Phase 2: Enhanced Rendering ✅
- [x] Line and circle rendering
- [x] 3D rendering support
- [x] Advanced camera system
- [x] HDR rendering system
- [x] Post-processing pipeline
- [x] Shadow mapping
- [x] Anti-aliasing support
- [x] Frustum culling

### Phase 3: Advanced Features ✅
- [x] Advanced rendering features (HDR, post-processing, shadows)
- [x] Performance optimization systems
- [x] Memory profiling and optimization
- [x] Quality assessment tools
- [x] Sandbox application with comprehensive testing

### Phase 4: Game Systems 📋
- [ ] Entity Component System (ECS)
- [ ] Resource management
- [ ] Scene management
- [ ] Physics integration
- [ ] Audio system

### Phase 5: Developer Tools 📋
- [ ] Scene editor
- [ ] Asset pipeline
- [ ] Scripting support
- [ ] Advanced profiling tools

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

## 📚 Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[API Reference](docs/api/)** - Complete API documentation
- **[Architecture](docs/architecture/)** - Engine architecture and design
- **[Algorithms](docs/algorithms/)** - Implementation details and algorithms
- **[Developer Guides](docs/guides/)** - Getting started and best practices
- **[Examples](docs/examples/)** - Code examples and tutorials

### Quick Links
- [Getting Started Guide](docs/guides/getting-started.md)
- [Renderer API](docs/api/renderer.md)
- [Batch Rendering Algorithm](docs/algorithms/batch-rendering.md)
- [Circle Rendering Algorithm](docs/algorithms/circle-rendering.md)

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/Zgine/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/Zgine/discussions)
- **Documentation**: [docs/README.md](docs/README.md)

---

**Zgine** - *Building games, one frame at a time* 🎮