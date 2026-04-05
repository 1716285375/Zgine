#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Serialization/ComponentSerializers/CoreSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/PhysicsSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/AudioSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/RenderingSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/ScriptSerializers.h>
#include <Zgine/Core/Log/Log.h>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// CreateEntityCommand
// ============================================================================

CreateEntityCommand::CreateEntityCommand(World* World, const std::string& name, PrimitiveType type)
    : m_World(World)
    , m_EntityName(name)
    , m_PrimitiveType(type)
    , m_EntityID(0)
{
}

bool CreateEntityCommand::Execute() {
    if (!m_World) {
        ZGINE_CORE_ERROR("CreateEntityCommand: World is null");
        return false;
    }

    // Create entity
    m_CreatedEntity = m_World->CreateEntity(m_EntityName);
    m_EntityID = (uint32_t)m_CreatedEntity;

    // Add components based on primitive type
    if (m_PrimitiveType != PrimitiveType::None) {
        m_CreatedEntity.AddComponent<PrimitiveComponent>(m_PrimitiveType);
        m_CreatedEntity.AddComponent<PBRMaterialComponent>();
    }

    return true;
}

bool CreateEntityCommand::Undo() {
    if (!m_World) {
        return false;
    }

    // Destroy the entity we created
    if (m_CreatedEntity) {
        m_World->DestroyEntity(m_CreatedEntity);
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

DeleteEntityCommand::DeleteEntityCommand(World* World, Entity entity)
    : m_World(World)
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
    if (!m_World || !m_Entity) {
        return false;
    }

    // Serialize all components before deletion
    m_SerializedData = json::object();

    // Build serializer list
    std::vector<std::unique_ptr<IComponentSerializer>> serializers;
    serializers.push_back(std::make_unique<TransformSerializer>());
    serializers.push_back(std::make_unique<CameraSerializer>());
    serializers.push_back(std::make_unique<PrimitiveSerializer>());
    serializers.push_back(std::make_unique<PBRMaterialSerializer>());
    serializers.push_back(std::make_unique<LightSerializers::DirectionalLight>());
    serializers.push_back(std::make_unique<LightSerializers::PointLight>());
    serializers.push_back(std::make_unique<LightSerializers::SpotLight>());
    serializers.push_back(std::make_unique<RigidbodySerializer>());
    serializers.push_back(std::make_unique<BoxColliderSerializer>());
    serializers.push_back(std::make_unique<CircleColliderSerializer>());
    serializers.push_back(std::make_unique<AudioSourceSerializer>());
    serializers.push_back(std::make_unique<AudioListenerSerializer>());
    serializers.push_back(std::make_unique<ColorSerializer>());
    serializers.push_back(std::make_unique<SpriteRendererSerializer>());
    serializers.push_back(std::make_unique<MeshSerializer>());
    serializers.push_back(std::make_unique<ScriptSerializer>());

    for (const auto& serializer : serializers) {
        if (serializer->HasComponent(m_Entity)) {
            serializer->Serialize(m_Entity, m_SerializedData);
        }
    }

    // Actually delete the entity
    m_World->DestroyEntity(m_Entity);
    m_Entity = Entity(); // Mark as deleted
    return true;
}

bool DeleteEntityCommand::Undo() {
    if (!m_World) {
        return false;
    }

    // Recreate entity with same name
    m_Entity = m_World->CreateEntity(m_EntityName);
    m_EntityID = (uint32_t)m_Entity;

    // Restore all components from serialized data
    std::vector<std::unique_ptr<IComponentSerializer>> serializers;
    serializers.push_back(std::make_unique<TransformSerializer>());
    serializers.push_back(std::make_unique<CameraSerializer>());
    serializers.push_back(std::make_unique<PrimitiveSerializer>());
    serializers.push_back(std::make_unique<PBRMaterialSerializer>());
    serializers.push_back(std::make_unique<LightSerializers::DirectionalLight>());
    serializers.push_back(std::make_unique<LightSerializers::PointLight>());
    serializers.push_back(std::make_unique<LightSerializers::SpotLight>());
    serializers.push_back(std::make_unique<RigidbodySerializer>());
    serializers.push_back(std::make_unique<BoxColliderSerializer>());
    serializers.push_back(std::make_unique<CircleColliderSerializer>());
    serializers.push_back(std::make_unique<AudioSourceSerializer>());
    serializers.push_back(std::make_unique<AudioListenerSerializer>());
    serializers.push_back(std::make_unique<ColorSerializer>());
    serializers.push_back(std::make_unique<SpriteRendererSerializer>());
    serializers.push_back(std::make_unique<MeshSerializer>());
    serializers.push_back(std::make_unique<ScriptSerializer>());

    for (const auto& serializer : serializers) {
        std::string typeName(serializer->GetComponentTypeName());
        if (m_SerializedData.contains(typeName)) {
            (void)serializer->Deserialize(m_SerializedData[typeName], m_Entity);
        }
    }

    return m_Entity.operator bool();
}

std::string DeleteEntityCommand::GetName() const {
    return "Delete " + m_EntityName;
}

} // namespace Zgine
