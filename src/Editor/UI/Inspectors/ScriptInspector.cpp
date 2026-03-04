#include <Zgine/Editor/UI/Inspectors/ScriptInspector.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Core/Base/Macro.h>
#include <imgui.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

void ScriptInspector::DrawNativeScriptProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& script = entity.GetComponent<ScriptComponent>();

    UI::DrawFilePathInput("Script Path", script.ScriptPath, 512);

    ImGui::Text("Initialized: %s", script.IsInitialized ? "Yes" : "No");
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
