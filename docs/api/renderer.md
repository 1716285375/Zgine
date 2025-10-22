# Renderer API Reference

The Zgine rendering system provides high-performance 2D rendering capabilities with batch processing and modern OpenGL backend.

## Overview

The rendering system is built around the `BatchRenderer2D` class, which provides efficient batch rendering of 2D primitives including quads, lines, and circles.

## Core Classes

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
