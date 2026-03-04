#include <Zgine/Editor/UI/Inspectors/CoreInspector.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/TransformEvents.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/Scene/Components/Components.h>
#include <imgui.h>
#include <cstring>

namespace Zgine {
namespace UI {
namespace Inspectors {

void CoreInspector::DrawTagProperties(Entity entity) {
    auto& tag = entity.GetComponent<TagComponent>();
    char buffer[256];
    std::strncpy(buffer, tag.Tag.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
        tag.Tag = std::string(buffer);
    }
}

void CoreInspector::DrawTransformProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& transform = entity.GetComponent<TransformComponent>();

    bool changed = false;
    changed |= UI::DrawVec3Control("Translation", transform.Translation, 0.1f);
    changed |= UI::DrawVec3Control("Rotation", transform.Rotation, 1.0f);
    changed |= UI::DrawVec3Control("Scale", transform.Scale, 0.1f);

    // Note: Event publishing removed - should be done by command pattern
    // If needed, caller can publish events
}

void CoreInspector::DrawRelationshipProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Relationship")) return;

    if (!entity.HasComponent<RelationshipComponent>()) {
        ImGui::TextDisabled("No relationship data");
        return;
    }

    auto& rel = entity.GetComponent<RelationshipComponent>();

    ImGui::Text("Parent: %s", rel.Parent != entt::null ? "Has Parent" : "Root");
    ImGui::Text("Children: %zu", rel.Children.size());
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
