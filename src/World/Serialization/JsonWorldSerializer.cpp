#include <Zgine/World/Serialization/JsonWorldSerializer.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Platform/IO/File.h>
#include <Zgine/Core/UUID/UUID.h>
#include <World/Core/WorldRegistryAccess.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

namespace Zgine {

JsonWorldSerializer::JsonWorldSerializer() {
    // Component serializers will be registered externally
}

std::string JsonWorldSerializer::Serialize(World* World) const {
    if (!World) {
        ZGINE_CORE_ERROR("JsonWorldSerializer: World is null");
        return "{}";
    }

    json sceneJson;
    sceneJson["Version"] = kSceneVersion;
    sceneJson["World"] = json::object();
    sceneJson["Entities"] = json::array();

    auto& registry = Internal::GetRegistry(*World);

    // Iterate through all entities with TagComponent
    auto view = registry.view<TagComponent>();
    for (auto entityHandle : view) {
        Entity entity(Internal::FromEnTT(entityHandle), World);
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
            if (rel.Parent) {
                Entity parentEntity(rel.Parent, World);
                if (parentEntity.HasComponent<IDComponent>()) {
                    entityJson["Parent"] = parentEntity.GetComponent<IDComponent>().ID.ToString();
                }
            }
        }

        sceneJson["Entities"].push_back(entityJson);
    }

    return sceneJson.dump(4); // 4-space indentation
}

bool JsonWorldSerializer::Deserialize(std::string_view data, World* World) {
    if (!World) {
        ZGINE_CORE_ERROR("JsonWorldSerializer: World is null");
        return false;
    }

    try {
        json sceneJson = json::parse(data);

        if (!sceneJson.contains("Entities")) {
            ZGINE_CORE_ERROR("JsonWorldSerializer: Invalid World JSON format");
            return false;
        }

        uint32_t version = 0;
        if (sceneJson.contains("Version")) {
            version = sceneJson["Version"].get<uint32_t>();
        }
        if (version > kSceneVersion) {
            ZGINE_CORE_WARN("JsonWorldSerializer: World version {} is newer than supported {}",
                           version, kSceneVersion);
        }

        std::unordered_map<std::string, EntityHandle> uuidMap;
        std::vector<std::pair<EntityHandle, std::string>> parentLinks;

        // First pass: Create entities and deserialize components
        for (const auto& entityJson : sceneJson["Entities"]) {
            Entity entity = World->CreateEntity();

            // Restore UUID
            if (entityJson.contains("UUID") && entityJson["UUID"].is_string()) {
                std::string uuid = entityJson["UUID"].get<std::string>();
                if (entity.HasComponent<IDComponent>()) {
                    entity.GetComponent<IDComponent>().ID = UUID::FromString(uuid);
                }
                uuidMap[uuid] = entity.GetHandle();
            }

            // Restore Tag
            if (entityJson.contains("Tag")) {
                auto& tag = entity.GetComponent<TagComponent>();
                tag.Tag = entityJson["Tag"].get<std::string>();
            }

            // Use registered component serializers
            for (const auto& serializer : m_ComponentSerializers) {
                std::string_view componentName = serializer->GetComponentTypeName();
                if (entityJson.contains(std::string(componentName))) {
                    serializer->Deserialize(entityJson[std::string(componentName)], entity);
                }
            }

            // Store parent relationships for second pass
            if (entityJson.contains("Parent") && entityJson["Parent"].is_string()) {
                parentLinks.emplace_back(entity.GetHandle(),
                                        entityJson["Parent"].get<std::string>());
            }
        }

        // Second pass: Rebuild hierarchy
        for (const auto& [child, parentUuid] : parentLinks) {
            auto parentIt = uuidMap.find(parentUuid);
            if (parentIt != uuidMap.end()) {
                Entity childEntity(child, World);
                Entity parentEntity(parentIt->second, World);
                World->SetParent(childEntity, parentEntity);
            }
        }

        ZGINE_CORE_INFO("World deserialized successfully");
        return true;

    } catch (const json::exception& e) {
        ZGINE_CORE_ERROR("JsonWorldSerializer: JSON parse error: {}", e.what());
        return false;
    } catch (...) {
        ZGINE_CORE_ERROR("JsonWorldSerializer: Unknown error during deserialization");
        return false;
    }
}

bool JsonWorldSerializer::SerializeToFile(World* World, std::string_view filePath) const {
    std::string jsonString = Serialize(World);
    return File::WriteFile(std::string(filePath), jsonString);
}

bool JsonWorldSerializer::DeserializeFromFile(std::string_view filePath, World* World) {
    std::string jsonString = File::ReadFile(std::string(filePath));
    if (jsonString.empty()) {
        ZGINE_CORE_ERROR("JsonWorldSerializer: Failed to read file: {}", filePath);
        return false;
    }
    return Deserialize(jsonString, World);
}

void JsonWorldSerializer::RegisterComponentSerializer(std::unique_ptr<IComponentSerializer> serializer) {
    if (!serializer) return;

    std::string_view typeName = serializer->GetComponentTypeName();
    m_SerializerMap[std::string(typeName)] = serializer.get();
    m_ComponentSerializers.push_back(std::move(serializer));

    ZGINE_CORE_TRACE("Registered component serializer: {}", typeName);
}

} // namespace Zgine
