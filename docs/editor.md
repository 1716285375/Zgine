# Zgine Editor UI Architecture - 工业级设计方案

## 🎯 设计哲学

### 问题分析：原方案的局限性

**原始设计的问题**:

**为什么这个设计不够优秀？**

1. ❌ **静态方法反模式**: `MainMenu::Render()` 使得状态管理困难
2. ❌ **无法测试**: 静态类难以进行单元测试
3. ❌ **缺乏生命周期**: 无法管理 UI 状态（打开/关闭、最小化等）
4. ❌ **不符合 ImGui 哲学**: ImGui 是立即模式 GUI，需要状态管理
5. ❌ **扩展性差**: 无法动态添加/移除面板
6. ❌ **没有面板系统**: 缺少通用的面板基类和管理器
7. ❌ **违反职责分离**: UI 逻辑和业务逻辑混杂

---

## 🏗️ 工业级编辑器架构

### 架构层次

```
┌─────────────────────────────────────────────────────┐
│              Application Layer                       │
│  (EditorApplication - 主循环、事件分发)              │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────────┐
│              Editor Layer                            │
│  (EditorLayer - 管理编辑器生命周期)                  │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────────┐
│           Panel System Layer                         │
│  (EditorPanelManager + EditorPanel 基类)            │
└────────────────┬────────────────────────────────────┘
                 │
    ┌────────────┼────────────┬──────────────┐
    │            │            │              │
┌───▼──┐  ┌─────▼────┐  ┌────▼────┐  ┌─────▼─────┐
│Hierarc│  │Inspector │  │Viewport │  │Content    │
│hy    │  │Panel     │  │Panel    │  │Browser    │
└──────┘  └──────────┘  └─────────┘  └───────────┘
```

---

## 📁 优化后的目录结构

```
src/
├── Editor/
│   ├── EditorApplication.h/cpp      // 编辑器应用主类
│   ├── EditorLayer.h/cpp            // 编辑器层（核心逻辑）
│   ├── EditorCamera.h/cpp           // 编辑器相机
│   ├── EditorSettings.h/cpp         // 编辑器设置
│   ├── SelectionContext.h/cpp       // 选择上下文（当前选中的实体）
│   ├── EditorResources.h/cpp        // 编辑器资源（图标、字体等）
│   ├── EditorPanelManager.h/cpp     // 面板管理器
│   │
│   ├── Panels/                      // 所有编辑器面板
│   │   ├── EditorPanel.h            // 面板基类（重要！）
│   │   │
│   │   ├── HierarchyPanel.h/cpp     // 场景层级面板
│   │   ├── InspectorPanel.h/cpp     // 属性检视面板
│   │   ├── ViewportPanel.h/cpp      // 视口面板
│   │   ├── ContentBrowserPanel.h/cpp // 资源浏览器
│   │   ├── ConsolePanel.h/cpp       // 控制台面板
│   │   ├── ScenePanel.h/cpp         // 场景设置面板
│   │   ├── PerformancePanel.h/cpp   // 性能监控面板
│   │   └── ProjectPanel.h/cpp       // 项目设置面板
│   │
│   ├── Widgets/                     // 可复用的 UI 组件
│   │   ├── PropertyWidgets.h/cpp    // 属性编辑器组件
│   │   ├── AssetWidget.h/cpp        // 资源选择器组件
│   │   ├── ColorPicker.h/cpp        // 颜色选择器
│   │   ├── DragDropTarget.h/cpp     // 拖放目标组件
│   │   └── SearchBar.h/cpp          // 搜索栏组件
│   │
│   ├── Gizmos/                      // 变换工具
│   │   ├── TransformGizmo.h/cpp     // 变换小工具（使用 ImGuizmo）
│   │   ├── GizmoManager.h/cpp       // 小工具管理器
│   │   └── GizmoRenderer.h/cpp      // 小工具渲染器
│   │
│   ├── Commands/                    // 命令模式（撤销/重做）
│   │   ├── EditorCommand.h          // 命令基类
│   │   ├── CommandHistory.h/cpp     // 命令历史
│   │   ├── TransformCommand.h/cpp   // 变换命令
│   │   ├── CreateEntityCommand.h/cpp // 创建实体命令
│   │   └── DeleteEntityCommand.h/cpp // 删除实体命令
│   │
│   └── Utils/
│       ├── EditorTheme.h/cpp        // 编辑器主题
│       ├── EditorIcons.h/cpp        // 图标管理
│       ├── ImGuiUtils.h/cpp         // ImGui 工具函数
│       └── EditorSerializer.h/cpp   // 编辑器布局序列化
│
└── UI/                              // 通用 UI 框架（可选）
    ├── ImGuiLayer.h/cpp             // ImGui 层封装
    └── ImGuiContext.h/cpp           // ImGui 上下文管理
```

---

## 🎨 核心设计模式

### 1. Panel System（面板系统）

#### EditorPanel 基类

```cpp
// src/Editor/Panels/EditorPanel.h
#pragma once

#include <string>
#include <imgui.h>

namespace Zgine {

class EditorPanel {
public:
    EditorPanel(const std::string& name, bool openByDefault = true)
        : m_Name(name), m_IsOpen(openByDefault) {}
    
    virtual ~EditorPanel() = default;
    
    // 核心接口
    virtual void OnGuiRender() = 0;  // 纯虚函数，子类必须实现
    
    // 生命周期
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnEvent(Event& e) {}
    
    // 状态管理
    void SetOpen(bool open) { m_IsOpen = open; }
    bool IsOpen() const { return m_IsOpen; }
    
    const std::string& GetName() const { return m_Name; }
    
    // 焦点管理
    bool IsFocused() const { return m_IsFocused; }
    bool IsHovered() const { return m_IsHovered; }
    
    // 序列化（保存布局）
    virtual void Serialize(YAML::Emitter& out) {}
    virtual void Deserialize(const YAML::Node& node) {}
    
protected:
    // ImGui 窗口帮助函数
    void BeginPanel(ImGuiWindowFlags flags = 0) {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen, flags);
        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();
    }
    
    void EndPanel() {
        ImGui::End();
    }
    
    std::string m_Name;
    bool m_IsOpen = true;
    bool m_IsFocused = false;
    bool m_IsHovered = false;
};

} // namespace Zgine
```

---

#### EditorPanelManager（面板管理器）

```cpp
// src/Editor/EditorPanelManager.h
#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Zgine {

class EditorPanel;

class EditorPanelManager {
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> AddPanel(Args&&... args) {
        static_assert(std::is_base_of_v<EditorPanel, T>, "T must derive from EditorPanel");
        auto panel = std::make_shared<T>(std::forward<Args>(args)...);
        m_Panels.push_back(panel);
        panel->OnAttach();
        return panel;
    }

    void RemovePanel(const std::shared_ptr<EditorPanel>& panel);

    void OnUpdate(float deltaTime);
    void OnGuiRender();

    void RenderPanelMenuItems();
    std::shared_ptr<EditorPanel> FindPanel(const std::string& name) const;

    void Clear();

private:
    std::vector<std::shared_ptr<EditorPanel>> m_Panels;
};

} // namespace Zgine
```

---

### 2. EditorLayer（编辑器层）

```cpp
// src/Editor/EditorLayer.h
#pragma once

#include "Core/Layer.h"
#include "EditorPanelManager.h"
#include "SelectionContext.h"
#include "EditorCamera.h"
#include "Commands/CommandHistory.h"

namespace Zgine {

class EditorLayer : public Layer {
public:
    EditorLayer();
    virtual ~EditorLayer() = default;
    
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnGuiRender() override;
    virtual void OnEvent(Event& e) override;
    
private:
    void InitializePanels();
    void RenderMenuBar();
    void RenderDockspace();
    
    void NewScene();
    void OpenScene();
    void SaveScene();
    void SaveSceneAs();
    
    // Shortcuts
    void HandleKeyboardShortcuts();
    
private:
    // 核心系统
    EditorPanelManager m_PanelManager;
    SelectionContext m_SelectionContext;
    CommandHistory m_CommandHistory;
    
    // 场景管理
    Ref<Scene> m_ActiveScene;
    std::string m_CurrentScenePath;
    
    // 编辑器相机
    EditorCamera m_EditorCamera;
    
    // 编辑器状态
    enum class SceneState { Edit, Play, Pause };
    SceneState m_SceneState = SceneState::Edit;
    
    // 性能统计
    float m_FrameTime = 0.0f;
    int m_FPS = 0;
};

} // namespace Zgine
```

```cpp
// src/Editor/EditorLayer.cpp

void EditorLayer::OnAttach() {
    // 初始化编辑器资源
    EditorResources::Init();
    
    // 设置 ImGui 主题
    EditorTheme::SetDarkTheme();
    
    // 注册所有面板
    InitializePanels();
    
    // 创建默认场景
    NewScene();
}

void EditorLayer::InitializePanels() {
    // 注册核心面板（按顺序）
    auto hierarchyPanel = m_PanelManager.AddPanel<HierarchyPanel>("Hierarchy", m_SelectionContext);
    auto inspectorPanel = m_PanelManager.AddPanel<InspectorPanel>("Inspector", m_SelectionContext);
    auto viewportPanel = m_PanelManager.AddPanel<ViewportPanel>("Viewport", m_EditorCamera);
    auto contentBrowser = m_PanelManager.AddPanel<ContentBrowserPanel>("Content Browser");
    auto consolePanel = m_PanelManager.AddPanel<ConsolePanel>("Console");
    auto performancePanel = m_PanelManager.AddPanel<PerformancePanel>("Performance", false); // 默认关闭
    
    // 设置依赖关系
    hierarchyPanel->SetScene(m_ActiveScene.get());
    inspectorPanel->SetScene(m_ActiveScene.get());
    viewportPanel->SetScene(m_ActiveScene.get());
}

void EditorLayer::OnGuiRender() {
    // 1. 设置 Dockspace
    RenderDockspace();
    
    // 2. 主菜单栏
    RenderMenuBar();
    
    // 3. 渲染所有面板
    m_PanelManager.OnGuiRender();
    
    // 4. 工具栏（Play/Pause/Stop）
    RenderToolbar();
}

void EditorLayer::RenderDockspace() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    
    ImGui::End();
}

void EditorLayer::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        // File Menu
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                NewScene();
            }
            if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
                OpenScene();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                SaveScene();
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                SaveSceneAs();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                Application::Get().Close();
            }
            ImGui::EndMenu();
        }
        
        // Edit Menu
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, m_CommandHistory.CanUndo())) {
                m_CommandHistory.Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, m_CommandHistory.CanRedo())) {
                m_CommandHistory.Redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Settings")) {
                // Open settings
            }
            ImGui::EndMenu();
        }
        
        // View Menu (Panel toggles)
        if (ImGui::BeginMenu("View")) {
            m_PanelManager.RenderPanelMenuItems();
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
}
```

---

### 3. 具体面板实现示例

#### HierarchyPanel（层级面板）

```cpp
// src/Editor/Panels/HierarchyPanel.h
#pragma once

#include "EditorPanel.h"
#include "Scene/Scene.h"
#include "Editor/SelectionContext.h"

namespace Zgine {

class HierarchyPanel : public EditorPanel {
public:
    HierarchyPanel(const std::string& name, SelectionContext& selectionContext);
    
    void SetScene(Scene* scene) { m_Scene = scene; }
    
    virtual void OnGuiRender() override;
    
private:
    void DrawEntityNode(Entity entity);
    void DrawContextMenu();
    
private:
    Scene* m_Scene = nullptr;
    SelectionContext& m_SelectionContext;
    
    // UI 状态
    Entity m_DraggedEntity;
    std::string m_SearchQuery;
};

} // namespace Zgine
```

```cpp
// src/Editor/Panels/HierarchyPanel.cpp

HierarchyPanel::HierarchyPanel(const std::string& name, SelectionContext& selectionContext)
    : EditorPanel(name), m_SelectionContext(selectionContext) {
}

void HierarchyPanel::OnGuiRender() {
    BeginPanel();
    
    if (!m_Scene) {
        ImGui::TextDisabled("No active scene");
        EndPanel();
        return;
    }
    
    // 搜索栏
    char searchBuffer[256];
    strcpy(searchBuffer, m_SearchQuery.c_str());
    if (ImGui::InputTextWithHint("##Search", "Search entities...", searchBuffer, 256)) {
        m_SearchQuery = searchBuffer;
    }
    
    ImGui::Separator();
    
    // 创建实体按钮
    if (ImGui::Button("+ Create Entity", ImVec2(-1, 0))) {
        ImGui::OpenPopup("CreateEntityPopup");
    }
    
    // 创建实体弹窗
    if (ImGui::BeginPopup("CreateEntityPopup")) {
        if (ImGui::MenuItem("Empty Entity")) {
            m_Scene->CreateEntity("Empty Entity");
        }
        if (ImGui::MenuItem("Cube")) {
            auto entity = m_Scene->CreateEntity("Cube");
            // 添加 MeshComponent 等
        }
        if (ImGui::MenuItem("Sphere")) {
            // ...
        }
        if (ImGui::MenuItem("Camera")) {
            // ...
        }
        if (ImGui::MenuItem("Light")) {
            ImGui::OpenPopup("LightTypePopup");
        }
        
        // 光源子菜单
        if (ImGui::BeginPopup("LightTypePopup")) {
            if (ImGui::MenuItem("Directional Light")) {
                auto entity = m_Scene->CreateEntity("Directional Light");
                entity.AddComponent<DirectionalLightComponent>();
            }
            if (ImGui::MenuItem("Point Light")) {
                auto entity = m_Scene->CreateEntity("Point Light");
                entity.AddComponent<PointLightComponent>();
            }
            ImGui::EndPopup();
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    // 实体列表（树形结构）
    ImGui::BeginChild("EntityList");
    
    m_Scene->Registry.each([&](auto entityID) {
        Entity entity{ entityID, m_Scene };
        
        // 过滤搜索
        if (!m_SearchQuery.empty()) {
            if (entity.HasComponent<TagComponent>()) {
                auto& tag = entity.GetComponent<TagComponent>().Tag;
                if (tag.find(m_SearchQuery) == std::string::npos) {
                    return; // 跳过不匹配的实体
                }
            }
        }
        
        // 只绘制根实体（没有父节点的）
        if (!entity.HasComponent<ParentComponent>()) {
            DrawEntityNode(entity);
        }
    });
    
    // 右键点击空白区域
    if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            m_Scene->CreateEntity("Empty Entity");
        }
        ImGui::EndPopup();
    }
    
    ImGui::EndChild();
    
    EndPanel();
}

void HierarchyPanel::DrawEntityNode(Entity entity) {
    auto& tag = entity.GetComponent<TagComponent>().Tag;
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    
    // 选中状态
    if (m_SelectionContext.IsSelected(entity)) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    // 是否有子节点
    bool hasChildren = entity.HasComponent<ChildrenComponent>();
    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    
    // 图标（可选）
    const char* icon = "🔷"; // 默认图标
    if (entity.HasComponent<CameraComponent>()) icon = "📷";
    else if (entity.HasComponent<DirectionalLightComponent>()) icon = "☀️";
    else if (entity.HasComponent<PointLightComponent>()) icon = "💡";
    
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s %s", icon, tag.c_str());
    
    // 点击选择
    if (ImGui::IsItemClicked()) {
        m_SelectionContext.Select(entity);
    }
    
    // 拖放
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ENTITY_NODE", &entity, sizeof(Entity));
        ImGui::Text("Moving: %s", tag.c_str());
        ImGui::EndDragDropSource();
    }
    
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            Entity droppedEntity = *(Entity*)payload->Data;
            // 设置父子关系
            // entity.AddChild(droppedEntity);
        }
        ImGui::EndDragDropTarget();
    }
    
    // 右键菜单
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Duplicate")) {
            // m_Scene->DuplicateEntity(entity);
        }
        if (ImGui::MenuItem("Delete", "Delete")) {
            m_Scene->DestroyEntity(entity);
            m_SelectionContext.Deselect();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Rename", "F2")) {
            // 进入重命名模式
        }
        ImGui::EndPopup();
    }
    
    // 递归绘制子节点
    if (opened && hasChildren) {
        auto& children = entity.GetComponent<ChildrenComponent>().Children;
        for (auto childID : children) {
            Entity child{ childID, m_Scene };
            DrawEntityNode(child);
        }
        ImGui::TreePop();
    }
}
```

---

#### InspectorPanel（属性检视面板）

```cpp
// src/Editor/Panels/InspectorPanel.h
#pragma once

#include "EditorPanel.h"
#include "Editor/SelectionContext.h"
#include "Editor/Widgets/PropertyWidgets.h"

namespace Zgine {

class InspectorPanel : public EditorPanel {
public:
    InspectorPanel(const std::string& name, SelectionContext& selectionContext);
    
    void SetScene(Scene* scene) { m_Scene = scene; }
    
    virtual void OnGuiRender() override;
    
private:
    template<typename T>
    void DrawComponent(const std::string& name, Entity entity, std::function<void(T&)> drawFunc);
    
    void DrawComponents(Entity entity);
    void DrawAddComponentMenu(Entity entity);
    
private:
    Scene* m_Scene = nullptr;
    SelectionContext& m_SelectionContext;
};

} // namespace Zgine
```

```cpp
// src/Editor/Panels/InspectorPanel.cpp

void InspectorPanel::OnGuiRender() {
    BeginPanel();
    
    Entity selectedEntity = m_SelectionContext.GetSelectedEntity();
    
    if (!selectedEntity) {
        ImGui::TextDisabled("No entity selected");
        EndPanel();
        return;
    }
    
    // Entity 名称编辑
    auto& tag = selectedEntity.GetComponent<TagComponent>();
    char buffer[256];
    strcpy(buffer, tag.Tag.c_str());
    
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##EntityName", buffer, 256)) {
        tag.Tag = buffer;
    }
    ImGui::PopItemWidth();
    
    ImGui::Separator();
    
    // 绘制所有组件
    DrawComponents(selectedEntity);
    
    // Add Component 按钮
    ImGui::Spacing();
    if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
        ImGui::OpenPopup("AddComponentPopup");
    }
    
    if (ImGui::BeginPopup("AddComponentPopup")) {
        DrawAddComponentMenu(selectedEntity);
        ImGui::EndPopup();
    }
    
    EndPanel();
}

template<typename T>
void InspectorPanel::DrawComponent(const std::string& name, Entity entity, 
                                     std::function<void(T&)> drawFunc) {
    if (!entity.HasComponent<T>()) return;
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed 
                              | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
    
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImGui::Separator();
    
    bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", name.c_str());
    ImGui::PopStyleVar();
    
    // Remove Component 按钮
    ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
    if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight })) {
        ImGui::OpenPopup("ComponentSettings");
    }
    
    bool removeComponent = false;
    if (ImGui::BeginPopup("ComponentSettings")) {
        if (ImGui::MenuItem("Remove Component")) {
            removeComponent = true;
        }
        if (ImGui::MenuItem("Reset Component")) {
            // 重置组件到默认值
        }
        if (ImGui::MenuItem("Copy Component")) {
            // 复制组件数据
        }
        ImGui::EndPopup();
    }
    
    if (open) {
        auto& component = entity.GetComponent<T>();
        drawFunc(component);
        ImGui::TreePop();
    }
    
    if (removeComponent) {
        entity.RemoveComponent<T>();
    }
}

void InspectorPanel::DrawComponents(Entity entity) {
    // TransformComponent
    DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
        PropertyWidgets::DrawVec3Control("Position", component.Translation);
        PropertyWidgets::DrawVec3Control("Rotation", component.Rotation);
        PropertyWidgets::DrawVec3Control("Scale", component.Scale, 1.0f);
    });
    
    // MeshComponent
    DrawComponent<MeshComponent>("Mesh Renderer", entity, [](auto& component) {
        ImGui::Text("Mesh: %s", component.MeshResource ? "Loaded" : "None");
        
        // 材质引用
        ImGui::Text("Material:");
        // 资产选择器
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MATERIAL")) {
                // 设置材质
            }
            ImGui::EndDragDropTarget();
        }
    });
    
    // MaterialComponent
    DrawComponent<MaterialComponent>("Material", entity, [](auto& component) {
        PropertyWidgets::DrawColorControl("Albedo", component.Albedo);
        ImGui::SliderFloat("Metallic", &component.Metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &component.Roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("AO", &component.AO, 0.0f, 1.0f);
        
        PropertyWidgets::DrawColorControl("Emissive", component.Emissive);
        ImGui::SliderFloat("Emissive Strength", &component.EmissiveStrength, 0.0f, 10.0f);
        
        // 纹理槽
        PropertyWidgets::DrawTextureSlot("Albedo Map", component.AlbedoMap);
        PropertyWidgets::DrawTextureSlot("Normal Map", component.NormalMap);
        PropertyWidgets::DrawTextureSlot("Metallic-Roughness", component.MetallicRoughnessMap);
    });
    
    // RigidBodyComponent
    DrawComponent<RigidBodyComponent>("Rigid Body", entity, [](auto& component) {
        const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
        int currentType = (int)component.BodyType;
        if (ImGui::Combo("Body Type", &currentType, bodyTypes, 3)) {
            component.BodyType = (RigidBodyComponent::Type)currentType;
        }
        
        if (component.BodyType == RigidBodyComponent::Type::Dynamic) {
            ImGui::DragFloat("Mass", &component.Mass, 0.1f, 0.01f, 1000.0f);
            ImGui::DragFloat("Linear Damping", &component.LinearDamping, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Angular Damping", &component.AngularDamping, 0.01f, 0.0f, 1.0f);
        }
        
        ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
        
        ImGui::Checkbox("Lock Rotation X", &component.LockRotationX);
        ImGui::Checkbox("Lock Rotation Y", &component.LockRotationY);
        ImGui::Checkbox("Lock Rotation Z", &component.LockRotationZ);
    });
    
    // ... 其他组件
}
```

---

### 4. 可复用组件（Widgets）

```cpp
// src/Editor/Widgets/PropertyWidgets.h
#pragma once

#include <imgui.h>
#include <glm/glm.hpp>
#include <string>

namespace Zgine::PropertyWidgets {

// Vec3 控制器（带 XYZ 标签和颜色）
bool DrawVec3Control(const std::string& label, glm::vec3& values, 
                      float resetValue = 0.0f, float columnWidth = 100.0f);

// 颜色选择器
bool DrawColorControl(const std::string& label, glm::vec3& color);

// 纹理槽（支持拖放）
bool DrawTextureSlot(const std::string& label, std::shared_ptr<Texture>& texture);

// 资产引用（通用）
template<typename T>
bool DrawAssetReference(const std::string& label, std::shared_ptr<T>& asset, 
                         const char* dragDropID);

// 文件路径选择器
bool DrawFilePath(const std::string& label, std::string& path, 
                   const char* filter = nullptr);

} // namespace Zgine::PropertyWidgets
```

```cpp
// src/Editor/Widgets/PropertyWidgets.cpp

bool PropertyWidgets::DrawVec3Control(const std::string& label, glm::vec3& values, 
                                       float resetValue, float columnWidth) {
    bool modified = false;
    
    ImGui::PushID(label.c_str());
    
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
    
    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
    
    // X
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    // Y
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    // Z
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
    ImGui::PopItemWidth();
    
    ImGui::PopStyleVar();
    
    ImGui::Columns(1);
    ImGui::PopID();
    
    return modified;
}

bool PropertyWidgets::DrawColorControl(const std::string& label, glm::vec3& color) {
    ImGui::Text("%s", label.c_str());
    ImGui::SameLine();
    return ImGui::ColorEdit3(("##" + label).c_str(), &color.x);
}

bool PropertyWidgets::DrawTextureSlot(const std::string& label, std::shared_ptr<Texture>& texture) {
    ImGui::Text("%s", label.c_str());
    
    // 显示纹理预览
    ImTextureID texID = texture ? (ImTextureID)(uint64_t)texture->GetRendererID() : 0;
    ImGui::Image(texID, ImVec2(64, 64));
    
    // 拖放接受
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE")) {
            // 设置纹理
            texture = *(std::shared_ptr<Texture>*)payload->Data;
            ImGui::EndDragDropTarget();
            return true;
        }
        ImGui::EndDragDropTarget();
    }
    
    // 右键清除
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Clear")) {
            texture.reset();
            ImGui::EndPopup();
            return true;
        }
        ImGui::EndPopup();
    }
    
    return false;
}
```

---


## 📚 总结与建议

### ✅ 优化设计的核心原则

1. **面向对象设计**: 使用继承和多态而非静态方法
2. **职责分离**: UI 渲染与业务逻辑分离
3. **依赖注入**: 明确的依赖关系，便于测试
4. **生命周期管理**: 完整的初始化/更新/清理流程
5. **状态封装**: 每个面板管理自己的状态
6. **可扩展性**: 易于添加新面板和功能
7. **参考工业标准**: 学习 Unity/Unreal/Godot 的编辑器架构

### 依次实现
1. **实现基础框架**
   - EditorPanel 基类
   - EditorPanelManager
   - EditorLayer

2. **实现核心面板**
   - HierarchyPanel
   - InspectorPanel

3. **实现扩展面板**
   - ViewportPanel
   - ContentBrowserPanel
   - ConsolePanel

4. **实现高级功能**
   - 命令系统（撤销/重做）
   - 布局序列化
   - 自定义主题

---

这套架构设计经过深思熟虑，完全符合现代游戏引擎编辑器的设计哲学！
