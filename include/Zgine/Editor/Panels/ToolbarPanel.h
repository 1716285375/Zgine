#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <functional>

namespace Zgine {

/**
 * @brief Toolbar panel for editor controls (Play/Pause, Gizmo, etc.)
 *
 * A horizontal toolbar at the top of the editor containing:
 * - Play/Pause/Stop controls
 * - Gizmo operation selection (Move/Rotate/Scale)
 * - Gizmo mode selection (Local/World)
 * - Grid and Axes toggles
 * - Current editor mode display
 */
class ToolbarPanel : public EditorPanel {
public:
    ToolbarPanel(EditorContext& context);
    ~ToolbarPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    float GetHeight() const;

    // Callbacks and state
    void SetPlayModeCallback(const std::function<void(EditorMode)>& callback);

    // Refs to editor state (externally managed)
    void SetModeRef(EditorMode* mode) { m_ModeRef = mode; }
    void SetGizmoOperationRef(int* op) { m_GizmoOpRef = op; }
    void SetGizmoModeRef(int* mode) { m_GizmoModeRef = mode; }
    void SetShowGridRef(bool* showGrid) { m_ShowGridRef = showGrid; }
    void SetShowAxesRef(bool* showAxes) { m_ShowAxesRef = showAxes; }

private:
    std::function<void(EditorMode)> m_OnPlayMode;

    // References to external state
    EditorMode* m_ModeRef = nullptr;
    int* m_GizmoOpRef = nullptr;
    int* m_GizmoModeRef = nullptr;
    bool* m_ShowGridRef = nullptr;
    bool* m_ShowAxesRef = nullptr;
};

} // namespace Zgine
