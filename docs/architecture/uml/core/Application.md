# Application 类 UML 图

## Application 类详细结构

```mermaid
classDiagram
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
```

## Application 类关系图

```mermaid
classDiagram
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

    class WindowCloseEvent {
        +GetStaticType() EventType
        +GetEventType() EventType
        +GetName() char*
        +GetCategoryFlags() int
        +ToString() string
    }

    Application --> Window : contains
    Application --> ImGuiLayer : contains
    Application --> LayerStack : contains
    Application --> Event : handles
    LayerStack --> Layer : manages
    Layer <|-- ImGuiLayer : implements
    Event <|-- WindowCloseEvent : implements
```

## Application 类说明

### 职责
- **应用程序生命周期管理**: 管理整个引擎的启动、运行和关闭
- **窗口管理**: 创建和管理主窗口
- **层栈管理**: 管理应用程序层的添加、移除和执行顺序
- **事件处理**: 处理应用程序级别的事件
- **ImGui集成**: 管理调试界面层

### 设计模式
- **单例模式**: 通过静态实例确保全局唯一性
- **工厂模式**: 通过CreateApplication()工厂函数创建实例

### 关键特性
- **线程安全**: 单例实现确保线程安全
- **资源管理**: 使用智能指针管理资源
- **事件驱动**: 基于事件系统的松耦合设计
- **可扩展性**: 通过层系统支持插件和扩展
