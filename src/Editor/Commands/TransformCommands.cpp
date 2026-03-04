#include <Zgine/Editor/Commands/TransformCommands.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

TransformEntityCommand::TransformEntityCommand(Entity entity,
                                               const Math::Vector3& newTranslation,
                                               const Math::Vector3& newRotation,
                                               const Math::Vector3& newScale)
    : m_Entity(entity)
    , m_EntityID((uint32_t)entity)
    , m_NewTranslation(newTranslation)
    , m_NewRotation(newRotation)
    , m_NewScale(newScale)
    , m_FirstExecution(true)
{
    // Store old values
    if (entity && entity.HasComponent<TransformComponent>()) {
        auto& tc = entity.GetComponent<TransformComponent>();
        m_OldTranslation = tc.Translation;
        m_OldRotation = tc.Rotation;
        m_OldScale = tc.Scale;
    }
}

bool TransformEntityCommand::Execute() {
    if (!m_Entity || !m_Entity.HasComponent<TransformComponent>()) {
        ZGINE_CORE_ERROR("TransformEntityCommand: Invalid entity");
        return false;
    }

    auto& tc = m_Entity.GetComponent<TransformComponent>();

    // On first execution, we already have old values stored in constructor
    // On redo, we just apply new values
    if (!m_FirstExecution) {
        // Nothing special needed
    }

    tc.Translation = m_NewTranslation;
    tc.Rotation = m_NewRotation;
    tc.Scale = m_NewScale;

    m_FirstExecution = false;
    return true;
}

bool TransformEntityCommand::Undo() {
    if (!m_Entity || !m_Entity.HasComponent<TransformComponent>()) {
        ZGINE_CORE_ERROR("TransformEntityCommand::Undo: Invalid entity");
        return false;
    }

    auto& tc = m_Entity.GetComponent<TransformComponent>();
    tc.Translation = m_OldTranslation;
    tc.Rotation = m_OldRotation;
    tc.Scale = m_OldScale;

    return true;
}

std::string TransformEntityCommand::GetName() const {
    return "Transform Entity";
}

bool TransformEntityCommand::CanMergeWith(const IEditorCommand* other) const {
    // Only merge with other TransformEntityCommand on the same entity
    auto* otherTransform = dynamic_cast<const TransformEntityCommand*>(other);
    if (!otherTransform) {
        return false;
    }

    return m_EntityID == otherTransform->m_EntityID;
}

void TransformEntityCommand::MergeWith(const IEditorCommand* other) {
    auto* otherTransform = static_cast<const TransformEntityCommand*>(other);

    // Keep our old values (for undo), but update new values
    m_NewTranslation = otherTransform->m_NewTranslation;
    m_NewRotation = otherTransform->m_NewRotation;
    m_NewScale = otherTransform->m_NewScale;
}

} // namespace Zgine
