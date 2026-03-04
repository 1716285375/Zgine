#pragma once

#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Core/Entity.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <string>

namespace Zgine {

/**
 * @brief Command to create a new entity
 */
class CreateEntityCommand : public IEditorCommand {
public:
    CreateEntityCommand(Scene* scene, const std::string& name, PrimitiveType type = PrimitiveType::None);
    ~CreateEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

    Entity GetCreatedEntity() const { return m_CreatedEntity; }

private:
    Scene* m_Scene;
    std::string m_EntityName;
    PrimitiveType m_PrimitiveType;
    Entity m_CreatedEntity;
    uint32_t m_EntityID = 0; // For restoring after undo
};

/**
 * @brief Command to delete an entity
 */
class DeleteEntityCommand : public IEditorCommand {
public:
    DeleteEntityCommand(Scene* scene, Entity entity);
    ~DeleteEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

private:
    Scene* m_Scene;
    Entity m_Entity;
    uint32_t m_EntityID;
    std::string m_EntityName;
    // TODO: Store entity serialized data for full restoration
};

} // namespace Zgine
