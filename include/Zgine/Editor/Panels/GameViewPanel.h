#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>

namespace Zgine {

/**
 * @brief Game view panel for runtime game preview
 *
 * Displays the game's rendered output during play mode.
 * Similar to ViewportPanel but without editor gizmos and tools,
 * showing exactly what the player would see.
 *
 * **Features**:
 * - Clean game-only view
 * - Automatic aspect ratio handling
 * - Play mode synchronization
 * - Fullscreen toggle support (future)
 *
 * **Usage**:
 * Activated during play mode to show the game's final output.
 */
class GameViewPanel : public EditorPanel {
public:
    GameViewPanel(const std::string& name, EditorContext& context,
                  unsigned int& sceneTextureId);
    ~GameViewPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

private:
    unsigned int& m_SceneTextureId;
};

} // namespace Zgine
