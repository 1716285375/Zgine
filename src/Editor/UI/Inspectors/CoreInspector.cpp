#include <Zgine/Editor/UI/Inspectors/CoreInspector.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/TransformCommands.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/World/Components/Components.h>
#include <imgui.h>
#include <cstring>
#include <memory>

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

void CoreInspector::DrawTransformProperties(Entity entity, EditorCommandHistory* commandHistory) {
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& transform = entity.GetComponent<TransformComponent>();
    Math::Vector3 translation = transform.Translation;
    Math::Vector3 rotation = transform.Rotation;
    Math::Vector3 scale = transform.Scale;

    bool changed = false;
    changed |= UI::DrawVec3Control("Translation", translation, 0.1f);
    changed |= UI::DrawVec3Control("Rotation", rotation, 1.0f);
    changed |= UI::DrawVec3Control("Scale", scale, 0.1f);

    if (!changed) {
        return;
    }

    if (commandHistory) {
        commandHistory->Execute(std::make_unique<TransformEntityCommand>(
            entity,
            translation,
            rotation,
            scale
        ));
        return;
    }

    transform.Translation = translation;
    transform.Rotation = rotation;
    transform.Scale = scale;
}

void CoreInspector::DrawRelationshipProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Relationship")) return;

    if (!entity.HasComponent<RelationshipComponent>()) {
        ImGui::TextDisabled("No relationship data");
        return;
    }

    auto& rel = entity.GetComponent<RelationshipComponent>();

    ImGui::Text("Parent: %s", rel.Parent ? "Has Parent" : "Root");
    ImGui::Text("Children: %zu", rel.Children.size());
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
