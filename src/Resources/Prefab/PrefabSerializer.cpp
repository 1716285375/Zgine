#include <Zgine/Resources/Prefab/PrefabSerializer.h>

#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Serialization/ComponentSerializers/AudioSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/CoreSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/PhysicsSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/RenderingSerializers.h>
#include <Zgine/World/Serialization/ComponentSerializers/ScriptSerializers.h>
#include <Zgine/World/Serialization/IComponentSerializer.h>
#include <Zgine/World/Serialization/WorldSerializer.h>

#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Zgine {

namespace {

std::vector<std::unique_ptr<IComponentSerializer>> CreateComponentSerializers() {
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
    return serializers;
}

void CollectHierarchy(World& world, Entity entity, std::vector<Entity>& outEntities) {
    if (!entity) {
        return;
    }

    outEntities.push_back(entity);
    for (Entity child : world.GetChildren(entity)) {
        CollectHierarchy(world, child, outEntities);
    }
}

std::string GetEntityUUID(Entity entity) {
    if (!entity || !entity.HasComponent<IDComponent>()) {
        return {};
    }
    return entity.GetComponent<IDComponent>().ID.ToString();
}

Entity FindEntityByUUID(World& world, const std::string& uuid) {
    for (Entity entity : world.GetAllEntities()) {
        if (entity.HasComponent<IDComponent>() &&
            entity.GetComponent<IDComponent>().ID.ToString() == uuid) {
            return entity;
        }
    }
    return {};
}

} // namespace

std::optional<PrefabAsset> PrefabSerializer::CreateFromEntityHierarchy(Entity root) {
    if (!root || !root.IsValid() || !root.HasComponent<IDComponent>()) {
        return std::nullopt;
    }

    World* sourceWorld = root.GetWorld();
    if (!sourceWorld) {
        return std::nullopt;
    }

    std::vector<Entity> entities;
    CollectHierarchy(*sourceWorld, root, entities);

    const auto serializers = CreateComponentSerializers();
    const std::string rootUUID = GetEntityUUID(root);

    PrefabAsset prefab;
    prefab.Handle = AssetHandle::New();
    prefab.TemplateRootUUID = rootUUID;
    prefab.Name = root.HasComponent<TagComponent>() ? root.GetComponent<TagComponent>().Tag : "Prefab";
    prefab.Entities = nlohmann::json::array();

    std::unordered_map<std::string, bool> included;
    for (Entity entity : entities) {
        included[GetEntityUUID(entity)] = true;
    }

    for (Entity entity : entities) {
        if (!entity.HasComponent<IDComponent>()) {
            continue;
        }

        nlohmann::json entityJson;
        entityJson["UUID"] = GetEntityUUID(entity);

        if (entity.HasComponent<TagComponent>()) {
            entityJson["Tag"] = entity.GetComponent<TagComponent>().Tag;
        }

        for (const auto& serializer : serializers) {
            if (serializer->HasComponent(entity)) {
                serializer->Serialize(entity, entityJson);
            }
        }

        if (entity != root && entity.HasComponent<RelationshipComponent>()) {
            const auto& relationship = entity.GetComponent<RelationshipComponent>();
            if (relationship.Parent) {
                Entity parent(relationship.Parent, sourceWorld);
                const std::string parentUUID = GetEntityUUID(parent);
                if (!parentUUID.empty() && included.contains(parentUUID)) {
                    entityJson["Parent"] = parentUUID;
                }
            }
        }

        prefab.Entities.push_back(std::move(entityJson));
    }

    return prefab.IsValid() ? std::optional<PrefabAsset>{std::move(prefab)} : std::nullopt;
}

Entity PrefabSerializer::Instantiate(const PrefabAsset& prefab, World& targetWorld) {
    if (!prefab.IsValid()) {
        return {};
    }

    nlohmann::json entities = prefab.Entities;
    std::unordered_map<std::string, std::string> uuidRemap;

    for (auto& entityJson : entities) {
        if (!entityJson.contains("UUID") || !entityJson["UUID"].is_string()) {
            continue;
        }

        const std::string oldUUID = entityJson["UUID"].get<std::string>();
        const std::string newUUID = UUID::New().ToString();
        uuidRemap[oldUUID] = newUUID;
        entityJson["UUID"] = newUUID;
    }

    for (auto& entityJson : entities) {
        if (!entityJson.contains("Parent") || !entityJson["Parent"].is_string()) {
            continue;
        }

        const std::string oldParentUUID = entityJson["Parent"].get<std::string>();
        auto it = uuidRemap.find(oldParentUUID);
        if (it != uuidRemap.end()) {
            entityJson["Parent"] = it->second;
        } else {
            entityJson.erase("Parent");
        }
    }

    nlohmann::json worldJson;
    worldJson["Version"] = 1;
    worldJson["World"] = nlohmann::json::object();
    worldJson["Entities"] = entities;

    WorldSerializer serializer(&targetWorld);
    if (!serializer.Deserialize(worldJson.dump())) {
        return {};
    }

    auto rootIt = uuidRemap.find(prefab.TemplateRootUUID);
    if (rootIt == uuidRemap.end()) {
        return {};
    }

    return FindEntityByUUID(targetWorld, rootIt->second);
}

std::string PrefabSerializer::SerializeToString(const PrefabAsset& prefab) {
    return prefab.Serialize().dump(4);
}

std::optional<PrefabAsset> PrefabSerializer::DeserializeFromString(std::string_view data) {
    try {
        return PrefabAsset::Deserialize(nlohmann::json::parse(data));
    } catch (...) {
        return std::nullopt;
    }
}

bool PrefabSerializer::SaveToFile(const PrefabAsset& prefab, const std::filesystem::path& path) {
    if (!prefab.IsValid()) {
        return false;
    }

    std::error_code ec;
    const std::filesystem::path parent = path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent, ec);
        if (ec) {
            return false;
        }
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        return false;
    }

    const std::string data = SerializeToString(prefab);
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return static_cast<bool>(out);
}

std::optional<PrefabAsset> PrefabSerializer::LoadFromFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return std::nullopt;
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();
    const std::string data = buffer.str();
    if (data.empty()) {
        return std::nullopt;
    }
    return DeserializeFromString(data);
}

} // namespace Zgine
