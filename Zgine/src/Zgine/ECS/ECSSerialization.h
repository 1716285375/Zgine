#pragma once

#include "ECS.h"
#include "Zgine/JSON/IJSONValue.h"
#include "Zgine/Core/SmartPointers.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace Zgine {
namespace ECS {

// ECS序列化器 - 负责实体和组件的序列化/反序列化
class ECSSerializer {
public:
    // 序列化整个ECS管理器到JSON
    static Scope<JSON::IJSONValue> SerializeECSManager(const ECSManager& manager, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    
    // 从JSON反序列化到ECS管理器
    static bool DeserializeECSManager(ECSManager& manager, const JSON::IJSONValue& root);
    
    // 序列化单个实体
    static Scope<JSON::IJSONValue> SerializeEntity(const Entity& entity, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    
    // 反序列化单个实体
    static Entity DeserializeEntity(ECSManager& manager, const JSON::IJSONValue& entityData);
    
    // 保存到文件
    static bool SaveToFile(const ECSManager& manager, const std::string& filepath, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    
    // 从文件加载
    static bool LoadFromFile(ECSManager& manager, const std::string& filepath, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);

public:
    // 组件序列化模板
    template<typename Component>
    static Scope<JSON::IJSONValue> SerializeComponent(const Component& component, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    
    // 组件反序列化模板
    template<typename Component>
    static Component DeserializeComponent(const JSON::IJSONValue& componentData);
};

// 场景管理器 - 管理多个ECS场景
class SceneManager {
public:
    SceneManager();
    ~SceneManager();
    
    // 场景管理
    void CreateScene(const std::string& name);
    void LoadScene(const std::string& name);
    void SaveScene(const std::string& name);
    void DeleteScene(const std::string& name);
    
    // 获取当前场景
    ECSManager& GetCurrentScene() { return *m_CurrentScene; }
    const std::string& GetCurrentSceneName() const { return m_CurrentSceneName; }
    
    // 场景列表
    std::vector<std::string> GetSceneNames() const;
    bool HasScene(const std::string& name) const;
    
    // 场景切换
    bool SwitchToScene(const std::string& name);
    
    // 自动保存
    void SetAutoSave(bool enabled) { m_AutoSave = enabled; }
    bool IsAutoSaveEnabled() const { return m_AutoSave; }
    
    // 场景目录
    void SetSceneDirectory(const std::string& directory) { m_SceneDirectory = directory; }
    const std::string& GetSceneDirectory() const { return m_SceneDirectory; }

private:
    std::unordered_map<std::string, Scope<ECSManager>> m_Scenes;
    ECSManager* m_CurrentScene;
    std::string m_CurrentSceneName;
    std::string m_SceneDirectory;
    bool m_AutoSave;
    
    void EnsureSceneDirectory();
    std::string GetSceneFilePath(const std::string& sceneName) const;
};

} // namespace ECS
} // namespace Zgine
