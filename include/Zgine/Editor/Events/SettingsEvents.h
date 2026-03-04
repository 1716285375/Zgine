#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <sstream>
#include <string>

namespace Zgine {

/**
 * @brief Event fired when editor settings change
 */
class SettingsChangedEvent : public EditorEvent {
public:
    explicit SettingsChangedEvent(const std::string& settingKey = "")
        : m_SettingKey(settingKey) {}

    const std::string& GetSettingKey() const noexcept { return m_SettingKey; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "SettingsChangedEvent [ID:" << GetEventID();
        if (!m_SettingKey.empty()) {
            ss << ", Key:" << m_SettingKey;
        }
        ss << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SettingsChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Settings)

private:
    std::string m_SettingKey;
};

} // namespace Zgine
