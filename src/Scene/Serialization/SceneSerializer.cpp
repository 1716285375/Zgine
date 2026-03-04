#include <Zgine/Scene/Serialization/SceneSerializer.h>
#include <Zgine/Scene/Serialization/JsonSceneSerializer.h>
#include <Zgine/Scene/Serialization/ComponentSerializers/CoreSerializers.h>
#include <Zgine/Scene/Core/Scene.h>

namespace Zgine {

SceneSerializer::SceneSerializer(Scene* scene)
    : m_Scene(scene)
{
    // Create JSON serializer as default implementation
    auto jsonSerializer = std::make_unique<JsonSceneSerializer>();

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

SceneSerializer::~SceneSerializer() = default;

std::string SceneSerializer::Serialize() const {
    if (!m_Serializer || !m_Scene) {
        return "{}";
    }
    return m_Serializer->Serialize(m_Scene);
}

bool SceneSerializer::SerializeToFile(const std::string& filePath) const {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->SerializeToFile(m_Scene, filePath);
}

bool SceneSerializer::Deserialize(const std::string& jsonString) {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->Deserialize(jsonString, m_Scene);
}

bool SceneSerializer::DeserializeFromFile(const std::string& filePath) {
    if (!m_Serializer || !m_Scene) {
        return false;
    }
    return m_Serializer->DeserializeFromFile(filePath, m_Scene);
}

} // namespace Zgine
