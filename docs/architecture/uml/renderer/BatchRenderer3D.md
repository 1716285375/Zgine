# BatchRenderer3D 类 UML 图

## BatchRenderer3D 类详细结构

```mermaid
classDiagram
    class BatchRenderer3D {
        -static uint32_t s_MaxCubes
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_CubeVertexArray
        -static Ref~VertexBuffer~ s_CubeVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static uint32_t s_CubeIndexCount
        -static CubeVertex* s_CubeVertexBufferBase
        -static CubeVertex* s_CubeVertexBufferPtr
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Flush()
        +DrawCube(vec3&, vec3&, vec4&)
        +DrawRotatedCube(vec3&, vec3&, vec3&, vec4&)
        +GetStats() RenderStats
        +ResetStats()
    }
```

## BatchRenderer3D 类关系图

```mermaid
classDiagram
    class BatchRenderer3D {
        -static uint32_t s_MaxCubes
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_CubeVertexArray
        -static Ref~VertexBuffer~ s_CubeVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static uint32_t s_CubeIndexCount
        -static CubeVertex* s_CubeVertexBufferBase
        -static CubeVertex* s_CubeVertexBufferPtr
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Flush()
        +DrawCube(vec3&, vec3&, vec4&)
        +DrawRotatedCube(vec3&, vec3&, vec3&, vec4&)
        +GetStats() RenderStats
        +ResetStats()
    }

    class CubeVertex {
        +vec3 Position
        +vec4 Color
        +vec2 TexCoord
        +float TexIndex
    }

    class RenderStats {
        +uint32_t DrawCalls
        +uint32_t CubeCount
        +uint32_t VertexCount
        +uint32_t IndexCount
        +GetTotalVertexCount() uint32_t
        +GetTotalIndexCount() uint32_t
    }

    class PerspectiveCamera {
        -mat4 m_ProjectionMatrix
        -mat4 m_ViewMatrix
        -mat4 m_ViewProjectionMatrix
        -vec3 m_Position
        -vec3 m_Front
        -vec3 m_Up
        -vec3 m_Right
        -vec3 m_WorldUp
        -float m_Yaw
        -float m_Pitch
        -float m_MovementSpeed
        -float m_MouseSensitivity
        -float m_Zoom
        +PerspectiveCamera(vec3&, vec3&, float, float)
        +SetProjection(float, float, float, float)
        +RecalculateViewMatrix()
        +GetProjectionMatrix() mat4&
        +GetViewMatrix() mat4&
        +GetViewProjectionMatrix() mat4&
        +GetPosition() vec3&
        +SetPosition(vec3&)
        +ProcessKeyboard(CameraMovement, float)
        +ProcessMouseMovement(float, float, bool)
        +ProcessMouseScroll(float)
        +GetFront() vec3&
        +GetUp() vec3&
        +GetRight() vec3&
        +GetYaw() float
        +SetYaw(float)
        +GetPitch() float
        +SetPitch(float)
        +GetZoom() float
        +SetZoom(float)
    }

    class VertexArray {
        <<interface>>
        +Bind()*
        +Unbind()*
        +AddVertexBuffer(Ref~VertexBuffer~)*
        +SetIndexBuffer(Ref~IndexBuffer~)*
        +GetVertexBuffers() vector~Ref~VertexBuffer~~
        +GetIndexBuffer() Ref~IndexBuffer~
        +Create() Ref~VertexArray~
    }

    class VertexBuffer {
        <<interface>>
        +Bind()*
        +Unbind()*
        +GetLayout() BufferLayout&*
        +SetLayout(BufferLayout)*
        +Create(void*, uint32_t) Ref~VertexBuffer~
    }

    class Shader {
        -uint32_t m_RendererID
        +Shader(string&, string&)
        +~Shader()
        +Bind()
        +Unbind()
        +GetRendererID() uint32_t
        +UploadUniformFloat(string&, float)
        +UploadUniformFloat2(string&, vec2&)
        +UploadUniformFloat3(string&, vec3&)
        +UploadUniformFloat4(string&, vec4&)
        +UploadUniformInt(string&, int)
        +UploadUniformMat4(string&, mat4&)
        -GetUniformLocation(string&) int
    }

    BatchRenderer3D --> CubeVertex : uses
    BatchRenderer3D --> RenderStats : uses
    BatchRenderer3D --> PerspectiveCamera : uses
    BatchRenderer3D --> VertexArray : uses
    BatchRenderer3D --> VertexBuffer : uses
    BatchRenderer3D --> Shader : uses
```

## BatchRenderer3D 渲染流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant BR3D as BatchRenderer3D
    participant VA as VertexArray
    participant VB as VertexBuffer
    participant Sh as Shader
    participant Cam as PerspectiveCamera

    App->>BR3D: Init()
    BR3D->>VA: Create Cube VertexArray
    BR3D->>VB: Create Cube VertexBuffer
    BR3D->>Sh: Create 3D Shader
    
    loop Every Frame
        App->>BR3D: BeginScene(Camera)
        BR3D->>Cam: Get ViewProjectionMatrix
        BR3D->>Sh: Upload ViewProjectionMatrix
        
        loop Draw Calls
            App->>BR3D: DrawCube(position, size, color)
            BR3D->>BR3D: Add Cube to Buffer
            BR3D->>BR3D: Check Buffer Full
            
            alt Buffer Full
                BR3D->>BR3D: Flush()
                BR3D->>VA: Bind()
                BR3D->>Sh: Bind()
                BR3D->>BR3D: DrawIndexed()
                BR3D->>BR3D: Reset Buffer
            end
        end
        
        App->>BR3D: EndScene()
        BR3D->>BR3D: Flush()
    end
    
    App->>BR3D: Shutdown()
    BR3D->>VA: Cleanup VertexArray
    BR3D->>VB: Cleanup VertexBuffer
    BR3D->>Sh: Cleanup Shader
```

## BatchRenderer3D 类说明

### 职责
- **3D渲染**: 高效的3D图元批处理渲染
- **立方体渲染**: 支持基本立方体和旋转立方体
- **3D变换**: 支持3D空间的位置、旋转和缩放
- **性能优化**: 通过批处理减少绘制调用

### 设计模式
- **单例模式**: 静态方法提供全局访问
- **批处理模式**: 将多个绘制操作合并为一次调用
- **对象池模式**: 使用预分配的顶点缓冲区

### 支持的图元
- **立方体**: 基本立方体绘制
- **旋转立方体**: 支持任意旋转的立方体
- **3D变换**: 支持位置、旋转、缩放变换

### 性能特性
- **批处理**: 将多个立方体合并为一次绘制调用
- **内存优化**: 使用预分配的顶点缓冲区
- **统计信息**: 提供详细的渲染统计
- **动态调整**: 支持运行时调整缓冲区大小

### 关键特性
- **高性能**: 优化的3D渲染管线
- **易用性**: 简单的API接口
- **3D支持**: 完整的3D空间支持
- **可扩展**: 易于添加新的3D图元类型
- **调试友好**: 提供详细的性能统计

### 与BatchRenderer2D的区别
- **相机类型**: 使用透视相机而非正交相机
- **图元类型**: 专注于3D立方体而非2D图元
- **变换支持**: 支持完整的3D变换矩阵
- **深度测试**: 支持深度测试和深度缓冲
