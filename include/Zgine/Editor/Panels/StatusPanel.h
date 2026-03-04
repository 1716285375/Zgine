#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Editor/Core/SelectionContext.h>

namespace Zgine {

class Scene;

/**
 * @brief Status bar panel displaying editor state information
 *
 * Shows important editor state at the bottom of the window, including
 * selected entity information, scene statistics, and current mode.
 *
 * **Features**:
 * - Selected entity name and type
 * - Entity count in scene
 * - Editor mode display (Edit/Play/Pause)
 * - Quick stats summary
 *
 * **Usage**:
 * Typically docked at the bottom of the editor window.
 */
class StatusPanel : public EditorPanel {
public:
    StatusPanel(const std::string& name, EditorContext& context);
    ~StatusPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the scene to display statistics for */
    void SetScene(Scene* scene) { m_Scene = scene; }

private:
    Scene* m_Scene = nullptr;
    const SelectionContext& m_SelectionContext;
};

} // namespace Zgine
