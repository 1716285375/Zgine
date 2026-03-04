#pragma once

#include <functional>
#include <string>
#include <vector>
#include <imgui.h>

namespace Zgine::UI::Widgets {

/**
 * @brief Fluent API for building context menus
 *
 * Provides a clean, chainable interface for creating ImGui context menus.
 */
class MenuBuilder {
public:
    MenuBuilder() = default;
    ~MenuBuilder() = default;

    MenuBuilder& AddItem(
        const char* label,
        std::function<void()> action,
        bool enabled = true,
        const char* shortcut = nullptr
    );

    MenuBuilder& AddSeparator();

    MenuBuilder& AddSubmenu(
        const char* label,
        std::function<void(MenuBuilder&)> buildSubmenu,
        bool enabled = true
    );

    MenuBuilder& AddCheckbox(
        const char* label,
        bool* checked,
        std::function<void()> action = nullptr,
        bool enabled = true
    );

    bool Render(const char* id, ImGuiPopupFlags flags = 0);
    bool RenderContextMenu(const char* id, ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonRight);
    void Clear();

private:
    struct MenuItem {
        enum class Type {
            Item,
            Separator,
            Submenu,
            Checkbox
        };

        Type type;
        std::string label;
        std::function<void()> action;
        std::function<void(MenuBuilder&)> submenuBuilder;
        bool* checked = nullptr;
        bool enabled = true;
        std::string shortcut;
    };

    void RenderItems();
    std::vector<MenuItem> m_Items;
};

} // namespace Zgine::UI::Widgets
