#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <sstream>
#include <string>

namespace Zgine {

/**
 * @brief Event fired when a panel is docked/undocked
 */
class PanelLayoutChangedEvent : public EditorEvent {
public:
    explicit PanelLayoutChangedEvent(EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority) {}

    ZGINE_EDITOR_EVENT_CLASS_TYPE(PanelLayoutChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(UI)
    ZGINE_EDITOR_EVENT_TOSTRING()
};

/**
 * @brief Event fired when theme changes
 */
class ThemeChangedEvent : public EditorEvent {
public:
    explicit ThemeChangedEvent(const std::string& themeName) : m_ThemeName(themeName) {}

    const std::string& GetThemeName() const noexcept { return m_ThemeName; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ThemeChangedEvent [ID:" << GetEventID()
           << ", Theme:" << m_ThemeName << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ThemeChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(UI | Settings)

private:
    std::string m_ThemeName;
};

} // namespace Zgine
