#include <Zgine/Editor/UI/Inspectors/PhysicsInspector.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Foundation/Macro.h>
#include <imgui.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

void PhysicsInspector::DrawRigidbody2DProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& rb = entity.GetComponent<RigidbodyComponent>();

    const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
    int currentType = static_cast<int>(rb.Type);
    if (ImGui::Combo("Body Type", &currentType, bodyTypeStrings, 3)) {
        rb.Type = static_cast<RigidbodyType>(currentType);
    }

    UI::DrawFloatDrag("Mass", rb.Mass, 0.1f, 0.0f, 1000.0f);
    UI::DrawFloatDrag("Linear Drag", rb.LinearDrag, 0.01f, 0.0f, 100.0f);
    UI::DrawFloatDrag("Angular Drag", rb.AngularDrag, 0.01f, 0.0f, 100.0f);
    UI::DrawFloatDrag("Gravity Scale", rb.GravityScale, 0.1f, -10.0f, 10.0f);
    UI::DrawFloatDrag("Friction", rb.Friction, 0.01f, 0.0f, 1.0f);
    UI::DrawFloatDrag("Restitution", rb.Restitution, 0.01f, 0.0f, 1.0f);
    ImGui::Checkbox("Fixed Rotation", &rb.FixedRotation);
}

void PhysicsInspector::DrawBoxCollider2DProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& collider = entity.GetComponent<BoxColliderComponent>();
    UI::DrawVec3Control("Size", collider.Size, 0.1f, 0.0f, 100.0f);
    UI::DrawVec3Control("Offset", collider.Offset, 0.1f);
}

void PhysicsInspector::DrawCircleCollider2DProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Circle Collider", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& collider = entity.GetComponent<CircleColliderComponent>();
    UI::DrawFloatDrag("Radius", collider.Radius, 0.01f, 0.01f, 100.0f);
    UI::DrawVec3Control("Offset", collider.Offset, 0.1f);
    ImGui::Checkbox("Is Trigger", &collider.IsTrigger);
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
