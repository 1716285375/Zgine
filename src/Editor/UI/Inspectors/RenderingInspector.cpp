#include <Zgine/Editor/UI/Inspectors/RenderingInspector.h>
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Base/Macro.h>
#include <imgui.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

void RenderingInspector::DrawSpriteRendererProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& sprite = entity.GetComponent<SpriteRendererComponent>();
    UI::DrawColorEdit4("Color", sprite.Color);
    // Add more properties as needed
}

void RenderingInspector::DrawCameraProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& camera = entity.GetComponent<CameraComponent>();

    // Camera type
    const char* projectionTypes[] = { "Perspective", "Orthographic" };
    int currentType = static_cast<int>(camera.Camera->GetProjectionType());
    if (ImGui::Combo("Projection", &currentType, projectionTypes, 2)) {
        camera.Camera->SetProjectionType(static_cast<Camera::ProjectionType>(currentType));
    }

    // Camera properties based on type
    if (camera.Camera->GetProjectionType() == Camera::ProjectionType::Perspective) {
        float fov = camera.Camera->GetPerspectiveFOV();
        if (UI::DrawFloatDrag("FOV", fov, 1.0f, 1.0f, 120.0f)) {
            camera.Camera->SetPerspectiveFOV(fov);
        }

        float nearClip = camera.Camera->GetPerspectiveNear();
        if (UI::DrawFloatDrag("Near Clip", nearClip, 0.01f, 0.01f, 1000.0f)) {
            camera.Camera->SetPerspectiveNearClip(nearClip);
        }

        float farClip = camera.Camera->GetPerspectiveFar();
        if (UI::DrawFloatDrag("Far Clip", farClip, 1.0f, 0.1f, 10000.0f)) {
            camera.Camera->SetPerspectiveFarClip(farClip);
        }
    } else {
        float size = camera.Camera->GetOrthographicSize();
        if (UI::DrawFloatDrag("Size", size, 0.1f, 0.1f, 100.0f)) {
            camera.Camera->SetOrthographicSize(size);
        }

        float nearClip = camera.Camera->GetOrthographicNear();
        if (UI::DrawFloatDrag("Near Clip", nearClip, 0.1f, -1000.0f, 1000.0f)) {
            camera.Camera->SetOrthographicNearClip(nearClip);
        }

        float farClip = camera.Camera->GetOrthographicFar();
        if (UI::DrawFloatDrag("Far Clip", farClip, 1.0f, -1000.0f, 1000.0f)) {
            camera.Camera->SetOrthographicFarClip(farClip);
        }
    }

    ImGui::Checkbox("Primary", &camera.Primary);
    ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);
}

void RenderingInspector::DrawMeshRendererProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& mesh = entity.GetComponent<MeshComponent>();
    if (mesh.MeshHandle.IsValid()) {
        ImGui::Text("Handle: %s", mesh.MeshHandle.ToString().c_str());
    } else {
        ImGui::TextDisabled("No mesh assigned");
    }
    ImGui::Text("Has VAO: %s", mesh.VertexArray ? "Yes" : "No");
}

void RenderingInspector::DrawPrimitiveProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Primitive", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& primitive = entity.GetComponent<PrimitiveComponent>();

    const char* typeStrings[] = { "None", "Cube", "Plane", "Sphere", "Cylinder", "Cone" };
    int currentType = static_cast<int>(primitive.Type);
    if (ImGui::Combo("Shape", &currentType, typeStrings, 6)) {
        primitive.Type = static_cast<PrimitiveType>(currentType);
    }
}

void RenderingInspector::DrawColorProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& color = entity.GetComponent<ColorComponent>();
    UI::DrawColorEdit4("Base Color", color.Color);
}

void RenderingInspector::DrawPBRMaterialProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("PBR Material", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& material = entity.GetComponent<PBRMaterialComponent>();

    ImGui::Text("Material Parameters");
    UI::DrawColorEdit3("Albedo", material.Albedo);
    UI::DrawFloatSlider("Metallic", material.Metallic, 0.0f, 1.0f);
    UI::DrawFloatSlider("Roughness", material.Roughness, 0.0f, 1.0f);
    UI::DrawFloatSlider("AO", material.AO, 0.0f, 1.0f);
}

void RenderingInspector::DrawDirectionalLightProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& light = entity.GetComponent<DirectionalLightComponent>();

    UI::DrawVec3Control("Direction", light.Direction, 0.01f, -1.0f, 1.0f);
    UI::DrawColorEdit3("Color", light.Color);
    UI::DrawFloatDrag("Intensity", light.Intensity, 0.1f, 0.0f, 10.0f);
    ImGui::Checkbox("Cast Shadows", &light.CastShadows);
}

void RenderingInspector::DrawPointLightProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& light = entity.GetComponent<PointLightComponent>();

    UI::DrawVec3Control("Position", light.Position, 0.1f);
    UI::DrawColorEdit3("Color", light.Color);
    UI::DrawFloatDrag("Intensity", light.Intensity, 0.1f, 0.0f, 100.0f);
    UI::DrawFloatDrag("Radius", light.Radius, 0.1f, 0.1f, 100.0f);
    ImGui::Checkbox("Cast Shadows", &light.CastShadows);
}

void RenderingInspector::DrawSpotLightProperties(Entity entity) {
    if (!ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& light = entity.GetComponent<SpotLightComponent>();

    UI::DrawVec3Control("Position", light.Position, 0.1f);
    UI::DrawVec3Control("Direction", light.Direction, 0.01f, -1.0f, 1.0f);
    UI::DrawColorEdit3("Color", light.Color);
    UI::DrawFloatDrag("Intensity", light.Intensity, 0.1f, 0.0f, 100.0f);
    UI::DrawFloatDrag("Range", light.Range, 0.1f, 0.1f, 100.0f);
    UI::DrawFloatDrag("Inner Angle", light.InnerCutoff, 1.0f, 0.0f, 90.0f);
    UI::DrawFloatDrag("Outer Angle", light.OuterCutoff, 1.0f, 0.0f, 90.0f);
    ImGui::Checkbox("Cast Shadows", &light.CastShadows);
}

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
