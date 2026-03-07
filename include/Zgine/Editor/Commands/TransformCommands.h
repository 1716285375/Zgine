#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/World/Core/Entity.h>
#include <string>

namespace Zgine {

/**
 * @brief Command to modify entity transform
 *
 * This command can be merged with consecutive transform changes
 * on the same entity for better undo/redo UX.
 */
class TransformEntityCommand : public IEditorCommand {
public:
    TransformEntityCommand(Entity entity,
                          const Math::Vector3& newTranslation,
                          const Math::Vector3& newRotation,
                          const Math::Vector3& newScale);
    ~TransformEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

    bool CanMergeWith(const IEditorCommand* other) const override;
    void MergeWith(const IEditorCommand* other) override;

private:
    Entity m_Entity;
    uint32_t m_EntityID;

    // Old values (for undo)
    Math::Vector3 m_OldTranslation;
    Math::Vector3 m_OldRotation;
    Math::Vector3 m_OldScale;

    // New values (for execute/redo)
    Math::Vector3 m_NewTranslation;
    Math::Vector3 m_NewRotation;
    Math::Vector3 m_NewScale;

    bool m_FirstExecution = true;
};

} // namespace Zgine
