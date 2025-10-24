#pragma once

#include "Zgine/Core.h"
#include "Zgine/Config/ConfigManager.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace Zgine {
namespace UI {

// 菜单项类型
enum class MenuItemType {
    Button,
    Checkbox,
    Slider,
    Separator,
    Submenu
};

// 菜单项
struct MenuItem {
    std::string id;
    std::string label;
    MenuItemType type;
    std::function<void()> callback;
    std::vector<MenuItem> submenu;
    
    // 对于不同类型的值
    bool boolValue = false;
    float floatValue = 0.0f;
    float floatMin = 0.0f;
    float floatMax = 1.0f;
    
    // 可见性控制
    bool visible = true;
    bool enabled = true;
    
    MenuItem(const std::string& id, const std::string& label, MenuItemType type, std::function<void()> callback = nullptr)
        : id(id), label(label), type(type), callback(std::move(callback)) {}
};

// 系统托盘管理器
class TrayManager {
public:
    static TrayManager& Instance();
    
    // 菜单管理
    void AddMenuItem(const MenuItem& item);
    void RemoveMenuItem(const std::string& id);
    void UpdateMenuItem(const std::string& id, const MenuItem& item);
    MenuItem* GetMenuItem(const std::string& id);
    
    // 菜单渲染
    void RenderMenu();
    void RenderMenuItem(const MenuItem& item);
    
    // 配置集成
    void AddConfigMenuItem(const std::string& configKey, const std::string& label, const std::string& description = "");
    void AddHotReloadMenuItem();
    void AddJSONBackendMenuItem();
    
    // 快捷操作
    void AddSeparator();
    void AddButton(const std::string& id, const std::string& label, std::function<void()> callback);
    void AddCheckbox(const std::string& id, const std::string& label, bool defaultValue = false, std::function<void(bool)> callback = nullptr);
    void AddSlider(const std::string& id, const std::string& label, float min, float max, float defaultValue = 0.0f, std::function<void(float)> callback = nullptr);
    
    // 菜单状态
    void SetMenuItemVisible(const std::string& id, bool visible);
    void SetMenuItemEnabled(const std::string& id, bool enabled);
    void SetMenuItemLabel(const std::string& id, const std::string& label);
    
    // 清空菜单
    void ClearMenu();
    
    // 获取菜单项数量
    size_t GetMenuItemCount() const { return m_MenuItems.size(); }

private:
    TrayManager() = default;
    ~TrayManager() = default;
    
    std::vector<MenuItem> m_MenuItems;
    std::unordered_map<std::string, size_t> m_MenuItemIndexMap;
    
    void UpdateIndexMap();
    void RenderSubmenu(const std::vector<MenuItem>& submenu, const std::string& parentId);
};

// 预定义菜单项
namespace MenuItems {
    // 系统菜单
    constexpr const char* SYSTEM_EXIT = "system.exit";
    constexpr const char* SYSTEM_ABOUT = "system.about";
    constexpr const char* SYSTEM_SETTINGS = "system.settings";
    
    // 配置菜单
    constexpr const char* CONFIG_RELOAD = "config.reload";
    constexpr const char* CONFIG_SAVE = "config.save";
    constexpr const char* CONFIG_RESET = "config.reset";
    constexpr const char* CONFIG_HOT_RELOAD = "config.hot_reload";
    
    // JSON后端菜单
    constexpr const char* JSON_BACKEND_NLOHMANN = "json.backend.nlohmann";
    constexpr const char* JSON_BACKEND_RAPIDJSON = "json.backend.rapidjson";
    constexpr const char* JSON_BACKEND_CUSTOM = "json.backend.custom";
    
    // 渲染菜单
    constexpr const char* RENDER_VSYNC = "render.vsync";
    constexpr const char* RENDER_MSAA = "render.msaa";
    constexpr const char* RENDER_FULLSCREEN = "render.fullscreen";
    
    // 调试菜单
    constexpr const char* DEBUG_SHOW_FPS = "debug.show_fps";
    constexpr const char* DEBUG_SHOW_DEBUG_INFO = "debug.show_debug_info";
    constexpr const char* DEBUG_LOG_LEVEL = "debug.log_level";
}

} // namespace UI
} // namespace Zgine
