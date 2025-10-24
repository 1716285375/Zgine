#include "zgpch.h"
#include "ConfigManager.h"
#include "Zgine/JSON/JSONHelpers.h"
#include <fstream>
#include <sstream>

namespace Zgine {
namespace Config {

ConfigManager& ConfigManager::Instance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::LoadConfig(const std::string& filepath, JSON::JSONBackend backend) {
    try {
        auto configData = JSON::ParseJSONFromFile(filepath, backend);
        if (!configData) {
            ZG_CORE_ERROR("Failed to load config from: {}", filepath);
            return false;
        }
        
        m_ConfigData = std::move(configData);
        m_ConfigPath = filepath;
        m_LastFileTime = GetFileTime(filepath);
        
        ZG_CORE_INFO("Loaded config from: {}", filepath);
        return true;
    } catch (const std::exception& e) {
        ZG_CORE_ERROR("Exception loading config: {}", e.what());
        return false;
    }
}

bool ConfigManager::SaveConfig(const std::string& filepath, JSON::JSONBackend backend) {
    if (!m_ConfigData) {
        ZG_CORE_WARN("No config data to save");
        return false;
    }
    
    std::string path = filepath.empty() ? m_ConfigPath : filepath;
    if (path.empty()) {
        ZG_CORE_ERROR("No config path specified");
        return false;
    }
    
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            ZG_CORE_ERROR("Failed to open config file for writing: {}", path);
            return false;
        }
        
        file << m_ConfigData->ToString(2); // Pretty print with 2-space indentation
        file.close();
        
        m_LastFileTime = GetFileTime(path);
        ZG_CORE_INFO("Saved config to: {}", path);
        return true;
    } catch (const std::exception& e) {
        ZG_CORE_ERROR("Exception saving config: {}", e.what());
        return false;
    }
}

const JSON::IJSONValue* ConfigManager::GetJSONValue(const std::string& key) const {
    if (!m_ConfigData) {
        return nullptr;
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
            return nullptr;
        }
        current = &(*current)[k];
    }
    
    return current;
}

void ConfigManager::SetJSONValue(const std::string& key, std::unique_ptr<JSON::IJSONValue> value) {
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
    const std::string& finalKey = keys.back();
    auto oldValue = current->HasKey(finalKey) ? (*current)[finalKey].Clone() : nullptr;
    current->Set(finalKey, std::move(value));
    
    if (oldValue) {
        NotifyChange(key, *oldValue, (*current)[finalKey]);
    }
}

bool ConfigManager::HasKey(const std::string& key) const {
    return GetJSONValue(key) != nullptr;
}

void ConfigManager::RegisterChangeCallback(const std::string& key, ConfigChangeCallback callback) {
    m_ChangeCallbacks[key] = std::move(callback);
    ZG_CORE_INFO("Registered config change callback for key: {}", key);
}

void ConfigManager::UnregisterChangeCallback(const std::string& key) {
    auto it = m_ChangeCallbacks.find(key);
    if (it != m_ChangeCallbacks.end()) {
        m_ChangeCallbacks.erase(it);
        ZG_CORE_INFO("Unregistered config change callback for key: {}", key);
    }
}

void ConfigManager::EnableHotReload(bool enable) {
    m_HotReloadEnabled = enable;
    if (enable && !m_ConfigPath.empty()) {
        m_LastFileTime = GetFileTime(m_ConfigPath);
    }
    ZG_CORE_INFO("Config hot reload {}", enable ? "enabled" : "disabled");
}

void ConfigManager::CheckForFileChanges() {
    if (!m_HotReloadEnabled || m_ConfigPath.empty()) {
        return;
    }
    
    try {
        auto currentFileTime = GetFileTime(m_ConfigPath);
        if (currentFileTime > m_LastFileTime) {
            ZG_CORE_INFO("Config file changed, reloading: {}", m_ConfigPath);
            
            // 保存当前配置的副本用于比较
            auto oldConfig = m_ConfigData ? m_ConfigData->Clone() : nullptr;
            
            // 重新加载配置
            if (LoadConfig(m_ConfigPath)) {
                // 这里可以添加配置变更检测逻辑
                ZG_CORE_INFO("Config reloaded successfully");
            } else {
                ZG_CORE_ERROR("Failed to reload config");
            }
        }
    } catch (const std::exception& e) {
        ZG_CORE_ERROR("Exception checking for config file changes: {}", e.what());
    }
}

void ConfigManager::Reset() {
    m_ConfigData = JSON::CreateJSONObject();
    ZG_CORE_INFO("Config reset to default");
}

void ConfigManager::Clear() {
    m_ConfigData.reset();
    m_ConfigPath.clear();
    m_ChangeCallbacks.clear();
    m_HotReloadEnabled = false;
    ZG_CORE_INFO("Config cleared");
}

void ConfigManager::NotifyChange(const std::string& key, const JSON::IJSONValue& oldValue, const JSON::IJSONValue& newValue) {
    auto it = m_ChangeCallbacks.find(key);
    if (it != m_ChangeCallbacks.end()) {
        try {
            it->second(key, oldValue, newValue);
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Exception in config change callback for key {}: {}", key, e.what());
        }
    }
}

std::filesystem::file_time_type ConfigManager::GetFileTime(const std::string& filepath) {
    try {
        return std::filesystem::last_write_time(filepath);
    } catch (const std::exception& e) {
        ZG_CORE_ERROR("Failed to get file time for {}: {}", filepath, e.what());
        return std::filesystem::file_time_type::min();
    }
}

} // namespace Config
} // namespace Zgine
