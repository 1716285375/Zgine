# Circle Rendering Algorithm

This document describes the implementation details of the circle rendering algorithm in Zgine's BatchRenderer2D system.

## Overview

The circle rendering algorithm decomposes circles into multiple triangular sectors to maintain compatibility with the existing quad-based batch rendering system. This approach ensures efficient GPU utilization while providing high-quality circular rendering.

## Algorithm Design

### Core Concept

Instead of using traditional triangle fan rendering, we decompose circles into multiple triangular sectors, each represented as a quad with two identical vertices to form a triangle. This approach:

1. **Maintains Compatibility**: Uses the existing 4-vertex, 6-index quad system
2. **Ensures Efficiency**: Leverages the optimized batch rendering pipeline
3. **Provides Quality**: Supports configurable segment count for quality control

### Mathematical Foundation

#### Sector Calculation

For a circle with radius `r` and `n` segments:

```
angleStep = 2π / n
```

Each sector `i` spans from angle `i * angleStep` to `(i + 1) * angleStep`.

#### Vertex Generation

For sector `i`:
```cpp
float angle1 = i * angleStep;
float angle2 = (i + 1) * angleStep;

glm::vec3 center = position;
glm::vec3 v1 = position + glm::vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
glm::vec3 v2 = position + glm::vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
```

## Implementation Details

### Filled Circle Algorithm

```cpp
void BatchRenderer2D::DrawCircle(const glm::vec3& position, float radius, 
                                const glm::vec4& color, int segments, 
                                float thickness, float fade)
{
    // Clamp segments to reasonable range
    segments = glm::clamp(segments, 3, 64);
    const float angleStep = 2.0f * glm::pi<float>() / segments;
    
    // Draw circle as multiple triangular sectors
    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        // Calculate sector vertices
        glm::vec3 center = position;
        glm::vec3 v1 = position + glm::vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
        glm::vec3 v2 = position + glm::vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
        
        // Create triangular sector using center and two edge points
        glm::vec3 quadVertices[4] = {
            center,  // Bottom-left
            v1,      // Bottom-right  
            v2,      // Top-right
            center   // Top-left (same as center to form triangle)
        };
        
        // Add vertices to buffer
        for (int j = 0; j < 4; j++)
        {
            s_QuadVertexBufferPtr->Position = quadVertices[j];
            s_QuadVertexBufferPtr->Color = color;
            s_QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
            s_QuadVertexBufferPtr->TexIndex = 0.0f;
            s_QuadVertexBufferPtr++;
        }
        
        s_QuadIndexCount += 6;
    }
    
    s_Stats.QuadCount += segments;
}
```

### Circle Outline Algorithm

```cpp
void BatchRenderer2D::DrawCircleOutline(const glm::vec3& position, float radius, 
                                       const glm::vec4& color, float thickness, 
                                       int segments)
{
    // Clamp segments to reasonable range
    segments = glm::clamp(segments, 3, 64);
    const float angleStep = 2.0f * glm::pi<float>() / segments;
    
    // Draw circle outline as connected lines
    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        // Calculate line segment points
        glm::vec3 p1 = position + glm::vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
        glm::vec3 p2 = position + glm::vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
        
        // Draw line segment as a quad
        DrawLine(p1, p2, color, thickness);
    }
}
```

## Performance Analysis

### Complexity

- **Time Complexity**: O(n) where n is the number of segments
- **Space Complexity**: O(1) per circle (vertices stored in batch buffer)

### Memory Usage

For each circle:
- **Vertices**: 4 * n vertices (where n = segments)
- **Indices**: 6 * n indices
- **Total Memory**: (4 * sizeof(QuadVertex) + 6 * sizeof(uint32_t)) * n

### Quality vs Performance Trade-offs

| Segments | Quality | Performance | Use Case |
|----------|---------|------------|----------|
| 3-8      | Low     | High       | Basic shapes, distant objects |
| 16-24    | Medium  | Medium     | General purpose |
| 32-48    | High    | Medium     | Close-up objects |
| 64+      | Very High | Low      | High-detail rendering |

## Optimization Techniques

### 1. Segment Clamping

```cpp
segments = glm::clamp(segments, 3, 64);
```

Prevents excessive segment counts that could impact performance.

### 2. Batch Overflow Handling

```cpp
if (s_QuadIndexCount >= MaxIndices - 6)
    NextBatch();
```

Automatically handles batch overflow by flushing and starting a new batch.

### 3. Trigonometric Optimization

The algorithm uses standard trigonometric functions (`cos`, `sin`). For high-performance scenarios, consider:

- **Lookup Tables**: Pre-computed trigonometric values
- **SIMD Instructions**: Vectorized trigonometric calculations
- **Approximation Methods**: Fast trigonometric approximations

## Visual Quality Considerations

### Segment Count Guidelines

- **Minimum (3 segments)**: Forms a triangle, useful for low-detail rendering
- **Low Quality (8 segments)**: Visible polygon edges, suitable for distant objects
- **Medium Quality (16 segments)**: Smooth appearance at normal viewing distances
- **High Quality (32 segments)**: Very smooth, suitable for close-up rendering
- **Maximum (64 segments)**: Near-perfect circles, use sparingly

### Anti-aliasing

The current implementation does not include anti-aliasing. For smooth edges, consider:

1. **MSAA**: Multi-sample anti-aliasing at the GPU level
2. **FXAA**: Fast approximate anti-aliasing
3. **Distance Fields**: Signed distance field rendering

## Future Enhancements

### Planned Features

1. **Gradient Circles**: Support for radial gradients
2. **Elliptical Circles**: Non-uniform scaling support
3. **Arc Rendering**: Partial circle rendering
4. **Sector Rendering**: Pie chart style rendering

### Performance Improvements

1. **Instanced Rendering**: Batch multiple circles with same parameters
2. **LOD System**: Automatic segment reduction based on distance
3. **Caching**: Cache vertex data for frequently used circles

## Testing and Validation

### Unit Tests

```cpp
// Test basic circle rendering
TEST(CircleRendering, BasicCircle) {
    Zgine::BatchRenderer2D::Init();
    
    // Draw a circle
    Zgine::BatchRenderer2D::DrawCircle({0, 0, 0}, 1.0f, {1, 0, 0, 1}, 32);
    
    auto stats = Zgine::BatchRenderer2D::GetStats();
    EXPECT_EQ(stats.QuadCount, 32);
    
    Zgine::BatchRenderer2D::Shutdown();
}
```

### Visual Tests

- **Quality Verification**: Render circles with different segment counts
- **Performance Profiling**: Measure rendering time vs segment count
- **Memory Usage**: Monitor vertex buffer usage

## References

1. **OpenGL Programming Guide**: Triangle fan rendering techniques
2. **Real-Time Rendering**: Efficient primitive rendering
3. **GPU Gems**: Advanced rendering algorithms
4. **Mathematics for 3D Game Programming**: Trigonometric calculations
