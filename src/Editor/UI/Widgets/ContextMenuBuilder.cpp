#include <Zgine/Editor/UI/Widgets/ContextMenuBuilder.h>

namespace Zgine::UI::Widgets {

MenuBuilder& MenuBuilder::AddItem(
    const char* label,
    std::function<void()> action,
    bool enabled,
    const char* shortcut)
{
    MenuItem item;
    item.type = MenuItem::Type::Item;
    item.label = label;
    item.action = std::move(action);
    item.enabled = enabled;
    if (shortcut) {
        item.shortcut = shortcut;
    }
    m_Items.push_back(std::move(item));
    return *this;
}

MenuBuilder& MenuBuilder::AddSeparator() {
    MenuItem item;
    item.type = MenuItem::Type::Separator;
    m_Items.push_back(std::move(item));
    return *this;
}

MenuBuilder& MenuBuilder::AddSubmenu(
    const char* label,
    std::function<void(MenuBuilder&)> buildSubmenu,
    bool enabled)
{
    MenuItem item;
    item.type = MenuItem::Type::Submenu;
    item.label = label;
    item.submenuBuilder = std::move(buildSubmenu);
    item.enabled = enabled;
    m_Items.push_back(std::move(item));
    return *this;
}

MenuBuilder& MenuBuilder::AddCheckbox(
    const char* label,
    bool* checked,
    std::function<void()> action,
    bool enabled)
{
    MenuItem item;
    item.type = MenuItem::Type::Checkbox;
    item.label = label;
    item.checked = checked;
    item.action = std::move(action);
    item.enabled = enabled;
    m_Items.push_back(std::move(item));
    return *this;
}

bool MenuBuilder::Render(const char* id, ImGuiPopupFlags flags) {
    if (!ImGui::BeginPopup(id, flags)) {
        return false;
    }

    RenderItems();
    ImGui::EndPopup();
    return true;
}

bool MenuBuilder::RenderContextMenu(const char* id, ImGuiPopupFlags flags) {
    if (!ImGui::BeginPopupContextItem(id, flags)) {
        return false;
    }

    RenderItems();
    ImGui::EndPopup();
    return true;
}

void MenuBuilder::Clear() {
    m_Items.clear();
}

void MenuBuilder::RenderItems() {
    for (auto& item : m_Items) {
        switch (item.type) {
            case MenuItem::Type::Item: {
                ImGui::BeginDisabled(!item.enabled);
                const char* shortcut = item.shortcut.empty() ? nullptr : item.shortcut.c_str();
                if (ImGui::MenuItem(item.label.c_str(), shortcut)) {
                    if (item.action) {
                        item.action();
                    }
                }
                ImGui::EndDisabled();
                break;
            }

            case MenuItem::Type::Separator:
                ImGui::Separator();
                break;

            case MenuItem::Type::Submenu:
                ImGui::BeginDisabled(!item.enabled);
                if (ImGui::BeginMenu(item.label.c_str())) {
                    if (item.submenuBuilder) {
                        MenuBuilder submenu;
                        item.submenuBuilder(submenu);
                        submenu.RenderItems();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndDisabled();
                break;

            case MenuItem::Type::Checkbox:
                ImGui::BeginDisabled(!item.enabled);
                if (item.checked && ImGui::MenuItem(item.label.c_str(), nullptr, item.checked)) {
                    if (item.action) {
                        item.action();
                    }
                }
                ImGui::EndDisabled();
                break;
        }
    }
}

} // namespace Zgine::UI::Widgets
