#include <Zgine/Editor/Gizmo/GizmoController.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/Editor/Commands/TransformCommands.h>
#include <Zgine/Editor/Events/TransformEvents.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <imgui.h>
#include <ImGuizmo.h>

namespace Zgine {

GizmoController::GizmoController(EditorContext& context)
    : m_Context(context)
{
}

bool GizmoController::Render(Entity entity,
                              const Math::Matrix4& view,
                              const Math::Matrix4& projection,
                              const Math::Vector2& boundsMin,
                              const Math::Vector2& boundsMax) {
    if (!entity || !entity.HasComponent<TransformComponent>()) {
        if (m_WasActive && m_ManipulatedEntity) {
            CommitManipulation(m_ManipulatedEntity);
        }
        m_Active = false;
        m_WasActive = false;
        m_ManipulatedEntity = {};
        return false;
    }

    auto& tc = entity.GetComponent<TransformComponent>();

    // ---- Setup ImGuizmo viewport ----
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    float rectX = boundsMin.x;
    float rectY = boundsMin.y;
    float rectW = boundsMax.x - boundsMin.x;
    float rectH = boundsMax.y - boundsMin.y;
    ImGuizmo::SetRect(rectX, rectY, rectW, rectH);

    // ---- Build model matrix via ImGuizmo for round-trip consistency ----
    // ImGuizmo uses degrees for rotation, matching our TransformComponent
    float modelMatrix[16];
    float translationArr[3] = { tc.Translation.x, tc.Translation.y, tc.Translation.z };
    float rotationArr[3]    = { tc.Rotation.x,    tc.Rotation.y,    tc.Rotation.z };
    float scaleArr[3]       = { tc.Scale.x,        tc.Scale.y,       tc.Scale.z };
    ImGuizmo::RecomposeMatrixFromComponents(translationArr, rotationArr, scaleArr, modelMatrix);

    // ---- Map operation / mode ----
    ImGuizmo::OPERATION imOp = ImGuizmo::TRANSLATE;
    switch (m_Operation) {
        case GizmoOperation::Translate: imOp = ImGuizmo::TRANSLATE; break;
        case GizmoOperation::Rotate:    imOp = ImGuizmo::ROTATE;    break;
        case GizmoOperation::Scale:     imOp = ImGuizmo::SCALE;     break;
    }

    ImGuizmo::MODE imMode = (m_Space == GizmoSpace::World) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
    // Scale always uses LOCAL space (world-space scale is meaningless for non-uniform)
    if (m_Operation == GizmoOperation::Scale) {
        imMode = ImGuizmo::LOCAL;
    }

    // ---- Snap ----
    float snapValues[3] = { 0.0f, 0.0f, 0.0f };
    float* snapPtr = nullptr;

    bool useSnap = m_SnapEnabled || ImGui::GetIO().KeyCtrl;
    if (useSnap) {
        switch (m_Operation) {
            case GizmoOperation::Translate:
                snapValues[0] = snapValues[1] = snapValues[2] = m_TranslateSnap;
                break;
            case GizmoOperation::Rotate:
                snapValues[0] = snapValues[1] = snapValues[2] = m_RotateSnap;
                break;
            case GizmoOperation::Scale:
                snapValues[0] = snapValues[1] = snapValues[2] = m_ScaleSnap;
                break;
        }
        snapPtr = snapValues;
    }

    // ---- Manipulate ----
    float viewArr[16];
    float projArr[16];
    memcpy(viewArr, Math::ValuePtr(view), sizeof(float) * 16);
    memcpy(projArr, Math::ValuePtr(projection), sizeof(float) * 16);

    ImGuizmo::Manipulate(viewArr, projArr, imOp, imMode, modelMatrix, nullptr, snapPtr);

    // ---- Process result ----
    m_Active = ImGuizmo::IsUsing();

    // Detect start of manipulation — snapshot initial values
    if (m_Active && !m_WasActive) {
        m_StartTranslation = tc.Translation;
        m_StartRotation = tc.Rotation;
        m_StartScale = tc.Scale;
        m_ManipulatedEntity = entity;
    }

    bool changed = false;
    if (m_Active) {
        // Decompose the modified matrix back to components
        float newTranslation[3], newRotation[3], newScale[3];
        ImGuizmo::DecomposeMatrixToComponents(modelMatrix, newTranslation, newRotation, newScale);

        Math::Vector3 t(newTranslation[0], newTranslation[1], newTranslation[2]);
        Math::Vector3 r(newRotation[0],    newRotation[1],    newRotation[2]);
        Math::Vector3 s(newScale[0],       newScale[1],       newScale[2]);

        if (tc.Translation != t || tc.Rotation != r || tc.Scale != s) {
            tc.Translation = t;
            tc.Rotation = r;
            tc.Scale = s;
            changed = true;

            TransformChangedEvent event(entity);
            m_Context.GetEventBus().PublishImmediate(event);
        }
    }

    // Detect end of manipulation — commit command for undo/redo
    if (!m_Active && m_WasActive && m_ManipulatedEntity) {
        CommitManipulation(entity);
    }

    m_WasActive = m_Active;
    return changed;
}

bool GizmoController::IsUsing() const {
    return ImGuizmo::IsUsing();
}

void GizmoController::CommitManipulation(Entity entity) {
    if (!entity || !entity.HasComponent<TransformComponent>()) {
        m_ManipulatedEntity = {};
        return;
    }

    auto& tc = entity.GetComponent<TransformComponent>();

    bool changed = (m_StartTranslation != tc.Translation ||
                    m_StartRotation != tc.Rotation ||
                    m_StartScale != tc.Scale);

    if (changed) {
        // Save the final (current) values
        Math::Vector3 finalT = tc.Translation;
        Math::Vector3 finalR = tc.Rotation;
        Math::Vector3 finalS = tc.Scale;

        // Temporarily restore start values so the command constructor
        // captures them as the "old" state
        tc.Translation = m_StartTranslation;
        tc.Rotation = m_StartRotation;
        tc.Scale = m_StartScale;

        // Create command — constructor reads old from entity, new from args
        auto cmd = std::make_unique<TransformEntityCommand>(
            entity, finalT, finalR, finalS);

        // Restore final values before Execute overwrites them (same values, but clean)
        tc.Translation = finalT;
        tc.Rotation = finalR;
        tc.Scale = finalS;

        // Execute through history — command.Execute() sets entity to final values (no-op)
        auto& commandHistory = m_Context.GetCommandHistory();
        commandHistory.Execute(std::move(cmd));
    }

    m_ManipulatedEntity = {};
}

} // namespace Zgine
