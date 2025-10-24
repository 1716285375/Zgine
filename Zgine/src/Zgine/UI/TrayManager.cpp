#include "zgpch.h"
#include "TrayManager.h"
#include "Zgine/JSON/JSONHelpers.h"
#include "Zgine/JSON/JSONManager.h"
#include "Zgine/ImGui/ImGuiWrapper.h"
#include <algorithm>

namespace Zgine {
namespace UI {

TrayManager& TrayManager::Instance() {
    static TrayManager instance;
    return instance;
}

void TrayManager::AddMenuItem(const MenuItem& item) {
    // 检查是否已存在
    auto it = m_MenuItemIndexMap.find(item.id);
    if (it != m_MenuItemIndexMap.end()) {
        // 更新现有项
        m_MenuItems[it->second] = item;
    } else {
        // 添加新项
        m_MenuItems.push_back(item);
        UpdateIndexMap();
    }
    
    ZG_CORE_INFO("Added menu item: {}", item.id);
}

void TrayManager::RemoveMenuItem(const std::string& id) {
    auto it = m_MenuItemIndexMap.find(id);
    if (it != m_MenuItemIndexMap.end()) {
        m_MenuItems.erase(m_MenuItems.begin() + it->second);
        UpdateIndexMap();
        ZG_CORE_INFO("Removed menu item: {}", id);
    }
}

void TrayManager::UpdateMenuItem(const std::string& id, const MenuItem& item) {
    auto it = m_MenuItemIndexMap.find(id);
    if (it != m_MenuItemIndexMap.end()) {
        m_MenuItems[it->second] = item;
        ZG_CORE_INFO("Updated menu item: {}", id);
    }
}

MenuItem* TrayManager::GetMenuItem(const std::string& id) {
    auto it = m_MenuItemIndexMap.find(id);
    if (it != m_MenuItemIndexMap.end()) {
        return &m_MenuItems[it->second];
    }
    return nullptr;
}

void TrayManager::RenderMenu() {
    if (Zgine::ImGui::ImGuiWrapper::BeginMenuBar()) {
        for (const auto& item : m_MenuItems) {
            if (item.visible) {
                RenderMenuItem(item);
            }
        }
        Zgine::ImGui::ImGuiWrapper::EndMenuBar();
    }
}

void TrayManager::RenderMenuItem(const MenuItem& item) {
    if (!item.visible) return;
    
    switch (item.type) {
        case MenuItemType::Button:
            if (Zgine::ImGui::ImGuiWrapper::MenuItem(item.label.c_str(), nullptr, false, item.enabled)) {
                if (item.callback) {
                    item.callback();
                }
            }
            break;
            
        case MenuItemType::Checkbox:
            if (Zgine::ImGui::ImGuiWrapper::MenuItem(item.label.c_str(), nullptr, &item.boolValue, item.enabled)) {
                if (item.callback) {
                    item.callback();
                }
            }
            break;
            
        case MenuItemType::Separator:
            Zgine::ImGui::ImGuiWrapper::Separator();
            break;
            
        case MenuItemType::Submenu:
            if (Zgine::ImGui::ImGuiWrapper::BeginMenu(item.label.c_str(), item.enabled)) {
                RenderSubmenu(item.submenu, item.id);
                Zgine::ImGui::ImGuiWrapper::EndMenu();
            }
            break;
            
        case MenuItemType::Slider:
            // Slider在菜单中通常不直接渲染，可能需要特殊处理
            Zgine::ImGui::ImGuiWrapper::Text("%s: %.2f", item.label.c_str(), item.floatValue);
            break;
    }
}

void TrayManager::RenderSubmenu(const std::vector<MenuItem>& submenu, const std::string& parentId) {
    for (const auto& item : submenu) {
        if (item.visible) {
            RenderMenuItem(item);
        }
    }
}

void TrayManager::AddConfigMenuItem(const std::string& configKey, const std::string& label, const std::string& description) {
    auto& config = Config::ConfigManager::Instance();
    
    // 根据配置类型创建不同的菜单项
    if (config.HasKey(configKey)) {
        auto jsonValue = config.GetJSONValue(configKey);
        if (jsonValue) {
            if (jsonValue->IsBool()) {
                bool value = config.GetValue<bool>(configKey, false);
                AddCheckbox(configKey, label, value, [configKey](bool newValue) {
                    Config::ConfigManager::Instance().SetValue(configKey, newValue);
                });
            } else if (jsonValue->IsNumber()) {
                float value = config.GetValue<float>(configKey, 0.0f);
                AddSlider(configKey, label, 0.0f, 1.0f, value, [configKey](float newValue) {
                    Config::ConfigManager::Instance().SetValue(configKey, newValue);
                });
            }
        }
    }
}

void TrayManager::AddHotReloadMenuItem() {
    AddCheckbox(MenuItems::CONFIG_HOT_RELOAD, "Hot Reload", false, [](bool enabled) {
        Config::ConfigManager::Instance().EnableHotReload(enabled);
    });
}

void TrayManager::AddJSONBackendMenuItem() {
    // 创建JSON后端子菜单
    MenuItem jsonBackendMenu("json.backend", "JSON Backend", MenuItemType::Submenu);
    
    // 添加后端选项
    jsonBackendMenu.submenu.push_back(MenuItem(MenuItems::JSON_BACKEND_NLOHMANN, "Nlohmann", MenuItemType::Button, []() {
        JSON::JSONManager::Instance().SetDefaultBackend(JSON::JSONBackend::Nlohmann);
    }));
    
    jsonBackendMenu.submenu.push_back(MenuItem(MenuItems::JSON_BACKEND_RAPIDJSON, "RapidJSON", MenuItemType::Button, []() {
        JSON::JSONManager::Instance().SetDefaultBackend(JSON::JSONBackend::RapidJSON);
    }));
    
    AddMenuItem(jsonBackendMenu);
}

void TrayManager::AddSeparator() {
    AddMenuItem(MenuItem("separator_" + std::to_string(m_MenuItems.size()), "", MenuItemType::Separator));
}

void TrayManager::AddButton(const std::string& id, const std::string& label, std::function<void()> callback) {
    AddMenuItem(MenuItem(id, label, MenuItemType::Button, std::move(callback)));
}

void TrayManager::AddCheckbox(const std::string& id, const std::string& label, bool defaultValue, std::function<void(bool)> callback) {
    MenuItem item(id, label, MenuItemType::Checkbox);
    item.boolValue = defaultValue;
    if (callback) {
        item.callback = [callback, defaultValue]() {
            callback(!defaultValue); // 简单的切换逻辑
        };
    }
    AddMenuItem(item);
}

void TrayManager::AddSlider(const std::string& id, const std::string& label, float min, float max, float defaultValue, std::function<void(float)> callback) {
    MenuItem item(id, label, MenuItemType::Slider);
    item.floatValue = defaultValue;
    item.floatMin = min;
    item.floatMax = max;
    if (callback) {
        item.callback = [callback, defaultValue]() {
            callback(defaultValue);
        };
    }
    AddMenuItem(item);
}

void TrayManager::SetMenuItemVisible(const std::string& id, bool visible) {
    auto item = GetMenuItem(id);
    if (item) {
        item->visible = visible;
    }
}

void TrayManager::SetMenuItemEnabled(const std::string& id, bool enabled) {
    auto item = GetMenuItem(id);
    if (item) {
        item->enabled = enabled;
    }
}

void TrayManager::SetMenuItemLabel(const std::string& id, const std::string& label) {
    auto item = GetMenuItem(id);
    if (item) {
        item->label = label;
    }
}

void TrayManager::ClearMenu() {
    m_MenuItems.clear();
    m_MenuItemIndexMap.clear();
    ZG_CORE_INFO("Cleared all menu items");
}

void TrayManager::UpdateIndexMap() {
    m_MenuItemIndexMap.clear();
    for (size_t i = 0; i < m_MenuItems.size(); ++i) {
        m_MenuItemIndexMap[m_MenuItems[i].id] = i;
    }
}

} // namespace UI
} // namespace Zgine
