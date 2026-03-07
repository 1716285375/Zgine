#include <Zgine/Editor/Gizmo/GizmoController.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/Editor/Commands/TransformCommands.h>
#include <Zgine/Editor/Events/EntityEvents.h>
#include <Zgine/Editor/Events/TransformEvents.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <imgui.h>
#include <ImGuizmo.h>


namespace Zgine {

GizmoController::GizmoController(EditorContext& context)
    : m_Context(context)
    , m_Operation(GizmoOperation::Translate)
    , m_Space(GizmoSpace::Local)
    , m_Active(false)
    , m_WasActive(false)
{
}

bool GizmoController::Render(Entity entity,
                              const Math::Matrix4& view,
                              const Math::Matrix4& projection,
                              const Math::Vector2& boundsMin,
                              const Math::Vector2& boundsMax) {
    if (!entity || !entity.HasComponent<TransformComponent>()) {
        m_Active = false;
        m_WasActive = false;
        m_ManipulatedEntity = {};
        return false;
    }

    auto& transform = entity.GetComponent<TransformComponent>();

    // Store initial transform when manipulation begins
    if (!m_WasActive && ImGuizmo::IsOver()) {
        m_StartTranslation = transform.Translation;
        m_StartRotation = transform.Rotation;
        m_StartScale = transform.Scale;
    }

    Math::Matrix4 model = transform.GetTransform();

    // Setup ImGuizmo
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(boundsMin.x, boundsMin.y,
                      boundsMax.x - boundsMin.x,
                      boundsMax.y - boundsMin.y);

    // Manipulate (ImGuizmo requires non-const matrix pointers)
    Math::Matrix4 viewCopy = view;
    Math::Matrix4 projectionCopy = projection;
    Math::Matrix4 modelCopy = model;
    ImGuizmo::Manipulate(Math::ValuePtr(viewCopy), Math::ValuePtr(projectionCopy),
                         static_cast<ImGuizmo::OPERATION>(static_cast<int>(m_Operation)),
                         static_cast<ImGuizmo::MODE>(static_cast<int>(m_Space)),
                         Math::ValuePtr(modelCopy));

    // Copy back if modified
    if (ImGuizmo::IsUsing()) {
        model = modelCopy;
    }

    m_Active = ImGuizmo::IsUsing();

    // Detect start of manipulation
    if (m_Active && !m_WasActive) {
        BeginManipulation(entity, m_StartTranslation, m_StartRotation, m_StartScale);
        m_ManipulatedEntity = entity;
    }

    bool changed = false;
    if (m_Active) {
        // Decompose the modified matrix
        Math::Vector3 translation;
        Math::Vector3 rotation;
        Math::Vector3 scale;
        ImGuizmo::DecomposeMatrixToComponents(Math::ValuePtr(model),
                                              &translation.x,
                                              &rotation.x,
                                              &scale.x);

        // Update transform component
        if (transform.Translation != translation ||
            transform.Rotation != rotation ||
            transform.Scale != scale) {

            transform.Translation = translation;
            transform.Rotation = rotation;
            transform.Scale = scale;
            changed = true;

            // Publish real-time transform changed event (for visual feedback)
            TransformChangedEvent event(entity);
            m_Context.GetEventBus().PublishImmediate(event);
        }
    }

    // Detect end of manipulation
    if (!m_Active && m_WasActive && m_ManipulatedEntity) {
        EndManipulation(m_ManipulatedEntity,
                       transform.Translation,
                       transform.Rotation,
                       transform.Scale);
    }

    m_WasActive = m_Active;
    return changed;
}

bool GizmoController::IsUsing() const {
    return ImGuizmo::IsUsing();
}

void GizmoController::BeginManipulation(Entity entity,
                                       const Math::Vector3& startTranslation,
                                       const Math::Vector3& startRotation,
                                       const Math::Vector3& startScale) {
    // Store start values for command creation
    m_StartTranslation = startTranslation;
    m_StartRotation = startRotation;
    m_StartScale = startScale;

    ZGINE_CORE_TRACE("Gizmo: Begin manipulation on entity {}", static_cast<uint32_t>(static_cast<entt::entity>(entity)));
}

void GizmoController::EndManipulation(Entity entity,
                                     const Math::Vector3& endTranslation,
                                     const Math::Vector3& endRotation,
                                     const Math::Vector3& endScale) {
    // Check if transform actually changed
    bool changed = (m_StartTranslation != endTranslation ||
                   m_StartRotation != endRotation ||
                   m_StartScale != endScale);

    if (!changed) {
        ZGINE_CORE_TRACE("Gizmo: Manipulation ended with no changes");
        return;
    }

    ZGINE_CORE_TRACE("Gizmo: End manipulation, creating command");

    // Create and execute transform command for undo/redo
    auto command = std::make_unique<TransformEntityCommand>(
        entity,
        endTranslation,
        endRotation,
        endScale
    );

    // Command history will handle execution and merging
    auto& commandHistory = m_Context.GetCommandHistory();
    commandHistory.Execute(std::move(command));

    // Reset manipulation state
    m_ManipulatedEntity = {};
}

} // namespace Zgine
