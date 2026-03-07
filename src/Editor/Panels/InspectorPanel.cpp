#include <Zgine/Editor/Panels/InspectorPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/Editor/Events/EntityEvents.h>
// NEW: Inspector includes
#include <Zgine/Editor/UI/Inspectors/CoreInspector.h>
#include <Zgine/Editor/UI/Inspectors/RenderingInspector.h>
#include <Zgine/Editor/UI/Inspectors/PhysicsInspector.h>
#include <Zgine/Editor/UI/Inspectors/AudioInspector.h>
#include <Zgine/Editor/UI/Inspectors/ScriptInspector.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Components/Components.h>
#include <imgui.h>

namespace Zgine {

InspectorPanel::InspectorPanel(const std::string& name, EditorContext& context)
    : EditorPanel(name, context)
{
}

void InspectorPanel::OnAttach() {
    // Subscribe to events if needed
}

void InspectorPanel::OnDetach() {
    // Unsubscribe from events if needed
}

void InspectorPanel::OnUpdate(float deltaTime) {
    ZGINE_UNUSED(deltaTime);
    // Inspector doesn't need per-frame updates
}

void InspectorPanel::OnGuiRender() {
    BeginPanel();

    auto& selectionContext = GetContext().GetSelectionContext();
    selectionContext.Validate(m_World);
    Entity selected = selectionContext.GetPrimary();

    if (!selected) {
        ImGui::Text("No entity selected");
        EndPanel();
        return;
    }

    const auto& selection = selectionContext.GetSelection();
    if (selection.size() > 1) {
        ImGui::Text("Multiple selection (%zu). Showing primary entity.", selection.size());
        ImGui::Separator();
    }

    // Delete button
    if (ImGui::Button("Delete Entity")) {
        DeleteEntity(m_World, selected);
        selectionContext.Remove(selected);
        EndPanel();
        return;
    }

    ImGui::SameLine();
    ImGui::Text("ID: %u", static_cast<uint32_t>(static_cast<entt::entity>(selected)));

    // Tag (always editable, no collapsing header)
    if (selected.HasComponent<TagComponent>()) {
        UI::Inspectors::CoreInspector::DrawTagProperties(selected);
    }

    ImGui::Separator();

    // Draw all components
    DrawComponentInspector<TransformComponent>(selected, &UI::Inspectors::CoreInspector::DrawTransformProperties);
    DrawComponentInspector<ColorComponent>(selected, &UI::Inspectors::RenderingInspector::DrawColorProperties);
    DrawComponentInspector<PBRMaterialComponent>(selected, &UI::Inspectors::RenderingInspector::DrawPBRMaterialProperties);
    DrawComponentInspector<DirectionalLightComponent>(selected, &UI::Inspectors::RenderingInspector::DrawDirectionalLightProperties);
    DrawComponentInspector<PointLightComponent>(selected, &UI::Inspectors::RenderingInspector::DrawPointLightProperties);
    DrawComponentInspector<SpotLightComponent>(selected, &UI::Inspectors::RenderingInspector::DrawSpotLightProperties);
    DrawComponentInspector<RigidbodyComponent>(selected, &UI::Inspectors::PhysicsInspector::DrawRigidbody2DProperties);
    DrawComponentInspector<BoxColliderComponent>(selected, &UI::Inspectors::PhysicsInspector::DrawBoxCollider2DProperties);
    DrawComponentInspector<AudioSourceComponent>(selected, &UI::Inspectors::AudioInspector::DrawAudioSourceProperties);
    DrawComponentInspector<AudioListenerComponent>(selected, &UI::Inspectors::AudioInspector::DrawAudioListenerProperties);
    DrawComponentInspector<ScriptComponent>(selected, &UI::Inspectors::ScriptInspector::DrawNativeScriptProperties);

    ImGui::Separator();

    // "Add Component" button
    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent")) {
        AddComponentMenuItem<ColorComponent>(selected, "Color");
        AddComponentMenuItem<PBRMaterialComponent>(selected, "PBR Material");
        AddComponentMenuItem<DirectionalLightComponent>(selected, "Directional Light");
        AddComponentMenuItem<PointLightComponent>(selected, "Point Light");
        AddComponentMenuItem<SpotLightComponent>(selected, "Spot Light");

        ImGui::Separator();

        AddComponentMenuItem<RigidbodyComponent>(selected, "Rigid Body");
        AddComponentMenuItem<BoxColliderComponent>(selected, "Box Collider");

        ImGui::Separator();

        AddComponentMenuItem<AudioSourceComponent>(selected, "Audio Source");
        AddComponentMenuItem<AudioListenerComponent>(selected, "Audio Listener");

        ImGui::Separator();

        AddComponentMenuItem<ScriptComponent>(selected, "Script");

        ImGui::EndPopup();
    }

    EndPanel();
}

template<typename Component>
void InspectorPanel::DrawComponentInspector(Entity entity, void (*drawFunc)(Entity)) {
    if (!entity.HasComponent<Component>()) return;

    ImGui::PushID(typeid(Component).name());

    bool removeComponent = false;
    if (ImGui::BeginPopupContextItem("ComponentSettings")) {
        if (ImGui::MenuItem("Remove Component")) {
            removeComponent = true;
        }
        ImGui::EndPopup();
    }

    // Call inspector draw function
    if (drawFunc) {
        drawFunc(entity);
    }

    ImGui::PopID();

    if (removeComponent) {
        entity.RemoveComponent<Component>();
    }
}

template<typename Component>
void InspectorPanel::AddComponentMenuItem(Entity entity, const char* label) {
    if (entity.HasComponent<Component>()) {
        ImGui::BeginDisabled();
        ImGui::MenuItem(label);
        ImGui::EndDisabled();
    } else {
        if (ImGui::MenuItem(label)) {
            entity.AddComponent<Component>();
        }
    }
}

void InspectorPanel::DeleteEntity(World* World, Entity entity) {
    if (!World || !entity) return;

    // Create command for entity deletion (supports undo/redo)
    auto command = std::make_unique<DeleteEntityCommand>(World, entity);

    // Execute command through CommandHistory
    auto& commandHistory = GetContext().GetCommandHistory();
    commandHistory.Execute(std::move(command));
}

} // namespace Zgine
