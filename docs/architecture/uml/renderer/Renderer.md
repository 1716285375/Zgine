# Renderer 类 UML 图

## Renderer 类详细结构

```mermaid
classDiagram
    class Renderer {
        -static SceneData* m_SceneData
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Submit(Ref~Shader~, Ref~VertexArray~)
        +GetAPI() RendererAPI::API
    }
```

## Renderer 类关系图

```mermaid
classDiagram
    class Renderer {
        -static SceneData* m_SceneData
        +Init()
        +Shutdown()
        +BeginScene(OrthographicCamera&)
        +BeginScene(PerspectiveCamera&)
        +EndScene()
        +Submit(Ref~Shader~, Ref~VertexArray~)
        +GetAPI() RendererAPI::API
    }

    class SceneData {
        +glm::mat4 ViewProjectionMatrix
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

    class RendererAPI {
        <<interface>>
        +SetClearColor(vec4&)*
        +Clear()*
        +DrawIndexed(Ref~VertexArray~)*
        +DrawIndexed(Ref~VertexArray~, uint32_t)*
        +GetAPI() API
    }

    Renderer --> SceneData : contains
    Renderer --> OrthographicCamera : uses
    Renderer --> PerspectiveCamera : uses
    Renderer --> Shader : uses
    Renderer --> VertexArray : uses
    Renderer --> RendererAPI : uses
```

## Renderer 渲染流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant R as Renderer
    participant RC as RenderCommand
    participant RA as RendererAPI
    participant Cam as Camera
    participant Sh as Shader
    participant VA as VertexArray

    App->>R: Init()
    R->>RC: Initialize RenderCommand
    RC->>RA: Set RendererAPI implementation
    
    loop Every Frame
        App->>R: BeginScene(Camera)
        R->>Cam: Get ViewProjectionMatrix
        R->>R: Store SceneData
        
        App->>R: Submit(Shader, VertexArray)
        R->>Sh: Bind()
        R->>VA: Bind()
        R->>RC: DrawIndexed(VertexArray)
        RC->>RA: DrawIndexed(VertexArray)
        
        App->>R: EndScene()
        R->>RC: Clear()
        RC->>RA: Clear()
    end
    
    App->>R: Shutdown()
    R->>RC: Cleanup RenderCommand
```

## Renderer 类说明

### 职责
- **渲染管理**: 管理整个渲染流程
- **场景管理**: 管理场景数据和相机
- **绘制提交**: 提交绘制命令到渲染管线
- **API抽象**: 提供平台无关的渲染接口

### 设计模式
- **外观模式**: 提供简化的渲染接口
- **单例模式**: 静态方法提供全局访问
- **策略模式**: 支持不同的渲染API

### 渲染流程
1. **Init()**: 初始化渲染系统
2. **BeginScene()**: 开始场景渲染，设置相机
3. **Submit()**: 提交绘制命令
4. **EndScene()**: 结束场景渲染
5. **Shutdown()**: 清理渲染资源

### 关键特性
- **平台无关**: 通过RendererAPI抽象实现跨平台
- **高效渲染**: 优化的渲染管线
- **灵活相机**: 支持正交和透视相机
- **资源管理**: 自动管理渲染资源
- **错误处理**: 内置错误检查和报告
