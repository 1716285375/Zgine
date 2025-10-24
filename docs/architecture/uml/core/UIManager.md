# UIManager 类 UML 图

## UIManager 类详细结构

```mermaid
classDiagram
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
```

## UIManager 类关系图

```mermaid
classDiagram
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

    class UITheme {
        <<enumeration>>
        Dark
        Light
        Classic
        Custom
    }

    class UILayout {
        +string name
        +vector~string~ windowOrder
        +unordered_map~string, ImVec2~ windowPositions
        +unordered_map~string, ImVec2~ windowSizes
        +bool isDefault
    }

    class WindowInfo {
        +function~void()~ renderFunction
        +bool isVisible
        +bool* visibilityPtr
        +bool isRegistered
    }

    class MenuCategory {
        +string name
        +function~void()~ renderFunction
        +vector~pair~string, function~void()~~ items
    }

    class RenderManager {
        <<interface>>
        +OnUpdate(float)*
        +OnImGuiRender()*
    }

    class SceneManager {
        <<interface>>
        +OnUpdate(float)*
        +OnImGuiRender()*
    }

    class SettingsManager {
        <<interface>>
        +OnUpdate(float)*
        +OnImGuiRender()*
    }

    class Test2DModule {
        <<interface>>
        +OnUpdate(float)*
        +OnImGuiRender()*
    }

    class Test3DModule {
        <<interface>>
        +OnUpdate(float)*
        +OnImGuiRender()*
    }

    UIManager --> UITheme : uses
    UIManager --> UILayout : uses
    UIManager --> WindowInfo : contains
    UIManager --> MenuCategory : contains
    UIManager --> RenderManager : references
    UIManager --> SceneManager : references
    UIManager --> SettingsManager : references
    UIManager --> Test2DModule : references
    UIManager --> Test3DModule : references
```

## UIManager 窗口管理流程图

```mermaid
flowchart TD
    A[RegisterWindow] --> B{Window exists?}
    B -->|No| C[Create WindowInfo]
    B -->|Yes| D[Update WindowInfo]
    C --> E[Store in m_Windows]
    D --> E
    
    F[ShowWindow] --> G{Window registered?}
    G -->|No| H[Error: Window not found]
    G -->|Yes| I[Set visibility flag]
    
    J[OnImGuiRender] --> K[Render Main Menu]
    K --> L[Render Status Bar]
    L --> M[Render Performance Overlay]
    M --> N[Render Registered Windows]
    N --> O[Apply Theme]
```

## UIManager 类说明

### 职责
- **窗口管理**: 管理ImGui窗口的注册、显示和隐藏
- **菜单管理**: 管理主菜单和子菜单的创建和组织
- **主题管理**: 管理UI主题的切换和应用
- **布局管理**: 管理窗口布局的保存和加载
- **性能监控**: 提供性能监控UI界面

### 设计模式
- **管理器模式**: 统一管理UI相关功能
- **观察者模式**: 通过回调函数响应UI事件
- **策略模式**: 支持不同的UI主题策略

### 核心功能
- **窗口注册**: 动态注册和注销ImGui窗口
- **菜单系统**: 创建层次化的菜单结构
- **主题切换**: 支持多种UI主题
- **布局保存**: 保存和恢复窗口布局
- **性能显示**: 显示引擎性能统计

### 关键特性
- **动态性**: 支持运行时动态添加/移除窗口
- **可配置**: 支持多种UI配置选项
- **扩展性**: 易于扩展新的UI功能
- **性能优化**: 高效的UI渲染和更新
- **用户友好**: 提供直观的调试界面
