#include <Zgine/World/Serialization/WorldSerializer.h>
#include <Zgine/World/Serialization/JsonWorldSerializer.h>
#include <Zgine/World/Serialization/ComponentSerializers/CoreSerializers.h>
#include <Zgine/World/Core/World.h>

namespace Zgine {

WorldSerializer::WorldSerializer(World* World)
    : m_Scene(World)
{
    // Create JSON serializer as default implementation
    auto jsonSerializer = std::make_unique<JsonWorldSerializer>();

    // Register all component serializers
    // Core components
    jsonSerializer->RegisterComponentSerializer(std::make_unique<TransformSerializer>());
    jsonSerializer->RegisterComponentSerializer(std::make_unique<CameraSerializer>());
    jsonSerializer->RegisterComponentSerializer(std::make_unique<PrimitiveSerializer>());
    jsonSerializer->RegisterComponentSerializer(std::make_unique<PBRMaterialSerializer>());

    // Light components
    jsonSerializer->RegisterComponentSerializer(std::make_unique<LightSerializers::DirectionalLight>());
    jsonSerializer->RegisterComponentSerializer(std::make_unique<LightSerializers::PointLight>());
    jsonSerializer->RegisterComponentSerializer(std::make_unique<LightSerializers::SpotLight>());

    // TODO: Register other component serializers as needed:
    // - RigidBodySerializer
    // - BoxColliderSerializer
    // - AudioSourceSerializer
    // -  MeshSerializer
    // - ColorSerializer
    // - ScriptSerializer
    // etc.

    m_Serializer = std::move(jsonSerializer);
}

WorldSerializer::~WorldSerializer() = default;

std::string WorldSerializer::Serialize() const {
    if (!m_Serializer || !m_Scene) {
        return "{}";
    }
    return m_Serializer->Serialize(m_Scene);
}

bool WorldSerializer::SerializeToFile(const std::string& filePath) const {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->SerializeToFile(m_Scene, filePath);
}

bool WorldSerializer::Deserialize(const std::string& jsonString) {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->Deserialize(jsonString, m_Scene);
}

bool WorldSerializer::DeserializeFromFile(const std::string& filePath) {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->DeserializeFromFile(filePath, m_Scene);
}

} // namespace Zgine
