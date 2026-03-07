#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Editor/Panels/EditorPanel.h>
#include <memory>

namespace Zgine {

class World;
class GizmoController;

/**
 * @brief Main viewport panel for World rendering and editing
 *
 * Displays the rendered World and handles user interactions
 * such as entity selection, gizmo manipulation, and asset drag-drop.
 *
 * **Features**:
 * - Real-time World rendering
 * - Entity selection via mouse picking
 * - Transform gizmos (Move/Rotate/Scale)
 * - Camera controls
 * - Asset drag-and-drop onto World
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

    /** Set the World to render */
    void SetScene(World* World) { m_World = World; }

    /** Set the dock ID for this viewport */
    void SetDockId(ImGuiID dockId) { m_DockId = dockId; }

private:
    World* m_World = nullptr;
    ImGuiID m_DockId = 0;
    std::unique_ptr<GizmoController> m_GizmoController;
};

} // namespace Zgine
