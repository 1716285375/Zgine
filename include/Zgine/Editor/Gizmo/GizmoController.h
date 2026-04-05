#pragma once

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <Zgine/World/Core/Entity.h>
#include <memory>

namespace Zgine {

class EditorContext;
class IEditorCommand;

enum class GizmoOperation : int {
    Translate = 0,
    Rotate = 1,
    Scale = 2
};

enum class GizmoSpace : int {
    Local = 0,
    World = 1
};

class GizmoController {
public:
    explicit GizmoController(EditorContext& context);
    ~GizmoController() = default;

    ZGINE_NON_COPYABLE_NON_MOVABLE(GizmoController)

    bool Render(Entity entity,
                const Math::Matrix4& view,
                const Math::Matrix4& projection,
                const Math::Vector2& boundsMin,
                const Math::Vector2& boundsMax);

    void SetOperation(GizmoOperation op) { m_Operation = op; }
    GizmoOperation GetOperation() const { return m_Operation; }

    void SetSpace(GizmoSpace space) { m_Space = space; }
    GizmoSpace GetSpace() const { return m_Space; }

    void SetSnapEnabled(bool enabled) { m_SnapEnabled = enabled; }
    bool IsSnapEnabled() const { return m_SnapEnabled; }

    void SetTranslateSnap(float snap) { m_TranslateSnap = snap; }
    void SetRotateSnap(float snap) { m_RotateSnap = snap; }
    void SetScaleSnap(float snap) { m_ScaleSnap = snap; }

    bool IsActive() const { return m_Active; }
    bool IsUsing() const;
    Entity GetManipulatedEntity() const { return m_ManipulatedEntity; }

private:
    void CommitManipulation(Entity entity);

private:
    EditorContext& m_Context;

    GizmoOperation m_Operation = GizmoOperation::Translate;
    GizmoSpace m_Space = GizmoSpace::Local;
    bool m_Active = false;
    bool m_WasActive = false;

    // Snap
    bool m_SnapEnabled = false;
    float m_TranslateSnap = 0.5f;
    float m_RotateSnap = 15.0f;
    float m_ScaleSnap = 0.1f;

    // Manipulation tracking
    Entity m_ManipulatedEntity;
    Math::Vector3 m_StartTranslation;
    Math::Vector3 m_StartRotation;
    Math::Vector3 m_StartScale;
};

} // namespace Zgine
