# Getting Started Guide

Welcome to Zgine! This guide will help you get started with the Zgine game engine, from installation to creating your first application.

## Installation

### Prerequisites

Before installing Zgine, ensure you have the following:

- **Visual Studio 2022** (Windows) or **GCC 9+** (Linux/macOS)
- **CMake 3.16+** or **Premake5**
- **Git** for version control

### Windows Installation

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/Zgine.git
   cd Zgine
   ```

2. **Generate Project Files**
   ```bash
   vendor/bin/premake/premake5.exe vs2022
   ```

3. **Build the Engine**
   ```bash
   msbuild Zgine.sln /p:Configuration=Debug /p:Platform=x64
   ```

4. **Run Sandbox**
   ```bash
   bin/Debug-windows-x86_64/Sandbox/Sandbox.exe
   ```

### Linux Installation

1. **Clone and Build**
   ```bash
   git clone https://github.com/yourusername/Zgine.git
   cd Zgine
   vendor/bin/premake/premake5 gmake2
   make
   ```

2. **Run Sandbox**
   ```bash
   bin/Debug-linux-x86_64/Sandbox/Sandbox
   ```

### macOS Installation

1. **Clone and Build**
   ```bash
   git clone https://github.com/yourusername/Zgine.git
   cd Zgine
   vendor/bin/premake/premake5 xcode4
   # Open Zgine.xcodeproj in Xcode and build
   ```

## Creating Your First Application

### Basic Application Structure

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
    
    ~MyApp() {}
};

// Entry point
Zgine::Application* Zgine::CreateApplication()
{
    return new MyApp();
}
```

### Creating a Custom Layer

```cpp
class MyGameLayer : public Zgine::Layer
{
public:
    MyGameLayer()
        : Layer("MyGameLayer")
        , m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
    {
        // Initialize your layer
        Zgine::BatchRenderer2D::Init();
    }
    
    virtual ~MyGameLayer()
    {
        Zgine::BatchRenderer2D::Shutdown();
    }
    
    virtual void OnUpdate() override
    {
        // Update logic
        m_Camera.SetPosition({0.0f, 0.0f, 0.0f});
        
        // Rendering
        Zgine::BatchRenderer2D::BeginScene(m_Camera);
        
        // Draw primitives
        Zgine::BatchRenderer2D::DrawQuad(
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f} // Red color
        );
        
        Zgine::BatchRenderer2D::EndScene();
    }
    
    virtual void OnImGuiRender() override
    {
        ImGui::Begin("My Game Layer");
        ImGui::Text("Hello, Zgine!");
        ImGui::End();
    }
    
private:
    Zgine::OrthographicCamera m_Camera;
};
```

## Basic Rendering

### Drawing Primitives

#### Quad Rendering

```cpp
// Basic colored quad
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    {1.0f, 0.0f, 0.0f, 1.0f} // Red color
);

// Rotated quad
Zgine::BatchRenderer2D::DrawRotatedQuad(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    glm::pi<float>() / 4.0f, // 45 degrees rotation
    {0.0f, 1.0f, 0.0f, 1.0f} // Green color
);

// Textured quad
auto texture = Zgine::Texture2D::Create("path/to/texture.png");
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f},
    texture,
    {1.0f, 1.0f, 1.0f, 1.0f} // White tint
);
```

#### Line Rendering

```cpp
// Draw a line
Zgine::BatchRenderer2D::DrawLine(
    {-1.0f, 0.0f, 0.0f},    // Start point
    {1.0f, 0.0f, 0.0f},     // End point
    {0.0f, 0.0f, 1.0f, 1.0f}, // Blue color
    0.05f                   // Thickness
);
```

#### Circle Rendering

```cpp
// Filled circle
Zgine::BatchRenderer2D::DrawCircle(
    {0.0f, 0.0f, 0.0f},     // Position
    0.5f,                   // Radius
    {1.0f, 1.0f, 0.0f, 1.0f}, // Yellow color
    32                       // Segments
);

// Circle outline
Zgine::BatchRenderer2D::DrawCircleOutline(
    {0.0f, 0.0f, 0.0f},     // Position
    0.5f,                   // Radius
    {1.0f, 1.0f, 1.0f, 1.0f}, // White color
    0.02f,                  // Thickness
    32                       // Segments
);
```

### Camera System

```cpp
class CameraExample : public Zgine::Layer
{
public:
    CameraExample()
        : Layer("CameraExample")
        , m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
        , m_CameraPosition(0.0f)
        , m_CameraSpeed(0.02f)
    {
    }
    
    virtual void OnUpdate() override
    {
        // Camera movement
        if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
            m_CameraPosition.x += m_CameraSpeed;
        else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
            m_CameraPosition.x -= m_CameraSpeed;
            
        if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
            m_CameraPosition.y -= m_CameraSpeed;
        else if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
            m_CameraPosition.y += m_CameraSpeed;
        
        // Update camera
        m_Camera.SetPosition(m_CameraPosition);
        
        // Render
        Zgine::BatchRenderer2D::BeginScene(m_Camera);
        // ... draw primitives
        Zgine::BatchRenderer2D::EndScene();
    }
    
private:
    Zgine::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraSpeed;
};
```

## Input Handling

### Keyboard Input

```cpp
virtual void OnUpdate() override
{
    if (Zgine::Input::IsKeyPressed(ZG_KEY_SPACE))
    {
        // Space key is pressed
    }
    
    if (Zgine::Input::IsKeyPressed(ZG_KEY_ENTER))
    {
        // Enter key is pressed
    }
}
```

### Mouse Input

```cpp
virtual void OnEvent(Zgine::Event& e) override
{
    Zgine::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Zgine::MouseButtonPressedEvent>(
        [](Zgine::MouseButtonPressedEvent& e) {
            if (e.GetMouseButton() == ZG_MOUSE_BUTTON_LEFT)
            {
                // Left mouse button clicked
                return true;
            }
            return false;
        });
}
```

## ImGui Integration

### Basic ImGui Usage

```cpp
virtual void OnImGuiRender() override
{
    ImGui::Begin("My Window");
    
    // Text
    ImGui::Text("Hello, ImGui!");
    
    // Button
    if (ImGui::Button("Click Me"))
    {
        // Button clicked
    }
    
    // Slider
    static float value = 0.5f;
    ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
    
    // Checkbox
    static bool enabled = true;
    ImGui::Checkbox("Enabled", &enabled);
    
    ImGui::End();
}
```

### Performance Monitoring

```cpp
virtual void OnImGuiRender() override
{
    ImGui::Begin("Performance");
    
    auto stats = Zgine::BatchRenderer2D::GetStats();
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quad Count: %d", stats.QuadCount);
    ImGui::Text("Vertex Count: %d", stats.VertexCount);
    ImGui::Text("Index Count: %d", stats.IndexCount);
    
    if (ImGui::Button("Reset Stats"))
    {
        Zgine::BatchRenderer2D::ResetStats();
    }
    
    ImGui::End();
}
```

## Best Practices

### Performance Tips

1. **Batch Similar Primitives**: Group similar rendering calls together
2. **Minimize Draw Calls**: Use batch rendering to reduce draw calls
3. **Reuse Textures**: Cache and reuse textures when possible
4. **Monitor Statistics**: Track rendering performance regularly

### Memory Management

1. **Use Smart Pointers**: Prefer smart pointers over raw pointers
2. **RAII Principles**: Use RAII for resource management
3. **Avoid Manual Memory Management**: Let smart pointers handle cleanup
4. **Monitor Memory Usage**: Use profiling tools to track memory usage

### Code Organization

1. **Layer Separation**: Organize code into logical layers
2. **Event Handling**: Use the event system for decoupled communication
3. **Modular Design**: Keep modules independent and focused
4. **Documentation**: Document your code and APIs

## Common Patterns

### Game Loop Pattern

```cpp
class GameLayer : public Zgine::Layer
{
public:
    virtual void OnUpdate() override
    {
        // 1. Handle input
        HandleInput();
        
        // 2. Update game logic
        UpdateGameLogic();
        
        // 3. Render
        Render();
    }
    
private:
    void HandleInput() { /* Input handling */ }
    void UpdateGameLogic() { /* Game logic */ }
    void Render() { /* Rendering */ }
};
```

### Resource Management Pattern

```cpp
class ResourceManager
{
public:
    std::shared_ptr<Zgine::Texture2D> LoadTexture(const std::string& path)
    {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end())
            return it->second;
            
        auto texture = Zgine::Texture2D::Create(path);
        m_Textures[path] = texture;
        return texture;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Zgine::Texture2D>> m_Textures;
};
```

## Troubleshooting

### Common Issues

#### Build Errors
- **Missing Dependencies**: Ensure all required libraries are installed
- **Compiler Version**: Use supported compiler versions
- **Platform Issues**: Check platform-specific requirements

#### Runtime Errors
- **Memory Issues**: Check for memory leaks and invalid pointers
- **Rendering Issues**: Verify OpenGL context initialization
- **Input Issues**: Check event handling and input mapping

#### Performance Issues
- **Low FPS**: Monitor draw calls and primitive count
- **Memory Usage**: Track memory allocation and deallocation
- **CPU Usage**: Profile CPU-intensive operations

### Debugging Tips

1. **Use Statistics**: Monitor rendering statistics
2. **Enable Logging**: Use the logging system for debugging
3. **Profile Code**: Use profiling tools to identify bottlenecks
4. **Test Incrementally**: Test features incrementally

## Next Steps

Now that you have the basics, explore:

- [API Reference](../api/renderer.md) - Complete API documentation
- [Architecture Overview](../architecture/overview.md) - Engine architecture
- [Performance Guide](./performance.md) - Performance optimization
- [Examples](../examples/) - Code examples and tutorials

## Support

- **Documentation**: [docs/](../README.md)
- **Issues**: [GitHub Issues](https://github.com/yourusername/Zgine/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/Zgine/discussions)
