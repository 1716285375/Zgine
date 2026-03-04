#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

// ============================================================================
// CreateEntityCommand
// ============================================================================

CreateEntityCommand::CreateEntityCommand(Scene* scene, const std::string& name, PrimitiveType type)
    : m_Scene(scene)
    , m_EntityName(name)
    , m_PrimitiveType(type)
    , m_EntityID(0)
{
}

bool CreateEntityCommand::Execute() {
    if (!m_Scene) {
        ZGINE_CORE_ERROR("CreateEntityCommand: Scene is null");
        return false;
    }

    // Create entity
    m_CreatedEntity = m_Scene->CreateEntity(m_EntityName);
    m_EntityID = (uint32_t)m_CreatedEntity;

    // Add components based on primitive type
    // Note: This is simplified. In a real implementation, you would call
    // the actual primitive creation logic from PrimitiveMesh or similar.
    if (m_PrimitiveType != PrimitiveType::None) {
        // Add mesh component, etc.
        // TODO: Implement proper primitive mesh setup
    }

    return true;
}

bool CreateEntityCommand::Undo() {
    if (!m_Scene) {
        return false;
    }

    // Destroy the entity we created
    if (m_CreatedEntity) {
        m_Scene->DestroyEntity(m_CreatedEntity);
        m_CreatedEntity = Entity(); // Invalidate
    }

    return true;
}

std::string CreateEntityCommand::GetName() const {
    return "Create " + m_EntityName;
}

// ============================================================================
// DeleteEntityCommand
// ============================================================================

DeleteEntityCommand::DeleteEntityCommand(Scene* scene, Entity entity)
    : m_Scene(scene)
    , m_Entity(entity)
    , m_EntityID((uint32_t)entity)
{
    // Store entity name for restoration
    if (entity && entity.HasComponent<TagComponent>()) {
        m_EntityName = entity.GetComponent<TagComponent>().Tag;
    } else {
        m_EntityName = "Entity";
    }
}

bool DeleteEntityCommand::Execute() {
    if (!m_Scene || !m_Entity) {
        return false;
    }

    // Store entity state before deletion
    // TODO: Implement full component serialization

    // Actually delete the entity
    m_Scene->DestroyEntity(m_Entity);
    m_Entity = Entity(); // Mark as deleted
    return true;
}

bool DeleteEntityCommand::Undo() {
    if (!m_Scene) {
        return false;
    }

    // Recreate entity with same name
    // TODO: Restore all components from serialized data
    m_Entity = m_Scene->CreateEntity(m_EntityName);
    m_EntityID = (uint32_t)m_Entity;

    return m_Entity.operator bool();
}

std::string DeleteEntityCommand::GetName() const {
    return "Delete " + m_EntityName;
}

} // namespace Zgine
