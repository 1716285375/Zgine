#include <Zgine/Editor/Core/Editor.h>
#include <Zgine/Platform/Window.h>
#include <Zgine/Gui/Fonts/FontManager.h>
#include <Zgine/Gui/Themes/ImGuiTheme.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Editor/Panels/HierarchyPanel.h>
#include <Zgine/Editor/Panels/InspectorPanel.h>
#include <Zgine/Editor/Panels/ViewportPanel.h>
#include <Zgine/Editor/Panels/ContentBrowserPanel.h>
#include <Zgine/Editor/Panels/ConsolePanel.h>
#include <Zgine/Editor/Panels/PerformancePanel.h>
#include <Zgine/Editor/Panels/GameViewPanel.h>
#include <Zgine/Editor/Panels/StatusPanel.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <filesystem>
#include <cstring>

namespace Zgine {

namespace {

ImGuiWindow* ResolveDockTabWindow(ImGuiDockNode* node, ImGuiTabBar* tab_bar) {
    if (!node) {
        return nullptr;
    }

    ImGuiID targetTabId = 0;
    if (tab_bar) {
        targetTabId = tab_bar->VisibleTabId ? tab_bar->VisibleTabId : tab_bar->SelectedTabId;
        if (targetTabId != 0) {
            for (int i = 0; i < tab_bar->Tabs.Size; ++i) {
                ImGuiTabItem* tab = &tab_bar->Tabs[i];
                if (tab->ID == targetTabId && tab->Window) {
                    return tab->Window;
                }
            }
        }
    }

    if (node->VisibleWindow) {
        return node->VisibleWindow;
    }
    return node->Windows.Size > 0 ? node->Windows[0] : nullptr;
}

}

Editor::Editor() {
    m_AssetsRoot = "assets";
}

Editor::~Editor() {
    Shutdown();
}

void Editor::Initialize(Window* window) {
    if (m_Initialized) {
        return;
    }

    m_Window = window;

    // Check if ImGui context already exists (created by Application's GuiLayer)
    // If not, create one (for standalone usage)
    if (ImGui::GetCurrentContext() == nullptr) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_ImGuiContextCreated = true;

        // Only initialize backends if we created the context
        m_ImGuiPlatformInitialized = ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNativeWindow()), true);
        if (!m_ImGuiPlatformInitialized) {
            ZGINE_CORE_ERROR("Failed to initialize ImGui GLFW backend");
            Shutdown();
            return;
        }

        m_ImGuiRendererInitialized = ImGui_ImplOpenGL3_Init("#version 450");
        if (!m_ImGuiRendererInitialized) {
            ZGINE_CORE_ERROR("Failed to initialize ImGui OpenGL3 backend");
            Shutdown();
            return;
        }
    } else {
        // Context exists, we didn't create it
        m_ImGuiContextCreated = false;
        m_ImGuiPlatformInitialized = false;
        m_ImGuiRendererInitialized = false;
    }

    // Configure ImGui (works whether context is new or existing)
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingAlwaysTabBar = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.UserData = this;
    ImGui::GetCurrentContext()->DockNodeWindowMenuHandler = &Editor::DockNodeWindowMenuHandler;

    ImGuiTheme::ApplyTheme(ImGuiThemeType::Dark);
    m_CurrentTheme = ImGuiThemeType::Dark;

    ImGuiStyle& style = ImGui::GetStyle();
    style.DockingNodeHasCloseButton = false;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Gizmo and viewport state are now managed by ViewportContext
    m_Mode = EditorMode::Edit;

    FontManager::Initialize();
    if (FontManager::GetDefaultFont()) {
        io.FontDefault = FontManager::GetDefaultFont();
        ZGINE_CORE_INFO("Default font set: Inter");
    }

    if (!std::filesystem::exists(m_AssetsRoot)) {
        ZGINE_CORE_WARN("Assets directory not found: {}", m_AssetsRoot.string());
        m_AssetsRoot = std::filesystem::current_path();
    }

    ImVec4 windowBg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    m_ClearColor = { windowBg.x, windowBg.y, windowBg.z, windowBg.w };

    // ✅ CRITICAL: Initialize EditorContext (EventBus, SelectionContext, CommandHistory)
    m_Context.Initialize();

    InitializePanels();

    m_Initialized = true;
    ZGINE_CORE_INFO("Editor Initialized");
}

void Editor::Shutdown() {
    m_PanelManager.Clear();

    // Shutdown EditorContext and its services
    m_Context.Shutdown();

    if (m_ImGuiRendererInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        m_ImGuiRendererInitialized = false;
    }
    if (m_ImGuiPlatformInitialized) {
        ImGui_ImplGlfw_Shutdown();
        m_ImGuiPlatformInitialized = false;
    }
    FontManager::Shutdown();
    if (m_ImGuiContextCreated) {
        ImGui::DestroyContext();
        m_ImGuiContextCreated = false;
    }
    m_Initialized = false;
}

void Editor::BeginFrame() {
    if (!m_Initialized) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndFrame() {
    if (!m_Initialized) {
        return;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void Editor::OnUpdate(World* World) {
    ZGINE_UNUSED(World);
}

// All old callback setters removed - use EditorContext, EventBus, and Commands instead

void Editor::SetSceneTexture(unsigned int textureId) {
    m_Context.GetViewportContext().SetSceneTexture(textureId);
}

void Editor::SetSceneViewProjection(const Math::Matrix4& view, const Math::Matrix4& projection) {
    m_Context.GetViewportContext().SetViewProjection(view, projection);
}

void Editor::SetRenderStats(const RenderStats& stats) {
    m_RenderStats = stats;
}

const Math::Vector2& Editor::GetSceneViewportSize() const {
    return m_Context.GetViewportContext().GetViewportSize();
}

bool Editor::IsSceneViewportHovered() const {
    return m_Context.GetViewportContext().IsHovered();
}

bool Editor::IsSceneViewportFocused() const {
    return m_Context.GetViewportContext().IsFocused();
}

const Math::Vector2& Editor::GetSceneViewportBoundsMin() const {
    return m_Context.GetViewportContext().GetViewportBoundsMin();
}

const Math::Vector2& Editor::GetSceneViewportBoundsMax() const {
    return m_Context.GetViewportContext().GetViewportBoundsMax();
}

void Editor::Render(World* World) {
    if (!m_Initialized) {
        return;
    }

    AttachScene(World);
    RenderDockSpace(World);
    HandleShortcuts(World);
    m_PanelManager.OnUpdate(ImGui::GetIO().DeltaTime);
    m_PanelManager.OnGuiRender();
}

void Editor::RenderDockSpace(World* World) {
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    auto ensureTabBarVisible = [](ImGuiID dockId) {
        if (dockId == 0) {
            return;
        }
        if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockId)) {
            ImGuiDockNodeFlags flags = node->LocalFlags;
            flags &= ~ImGuiDockNodeFlags_NoTabBar;
            flags &= ~ImGuiDockNodeFlags_AutoHideTabBar;
            flags &= ~ImGuiDockNodeFlags_HiddenTabBar;
            flags &= ~ImGuiDockNodeFlags_NoWindowMenuButton;
            flags &= ~ImGuiDockNodeFlags_NoCloseButton;
            node->SetLocalFlags(flags);
        }
    };

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);

    RenderMainMenu(World);
    const float toolbarHeight = m_Toolbar->GetHeight();
    ImGui::Dummy(ImVec2(0.0f, toolbarHeight));
    ImVec2 dockspaceSize = ImGui::GetContentRegionAvail();
    m_DockspaceId = ImGui::GetID("ZgineDockSpace");
    ImGui::DockSpace(m_DockspaceId, dockspaceSize, dockspaceFlags);
    ImGui::End();

    m_Toolbar->OnGuiRender();
    NormalizeDockspaceNodeFlags();

    if (m_DockspaceBuilt && ImGui::DockBuilderGetNode(m_DockspaceId) == nullptr) {
        m_DockspaceBuilt = false;
    }

    if (!m_DockspaceBuilt) {
        ImGui::DockBuilderRemoveNode(m_DockspaceId);
        ImGui::DockBuilderAddNode(m_DockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(m_DockspaceId, dockspaceSize);

        ImGuiID dockMainId = m_DockspaceId;
        ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.20f, nullptr, &dockMainId);
        ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.25f, nullptr, &dockMainId);
        ImGuiID dockRightBottomId = ImGui::DockBuilderSplitNode(dockRightId, ImGuiDir_Down, 0.25f, nullptr, &dockRightId);
        ImGuiID dockBottomId = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.20f, nullptr, &dockMainId);
        ImGuiID dockBottomLeftId = ImGui::DockBuilderSplitNode(dockBottomId, ImGuiDir_Left, 0.55f, nullptr, &dockBottomId);

        m_SceneDockId = dockMainId;
        m_LeftDockId = dockLeftId;
        m_RightDockId = dockRightId;
        m_RightBottomDockId = dockRightBottomId;
        m_BottomDockId = dockBottomId;
        m_BottomLeftDockId = dockBottomLeftId;

        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftId);
        ImGui::DockBuilderDockWindow("Inspector", dockRightId);
        ImGui::DockBuilderDockWindow("World", m_SceneDockId);
        ImGui::DockBuilderDockWindow("Asset Browser", dockBottomLeftId);
        ImGui::DockBuilderDockWindow("Console", dockBottomId);
        ImGui::DockBuilderDockWindow("Profiler", dockBottomId);
        ImGui::DockBuilderDockWindow("Game View", dockBottomId);
        ImGui::DockBuilderDockWindow("Status", dockRightBottomId);

        if (ImGuiDockNode* sceneNode = ImGui::DockBuilderGetNode(m_SceneDockId)) {
            ImGuiDockNodeFlags sceneFlags = sceneNode->LocalFlags;
            sceneFlags &= ~(ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_AutoHideTabBar |
                            ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton);
            sceneNode->SetLocalFlags(sceneFlags);
        }

        ImGui::DockBuilderFinish(m_DockspaceId);
        m_DockspaceBuilt = true;
    }

    if (m_DockspaceBuilt) {
        ensureTabBarVisible(m_LeftDockId);
        ensureTabBarVisible(m_RightDockId);
        ensureTabBarVisible(m_RightBottomDockId);
        ensureTabBarVisible(m_SceneDockId);
    }

}

void Editor::NormalizeDockspaceNodeFlags() {
    if (m_DockspaceId == 0) {
        return;
    }

    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (!ctx) {
        return;
    }

    ImGuiDockNode* root = ImGui::DockBuilderGetNode(m_DockspaceId);
    if (!root) {
        return;
    }

    ImGuiStorage& nodes = ctx->DockContext.Nodes;
    const ImGuiDockNodeFlags clearFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_HiddenTabBar |
                                          ImGuiDockNodeFlags_AutoHideTabBar | ImGuiDockNodeFlags_NoWindowMenuButton |
                                          ImGuiDockNodeFlags_NoCloseButton;
    for (int i = 0; i < nodes.Data.Size; ++i) {
        ImGuiDockNode* node = static_cast<ImGuiDockNode*>(nodes.Data[i].val_p);
        if (!node) {
            continue;
        }
        ImGuiDockNode* nodeRoot = ImGui::DockNodeGetRootNode(node);
        if (!nodeRoot || nodeRoot->ID != m_DockspaceId) {
            continue;
        }
        node->LocalFlags &= ~clearFlags;
        node->LocalFlagsInWindows &= ~clearFlags;
        node->SharedFlags &= ~clearFlags;
        node->WantHiddenTabBarUpdate = true;
        node->WantHiddenTabBarToggle = false;
        node->UpdateMergedFlags();
    }
}

void Editor::DockNodeWindowMenuHandler(ImGuiContext* ctx, ImGuiDockNode* node, ImGuiTabBar* tab_bar) {
    ImGui::DockNodeWindowMenuHandler_Default(ctx, node, tab_bar);
    if (!ctx) {
        return;
    }
    Editor* editor = static_cast<Editor*>(ctx->IO.UserData);
    if (!editor) {
        return;
    }
    editor->HandleDockNodeWindowMenu(node, tab_bar);
}

void Editor::HandleDockNodeWindowMenu(ImGuiDockNode* node, ImGuiTabBar* tab_bar) {
    if (!node) {
        return;
    }

    ImGuiWindow* target = ResolveDockTabWindow(node, tab_bar);
    if (!target) {
        return;
    }

    const bool canClose = CanCloseDockedWindow(node);
    ImGui::Separator();
    ImGui::BeginDisabled(!canClose);
    if (ImGui::MenuItem("Close Tab")) {
        ClosePanelWindow(target);
    }
    ImGui::EndDisabled();
}

int Editor::CountDockedWindows(ImGuiDockNode* node) const {
    if (!node) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < node->Windows.Size; ++i) {
        ImGuiWindow* window = node->Windows[i];
        if (window && window->WasActive) {
            ++count;
        }
    }

    if (node->ChildNodes[0]) {
        count += CountDockedWindows(node->ChildNodes[0]);
    }
    if (node->ChildNodes[1]) {
        count += CountDockedWindows(node->ChildNodes[1]);
    }

    return count;
}

bool Editor::CanCloseDockedWindow(ImGuiDockNode* node) const {
    if (!node || m_DockspaceId == 0) {
        return true;
    }

    ImGuiDockNode* root = ImGui::DockNodeGetRootNode(node);
    if (!root || root->ID != m_DockspaceId) {
        return true;
    }

    return CountDockedWindows(root) > 1;
}

void Editor::ClosePanelWindow(ImGuiWindow* window) {
    if (!window) {
        return;
    }

    auto panel = m_PanelManager.FindPanel(window->Name);
    if (panel) {
        panel->SetOpen(false);
    }
}

void Editor::RenderMainMenu(World* World) {
    if (!ImGui::BeginMenuBar()) {
        return;
    }

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New World", "Ctrl+N")) {
            if (World) {
                World->Clear();
                m_Context.GetSelectionContext().Clear();
            }
        }
        if (ImGui::MenuItem("Open World", "Ctrl+O")) {
            // TODO: Implement World loading via events or commands
            // GetContext().GetEventBus().PublishImmediate(SceneLoadRequestedEvent(World));
        }
        if (ImGui::MenuItem("Save World", "Ctrl+S")) {
            // TODO: Implement World saving via events or commands
            // GetContext().GetEventBus().PublishImmediate(SceneSaveRequestedEvent(World));
        }
        ImGui::Separator();
        ImGui::MenuItem("Exit", nullptr, false, false);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
        ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
        ImGui::Separator();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        m_PanelManager.RenderPanelMenuItems();
        ImGui::Separator();
        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Dark", nullptr, m_CurrentTheme == ImGuiThemeType::Dark)) {
                SetTheme(ImGuiThemeType::Dark);
            }
            if (ImGui::MenuItem("Light", nullptr, m_CurrentTheme == ImGuiThemeType::Light)) {
                SetTheme(ImGuiThemeType::Light);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Reset Layout")) {
            m_DockspaceBuilt = false;
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {
        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Empty")) {
                if (World) {
                    Entity created = World->CreateEntity("Empty");
                    m_Context.GetSelectionContext().SetPrimary(created);
                }
            }
            if (ImGui::MenuItem("Cube")) {
                if (World) {
                    Entity created = World->CreateEntity("Cube");
                    created.AddComponent<TransformComponent>();
                    // TODO: Add mesh rendering component for cube primitive
                    m_Context.GetSelectionContext().SetPrimary(created);
                }
            }
            if (ImGui::MenuItem("Plane")) {
                if (World) {
                    Entity created = World->CreateEntity("Plane");
                    created.AddComponent<TransformComponent>();
                    // TODO: Add mesh rendering component for plane primitive
                    m_Context.GetSelectionContext().SetPrimary(created);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Gizmo")) {
            if (ImGui::MenuItem("Move", "W", m_GizmoOperation == ImGuizmo::TRANSLATE)) {
                m_GizmoOperation = ImGuizmo::TRANSLATE;
            }
            if (ImGui::MenuItem("Rotate", "E", m_GizmoOperation == ImGuizmo::ROTATE)) {
                m_GizmoOperation = ImGuizmo::ROTATE;
            }
            if (ImGui::MenuItem("Scale", "R", m_GizmoOperation == ImGuizmo::SCALE)) {
                m_GizmoOperation = ImGuizmo::SCALE;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Local", nullptr, m_GizmoMode == ImGuizmo::LOCAL)) {
                m_GizmoMode = ImGuizmo::LOCAL;
            }
            if (ImGui::MenuItem("World", nullptr, m_GizmoMode == ImGuizmo::WORLD)) {
                m_GizmoMode = ImGuizmo::WORLD;
            }
            ImGui::Separator();
            ImGui::MenuItem("Grid", nullptr, &m_ShowGrid);
            ImGui::MenuItem("Axes", nullptr, &m_ShowAxes);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("About", nullptr, false, false);
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void Editor::HandleShortcuts(World* World) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) {
        return;
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
        // TODO: Implement World saving via events or commands
        // GetContext().GetEventBus().PublishImmediate(SceneSaveRequestedEvent(World));
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O)) {
        // TODO: Implement World loading via events or commands
        // GetContext().GetEventBus().PublishImmediate(SceneLoadRequestedEvent(World));
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N)) {
        if (World) {
            World->Clear();
            m_Context.GetSelectionContext().Clear();
        }
    }
    Entity selected = m_Context.GetSelectionContext().GetPrimary();
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && selected) {
        if (World) {
            World->DestroyEntity(selected);
        }
        m_Context.GetSelectionContext().Remove(selected);
    }
}

// CreatePrimitive and DeleteEntity removed - now handled by Panels

Entity Editor::GetSelectedEntity() const {
    return m_Context.GetSelectionContext().GetPrimary();
}

void Editor::SetSelectedEntity(Entity entity) {
    if (entity) {
        m_Context.GetSelectionContext().SetPrimary(entity);
    } else {
        m_Context.GetSelectionContext().Clear();
    }
}

void Editor::SetTheme(ImGuiThemeType theme) {
    m_CurrentTheme = theme;
    ImGuiTheme::ApplyTheme(theme);
    ImVec4 windowBg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    m_ClearColor = { windowBg.x, windowBg.y, windowBg.z, windowBg.w };
}

void Editor::InitializePanels() {
    // Create and initialize toolbar
    m_Toolbar = std::make_unique<ToolbarPanel>(m_Context);
    m_Toolbar->SetPlayModeCallback(m_OnPlayMode);
    m_Toolbar->SetModeRef(&m_Mode);
    m_Toolbar->SetGizmoOperationRef(&m_GizmoOperation);
    m_Toolbar->SetGizmoModeRef(&m_GizmoMode);
    m_Toolbar->SetShowGridRef(&m_ShowGrid);
    m_Toolbar->SetShowAxesRef(&m_ShowAxes);

    // Register all panels
    m_PanelManager.AddPanel<HierarchyPanel>("Hierarchy", m_Context);
    m_PanelManager.AddPanel<InspectorPanel>("Inspector", m_Context);
    m_PanelManager.AddPanel<ViewportPanel>("World", m_Context);
    m_PanelManager.AddPanel<ContentBrowserPanel>("Asset Browser", m_Context, m_AssetsRoot);
    m_PanelManager.AddPanel<ConsolePanel>("Console", m_Context);
    auto* perfPanel = m_PanelManager.AddPanel<PerformancePanel>("Profiler", m_Context, &m_RenderStats).get();
    if (perfPanel) perfPanel->SetOpen(false);

    m_PanelManager.AddPanel<StatusPanel>("Status", m_Context);
}

void Editor::AttachScene(World* World) {
    // Set the World in EditorContext (this will create SceneViewModel)
    m_Context.SetActiveScene(World);

    // Attach World to panels
    for (auto& panel : m_PanelManager.GetPanels()) {
        if (auto* hierarchyPanel = dynamic_cast<HierarchyPanel*>(panel.get())) {
            hierarchyPanel->SetScene(World);
        } else if (auto* inspectorPanel = dynamic_cast<InspectorPanel*>(panel.get())) {
            inspectorPanel->SetScene(World);
        } else if (auto* viewportPanel = dynamic_cast<ViewportPanel*>(panel.get())) {
            viewportPanel->SetScene(World);
        } else if (auto* statusPanel = dynamic_cast<StatusPanel*>(panel.get())) {
            statusPanel->SetScene(World);
        }
    }
}

}
