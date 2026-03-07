#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <memory>
#include <string>
#include <Zgine/World/Core/Entity.h>

namespace Zgine {

// Forward declarations
class SelectionContext;
class EditorEventBus;
class EditorCommandHistory;
class World;
class SceneViewModel;

/**
 * @brief Viewport rendering context
 *
 * Manages viewport-specific state such as World texture,
 * viewport bounds, camera matrices, and rendering flags.
 */
class ViewportContext {
public:
    ViewportContext();
    ~ViewportContext() = default;

    // World texture
    void SetSceneTexture(unsigned int textureId) { m_SceneTextureId = textureId; }
    unsigned int GetSceneTexture() const { return m_SceneTextureId; }

    // Viewport size and bounds
    void SetViewportSize(const Math::Vector2& size) { m_ViewportSize = size; }
    const Math::Vector2& GetViewportSize() const { return m_ViewportSize; }

    void SetViewportBounds(const Math::Vector2& min, const Math::Vector2& max) {
        m_ViewportBounds[0] = min;
        m_ViewportBounds[1] = max;
    }
    const Math::Vector2& GetViewportBoundsMin() const { return m_ViewportBounds[0]; }
    const Math::Vector2& GetViewportBoundsMax() const { return m_ViewportBounds[1]; }

    // Focus and hover state
    void SetFocused(bool focused) { m_Focused = focused; }
    bool IsFocused() const { return m_Focused; }

    void SetHovered(bool hovered) { m_Hovered = hovered; }
    bool IsHovered() const { return m_Hovered; }

    // Camera matrices
    void SetViewProjection(const Math::Matrix4& view, const Math::Matrix4& projection) {
        m_View = view;
        m_Projection = projection;
        m_ViewProjectionValid = true;
    }
    const Math::Matrix4& GetView() const { return m_View; }
    const Math::Matrix4& GetProjection() const { return m_Projection; }
    bool IsViewProjectionValid() const { return m_ViewProjectionValid; }

    // Rendering flags
    void SetShowGrid(bool show) { m_ShowGrid = show; }
    bool GetShowGrid() const { return m_ShowGrid; }

    void SetShowAxes(bool show) { m_ShowAxes = show; }
    bool GetShowAxes() const { return m_ShowAxes; }

private:
    unsigned int m_SceneTextureId = 0;
    Math::Vector2 m_ViewportSize = { 0.0f, 0.0f };
    Math::Vector2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
    bool m_Focused = false;
    bool m_Hovered = false;

    Math::Matrix4 m_View = Math::Matrix4(1.0f);
    Math::Matrix4 m_Projection = Math::Matrix4(1.0f);
    bool m_ViewProjectionValid = false;

    bool m_ShowGrid = true;
    bool m_ShowAxes = true;
};

/**
 * @brief World context for editor
 *
 * Manages the active World and related state.
 */
class SceneContext {
public:
    SceneContext() = default;
    ~SceneContext() = default;

    void SetActiveScene(World* World) { m_ActiveScene = World; }
    World* GetActiveScene() const { return m_ActiveScene; }

    void SetScenePath(const std::string& path) { m_ScenePath = path; }
    const std::string& GetScenePath() const { return m_ScenePath; }

    void SetSceneDirty(bool dirty) { m_SceneDirty = dirty; }
    bool IsSceneDirty() const { return m_SceneDirty; }

private:
    World* m_ActiveScene = nullptr;
    std::string m_ScenePath;
    bool m_SceneDirty = false;
};

/**
 * @brief Central service locator for editor services
 *
 * EditorContext provides unified access to all editor subsystems
 * and manages their lifecycles. This eliminates the need for
 * passing multiple parameters to panels and other components.
 *
 * Design Pattern: Service Locator
 *
 * Usage:
 * @code
 *   EditorContext& ctx = GetContext();
 *   ctx.GetSelectionContext().Select(entity);
 *   ctx.GetEventBus().Publish(EntitySelectedEvent(entity));
 * @endcode
 */
class EditorContext {
public:
    EditorContext();
    ~EditorContext();

    ZGINE_NON_COPYABLE(EditorContext)

    // Initialize all services
    void Initialize();
    void Shutdown();

    // Service accessors
    SelectionContext& GetSelectionContext() { return *m_SelectionContext; }
    const SelectionContext& GetSelectionContext() const { return *m_SelectionContext; }

    EditorEventBus& GetEventBus() { return *m_EventBus; }
    const EditorEventBus& GetEventBus() const { return *m_EventBus; }

    EditorCommandHistory& GetCommandHistory() { return *m_CommandHistory; }
    const EditorCommandHistory& GetCommandHistory() const { return *m_CommandHistory; }

    ViewportContext& GetViewportContext() { return m_ViewportContext; }
    const ViewportContext& GetViewportContext() const { return m_ViewportContext; }

    SceneContext& GetSceneContext() { return m_SceneContext; }
    const SceneContext& GetSceneContext() const { return m_SceneContext; }

    // ViewModel (MVVM)
    SceneViewModel* GetSceneViewModel() { return m_SceneViewModel.get(); }

    // Helper to set active World and create/update ViewModel
    void SetActiveScene(World* World);

private:
    // Core services (owned)
    std::unique_ptr<SelectionContext> m_SelectionContext;
    std::unique_ptr<EditorEventBus> m_EventBus;
    std::unique_ptr<EditorCommandHistory> m_CommandHistory;

    // ViewModel (MVVM Presentation Layer)
    std::unique_ptr<SceneViewModel> m_SceneViewModel; // Added SceneViewModel member

    // Contexts (value types)
    ViewportContext m_ViewportContext;
    SceneContext m_SceneContext;

    bool m_Initialized = false;
};

} // namespace Zgine
