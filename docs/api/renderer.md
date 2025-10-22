# Renderer API Reference

The Zgine rendering system provides high-performance 2D and 3D rendering capabilities with batch processing and modern OpenGL backend.

## Overview

The rendering system consists of two main components:
- `BatchRenderer2D`: Efficient batch rendering of 2D primitives including quads, lines, and circles
- `BatchRenderer3D`: High-performance 3D rendering with support for cubes, spheres, planes, and complex transformations

## 2D Rendering System

### BatchRenderer2D

The main rendering class for 2D graphics.

#### Static Methods

##### Initialization

```cpp
static void Init();
static void Shutdown();
```

**Description**: Initialize and shutdown the batch renderer system.

**Parameters**: None

**Returns**: void

**Example**:
```cpp
// Initialize the renderer
Zgine::BatchRenderer2D::Init();

// Use renderer...

// Shutdown when done
Zgine::BatchRenderer2D::Shutdown();
```

##### Scene Management

```cpp
static void BeginScene(const OrthographicCamera& camera);
static void EndScene();
static void Flush();
```

**Description**: Manage rendering scenes and batches.

**Parameters**:
- `camera`: Orthographic camera for view-projection matrix

**Returns**: void

**Example**:
```cpp
Zgine::OrthographicCamera camera(-2.0f, 2.0f, -1.5f, 1.5f);
Zgine::BatchRenderer2D::BeginScene(camera);

// Draw primitives...

Zgine::BatchRenderer2D::EndScene();
```

## Primitive Rendering

### Quad Rendering

#### Basic Quad

```cpp
static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
```

**Description**: Draw a colored quad.

**Parameters**:
- `position`: Position of the quad (2D or 3D)
- `size`: Size of the quad (width, height)
- `color`: RGBA color (0.0-1.0 range)

**Returns**: void

**Example**:
```cpp
// Draw a red quad
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    {1.0f, 0.0f, 0.0f, 1.0f} // Red color
);
```

#### Textured Quad

```cpp
static void DrawQuad(const glm::vec2& position, const glm::vec2& size, 
                    const Ref<Texture2D>& texture, 
                    const glm::vec4& tintColor = glm::vec4(1.0f));
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, 
                    const Ref<Texture2D>& texture, 
                    const glm::vec4& tintColor = glm::vec4(1.0f));
```

**Description**: Draw a textured quad with optional tinting.

**Parameters**:
- `position`: Position of the quad
- `size`: Size of the quad
- `texture`: Texture to apply (using `Ref<Texture2D>` smart pointer)
- `tintColor`: Color tint (default: white)

**Returns**: void

**Example**:
```cpp
auto texture = Zgine::Texture2D::Create("path/to/texture.png");
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f},
    texture,
    {1.0f, 1.0f, 1.0f, 1.0f} // White tint
);
```

### Rotated Quad Rendering

```cpp
static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, 
                           float rotation, const glm::vec4& color);
static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, 
                           float rotation, const glm::vec4& color);
static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, 
                           float rotation, const Ref<Texture2D>& texture, 
                           const glm::vec4& tintColor = glm::vec4(1.0f));
static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, 
                           float rotation, const Ref<Texture2D>& texture, 
                           const glm::vec4& tintColor = glm::vec4(1.0f));
```

**Description**: Draw a rotated quad.

**Parameters**:
- `position`: Position of the quad
- `size`: Size of the quad
- `rotation`: Rotation angle in radians
- `color`/`texture`: Color or texture
- `tintColor`: Color tint for textured quads

**Returns**: void

**Example**:
```cpp
// Draw a rotated red quad
Zgine::BatchRenderer2D::DrawRotatedQuad(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    glm::pi<float>() / 4.0f, // 45 degrees rotation
    {1.0f, 0.0f, 0.0f, 1.0f} // Red color
);
```

### Line Rendering

```cpp
static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, 
                    const glm::vec4& color, float thickness = 0.1f);
```

**Description**: Draw a line between two points.

**Parameters**:
- `p0`: Start point
- `p1`: End point
- `color`: Line color
- `thickness`: Line thickness (default: 0.1f)

**Returns**: void

**Example**:
```cpp
// Draw a thick blue line
Zgine::BatchRenderer2D::DrawLine(
    {-1.0f, 0.0f, 0.0f},    // Start point
    {1.0f, 0.0f, 0.0f},     // End point
    {0.0f, 0.0f, 1.0f, 1.0f}, // Blue color
    0.05f                   // Thickness
);
```

### Circle Rendering

#### Filled Circle

```cpp
static void DrawCircle(const glm::vec3& position, float radius, 
                      const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);
static void DrawCircle(const glm::vec3& position, float radius, 
                      const glm::vec4& color, int segments, 
                      float thickness = 1.0f, float fade = 0.005f);
```

**Description**: Draw a filled circle.

**Parameters**:
- `position`: Center position
- `radius`: Circle radius
- `color`: Circle color
- `segments`: Number of segments (3-64, default: 32)
- `thickness`: Thickness parameter (reserved for future use)
- `fade`: Fade parameter (reserved for future use)

**Returns**: void

**Example**:
```cpp
// Draw a smooth green circle
Zgine::BatchRenderer2D::DrawCircle(
    {0.0f, 0.0f, 0.0f},     // Position
    0.5f,                   // Radius
    {0.0f, 1.0f, 0.0f, 1.0f}, // Green color
    32                       // Segments
);
```

#### Circle Outline

```cpp
static void DrawCircleOutline(const glm::vec3& position, float radius, 
                             const glm::vec4& color, float thickness = 0.1f, 
                             int segments = 32);
```

**Description**: Draw a circle outline.

**Parameters**:
- `position`: Center position
- `radius`: Circle radius
- `color`: Outline color
- `thickness`: Outline thickness (default: 0.1f)
- `segments`: Number of segments (default: 32)

**Returns**: void

**Example**:
```cpp
// Draw a thin white circle outline
Zgine::BatchRenderer2D::DrawCircleOutline(
    {0.0f, 0.0f, 0.0f},     // Position
    0.5f,                   // Radius
    {1.0f, 1.0f, 1.0f, 1.0f}, // White color
    0.02f,                  // Thin outline
    32                       // Segments
);
```

## Statistics

### RenderStats

```cpp
struct RenderStats
{
    uint32_t DrawCalls = 0;
    uint32_t QuadCount = 0;
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;
};
```

**Description**: Rendering statistics structure.

**Fields**:
- `DrawCalls`: Number of draw calls made
- `QuadCount`: Number of quads rendered
- `VertexCount`: Number of vertices processed
- `IndexCount`: Number of indices processed

### Statistics Methods

```cpp
static RenderStats GetStats();
static void ResetStats();
```

**Description**: Get and reset rendering statistics.

**Returns**: 
- `GetStats()`: Current rendering statistics
- `ResetStats()`: void

**Example**:
```cpp
// Get current statistics
auto stats = Zgine::BatchRenderer2D::GetStats();
std::cout << "Draw Calls: " << stats.DrawCalls << std::endl;
std::cout << "Quads: " << stats.QuadCount << std::endl;

// Reset statistics
Zgine::BatchRenderer2D::ResetStats();
```

## Performance Considerations

### Batch Limits

- **Max Quads**: 10,000 per batch
- **Max Vertices**: 40,000 per batch (MaxQuads * 4)
- **Max Indices**: 60,000 per batch (MaxQuads * 6)
- **Max Texture Slots**: 32 textures per batch

### Best Practices

1. **Batch Similar Primitives**: Group similar rendering calls together
2. **Minimize Draw Calls**: Use batch rendering to reduce draw calls
3. **Texture Management**: Reuse textures when possible
4. **Statistics Monitoring**: Monitor rendering statistics for optimization

## Additional 2D Primitives

### Triangle

```cpp
static void DrawTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);
```

**Description**: Draw a filled triangle defined by three vertices.

**Parameters**:
- `p0`, `p1`, `p2`: Triangle vertices
- `color`: Triangle color

**Returns**: void

**Example**:
```cpp
// Draw a red triangle
Zgine::BatchRenderer2D::DrawTriangle(
    {-0.5f, -0.5f, 0.0f},  // Bottom-left
    {0.5f, -0.5f, 0.0f},   // Bottom-right
    {0.0f, 0.5f, 0.0f},    // Top
    {1.0f, 0.0f, 0.0f, 1.0f} // Red color
);
```

### Ellipse

```cpp
static void DrawEllipse(const glm::vec3& position, float radiusX, float radiusY, const glm::vec4& color, int segments = 32);
```

**Description**: Draw a filled ellipse.

**Parameters**:
- `position`: Center position
- `radiusX`: Horizontal radius
- `radiusY`: Vertical radius
- `color`: Ellipse color
- `segments`: Number of segments (default: 32)

**Returns**: void

**Example**:
```cpp
// Draw a purple ellipse
Zgine::BatchRenderer2D::DrawEllipse(
    {0.0f, 0.0f, 0.0f},     // Position
    0.4f, 0.2f,             // radiusX, radiusY
    {0.8f, 0.2f, 0.8f, 1.0f}, // Purple color
    16                       // Segments
);
```

### Ellipse Outline

```cpp
static void DrawEllipseOutline(const glm::vec3& position, float radiusX, float radiusY, const glm::vec4& color, float thickness = 0.1f, int segments = 32);
```

**Description**: Draw an ellipse outline.

**Parameters**:
- `position`: Center position
- `radiusX`: Horizontal radius
- `radiusY`: Vertical radius
- `color`: Outline color
- `thickness`: Line thickness (default: 0.1f)
- `segments`: Number of segments (default: 32)

**Returns**: void

### Arc

```cpp
static void DrawArc(const glm::vec3& position, float radius, float startAngle, float endAngle, const glm::vec4& color, float thickness = 0.1f, int segments = 32);
```

**Description**: Draw an arc (partial circle).

**Parameters**:
- `position`: Center position
- `radius`: Arc radius
- `startAngle`: Starting angle in radians
- `endAngle`: Ending angle in radians
- `color`: Arc color
- `thickness`: Line thickness (default: 0.1f)
- `segments`: Number of segments (default: 32)

**Returns**: void

**Example**:
```cpp
// Draw a quarter circle arc
Zgine::BatchRenderer2D::DrawArc(
    {0.0f, 0.0f, 0.0f},     // Position
    0.5f,                   // Radius
    0.0f,                   // Start angle
    glm::pi<float>() / 2.0f, // End angle (90 degrees)
    {1.0f, 1.0f, 0.0f, 1.0f}, // Yellow color
    0.05f,                  // Thickness
    16                      // Segments
);
```

## Advanced 2D Features

### Gradient Quad

```cpp
static void DrawQuadGradient(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
```

**Description**: Draw a quad with gradient colors at each corner.

**Parameters**:
- `position`: Center position
- `size`: Quad size
- `colorTopLeft`: Top-left corner color
- `colorTopRight`: Top-right corner color
- `colorBottomLeft`: Bottom-left corner color
- `colorBottomRight`: Bottom-right corner color

**Returns**: void

**Example**:
```cpp
// Draw a gradient quad
Zgine::BatchRenderer2D::DrawQuadGradient(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    {1.0f, 0.0f, 0.0f, 1.0f}, // Top-left: Red
    {0.0f, 1.0f, 0.0f, 1.0f}, // Top-right: Green
    {0.0f, 0.0f, 1.0f, 1.0f}, // Bottom-left: Blue
    {1.0f, 1.0f, 0.0f, 1.0f}  // Bottom-right: Yellow
);
```

### Rotated Gradient Quad

```cpp
static void DrawRotatedQuadGradient(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
```

**Description**: Draw a rotated quad with gradient colors.

**Parameters**:
- `position`: Center position
- `size`: Quad size
- `rotation`: Rotation angle in radians
- `colorTopLeft`: Top-left corner color
- `colorTopRight`: Top-right corner color
- `colorBottomLeft`: Bottom-left corner color
- `colorBottomRight`: Bottom-right corner color

**Returns**: void

## Transform Functions

### Quad with Transform Matrix

```cpp
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::mat4& transform, const glm::vec4& color);
static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
```

**Description**: Draw a quad with custom transformation matrix.

**Parameters**:
- `position`: Center position
- `size`: Quad size
- `transform`: Transformation matrix
- `color`: Quad color (for solid color version)
- `texture`: Texture to apply (for textured version)
- `tintColor`: Color tint for textured quad (default: white)

**Returns**: void

**Example**:
```cpp
// Create a transformation matrix
glm::mat4 transform = glm::mat4(1.0f);
transform = glm::translate(transform, glm::vec3(1.0f, 0.0f, 0.0f));
transform = glm::rotate(transform, glm::pi<float>() / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
transform = glm::scale(transform, glm::vec3(1.5f, 0.5f, 1.0f));

// Draw transformed quad
Zgine::BatchRenderer2D::DrawQuad(
    {0.0f, 0.0f, 0.0f},     // Position
    {1.0f, 1.0f},           // Size
    transform,              // Transform matrix
    {0.5f, 0.8f, 0.2f, 1.0f} // Color
);
```

## Error Handling

The renderer handles errors gracefully:

- **Batch Overflow**: Automatically flushes and starts new batch
- **Invalid Parameters**: Clamps values to valid ranges
- **Memory Issues**: Uses smart pointers for automatic cleanup

## Thread Safety

**Note**: The current implementation is not thread-safe. All rendering operations should be performed on the main thread.

## 3D Rendering System

### PerspectiveCamera

The 3D camera class for perspective projection rendering.

#### Constructor

```cpp
PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, 
                  float nearClip = 0.1f, float farClip = 1000.0f);
```

**Description**: Creates a perspective camera with specified parameters.

**Parameters**:
- `fov`: Field of view in degrees
- `aspectRatio`: Width to height ratio
- `nearClip`: Near clipping plane distance
- `farClip`: Far clipping plane distance

#### Methods

##### Projection Settings

```cpp
void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);
void SetFOV(float fov);
void SetAspectRatio(float aspectRatio);
void SetNearClip(float nearClip);
void SetFarClip(float farClip);
```

**Description**: Configure camera projection parameters.

##### Position and Rotation

```cpp
const glm::vec3& GetPosition() const;
void SetPosition(const glm::vec3& position);
const glm::vec3& GetRotation() const;
void SetRotation(const glm::vec3& rotation);
```

**Description**: Get and set camera position and rotation (yaw, pitch, roll).

##### Movement

```cpp
void MoveForward(float distance);
void MoveRight(float distance);
void MoveUp(float distance);
void Rotate(float yaw, float pitch);
void SetYaw(float yaw);
void SetPitch(float pitch);
```

**Description**: Move and rotate the camera.

##### Matrices

```cpp
const glm::mat4& GetProjectionMatrix() const;
const glm::mat4& GetViewMatrix() const;
const glm::mat4& GetViewProjectionMatrix() const;
```

**Description**: Get camera transformation matrices.

### BatchRenderer3D

The main rendering class for 3D graphics.

#### Static Methods

##### Initialization

```cpp
static void Init();
static void Shutdown();
```

**Description**: Initialize and shutdown the 3D batch renderer system.

##### Scene Management

```cpp
static void BeginScene(const PerspectiveCamera& camera);
static void EndScene();
static void Flush();
```

**Description**: Manage 3D rendering scenes and batches.

##### 3D Primitives

```cpp
// Cubes
static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
static void DrawCube(const glm::vec3& position, const glm::vec3& size, 
                     const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
static void DrawCube(const glm::vec3& position, const glm::vec3& size, 
                     const glm::mat4& transform, const glm::vec4& color);
static void DrawCube(const glm::vec3& position, const glm::vec3& size, 
                     const glm::mat4& transform, const Ref<Texture2D>& texture, 
                     const glm::vec4& tintColor = glm::vec4(1.0f));

// Spheres
static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, 
                       int segments = 32);
static void DrawSphere(const glm::vec3& position, float radius, 
                       const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), 
                       int segments = 32);

// Planes
static void DrawPlane(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
static void DrawPlane(const glm::vec3& position, const glm::vec2& size, 
                      const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
```

**Description**: Render 3D primitives with various options.

##### Statistics

```cpp
static RenderStats3D GetStats();
static void ResetStats();
```

**Description**: Get and reset rendering statistics.

#### RenderStats3D Structure

```cpp
struct RenderStats3D
{
    uint32_t DrawCalls = 0;
    uint32_t TriangleCount = 0;
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;
};
```

## 3D Examples

### Basic 3D Scene

```cpp
#include <Zgine.h>

class My3DLayer : public Zgine::Layer
{
public:
    My3DLayer() : Layer("My3DLayer"), m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f) 
    {
        m_Camera.SetPosition({0.0f, 5.0f, 10.0f});
    }

    virtual void OnUpdate(Zgine::Timestep ts) override
    {
        // Update camera
        if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
            m_Camera.MoveForward(5.0f * ts);
        if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
            m_Camera.MoveForward(-5.0f * ts);

        // Begin 3D rendering
        Zgine::BatchRenderer3D::BeginScene(m_Camera);

        // Draw cubes
        Zgine::BatchRenderer3D::DrawCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 
                                         {1.0f, 0.0f, 0.0f, 1.0f});
        Zgine::BatchRenderer3D::DrawCube({2.0f, 0.0f, 0.0f}, {0.5f, 2.0f, 0.5f}, 
                                         {0.0f, 1.0f, 0.0f, 1.0f});

        // Draw spheres
        Zgine::BatchRenderer3D::DrawSphere({-2.0f, 0.0f, 0.0f}, 1.0f, 
                                          {0.0f, 0.0f, 1.0f, 1.0f}, 32);

        // Draw ground plane
        Zgine::BatchRenderer3D::DrawPlane({0.0f, -2.0f, 0.0f}, {20.0f, 20.0f}, 
                                          {0.3f, 0.3f, 0.3f, 1.0f});

        // End rendering
        Zgine::BatchRenderer3D::EndScene();
    }

private:
    Zgine::PerspectiveCamera m_Camera;
};
```

### Combined 2D/3D Scene

```cpp
class CombinedLayer : public Zgine::Layer
{
public:
    CombinedLayer() : Layer("Combined"), 
        m_2DCamera(-2.0f, 2.0f, -1.5f, 1.5f),
        m_3DCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f) {}

    virtual void OnUpdate(Zgine::Timestep ts) override
    {
        // Render 3D scene first
        Zgine::BatchRenderer3D::BeginScene(m_3DCamera);
        Zgine::BatchRenderer3D::DrawCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 
                                         {1.0f, 0.0f, 0.0f, 1.0f});
        Zgine::BatchRenderer3D::EndScene();

        // Render 2D overlay
        Zgine::BatchRenderer2D::BeginScene(m_2DCamera);
        Zgine::BatchRenderer2D::DrawQuad({-1.5f, 1.0f}, {0.3f, 0.1f}, {1.0f, 1.0f, 1.0f, 0.8f});
        Zgine::BatchRenderer2D::EndScene();
    }

private:
    Zgine::OrthographicCamera m_2DCamera;
    Zgine::PerspectiveCamera m_3DCamera;
};
```
