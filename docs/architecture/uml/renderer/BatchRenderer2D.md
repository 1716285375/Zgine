# BatchRenderer2D 类 UML 图

## BatchRenderer2D 类详细结构

```mermaid
classDiagram
    class BatchRenderer2D {
        -static uint32_t s_MaxQuads
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_QuadVertexArray
        -static Ref~VertexBuffer~ s_QuadVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static Ref~Texture2D~ s_WhiteTexture
        -static uint32_t s_QuadIndexCount
        -static QuadVertex* s_QuadVertexBufferBase
        -static QuadVertex* s_QuadVertexBufferPtr
        -static array~Ref~Texture2D~~ s_TextureSlots
        -static uint32_t s_TextureSlotIndex
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +EndScene()
        +Flush()
        +DrawQuad(vec2&, vec2&, vec4&)
        +DrawQuad(vec3&, vec2&, vec4&)
        +DrawRotatedQuad(vec2&, vec2&, float, vec4&)
        +DrawLine(vec3&, vec3&, vec4&, float)
        +DrawCircle(vec3&, float, vec4&, float, float)
        +GetStats() RenderStats
        +ResetStats()
    }
```

## BatchRenderer2D 类关系图

```mermaid
classDiagram
    class BatchRenderer2D {
        -static uint32_t s_MaxQuads
        -static uint32_t s_MaxVertices
        -static uint32_t s_MaxIndices
        -static Ref~VertexArray~ s_QuadVertexArray
        -static Ref~VertexBuffer~ s_QuadVertexBuffer
        -static Ref~Shader~ s_TextureShader
        -static Ref~Texture2D~ s_WhiteTexture
        -static uint32_t s_QuadIndexCount
        -static QuadVertex* s_QuadVertexBufferBase
        -static QuadVertex* s_QuadVertexBufferPtr
        -static array~Ref~Texture2D~~ s_TextureSlots
        -static uint32_t s_TextureSlotIndex
        -static RenderStats s_Stats
        -static bool s_Initialized
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +EndScene()
        +Flush()
        +DrawQuad(vec2&, vec2&, vec4&)
        +DrawQuad(vec3&, vec2&, vec4&)
        +DrawRotatedQuad(vec2&, vec2&, float, vec4&)
        +DrawLine(vec3&, vec3&, vec4&, float)
        +DrawCircle(vec3&, float, vec4&, float, float)
        +GetStats() RenderStats
        +ResetStats()
    }

    class QuadVertex {
        +vec3 Position
        +vec4 Color
        +vec2 TexCoord
        +float TexIndex
    }

    class RenderStats {
        +uint32_t DrawCalls
        +uint32_t QuadCount
        +uint32_t VertexCount
        +uint32_t IndexCount
        +GetTotalVertexCount() uint32_t
        +GetTotalIndexCount() uint32_t
    }

    class OrthographicCamera {
        -mat4 m_ProjectionMatrix
        -mat4 m_ViewMatrix
        -mat4 m_ViewProjectionMatrix
        -vec3 m_Position
        -float m_Rotation
        +OrthographicCamera(float, float, float, float)
        +SetProjection(float, float, float, float)
        +RecalculateViewMatrix()
        +GetProjectionMatrix() mat4&
        +GetViewMatrix() mat4&
        +GetViewProjectionMatrix() mat4&
        +GetPosition() vec3&
        +SetPosition(vec3&)
        +GetRotation() float
        +SetRotation(float)
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

    class Texture2D {
        <<interface>>
        +GetWidth() uint*
        +GetHeight() uint*
        +Bind(uint32_t)*
        +GetRendererID() uint32_t*
        +Create(uint32_t, uint32_t)*
        +Create(string&)*
    }

    BatchRenderer2D --> QuadVertex : uses
    BatchRenderer2D --> RenderStats : uses
    BatchRenderer2D --> OrthographicCamera : uses
    BatchRenderer2D --> VertexArray : uses
    BatchRenderer2D --> VertexBuffer : uses
    BatchRenderer2D --> Shader : uses
    BatchRenderer2D --> Texture2D : uses
```

## BatchRenderer2D 渲染流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant BR2D as BatchRenderer2D
    participant VA as VertexArray
    participant VB as VertexBuffer
    participant Sh as Shader
    participant Tex as Texture2D
    participant Cam as OrthographicCamera

    App->>BR2D: Init()
    BR2D->>VA: Create Quad VertexArray
    BR2D->>VB: Create Quad VertexBuffer
    BR2D->>Sh: Create Texture Shader
    BR2D->>Tex: Create White Texture
    
    loop Every Frame
        App->>BR2D: BeginScene(Camera)
        BR2D->>Cam: Get ViewProjectionMatrix
        BR2D->>Sh: Upload ViewProjectionMatrix
        
        loop Draw Calls
            App->>BR2D: DrawQuad(position, size, color)
            BR2D->>BR2D: Add Quad to Buffer
            BR2D->>BR2D: Check Buffer Full
            
            alt Buffer Full
                BR2D->>BR2D: Flush()
                BR2D->>VA: Bind()
                BR2D->>Sh: Bind()
                BR2D->>Tex: Bind Textures
                BR2D->>BR2D: DrawIndexed()
                BR2D->>BR2D: Reset Buffer
            end
        end
        
        App->>BR2D: EndScene()
        BR2D->>BR2D: Flush()
    end
    
    App->>BR2D: Shutdown()
    BR2D->>VA: Cleanup VertexArray
    BR2D->>VB: Cleanup VertexBuffer
    BR2D->>Sh: Cleanup Shader
    BR2D->>Tex: Cleanup Texture
```

## BatchRenderer2D 类说明

### 职责
- **2D渲染**: 高效的2D图元批处理渲染
- **图元支持**: 支持四边形、线条、圆形等2D图元
- **纹理批处理**: 支持多纹理的批处理渲染
- **性能优化**: 通过批处理减少绘制调用

### 设计模式
- **单例模式**: 静态方法提供全局访问
- **批处理模式**: 将多个绘制操作合并为一次调用
- **对象池模式**: 使用预分配的顶点缓冲区

### 支持的图元
- **四边形**: 基本矩形和旋转矩形
- **线条**: 带厚度的线条
- **圆形**: 实心圆形和圆形轮廓
- **三角形**: 基本三角形
- **椭圆**: 椭圆和椭圆轮廓
- **弧形**: 弧形绘制
- **渐变**: 支持颜色渐变的四边形

### 性能特性
- **批处理**: 将多个图元合并为一次绘制调用
- **纹理批处理**: 支持最多32个纹理的批处理
- **内存优化**: 使用预分配的顶点缓冲区
- **统计信息**: 提供详细的渲染统计
- **动态调整**: 支持运行时调整缓冲区大小

### 关键特性
- **高性能**: 优化的2D渲染管线
- **易用性**: 简单的API接口
- **灵活性**: 支持多种2D图元
- **可扩展**: 易于添加新的图元类型
- **调试友好**: 提供详细的性能统计
