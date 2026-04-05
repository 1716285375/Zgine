#pragma once

#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <nlohmann/json.hpp>
#include <string>

namespace Zgine {

/**
 * @brief Command to create a new entity
 */
class CreateEntityCommand : public IEditorCommand {
public:
    CreateEntityCommand(World* World, const std::string& name, PrimitiveType type = PrimitiveType::None);
    ~CreateEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

    Entity GetCreatedEntity() const { return m_CreatedEntity; }

private:
    World* m_World;
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
    DeleteEntityCommand(World* World, Entity entity);
    ~DeleteEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    std::string GetName() const override;

private:
    World* m_World;
    Entity m_Entity;
    uint32_t m_EntityID;
    std::string m_EntityName;
    nlohmann::json m_SerializedData;
};

} // namespace Zgine
