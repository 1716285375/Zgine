# ImGuiLayer 类 UML 图

## ImGuiLayer 类详细结构

```mermaid
classDiagram
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
```

## ImGuiLayer 类继承关系图

```mermaid
classDiagram
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

    Layer <|-- ImGuiLayer : implements
```

## ImGuiLayer 类关系图

```mermaid
classDiagram
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

    class Window {
        <<interface>>
        +OnUpdate()*
        +GetWidth() uint*
        +GetHeight() uint*
        +SetEventCallback(EventCallbackFn)*
        +SetVSync(bool)*
        +IsVSync() bool*
        +GetNativeWindow() void**
    }

    Application --> ImGuiLayer : contains
    ImGuiLayer --> Event : handles
    ImGuiLayer --> Window : uses
```

## ImGuiLayer 生命周期图

```mermaid
sequenceDiagram
    participant App as Application
    participant IGL as ImGuiLayer
    participant ImGui as ImGui Library
    participant Win as Window

    App->>IGL: OnAttach()
    IGL->>ImGui: ImGui::CreateContext()
    IGL->>ImGui: ImGui::StyleColorsDark()
    IGL->>Win: Setup Platform/Renderer bindings
    
    loop Every Frame
        App->>IGL: Begin()
        IGL->>ImGui: ImGui::NewFrame()
        IGL->>ImGui: ImGui::BeginFrame()
        
        App->>IGL: OnImGuiRender()
        Note over IGL: Render ImGui widgets
        
        App->>IGL: End()
        IGL->>ImGui: ImGui::Render()
        IGL->>ImGui: ImGui_ImplOpenGL3_RenderDrawData()
    end
    
    App->>IGL: OnDetach()
    IGL->>ImGui: ImGui::DestroyContext()
```

## ImGuiLayer 类说明

### 职责
- **ImGui集成**: 管理ImGui库的初始化和清理
- **调试界面**: 提供调试和开发工具界面
- **事件处理**: 处理ImGui相关的事件
- **渲染管理**: 管理ImGui的渲染循环

### 设计模式
- **适配器模式**: 适配ImGui库到引擎的层系统
- **单例模式**: 通常作为全局唯一的调试层

### 生命周期方法
- **OnAttach()**: 初始化ImGui上下文和平台绑定
- **OnDetach()**: 清理ImGui上下文和资源
- **OnImGuiRender()**: 渲染ImGui调试界面
- **OnEvent()**: 处理ImGui相关事件
- **Begin()**: 开始ImGui帧
- **End()**: 结束ImGui帧并渲染

### 关键特性
- **平台无关**: 支持多种平台的ImGui集成
- **渲染优化**: 高效的ImGui渲染管道
- **事件处理**: 完整的鼠标键盘事件支持
- **调试友好**: 提供丰富的调试工具和界面
- **性能监控**: 内置性能监控和统计功能
