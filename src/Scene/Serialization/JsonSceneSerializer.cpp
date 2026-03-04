#include <Zgine/Scene/Serialization/JsonSceneSerializer.h>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Core/Entity.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/OS/File.h>
#include <Zgine/Core/UUID/UUID.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

namespace Zgine {

JsonSceneSerializer::JsonSceneSerializer() {
    // Component serializers will be registered externally
}

std::string JsonSceneSerializer::Serialize(Scene* scene) const {
    if (!scene) {
        ZGINE_CORE_ERROR("JsonSceneSerializer: Scene is null");
        return "{}";
    }

    json sceneJson;
    sceneJson["Version"] = kSceneVersion;
    sceneJson["Scene"] = json::object();
    sceneJson["Entities"] = json::array();

    auto& registry = scene->GetRegistry();

    // Iterate through all entities with TagComponent
    auto view = registry.view<TagComponent>();
    for (auto entityHandle : view) {
        Entity entity(entityHandle, scene);
        json entityJson;

        // Always serialize ID
        if (entity.HasComponent<IDComponent>()) {
            auto& id = entity.GetComponent<IDComponent>();
            entityJson["UUID"] = id.ID.ToString();
        }

        // Always serialize Tag
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>();
            entityJson["Tag"] = tag.Tag;
        }

        // Use registered component serializers
        for (const auto& serializer : m_ComponentSerializers) {
            if (serializer->HasComponent(entity)) {
                serializer->Serialize(entity, entityJson);
            }
        }

        // Handle parent relationships
        if (entity.HasComponent<RelationshipComponent>()) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            if (rel.Parent != entt::null) {
                Entity parentEntity(rel.Parent, scene);
                if (parentEntity.HasComponent<IDComponent>()) {
                    entityJson["Parent"] = parentEntity.GetComponent<IDComponent>().ID.ToString();
                }
            }
        }

        sceneJson["Entities"].push_back(entityJson);
    }

    return sceneJson.dump(4); // 4-space indentation
}

bool JsonSceneSerializer::Deserialize(const std::string& data, Scene* scene) {
    if (!scene) {
        ZGINE_CORE_ERROR("JsonSceneSerializer: Scene is null");
        return false;
    }

    try {
        json sceneJson = json::parse(data);

        if (!sceneJson.contains("Entities")) {
            ZGINE_CORE_ERROR("JsonSceneSerializer: Invalid scene JSON format");
            return false;
        }

        uint32_t version = 0;
        if (sceneJson.contains("Version")) {
            version = sceneJson["Version"].get<uint32_t>();
        }
        if (version > kSceneVersion) {
            ZGINE_CORE_WARN("JsonSceneSerializer: Scene version {} is newer than supported {}",
                           version, kSceneVersion);
        }

        std::unordered_map<std::string, entt::entity> uuidMap;
        std::vector<std::pair<entt::entity, std::string>> parentLinks;

        // First pass: Create entities and deserialize components
        for (const auto& entityJson : sceneJson["Entities"]) {
            Entity entity = scene->CreateEntity();

            // Restore UUID
            if (entityJson.contains("UUID") && entityJson["UUID"].is_string()) {
                std::string uuid = entityJson["UUID"].get<std::string>();
                if (entity.HasComponent<IDComponent>()) {
                    entity.GetComponent<IDComponent>().ID = UUID::FromString(uuid);
                }
                uuidMap[uuid] = static_cast<entt::entity>(entity);
            }

            // Restore Tag
            if (entityJson.contains("Tag")) {
                auto& tag = entity.GetComponent<TagComponent>();
                tag.Tag = entityJson["Tag"].get<std::string>();
            }

            // Use registered component serializers
            for (const auto& serializer : m_ComponentSerializers) {
                std::string componentName = serializer->GetComponentTypeName();
                if (entityJson.contains(componentName)) {
                    serializer->Deserialize(entityJson[componentName], entity);
                }
            }

            // Store parent relationships for second pass
            if (entityJson.contains("Parent") && entityJson["Parent"].is_string()) {
                parentLinks.emplace_back(static_cast<entt::entity>(entity),
                                        entityJson["Parent"].get<std::string>());
            }
        }

        // Second pass: Rebuild hierarchy
        for (const auto& [child, parentUuid] : parentLinks) {
            auto parentIt = uuidMap.find(parentUuid);
            if (parentIt != uuidMap.end()) {
                Entity childEntity(child, scene);
                Entity parentEntity(parentIt->second, scene);
                scene->SetParent(childEntity, parentEntity);
            }
        }

        ZGINE_CORE_INFO("Scene deserialized successfully");
        return true;

    } catch (const json::exception& e) {
        ZGINE_CORE_ERROR("JsonSceneSerializer: JSON parse error: {}", e.what());
        return false;
    } catch (...) {
        ZGINE_CORE_ERROR("JsonSceneSerializer: Unknown error during deserialization");
        return false;
    }
}

bool JsonSceneSerializer::SerializeToFile(Scene* scene, const std::string& filePath) const {
    std::string jsonString = Serialize(scene);
    return File::WriteFile(filePath, jsonString);
}

bool JsonSceneSerializer::DeserializeFromFile(const std::string& filePath, Scene* scene) {
    std::string jsonString = File::ReadFile(filePath);
    if (jsonString.empty()) {
        ZGINE_CORE_ERROR("JsonSceneSerializer: Failed to read file: {}", filePath);
        return false;
    }
    return Deserialize(jsonString, scene);
}

void JsonSceneSerializer::RegisterComponentSerializer(std::unique_ptr<IComponentSerializer> serializer) {
    if (!serializer) return;

    std::string typeName = serializer->GetComponentTypeName();
    m_SerializerMap[typeName] = serializer.get();
    m_ComponentSerializers.push_back(std::move(serializer));

    ZGINE_CORE_TRACE("Registered component serializer: {}", typeName);
}

} // namespace Zgine
