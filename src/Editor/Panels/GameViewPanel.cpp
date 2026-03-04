#include <Zgine/Editor/Panels/GameViewPanel.h>
#include <imgui.h>

namespace Zgine {

GameViewPanel::GameViewPanel(const std::string& name, EditorContext& context, unsigned int& sceneTextureId)
    : EditorPanel(name, context)
    , m_SceneTextureId(sceneTextureId) {}

void GameViewPanel::OnAttach() {
    // Initialize game view if needed
}

void GameViewPanel::OnDetach() {
    // Cleanup game view if needed
}

void GameViewPanel::OnUpdate(float deltaTime) {
    ZGINE_UNUSED(deltaTime);
    // Game view doesn't need per-frame updates
}

void GameViewPanel::OnGuiRender() {
    BeginPanel();

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSize.x < 1.0f || viewportSize.y < 1.0f) {
        viewportSize = ImVec2(1.0f, 1.0f);
    }
    if (m_SceneTextureId != 0) {
        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_SceneTextureId)),
                     viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    } else {
        ImGui::Text("Game viewport is not ready.");
    }

    EndPanel();
}

}
