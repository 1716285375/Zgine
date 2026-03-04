#pragma once

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <Zgine/Scene/Core/Entity.h>
#include <memory>

namespace Zgine {

class EditorContext;
class IEditorCommand;

/**
 * @brief Gizmo operation types for transformation manipulation
 *
 * Maps directly to ImGuizmo operation modes for consistency.
 */
enum class GizmoOperation : int {
    Translate = 0,  // ImGuizmo::TRANSLATE
    Rotate = 1,     // ImGuizmo::ROTATE
    Scale = 2       // ImGuizmo::SCALE
};

/**
 * @brief Gizmo coordinate space modes (Local vs World)
 */
enum class GizmoSpace : int {
    Local = 0,      // ImGuizmo::LOCAL
    World = 1       // ImGuizmo::WORLD
};

/**
 * @brief Controller for 3D transform gizmo manipulation
 *
 * Encapsulates ImGuizmo functionality with proper integration into
 * the editor's EventBus and Command system for undo/redo support.
 *
 * **Architecture**:
 * - Uses EditorContext to access EventBus and CommandHistory
 * - Publishes TransformChangedEvent when manipulation starts/ends
 * - Creates TransformEntityCommand for undo/redo support
 * - Merges consecutive transform changes on same entity
 *
 * **Usage**:
 * @code
 * GizmoController gizmo(editorContext);
 * gizmo.SetOperation(GizmoOperation::Translate);
 * if (selectedEntity) {
 *     gizmo.Render(selectedEntity, view, projection, boundsMin, boundsMax);
 * }
 * @endcode
 *
 * **Features**:
 * - Automatic command creation and execution
 * - Command merging for smooth undo/redo
 * - Event publishing for UI synchronization
 * - Proper state management
 */
class GizmoController {
public:
    /**
     * @brief Construct gizmo controller with editor context
     * @param context Reference to editor context for services
     */
    explicit GizmoController(EditorContext& context);
    ~GizmoController() = default;

    ZGINE_NON_COPYABLE_NON_MOVABLE(GizmoController)

    /**
     * @brief Render the gizmo for the selected entity
     *
     * Handles ImGuizmo rendering and creates transform commands when needed.
     * Commands are automatically executed and added to the command history.
     *
     * @param entity The entity to manipulate
     * @param view View matrix from camera
     * @param projection Projection matrix from camera
     * @param boundsMin Top-left viewport bounds
     * @param boundsMax Bottom-right viewport bounds
     * @return true if transform was modified this frame
     */
    bool Render(Entity entity,
                const Math::Matrix4& view,
                const Math::Matrix4& projection,
                const Math::Vector2& boundsMin,
                const Math::Vector2& boundsMax);

    // ========================================================================
    // Gizmo Configuration
    // ========================================================================

    /** Set current manipulation operation */
    void SetOperation(GizmoOperation op) { m_Operation = op; }

    /** Get current manipulation operation */
    GizmoOperation GetOperation() const { return m_Operation; }

    /** Set coordinate space mode */
    void SetSpace(GizmoSpace space) { m_Space = space; }

    /** Get coordinate space mode */
    GizmoSpace GetSpace() const { return m_Space; }

    // ========================================================================
    // State Queries
    // ========================================================================

    /** Check if gizmo is currently being manipulated */
    bool IsActive() const { return m_Active; }

    /** Check if gizmo is currently in use (direct ImGuizmo query) */
    bool IsUsing() const;

    /** Get the entity currently being manipulated (null if none) */
    Entity GetManipulatedEntity() const { return m_ManipulatedEntity; }

private:
    void BeginManipulation(Entity entity, const Math::Vector3& startTranslation,
                          const Math::Vector3& startRotation, const Math::Vector3& startScale);
    void EndManipulation(Entity entity, const Math::Vector3& endTranslation,
                        const Math::Vector3& endRotation, const Math::Vector3& endScale);

private:
    EditorContext& m_Context;

    // Gizmo state
    GizmoOperation m_Operation = GizmoOperation::Translate;
    GizmoSpace m_Space = GizmoSpace::Local;
    bool m_Active = false;
    bool m_WasActive = false;  // Track previous frame state

    // Manipulation tracking (for command creation)
    Entity m_ManipulatedEntity;
    Math::Vector3 m_StartTranslation;
    Math::Vector3 m_StartRotation;
    Math::Vector3 m_StartScale;
};

} // namespace Zgine
