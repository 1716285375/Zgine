# Window 类 UML 图

## Window 类详细结构

```mermaid
classDiagram
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
```

## Window 类关系图

```mermaid
classDiagram
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

    class EventCallbackFn {
        <<typedef>>
        function~void(Event&)~
    }

    Window <|-- WindowsWindow : implements
    WindowsWindow --> WindowProps : uses
    WindowsWindow --> WindowData : contains
    WindowsWindow --> GraphicsContext : contains
    GraphicsContext <|-- OpenGLContext : implements
    Window --> EventCallbackFn : uses
```

## Window 创建流程图

```mermaid
sequenceDiagram
    participant App as Application
    participant Win as Window
    participant WW as WindowsWindow
    participant GLFW as GLFW
    participant GC as GraphicsContext

    App->>Win: Create(WindowProps)
    Win->>WW: WindowsWindow(props)
    WW->>GLFW: glfwInit()
    WW->>GLFW: glfwCreateWindow(width, height, title)
    GLFW-->>WW: GLFWwindow*
    WW->>GC: Create GraphicsContext(window)
    GC->>GC: Init()
    WW->>WW: SetEventCallback()
    WW->>GLFW: glfwSetWindowUserPointer()
    WW->>GLFW: glfwSetWindowCloseCallback()
    WW->>GLFW: glfwSetWindowSizeCallback()
    WW->>GLFW: glfwSetKeyCallback()
    WW->>GLFW: glfwSetMouseButtonCallback()
    WW->>GLFW: glfwSetCursorPosCallback()
    WW->>GLFW: glfwSetScrollCallback()
    WW-->>Win: Return Window*
    Win-->>App: Return Window*
```

## Window 事件处理流程图

```mermaid
sequenceDiagram
    participant GLFW as GLFW
    participant WW as WindowsWindow
    participant EC as EventCallback
    participant App as Application

    GLFW->>WW: Window Event Occurs
    WW->>EC: Call EventCallback(event)
    EC->>App: OnEvent(event)
    
    alt Window Close Event
        App->>App: Handle Window Close
    else Window Resize Event
        App->>App: Handle Window Resize
    else Key Event
        App->>App: Handle Key Event
    else Mouse Event
        App->>App: Handle Mouse Event
    end
```

## Window 类说明

### 职责
- **窗口管理**: 管理窗口的创建、更新和销毁
- **事件处理**: 处理窗口相关的事件
- **图形上下文**: 管理图形渲染上下文
- **平台抽象**: 提供跨平台的窗口接口

### 设计模式
- **抽象工厂模式**: 通过Create()工厂方法创建窗口
- **观察者模式**: 通过事件回调处理窗口事件
- **适配器模式**: 适配不同平台的窗口实现

### 核心功能
- **窗口创建**: 创建和管理窗口实例
- **事件处理**: 处理窗口、键盘、鼠标事件
- **尺寸管理**: 管理窗口尺寸和位置
- **VSync控制**: 控制垂直同步
- **上下文管理**: 管理图形渲染上下文

### 平台支持
- **Windows**: 通过GLFW实现Windows窗口
- **Linux**: 通过GLFW实现Linux窗口
- **macOS**: 通过GLFW实现macOS窗口

### 关键特性
- **跨平台**: 统一的跨平台窗口接口
- **事件驱动**: 完整的事件处理机制
- **资源管理**: 自动管理窗口资源
- **性能优化**: 高效的窗口更新机制
- **可扩展性**: 易于扩展新的平台支持

### 事件类型
- **窗口事件**: 关闭、调整大小、移动、焦点
- **键盘事件**: 按键、释放、输入
- **鼠标事件**: 移动、按键、滚轮
- **应用事件**: 更新、渲染、定时器
