#pragma once

#include "Zgine/JSON/IJSONValue.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <chrono>

namespace Zgine {
namespace Config {

// 配置变更回调函数类型
using ConfigChangeCallback = std::function<void(const std::string& key, const JSON::IJSONValue& oldValue, const JSON::IJSONValue& newValue)>;

// 配置管理器
class ConfigManager {
public:
    static ConfigManager& Instance();
    
    // 配置加载和保存
    bool LoadConfig(const std::string& filepath, JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    bool SaveConfig(const std::string& filepath = "", JSON::JSONBackend backend = JSON::JSONBackend::Nlohmann);
    
    // 配置值操作
    template<typename T>
    T GetValue(const std::string& key, const T& defaultValue = T{}) const;
    
    template<typename T>
    void SetValue(const std::string& key, const T& value);
    
    // JSON值操作
    const JSON::IJSONValue* GetJSONValue(const std::string& key) const;
    void SetJSONValue(const std::string& key, std::unique_ptr<JSON::IJSONValue> value);
    
    // 配置检查
    bool HasKey(const std::string& key) const;
    bool IsEmpty() const { return m_ConfigData == nullptr; }
    
    // 回调管理
    void RegisterChangeCallback(const std::string& key, ConfigChangeCallback callback);
    void UnregisterChangeCallback(const std::string& key);
    
    // 热重载
    void EnableHotReload(bool enable = true);
    bool IsHotReloadEnabled() const { return m_HotReloadEnabled; }
    void CheckForFileChanges();
    
    // 配置路径
    void SetConfigPath(const std::string& path) { m_ConfigPath = path; }
    const std::string& GetConfigPath() const { return m_ConfigPath; }
    
    // 重置配置
    void Reset();
    void Clear();

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    std::unique_ptr<JSON::IJSONValue> m_ConfigData;
    std::string m_ConfigPath;
    bool m_HotReloadEnabled = false;
    std::filesystem::file_time_type m_LastFileTime;
    std::unordered_map<std::string, ConfigChangeCallback> m_ChangeCallbacks;
    
    void NotifyChange(const std::string& key, const JSON::IJSONValue& oldValue, const JSON::IJSONValue& newValue);
    std::filesystem::file_time_type GetFileTime(const std::string& filepath);
};

// 配置键常量
namespace Keys {
    // 渲染配置
    constexpr const char* RENDER_VSYNC = "render.vsync";
    constexpr const char* RENDER_MSAA = "render.msaa";
    constexpr const char* RENDER_RESOLUTION_WIDTH = "render.resolution.width";
    constexpr const char* RENDER_RESOLUTION_HEIGHT = "render.resolution.height";
    constexpr const char* RENDER_FULLSCREEN = "render.fullscreen";
    
    // 音频配置
    constexpr const char* AUDIO_MASTER_VOLUME = "audio.master_volume";
    constexpr const char* AUDIO_MUSIC_VOLUME = "audio.music_volume";
    constexpr const char* AUDIO_SFX_VOLUME = "audio.sfx_volume";
    
    // 输入配置
    constexpr const char* INPUT_MOUSE_SENSITIVITY = "input.mouse_sensitivity";
    constexpr const char* INPUT_KEYBOARD_REPEAT_DELAY = "input.keyboard_repeat_delay";
    
    // UI配置
    constexpr const char* UI_SCALE = "ui.scale";
    constexpr const char* UI_THEME = "ui.theme";
    constexpr const char* UI_LANGUAGE = "ui.language";
    
    // 调试配置
    constexpr const char* DEBUG_SHOW_FPS = "debug.show_fps";
    constexpr const char* DEBUG_SHOW_DEBUG_INFO = "debug.show_debug_info";
    constexpr const char* DEBUG_LOG_LEVEL = "debug.log_level";
    
    // ECS配置
    constexpr const char* ECS_MAX_ENTITIES = "ecs.max_entities";
    constexpr const char* ECS_COMPONENT_POOL_SIZE = "ecs.component_pool_size";
    constexpr const char* ECS_SYSTEM_UPDATE_RATE = "ecs.system_update_rate";
}

// 模板实现
template<typename T>
T ConfigManager::GetValue(const std::string& key, const T& defaultValue) const {
    if (!m_ConfigData) {
        return defaultValue;
    }
    
    // 简单的键值查找（支持点分隔的嵌套键）
    std::vector<std::string> keys;
    std::string currentKey;
    for (char c : key) {
        if (c == '.') {
            keys.push_back(currentKey);
            currentKey.clear();
        } else {
            currentKey += c;
        }
    }
    keys.push_back(currentKey);
    
    const JSON::IJSONValue* current = m_ConfigData.get();
    for (const auto& k : keys) {
        if (!current || !current->HasKey(k)) {
            return defaultValue;
        }
        current = &(*current)[k];
    }
    
    // 类型转换（这里简化处理，实际项目中可能需要更复杂的类型系统）
    if constexpr (std::is_same_v<T, bool>) {
        return current->AsBool();
    } else if constexpr (std::is_same_v<T, int>) {
        return current->AsInt();
    } else if constexpr (std::is_same_v<T, float>) {
        return current->AsFloat();
    } else if constexpr (std::is_same_v<T, double>) {
        return current->AsDouble();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return current->AsString();
    }
    
    return defaultValue;
}

template<typename T>
void ConfigManager::SetValue(const std::string& key, const T& value) {
    if (!m_ConfigData) {
        m_ConfigData = JSON::CreateJSONObject();
    }
    
    // 创建嵌套对象结构
    std::vector<std::string> keys;
    std::string currentKey;
    for (char c : key) {
        if (c == '.') {
            keys.push_back(currentKey);
            currentKey.clear();
        } else {
            currentKey += c;
        }
    }
    keys.push_back(currentKey);
    
    JSON::IJSONValue* current = m_ConfigData.get();
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        const std::string& k = keys[i];
        if (!current->HasKey(k)) {
            current->Set(k, JSON::CreateJSONObject());
        }
        current = &(*current)[k];
    }
    
    // 设置最终值
    std::unique_ptr<JSON::IJSONValue> newValue;
    if constexpr (std::is_same_v<T, bool>) {
        newValue = JSON::CreateJSONBool(value);
    } else if constexpr (std::is_same_v<T, int>) {
        newValue = JSON::CreateJSONInt(value);
    } else if constexpr (std::is_same_v<T, float>) {
        newValue = JSON::CreateJSONFloat(value);
    } else if constexpr (std::is_same_v<T, double>) {
        newValue = JSON::CreateJSONDouble(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        newValue = JSON::CreateJSONString(value);
    }
    
    if (newValue) {
        const std::string& finalKey = keys.back();
        auto oldValue = current->HasKey(finalKey) ? (*current)[finalKey].Clone() : nullptr;
        current->Set(finalKey, std::move(newValue));
        
        if (oldValue) {
            NotifyChange(key, *oldValue, (*current)[finalKey]);
        }
    }
}

} // namespace Config
} // namespace Zgine
