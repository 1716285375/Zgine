#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <sstream>
#include <string>

namespace Zgine {

/**
 * @brief Event fired when undo is performed
 */
class UndoPerformedEvent : public EditorEvent {
public:
    explicit UndoPerformedEvent(const std::string& commandName = "",
                               EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_CommandName(commandName) {}

    const std::string& GetCommandName() const noexcept { return m_CommandName; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "UndoPerformedEvent [ID:" << GetEventID();
        if (!m_CommandName.empty()) {
            ss << ", Command:" << m_CommandName;
        }
        ss << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(UndoPerformedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Undo)

private:
    std::string m_CommandName;
};

/**
 * @brief Event fired when redo is performed
 */
class RedoPerformedEvent : public EditorEvent {
public:
    explicit RedoPerformedEvent(const std::string& commandName = "",
                               EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_CommandName(commandName) {}

    const std::string& GetCommandName() const noexcept { return m_CommandName; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "RedoPerformedEvent [ID:" << GetEventID();
        if (!m_CommandName.empty()) {
            ss << ", Command:" << m_CommandName;
        }
        ss << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(RedoPerformedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Undo)

private:
    std::string m_CommandName;
};

} // namespace Zgine
