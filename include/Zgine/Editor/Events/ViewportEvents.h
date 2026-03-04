#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when viewport size changes
 */
class ViewportResizedEvent : public EditorEvent {
public:
    ViewportResizedEvent(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {}

    uint32_t GetWidth() const noexcept { return m_Width; }
    uint32_t GetHeight() const noexcept { return m_Height; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ViewportResizedEvent [ID:" << GetEventID()
           << ", Size:" << m_Width << "x" << m_Height << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ViewportResizedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Viewport)

private:
    uint32_t m_Width;
    uint32_t m_Height;
};

/**
 * @brief Event fired when viewport focus changes
 */
class ViewportFocusChangedEvent : public EditorEvent {
public:
    explicit ViewportFocusChangedEvent(bool focused) : m_Focused(focused) {}

    bool IsFocused() const noexcept { return m_Focused; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ViewportFocusChangedEvent [ID:" << GetEventID()
           << ", Focused:" << (m_Focused ? "true" : "false") << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ViewportFocusChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Viewport)

private:
    bool m_Focused;
};

} // namespace Zgine
