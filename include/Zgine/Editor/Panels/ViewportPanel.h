#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Editor/Panels/EditorPanel.h>
#include <memory>

namespace Zgine {

class Scene;
class GizmoController;

/**
 * @brief Main viewport panel for scene rendering and editing
 *
 * Displays the rendered scene and handles user interactions
 * such as entity selection, gizmo manipulation, and asset drag-drop.
 *
 * **Features**:
 * - Real-time scene rendering
 * - Entity selection via mouse picking
 * - Transform gizmos (Move/Rotate/Scale)
 * - Camera controls
 * - Asset drag-and-drop onto scene
 * - Grid and axes visualization
 *
 * **Mouse Controls**:
 * - Left Click: Select entity
 * - Right Click + Drag: Rotate camera
 * - Middle Mouse + Drag: Pan camera
 * - Scroll Wheel: Zoom
 *
 * **Keyboard Shortcuts**:
 * - W: Move gizmo
 * - E: Rotate gizmo
 * - R: Scale gizmo
 * - F: Focus on selected entity
 */
class ViewportPanel : public EditorPanel {
public:
    ViewportPanel(const std::string& name, EditorContext& context);
    ~ViewportPanel() override;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the scene to render */
    void SetScene(Scene* scene) { m_Scene = scene; }

    /** Set the dock ID for this viewport */
    void SetDockId(ImGuiID dockId) { m_DockId = dockId; }

private:
    Scene* m_Scene = nullptr;
    ImGuiID m_DockId = 0;
    std::unique_ptr<GizmoController> m_GizmoController;
};

} // namespace Zgine
