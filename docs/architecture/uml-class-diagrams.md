# Zgine 引擎 UML 类图

## 核心架构类图

```mermaid
classDiagram
    %% 应用程序核心
    class Application {
        -Scope~Window~ m_Window
        -Scope~ImGuiLayer~ m_ImGuiLayer
        -bool m_Running
        -LayerStack m_LayerStack
        -static Application* s_Instance
        +Application()
        +~Application()
        +Run()
        +OnEvent(Event& e)
        +PushLayer(Layer* layer)
        +PushOverlay(Layer* overlay)
        +PopLayer(Layer* layer)
        +PopOverlay(Layer* overlay)
        +GetWindow() Window&
        +Get() Application&
        +OnApplicationStart()
        -OnWindowClose(WindowCloseEvent& e) bool
    }

    class Layer {
        -string m_DebugName
        +Layer(string name)
        +~Layer()
        +OnAttach()
        +OnDetach()
        +OnUpdate(Timestep ts)
        +OnImGuiRender()
        +OnEvent(Event& event)
        +GetName() string&
    }

    class LayerStack {
        -vector~Layer*~ m_Layers
        -uint m_LayerInsertIndex
        +LayerStack()
        +~LayerStack()
        +PushLayer(Layer* layer)
        +PushOverlay(Layer* overlay)
        +PopLayer(Layer* layer)
        +PopOverlay(Layer* overlay)
        +begin() iterator
        +end() iterator
    }

    class ImGuiLayer {
        -float m_Time
        +ImGuiLayer()
        +~ImGuiLayer()
        +OnAttach()
        +OnDetach()
        +OnImGuiRender()
        +OnEvent(Event& e)
        +Begin()
        +End()
    }

    %% 窗口系统
    class Window {
        <<interface>>
        +~Window()
        +OnUpdate()*
        +GetWidth() uint*
        +GetHeight() uint*
        +SetEventCallback(EventCallbackFn)*
        +SetVSync(bool)*
        +IsVSync() bool*
        +GetNativeWindow() void**
        +Create(WindowProps) Window*
    }

    class WindowsWindow {
        -GLFWwindow* m_Window
        -Scope~GraphicsContext~ m_Context
        -WindowData m_Data
        +WindowsWindow(WindowProps)
        +~WindowsWindow()
        +OnUpdate()
        +GetWidth() uint
        +GetHeight() uint
        +SetEventCallback(EventCallbackFn)
        +SetVSync(bool)
        +IsVSync() bool
        +GetNativeWindow() void*
        -Init(WindowProps)
        -Shutdown()
    }

    class GraphicsContext {
        <<interface>>
        +Init()*
        +SwapBuffers()*
    }

    class OpenGLContext {
        -GLFWwindow* m_WindowHandle
        +OpenGLContext(GLFWwindow*)
        +~OpenGLContext()
        +Init()
        +SwapBuffers()
    }

    %% 渲染系统
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

    class RenderCommand {
        -static RendererAPI* s_RendererAPI
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
        +CheckOpenGLError(string&)
    }

    class RendererAPI {
        <<interface>>
        +SetClearColor(vec4&)*
        +Clear()*
        +DrawIndexed(Ref~VertexArray~)*
        +DrawIndexed(Ref~VertexArray~, uint32_t)*
        +GetAPI() API
    }

    class OpenGLRendererAPI {
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
    }

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

    %% 着色器和纹理
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

    class OpenGLTexture2D {
        -uint32_t m_RendererID
        -uint32_t m_Width
        -uint32_t m_Height
        -uint32_t m_InternalFormat
        -uint32_t m_DataFormat
        +OpenGLTexture2D(uint32_t, uint32_t)
        +OpenGLTexture2D(string&)
        +~OpenGLTexture2D()
        +GetWidth() uint32_t
        +GetHeight() uint32_t
        +Bind(uint32_t)
        +GetRendererID() uint32_t
    }

    %% 事件系统
    class Event {
        <<abstract>>
        -bool m_Handled
        +GetEventType() EventType*
        +GetName() char**
        +GetCategoryFlags() int*
        +ToString() string
        +IsInCategory(EventCategory) bool
        +Handled() bool
    }

    class EventDispatcher {
        -Event& m_Event
        +EventDispatcher(Event&)
        +Dispatch~T~(EventFn~T~) bool
    }

    class WindowCloseEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class KeyPressedEvent {
        -int m_KeyCode
        -int m_RepeatCount
        +KeyPressedEvent(int, int)
        +GetKeyCode() int
        +GetRepeatCount() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    %% Sandbox 应用层
    class SandboxApp {
        +SandboxApp()
        +~SandboxApp()
        +OnApplicationStart()
    }

    class MainControlLayer {
        -UIManager m_UIManager
        -RenderManager m_RenderManager
        -SceneManager m_SceneManager
        -SettingsManager m_SettingsManager
        -bool m_PerformanceMonitoringEnabled
        +MainControlLayer()
        +~MainControlLayer()
        +OnAttach()
        +OnUpdate(Timestep)
        +OnImGuiRender()
        +OnEvent(Event&)
        -OnWindowResize(WindowResizeEvent&) bool
    }

    class Test2DLayer {
        -Test2DModule m_Test2DModule
        +Test2DLayer()
        +~Test2DLayer()
        +OnAttach()
        +OnUpdate(Timestep)
        +OnImGuiRender()
        +OnEvent(Event&)
        +GetTest2DModule() Test2DModule&
    }

    class Test3DLayer {
        -Test3DModule m_Test3DModule
        +Test3DLayer()
        +~Test3DLayer()
        +OnAttach()
        +OnUpdate(Timestep)
        +OnImGuiRender()
        +OnEvent(Event&)
        +GetTest3DModule() Test3DModule&
    }

    class UIManager {
        -unordered_map~string, WindowInfo~ m_Windows
        -unordered_map~string, MenuCategory~ m_MenuCategories
        -unordered_map~string, UILayout~ m_Layouts
        -bool m_ShowMainMenu
        -bool m_ShowStatusBar
        -bool m_EnableDocking
        -bool m_ShowPerformanceOverlay
        -UITheme m_CurrentTheme
        -RenderManager* m_RenderManager
        -SceneManager* m_SceneManager
        -SettingsManager* m_SettingsManager
        -Test2DModule* m_Test2DModule
        -Test3DModule* m_Test3DModule
        +UIManager()
        +~UIManager()
        +OnAttach()
        +OnUpdate(float)
        +OnImGuiRender()
        +OnEvent(Event&)
        +RegisterWindow(string&, function~void()~, bool)
        +ShowWindow(string&, bool)
        +IsWindowVisible(string&) bool
        +ToggleWindow(string&)
        +SetTheme(UITheme)
        +GetCurrentTheme() UITheme
        +SaveCurrentLayout(string&)
        +LoadLayout(string&)
        +SetRenderManager(RenderManager*)
        +SetSceneManager(SceneManager*)
        +SetSettingsManager(SettingsManager*)
        +SetTest2DModule(Test2DModule*)
        +SetTest3DModule(Test3DModule*)
    }

    %% 关系定义
    Application --> LayerStack : contains
    Application --> Window : contains
    Application --> ImGuiLayer : contains
    LayerStack --> Layer : manages
    Layer <|-- ImGuiLayer : implements
    Layer <|-- MainControlLayer : implements
    Layer <|-- Test2DLayer : implements
    Layer <|-- Test3DLayer : implements
    
    Window <|-- WindowsWindow : implements
    WindowsWindow --> GraphicsContext : contains
    GraphicsContext <|-- OpenGLContext : implements
    
    Renderer --> RenderCommand : uses
    RenderCommand --> RendererAPI : uses
    RendererAPI <|-- OpenGLRendererAPI : implements
    
    BatchRenderer2D --> Shader : uses
    BatchRenderer2D --> Texture2D : uses
    BatchRenderer3D --> Shader : uses
    BatchRenderer3D --> Texture2D : uses
    
    Texture2D <|-- OpenGLTexture2D : implements
    
    Event <|-- WindowCloseEvent : implements
    Event <|-- KeyPressedEvent : implements
    EventDispatcher --> Event : dispatches
    
    SandboxApp --> Application : extends
    MainControlLayer --> UIManager : contains
    MainControlLayer --> RenderManager : contains
    MainControlLayer --> SceneManager : contains
    MainControlLayer --> SettingsManager : contains
    
    Test2DLayer --> Test2DModule : contains
    Test3DLayer --> Test3DModule : contains
```

## 渲染系统详细类图

```mermaid
classDiagram
    %% 渲染系统核心
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

    class RenderCommand {
        -static RendererAPI* s_RendererAPI
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
        +CheckOpenGLError(string&)
    }

    class RendererAPI {
        <<interface>>
        +SetClearColor(vec4&)*
        +Clear()*
        +DrawIndexed(Ref~VertexArray~)*
        +DrawIndexed(Ref~VertexArray~, uint32_t)*
        +GetAPI() API
    }

    class OpenGLRendererAPI {
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
    }

    %% 批处理渲染器
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

    %% 相机系统
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

    %% 着色器系统
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

    %% 纹理系统
    class Texture2D {
        <<interface>>
        +GetWidth() uint*
        +GetHeight() uint*
        +Bind(uint32_t)*
        +GetRendererID() uint32_t*
        +Create(uint32_t, uint32_t)*
        +Create(string&)*
    }

    class OpenGLTexture2D {
        -uint32_t m_RendererID
        -uint32_t m_Width
        -uint32_t m_Height
        -uint32_t m_InternalFormat
        -uint32_t m_DataFormat
        +OpenGLTexture2D(uint32_t, uint32_t)
        +OpenGLTexture2D(string&)
        +~OpenGLTexture2D()
        +GetWidth() uint32_t
        +GetHeight() uint32_t
        +Bind(uint32_t)
        +GetRendererID() uint32_t
    }

    %% 顶点数组和缓冲区
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

    class OpenGLVertexArray {
        -uint32_t m_RendererID
        -vector~Ref~VertexBuffer~~ m_VertexBuffers
        -Ref~IndexBuffer~ m_IndexBuffer
        +OpenGLVertexArray()
        +~OpenGLVertexArray()
        +Bind()
        +Unbind()
        +AddVertexBuffer(Ref~VertexBuffer~)
        +SetIndexBuffer(Ref~IndexBuffer~)
        +GetVertexBuffers() vector~Ref~VertexBuffer~~&
        +GetIndexBuffer() Ref~IndexBuffer~
    }

    class VertexBuffer {
        <<interface>>
        +Bind()*
        +Unbind()*
        +GetLayout() BufferLayout&*
        +SetLayout(BufferLayout)*
        +Create(void*, uint32_t) Ref~VertexBuffer~
    }

    class OpenGLVertexBuffer {
        -uint32_t m_RendererID
        -BufferLayout m_Layout
        +OpenGLVertexBuffer(float*, uint32_t)
        +~OpenGLVertexBuffer()
        +Bind()
        +Unbind()
        +GetLayout() BufferLayout&
        +SetLayout(BufferLayout)
    }

    class IndexBuffer {
        <<interface>>
        +Bind()*
        +Unbind()*
        +GetCount() uint32_t*
        +Create(uint32_t*, uint32_t) Ref~IndexBuffer~
    }

    class OpenGLIndexBuffer {
        -uint32_t m_RendererID
        -uint32_t m_Count
        +OpenGLIndexBuffer(uint32_t*, uint32_t)
        +~OpenGLIndexBuffer()
        +Bind()
        +Unbind()
        +GetCount() uint32_t
    }

    %% 关系定义
    Renderer --> RenderCommand : uses
    RenderCommand --> RendererAPI : uses
    RendererAPI <|-- OpenGLRendererAPI : implements
    
    BatchRenderer2D --> OrthographicCamera : uses
    BatchRenderer3D --> PerspectiveCamera : uses
    
    BatchRenderer2D --> Shader : uses
    BatchRenderer3D --> Shader : uses
    BatchRenderer2D --> Texture2D : uses
    BatchRenderer3D --> Texture2D : uses
    
    Texture2D <|-- OpenGLTexture2D : implements
    
    VertexArray <|-- OpenGLVertexArray : implements
    VertexBuffer <|-- OpenGLVertexBuffer : implements
    IndexBuffer <|-- OpenGLIndexBuffer : implements
    
    OpenGLVertexArray --> OpenGLVertexBuffer : contains
    OpenGLVertexArray --> OpenGLIndexBuffer : contains
```

## 事件系统类图

```mermaid
classDiagram
    %% 事件基类
    class Event {
        <<abstract>>
        -bool m_Handled
        +GetEventType() EventType*
        +GetName() char**
        +GetCategoryFlags() int*
        +ToString() string
        +IsInCategory(EventCategory) bool
        +Handled() bool
    }

    class EventDispatcher {
        -Event& m_Event
        +EventDispatcher(Event&)
        +Dispatch~T~(EventFn~T~) bool
    }

    %% 应用事件
    class WindowCloseEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class WindowResizeEvent {
        -uint32_t m_Width
        -uint32_t m_Height
        +WindowResizeEvent(uint32_t, uint32_t)
        +GetWidth() uint32_t
        +GetHeight() uint32_t
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class WindowFocusEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class WindowLostFocusEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class WindowMovedEvent {
        -uint32_t m_X
        -uint32_t m_Y
        +WindowMovedEvent(uint32_t, uint32_t)
        +GetX() uint32_t
        +GetY() uint32_t
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class AppTickEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class AppUpdateEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class AppRenderEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    %% 键盘事件
    class KeyPressedEvent {
        -int m_KeyCode
        -int m_RepeatCount
        +KeyPressedEvent(int, int)
        +GetKeyCode() int
        +GetRepeatCount() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class KeyReleasedEvent {
        -int m_KeyCode
        +KeyReleasedEvent(int)
        +GetKeyCode() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class KeyTypedEvent {
        -int m_KeyCode
        +KeyTypedEvent(int)
        +GetKeyCode() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    %% 鼠标事件
    class MouseButtonPressedEvent {
        -int m_Button
        +MouseButtonPressedEvent(int)
        +GetMouseButton() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class MouseButtonReleasedEvent {
        -int m_Button
        +MouseButtonReleasedEvent(int)
        +GetMouseButton() int
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class MouseMovedEvent {
        -float m_MouseX
        -float m_MouseY
        +MouseMovedEvent(float, float)
        +GetX() float
        +GetY() float
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    class MouseScrolledEvent {
        -float m_XOffset
        -float m_YOffset
        +MouseScrolledEvent(float, float)
        +GetXOffset() float
        +GetYOffset() float
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    %% 关系定义
    Event <|-- WindowCloseEvent : implements
    Event <|-- WindowResizeEvent : implements
    Event <|-- WindowFocusEvent : implements
    Event <|-- WindowLostFocusEvent : implements
    Event <|-- WindowMovedEvent : implements
    Event <|-- AppTickEvent : implements
    Event <|-- AppUpdateEvent : implements
    Event <|-- AppRenderEvent : implements
    Event <|-- KeyPressedEvent : implements
    Event <|-- KeyReleasedEvent : implements
    Event <|-- KeyTypedEvent : implements
    Event <|-- MouseButtonPressedEvent : implements
    Event <|-- MouseButtonReleasedEvent : implements
    Event <|-- MouseMovedEvent : implements
    Event <|-- MouseScrolledEvent : implements
    
    EventDispatcher --> Event : dispatches
```

## 平台抽象层类图

```mermaid
classDiagram
    %% 窗口系统
    class Window {
        <<interface>>
        +~Window()
        +OnUpdate()*
        +GetWidth() uint*
        +GetHeight() uint*
        +SetEventCallback(EventCallbackFn)*
        +SetVSync(bool)*
        +IsVSync() bool*
        +GetNativeWindow() void**
        +Create(WindowProps) Window*
    }

    class WindowsWindow {
        -GLFWwindow* m_Window
        -Scope~GraphicsContext~ m_Context
        -WindowData m_Data
        +WindowsWindow(WindowProps)
        +~WindowsWindow()
        +OnUpdate()
        +GetWidth() uint
        +GetHeight() uint
        +SetEventCallback(EventCallbackFn)
        +SetVSync(bool)
        +IsVSync() bool
        +GetNativeWindow() void*
        -Init(WindowProps)
        -Shutdown()
    }

    class WindowProps {
        +string Title
        +uint Width
        +uint Height
        +WindowProps(string, uint, uint)
    }

    class WindowData {
        +string Title
        +uint Width
        +uint Height
        +bool VSync
        +EventCallbackFn EventCallback
    }

    %% 图形上下文
    class GraphicsContext {
        <<interface>>
        +Init()*
        +SwapBuffers()*
    }

    class OpenGLContext {
        -GLFWwindow* m_WindowHandle
        +OpenGLContext(GLFWwindow*)
        +~OpenGLContext()
        +Init()
        +SwapBuffers()
    }

    %% 输入系统
    class Input {
        <<interface>>
        +IsKeyPressed(int) bool*
        +IsMouseButtonPressed(int) bool*
        +GetMousePosition() pair~float,float~*
        +GetMouseX() float*
        +GetMouseY() float*
    }

    class WindowsInput {
        +IsKeyPressed(int) bool
        +IsMouseButtonPressed(int) bool
        +GetMousePosition() pair~float,float~
        +GetMouseX() float
        +GetMouseY() float
    }

    %% 渲染API抽象
    class RendererAPI {
        <<interface>>
        +SetClearColor(vec4&)*
        +Clear()*
        +DrawIndexed(Ref~VertexArray~)*
        +DrawIndexed(Ref~VertexArray~, uint32_t)*
        +GetAPI() API
    }

    class OpenGLRendererAPI {
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
    }

    class Direct3DRendererAPI {
        +SetClearColor(vec4&)
        +Clear()
        +DrawIndexed(Ref~VertexArray~)
        +DrawIndexed(Ref~VertexArray~, uint32_t)
    }

    %% 缓冲区抽象
    class Buffer {
        <<interface>>
        +Bind()*
        +Unbind()*
    }

    class OpenGLBuffer {
        -uint32_t m_RendererID
        +OpenGLBuffer()
        +~OpenGLBuffer()
        +Bind()
        +Unbind()
    }

    class Direct3DBuffer {
        +Direct3DBuffer()
        +~Direct3DBuffer()
        +Bind()
        +Unbind()
    }

    %% 关系定义
    Window <|-- WindowsWindow : implements
    WindowsWindow --> WindowProps : uses
    WindowsWindow --> WindowData : contains
    WindowsWindow --> GraphicsContext : contains
    
    GraphicsContext <|-- OpenGLContext : implements
    
    Input <|-- WindowsInput : implements
    
    RendererAPI <|-- OpenGLRendererAPI : implements
    RendererAPI <|-- Direct3DRendererAPI : implements
    
    Buffer <|-- OpenGLBuffer : implements
    Buffer <|-- Direct3DBuffer : implements
```

## 性能监控系统类图

```mermaid
classDiagram
    %% 性能监控核心
    class PerformanceProfiler {
        -static unordered_map~string, ProfileResult~ s_ProfileResults
        -static bool s_Enabled
        +BeginSession(string&, string&)
        +EndSession()
        +WriteProfile(ProfileResult&)
        +GetProfileResults() unordered_map~string, ProfileResult~&
        +IsEnabled() bool
        +SetEnabled(bool)
    }

    class ProfileResult {
        +string Name
        +long long Start
        +long long End
        +uint32_t ThreadID
        +ProfileResult(string, long long, long long, uint32_t)
    }

    class PerformanceBenchmark {
        -static vector~BenchmarkResult~ s_Results
        -static bool s_Running
        -static string s_CurrentTest
        -static chrono::high_resolution_clock::time_point s_StartTime
        +StartBenchmark(string&)
        +EndBenchmark()
        +GetResults() vector~BenchmarkResult~&
        +ClearResults()
        +IsRunning() bool
        +GetCurrentTest() string&
    }

    class BenchmarkResult {
        +string TestName
        +double Duration
        +uint32_t Iterations
        +string Description
        +BenchmarkResult(string, double, uint32_t, string)
    }

    class PerformanceMonitorUI {
        -bool m_ShowWindow
        -bool m_ShowFPS
        -bool m_ShowFrameTime
        -bool m_ShowMemoryUsage
        -bool m_ShowDrawCalls
        -bool m_ShowVertexCount
        -bool m_ShowTextureCount
        -float m_UpdateInterval
        -float m_LastUpdateTime
        -vector~float~ m_FrameTimeHistory
        -vector~float~ m_FPSHistory
        -uint32_t m_MaxHistorySize
        +PerformanceMonitorUI()
        +~PerformanceMonitorUI()
        +OnImGuiRender()
        +SetShowWindow(bool)
        +IsWindowVisible() bool
        +SetUpdateInterval(float)
        +GetUpdateInterval() float
        -RenderPerformanceMetrics()
        -RenderFrameTimeGraph()
        -RenderFPSGraph()
        -RenderMemoryUsage()
        -RenderDrawCallStats()
        -RenderVertexCountStats()
        -RenderTextureCountStats()
        -UpdateHistory(vector~float~&, float)
    }

    class MemoryManager {
        -static unordered_map~string, MemoryInfo~ s_MemoryBlocks
        -static uint64_t s_TotalAllocated
        -static uint64_t s_TotalFreed
        -static uint32_t s_AllocationCount
        -static uint32_t s_DeallocationCount
        +Allocate(size_t, string&) void*
        +Deallocate(void*, string&)
        +GetTotalAllocated() uint64_t
        +GetTotalFreed() uint64_t
        +GetAllocationCount() uint32_t
        +GetDeallocationCount() uint32_t
        +GetMemoryInfo(string&) MemoryInfo&
        +GetAllMemoryBlocks() unordered_map~string, MemoryInfo~&
        +ClearMemoryInfo()
    }

    class MemoryInfo {
        +string Name
        +uint64_t Allocated
        +uint64_t Freed
        +uint32_t AllocationCount
        +uint32_t DeallocationCount
        +MemoryInfo(string, uint64_t, uint64_t, uint32_t, uint32_t)
    }

    class ObjectPool {
        -vector~T*~ m_Objects
        -queue~T*~ m_Available
        -size_t m_PoolSize
        -size_t m_UsedCount
        +ObjectPool(size_t)
        +~ObjectPool()
        +Get() T*
        +Return(T*)
        +GetPoolSize() size_t
        +GetUsedCount() size_t
        +GetAvailableCount() size_t
        +Clear()
    }

    class CullingSystem {
        -static vector~RenderableObject~ s_VisibleObjects
        -static vector~RenderableObject~ s_CulledObjects
        -static bool s_Enabled
        -static Frustum s_ViewFrustum
        +SetViewFrustum(Frustum&)
        +CullObjects(vector~RenderableObject~&)
        +GetVisibleObjects() vector~RenderableObject~&
        +GetCulledObjects() vector~RenderableObject~&
        +IsEnabled() bool
        +SetEnabled(bool)
        +ClearResults()
        -IsObjectVisible(RenderableObject&) bool
        -UpdateFrustum(mat4&)
    }

    class RenderableObject {
        +vec3 Position
        +vec3 BoundingBoxMin
        +vec3 BoundingBoxMax
        +bool IsVisible
        +RenderableObject(vec3, vec3, vec3)
    }

    class Frustum {
        +vec4 Planes[6]
        +Frustum()
        +Update(mat4&)
        +IsPointInside(vec3&) bool
        +IsBoxInside(vec3&, vec3&) bool
    }

    %% 关系定义
    PerformanceProfiler --> ProfileResult : uses
    PerformanceBenchmark --> BenchmarkResult : uses
    PerformanceMonitorUI --> PerformanceProfiler : uses
    PerformanceMonitorUI --> PerformanceBenchmark : uses
    PerformanceMonitorUI --> MemoryManager : uses
    
    MemoryManager --> MemoryInfo : uses
    ObjectPool --> MemoryManager : uses
    
    CullingSystem --> RenderableObject : uses
    CullingSystem --> Frustum : uses
```

## 总结

Zgine 引擎采用了现代化的 C++ 设计模式，具有以下特点：

1. **分层架构**: 清晰的层次结构，每层职责明确
2. **平台抽象**: 通过接口实现跨平台支持
3. **事件驱动**: 基于事件系统的松耦合设计
4. **模块化**: 各功能模块独立，便于维护和扩展
5. **性能优化**: 内置批处理渲染、内存池、剔除系统等优化
6. **可扩展性**: 支持插件系统和多渲染 API

该架构设计为游戏开发提供了坚实的基础，支持 2D 和 3D 渲染，具有良好的性能和可维护性。
