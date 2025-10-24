#include "zgpch.h"
#include "ECSSerialization.h"
#include "Zgine/JSON/JSONHelpers.h"
#include "Zgine/Core/SmartPointers.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <utility>

namespace Zgine {
namespace ECS {

// 组件序列化模板特化
template<>
Scope<JSON::IJSONValue> ECSSerializer::SerializeComponent<Position>(const Position& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    auto position = JSON::CreateJSONObject(backend);
    position->Set("x", JSON::CreateJSONFloat(component.position.x, backend));
    position->Set("y", JSON::CreateJSONFloat(component.position.y, backend));
    position->Set("z", JSON::CreateJSONFloat(component.position.z, backend));
    json->Set("position", std::move(position));
    return json;
}

template<>
Position ECSSerializer::DeserializeComponent<Position>(const JSON::IJSONValue& json) {
    Position component;
    if (json.HasKey("position")) {
        const auto& pos = json["position"];
        component.position.x = pos["x"].AsFloat();
        component.position.y = pos["y"].AsFloat();
        component.position.z = pos["z"].AsFloat();
    }
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Velocity>(const Velocity& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    auto velocity = JSON::CreateJSONObject(backend);
    velocity->Set("x", JSON::CreateJSONFloat(component.velocity.x, backend));
    velocity->Set("y", JSON::CreateJSONFloat(component.velocity.y, backend));
    velocity->Set("z", JSON::CreateJSONFloat(component.velocity.z, backend));
    json->Set("velocity", std::move(velocity));
    return json;
}

template<>
Velocity ECSSerializer::DeserializeComponent<Velocity>(const JSON::IJSONValue& json) {
    Velocity component;
    if (json.HasKey("velocity")) {
        const auto& vel = json["velocity"];
        component.velocity.x = vel["x"].AsFloat();
        component.velocity.y = vel["y"].AsFloat();
        component.velocity.z = vel["z"].AsFloat();
    }
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Renderable>(const Renderable& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    auto color = JSON::CreateJSONObject(backend);
    color->Set("r", JSON::CreateJSONFloat(component.color.r, backend));
    color->Set("g", JSON::CreateJSONFloat(component.color.g, backend));
    color->Set("b", JSON::CreateJSONFloat(component.color.b, backend));
    color->Set("a", JSON::CreateJSONFloat(component.color.a, backend));
    json->Set("color", std::move(color));
    json->Set("scale", JSON::CreateJSONFloat(component.scale, backend));
    return json;
}

template<>
Renderable ECSSerializer::DeserializeComponent<Renderable>(const JSON::IJSONValue& json) {
    Renderable component;
    if (json.HasKey("color")) {
        const auto& color = json["color"];
        component.color.r = color["r"].AsFloat();
        component.color.g = color["g"].AsFloat();
        component.color.b = color["b"].AsFloat();
        component.color.a = color["a"].AsFloat();
    }
    component.scale = json["scale"].AsFloat();
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Transform>(const Transform& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    
    auto position = JSON::CreateJSONObject(backend);
    position->Set("x", JSON::CreateJSONFloat(component.position.x, backend));
    position->Set("y", JSON::CreateJSONFloat(component.position.y, backend));
    position->Set("z", JSON::CreateJSONFloat(component.position.z, backend));
    json->Set("position", std::move(position));
    
    auto rotation = JSON::CreateJSONObject(backend);
    rotation->Set("x", JSON::CreateJSONFloat(component.rotation.x, backend));
    rotation->Set("y", JSON::CreateJSONFloat(component.rotation.y, backend));
    rotation->Set("z", JSON::CreateJSONFloat(component.rotation.z, backend));
    json->Set("rotation", std::move(rotation));
    
    auto scale = JSON::CreateJSONObject(backend);
    scale->Set("x", JSON::CreateJSONFloat(component.scale.x, backend));
    scale->Set("y", JSON::CreateJSONFloat(component.scale.y, backend));
    scale->Set("z", JSON::CreateJSONFloat(component.scale.z, backend));
    json->Set("scale", std::move(scale));
    
    return json;
}

template<>
Transform ECSSerializer::DeserializeComponent<Transform>(const JSON::IJSONValue& json) {
    Transform component;
    if (json.HasKey("position")) {
        const auto& pos = json["position"];
        component.position.x = pos["x"].AsFloat();
        component.position.y = pos["y"].AsFloat();
        component.position.z = pos["z"].AsFloat();
    }
    if (json.HasKey("rotation")) {
        const auto& rot = json["rotation"];
        component.rotation.x = rot["x"].AsFloat();
        component.rotation.y = rot["y"].AsFloat();
        component.rotation.z = rot["z"].AsFloat();
    }
    if (json.HasKey("scale")) {
        const auto& scale = json["scale"];
        component.scale.x = scale["x"].AsFloat();
        component.scale.y = scale["y"].AsFloat();
        component.scale.z = scale["z"].AsFloat();
    }
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Sprite>(const Sprite& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    
    auto color = JSON::CreateJSONObject(backend);
    color->Set("r", JSON::CreateJSONFloat(component.color.r, backend));
    color->Set("g", JSON::CreateJSONFloat(component.color.g, backend));
    color->Set("b", JSON::CreateJSONFloat(component.color.b, backend));
    color->Set("a", JSON::CreateJSONFloat(component.color.a, backend));
    json->Set("color", std::move(color));
    
    auto size = JSON::CreateJSONObject(backend);
    size->Set("x", JSON::CreateJSONFloat(component.size.x, backend));
    size->Set("y", JSON::CreateJSONFloat(component.size.y, backend));
    json->Set("size", std::move(size));
    
    json->Set("textureID", JSON::CreateJSONInt(component.textureID, backend));
    json->Set("visible", JSON::CreateJSONBool(component.visible, backend));
    
    return json;
}

template<>
Sprite ECSSerializer::DeserializeComponent<Sprite>(const JSON::IJSONValue& json) {
    Sprite component;
    if (json.HasKey("color")) {
        const auto& color = json["color"];
        component.color.r = color["r"].AsFloat();
        component.color.g = color["g"].AsFloat();
        component.color.b = color["b"].AsFloat();
        component.color.a = color["a"].AsFloat();
    }
    if (json.HasKey("size")) {
        const auto& size = json["size"];
        component.size.x = size["x"].AsFloat();
        component.size.y = size["y"].AsFloat();
    }
    component.textureID = json["textureID"].AsInt();
    component.visible = json["visible"].AsBool();
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Health>(const Health& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    json->Set("current", JSON::CreateJSONFloat(component.current, backend));
    json->Set("maximum", JSON::CreateJSONFloat(component.maximum, backend));
    json->Set("isAlive", JSON::CreateJSONBool(component.isAlive, backend));
    return json;
}

template<>
Health ECSSerializer::DeserializeComponent<Health>(const JSON::IJSONValue& json) {
    Health component;
    component.current = json["current"].AsFloat();
    component.maximum = json["maximum"].AsFloat();
    component.isAlive = json["isAlive"].AsBool();
    return component;
}

template<>
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeComponent<Tag>(const Tag& component, JSON::JSONBackend backend) {
    auto json = JSON::CreateJSONObject(backend);
    json->Set("name", JSON::CreateJSONString(component.name, backend));
    
    auto tagsArray = JSON::CreateJSONArray(backend);
    for (const auto& tag : component.tags) {
        tagsArray->Append(JSON::CreateJSONString(tag, backend));
    }
    json->Set("tags", std::move(tagsArray));
    
    return json;
}

template<>
Tag ECSSerializer::DeserializeComponent<Tag>(const JSON::IJSONValue& json) {
    Tag component;
    component.name = json["name"].AsString();
    if (json.HasKey("tags")) {
        const auto& tags = json["tags"];
        for (size_t i = 0; i < tags.Size(); ++i) {
            component.tags.push_back(tags[i].AsString());
        }
    }
    return component;
}

// ECS管理器序列化
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeECSManager(const ECSManager& manager, JSON::JSONBackend backend) {
    auto root = JSON::CreateJSONObject(backend);
    root->Set("version", JSON::CreateJSONString("1.0", backend));
    root->Set("entityCount", JSON::CreateJSONInt(static_cast<int>(manager.GetEntityCount()), backend));
    
    auto entities = JSON::CreateJSONArray(backend);
    
    // 获取所有实体
    auto allEntities = manager.GetEntitiesWithComponent<Position>();
    for (const auto& entity : allEntities) {
        auto entityData = SerializeEntity(entity, backend);
        if (entityData) {
            entities->Append(std::move(entityData));
        }
    }
    
    root->Set("entities", std::move(entities));
    return root;
}

// ECS管理器反序列化
bool ECSSerializer::DeserializeECSManager(ECSManager& manager, const JSON::IJSONValue& root) {
    if (!root.HasKey("entities")) {
        ZG_CORE_ERROR("Invalid ECS data format");
        return false;
    }
    
    // 反序列化实体
    const auto& entities = root["entities"];
    for (size_t i = 0; i < entities.Size(); ++i) {
        Entity entity = DeserializeEntity(manager, entities[i]);
        if (!entity.IsValid()) {
            ZG_CORE_WARN("Failed to deserialize entity");
            continue;
        }
    }
    
    ZG_CORE_INFO("Deserialized {} entities", entities.Size());
    return true;
}

// 单个实体序列化
std::unique_ptr<JSON::IJSONValue> ECSSerializer::SerializeEntity(const Entity& entity, JSON::JSONBackend backend) {
    auto entityData = JSON::CreateJSONObject(backend);
    entityData->Set("id", JSON::CreateJSONInt(static_cast<int>(entity.GetID()), backend));
    
    auto components = JSON::CreateJSONObject(backend);
    
    // 序列化所有组件
    if (entity.HasComponent<Position>()) {
        components->Set("Position", *SerializeComponent(entity.GetComponent<Position>(), backend));
    }
    if (entity.HasComponent<Velocity>()) {
        components->Set("Velocity", *SerializeComponent(entity.GetComponent<Velocity>(), backend));
    }
    if (entity.HasComponent<Renderable>()) {
        components->Set("Renderable", *SerializeComponent(entity.GetComponent<Renderable>(), backend));
    }
    if (entity.HasComponent<Transform>()) {
        components->Set("Transform", *SerializeComponent(entity.GetComponent<Transform>(), backend));
    }
    if (entity.HasComponent<Sprite>()) {
        components->Set("Sprite", *SerializeComponent(entity.GetComponent<Sprite>(), backend));
    }
    if (entity.HasComponent<Health>()) {
        components->Set("Health", *SerializeComponent(entity.GetComponent<Health>(), backend));
    }
    if (entity.HasComponent<Tag>()) {
        components->Set("Tag", *SerializeComponent(entity.GetComponent<Tag>(), backend));
    }
    
    entityData->Set("components", std::move(components));
    return entityData;
}

// 单个实体反序列化
Entity ECSSerializer::DeserializeEntity(ECSManager& manager, const JSON::IJSONValue& entityData) {
    Entity entity = manager.CreateEntity();
    
    if (!entityData.HasKey("components")) {
        return entity;
    }
    
    const auto& components = entityData["components"];
    
    // 反序列化组件
    if (components.HasKey("Position")) {
        entity.AddComponent<Position>(DeserializeComponent<Position>(components["Position"]));
    }
    if (components.HasKey("Velocity")) {
        entity.AddComponent<Velocity>(DeserializeComponent<Velocity>(components["Velocity"]));
    }
    if (components.HasKey("Renderable")) {
        entity.AddComponent<Renderable>(DeserializeComponent<Renderable>(components["Renderable"]));
    }
    if (components.HasKey("Transform")) {
        entity.AddComponent<Transform>(DeserializeComponent<Transform>(components["Transform"]));
    }
    if (components.HasKey("Sprite")) {
        entity.AddComponent<Sprite>(DeserializeComponent<Sprite>(components["Sprite"]));
    }
    if (components.HasKey("Health")) {
        entity.AddComponent<Health>(DeserializeComponent<Health>(components["Health"]));
    }
    if (components.HasKey("Tag")) {
        entity.AddComponent<Tag>(DeserializeComponent<Tag>(components["Tag"]));
    }
    
    return entity;
}

// 保存到文件
bool ECSSerializer::SaveToFile(const ECSManager& manager, const std::string& filepath, JSON::JSONBackend backend) {
    try {
        auto root = SerializeECSManager(manager, backend);
        if (!root) {
            ZG_CORE_ERROR("Failed to serialize ECS manager");
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            ZG_CORE_ERROR("Failed to open file for writing: {}", filepath);
            return false;
        }
        
        file << root->ToString(2); // Pretty print with 2-space indentation
        file.close();
        
        ZG_CORE_INFO("Saved ECS data to: {}", filepath);
        return true;
    }
    catch (const std::exception& e) {
        ZG_CORE_ERROR("Failed to save ECS data: {}", e.what());
        return false;
    }
}

// 从文件加载
bool ECSSerializer::LoadFromFile(ECSManager& manager, const std::string& filepath, JSON::JSONBackend backend) {
    try {
        auto root = JSON::ParseJSONFromFile(filepath, backend);
        if (!root) {
            ZG_CORE_ERROR("Failed to parse JSON from file: {}", filepath);
            return false;
        }
        
        return DeserializeECSManager(manager, *root);
    }
    catch (const std::exception& e) {
        ZG_CORE_ERROR("Failed to load ECS data: {}", e.what());
        return false;
    }
}

// SceneManager实现
SceneManager::SceneManager() 
    : m_CurrentScene(nullptr)
    , m_CurrentSceneName("default")
    , m_SceneDirectory("scenes")
    , m_AutoSave(false)
{
    EnsureSceneDirectory();
    CreateScene("default");
}

SceneManager::~SceneManager() {
    if (m_AutoSave) {
        SaveScene(m_CurrentSceneName);
    }
}

void SceneManager::CreateScene(const std::string& name) {
    if (m_Scenes.find(name) != m_Scenes.end()) {
        ZG_CORE_WARN("Scene '{}' already exists", name);
        return;
    }
    
    m_Scenes[name] = std::make_unique<ECSManager>();
    ZG_CORE_INFO("Created scene: {}", name);
}

void SceneManager::LoadScene(const std::string& name) {
    if (m_Scenes.find(name) == m_Scenes.end()) {
        ZG_CORE_ERROR("Scene '{}' not found", name);
        return;
    }
    
    std::string filepath = GetSceneFilePath(name);
    if (ECSSerializer::LoadFromFile(*m_Scenes[name], filepath)) {
        ZG_CORE_INFO("Loaded scene: {}", name);
    } else {
        ZG_CORE_ERROR("Failed to load scene: {}", name);
    }
}

void SceneManager::SaveScene(const std::string& name) {
    if (m_Scenes.find(name) == m_Scenes.end()) {
        ZG_CORE_ERROR("Scene '{}' not found", name);
        return;
    }
    
    std::string filepath = GetSceneFilePath(name);
    if (ECSSerializer::SaveToFile(*m_Scenes[name], filepath)) {
        ZG_CORE_INFO("Saved scene: {}", name);
    } else {
        ZG_CORE_ERROR("Failed to save scene: {}", name);
    }
}

void SceneManager::DeleteScene(const std::string& name) {
    if (m_Scenes.find(name) == m_Scenes.end()) {
        ZG_CORE_ERROR("Scene '{}' not found", name);
        return;
    }
    
    if (m_CurrentSceneName == name) {
        ZG_CORE_WARN("Cannot delete current scene: {}", name);
        return;
    }
    
    m_Scenes.erase(name);
    ZG_CORE_INFO("Deleted scene: {}", name);
}

std::vector<std::string> SceneManager::GetSceneNames() const {
    std::vector<std::string> names;
    for (const auto& pair : m_Scenes) {
        names.push_back(pair.first);
    }
    return names;
}

bool SceneManager::HasScene(const std::string& name) const {
    return m_Scenes.find(name) != m_Scenes.end();
}

bool SceneManager::SwitchToScene(const std::string& name) {
    if (m_Scenes.find(name) == m_Scenes.end()) {
        ZG_CORE_ERROR("Scene '{}' not found", name);
        return false;
    }
    
    if (m_AutoSave && m_CurrentScene) {
        SaveScene(m_CurrentSceneName);
    }
    
    m_CurrentScene = m_Scenes[name].get();
    m_CurrentSceneName = name;
    ZG_CORE_INFO("Switched to scene: {}", name);
    return true;
}

void SceneManager::EnsureSceneDirectory() {
    if (!std::filesystem::exists(m_SceneDirectory)) {
        std::filesystem::create_directories(m_SceneDirectory);
        ZG_CORE_INFO("Created scene directory: {}", m_SceneDirectory);
    }
}

std::string SceneManager::GetSceneFilePath(const std::string& sceneName) const {
    return m_SceneDirectory + "/" + sceneName + ".json";
}

} // namespace ECS
} // namespace Zgine
