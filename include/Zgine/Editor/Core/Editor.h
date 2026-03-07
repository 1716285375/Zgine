#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Manager/EditorPanelManager.h>
#include <Zgine/Editor/Panels/ToolbarPanel.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <Zgine/Gui/Themes/ImGuiTheme.h>
#include <Zgine/Renderer/Pipeline/RenderStats.h>
#include <imgui.h>

struct ImGuiDockNode;
struct ImGuiTabBar;
struct ImGuiWindow;

namespace Zgine {

class World;
class Window;

/**
 * @brief Main editor class for Zgine Engine
 *
 * Manages the editor lifecycle, UI layout, panels, and editor services.
 * This is the entry point for all editor functionality.
 *
 * Refactored to use EditorContext service locator pattern for low coupling.
 */
class Editor {
public:
    Editor();
    ~Editor();

    void Initialize(Window* window);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void OnUpdate(World* World);
    void Render(World* World);

    // World texture and rendering
    void SetSceneTexture(unsigned int textureId);
    void SetSceneViewProjection(const Math::Matrix4& view, const Math::Matrix4& projection);
    void SetRenderStats(const RenderStats& stats);

    // Viewport queries (delegate to EditorContext)
    const Math::Vector2& GetSceneViewportSize() const;
    bool IsSceneViewportHovered() const;
    bool IsSceneViewportFocused() const;
    const Math::Vector2& GetSceneViewportBoundsMin() const;
    const Math::Vector2& GetSceneViewportBoundsMax() const;

    // Editor state queries
    EditorMode GetMode() const { return m_Mode; }

    // Selection (delegate to EditorContext)
    Entity GetSelectedEntity() const;
    void SetSelectedEntity(Entity entity);

    // Theming
    void SetTheme(ImGuiThemeType theme);
    ImGuiThemeType GetCurrentTheme() const { return m_CurrentTheme; }
    const Math::Vector4& GetClearColor() const { return m_ClearColor; }

    // Access to EditorContext
    EditorContext& GetContext() { return m_Context; }
    const EditorContext& GetContext() const { return m_Context; }

private:
    void RenderDockSpace(World* World);
    void RenderMainMenu(World* World);
    void HandleShortcuts(World* World);
    void InitializePanels();
    void AttachScene(World* World);
    void SetupEventHandlers();
    void NormalizeDockspaceNodeFlags();

    static void DockNodeWindowMenuHandler(ImGuiContext* ctx, ImGuiDockNode* node, ImGuiTabBar* tab_bar);
    void HandleDockNodeWindowMenu(ImGuiDockNode* node, ImGuiTabBar* tab_bar);
    int CountDockedWindows(ImGuiDockNode* node) const;
    bool CanCloseDockedWindow(ImGuiDockNode* node) const;
    void ClosePanelWindow(ImGuiWindow* window);

private:
    // Core services
    Window* m_Window = nullptr;
    EditorContext m_Context;              // Service container
    EditorPanelManager m_PanelManager;    // Panel management
    std::unique_ptr<ToolbarPanel> m_Toolbar;  // Toolbar panel

    // Editor state
    EditorMode m_Mode = EditorMode::Edit;
    RenderStats m_RenderStats;

    // Gizmo and viewport display state (for toolbar)
    int m_GizmoOperation = 0;  // ImGuizmo::TRANSLATE
    int m_GizmoMode = 0;       // ImGuizmo::LOCAL
    bool m_ShowGrid = true;
    bool m_ShowAxes = true;

    // Play mode callback
    std::function<void(EditorMode)> m_OnPlayMode;

    // Theme
    ImGuiThemeType m_CurrentTheme = ImGuiThemeType::Dark;
    Math::Vector4 m_ClearColor = { 0.1f, 0.1f, 0.12f, 1.0f };

    // Dockspace layout
    ImGuiID m_DockspaceId = 0;
    ImGuiID m_SceneDockId = 0;
    ImGuiID m_LeftDockId = 0;
    ImGuiID m_RightDockId = 0;
    ImGuiID m_RightBottomDockId = 0;
    ImGuiID m_BottomDockId = 0;
    ImGuiID m_BottomLeftDockId = 0;
    bool m_DockspaceBuilt = false;

    // Asset root directory
    std::filesystem::path m_AssetsRoot;

    // ImGui initialization state
    bool m_ImGuiContextCreated = false;
    bool m_ImGuiPlatformInitialized = false;
    bool m_ImGuiRendererInitialized = false;
    bool m_Initialized = false;
};

} // namespace Zgine
