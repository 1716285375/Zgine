#include <Zgine/Editor/UI/Inspectors/AudioInspector.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Foundation/Macro.h>
#include <imgui.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

void AudioInspector::DrawAudioSourceProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& audio = entity.GetComponent<AudioSourceComponent>();

    UI::DrawFilePathInput("File Path", audio.FilePath);

    ImGui::Checkbox("Looping", &audio.IsLooping);
    ImGui::Checkbox("Spatialized", &audio.Spatialized);
    UI::DrawFloatDrag("Volume", audio.Volume, 0.01f, 0.0f, 4.0f);
    UI::DrawFloatDrag("Pitch", audio.Pitch, 0.01f, 0.1f, 4.0f);

    if (audio.Spatialized) {
        UI::DrawFloatDrag("Min Distance", audio.MinDistance, 0.1f, 0.0f, audio.MaxDistance);
        UI::DrawFloatDrag("Max Distance", audio.MaxDistance, 0.1f, audio.MinDistance, 10000.0f);
    }
}

void AudioInspector::DrawAudioListenerProperties(Entity entity) {
    ZGINE_UNUSED(entity);
    if (!ImGui::CollapsingHeader("Audio Listener", ImGuiTreeNodeFlags_DefaultOpen)) return;

    ImGui::Text("Uses Camera + Transform for listener position.");
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
