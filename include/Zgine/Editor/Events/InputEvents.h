#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when gizmo mode changes
 */
class GizmoModeChangedEvent : public EditorEvent {
public:
    explicit GizmoModeChangedEvent(GizmoMode mode) : m_Mode(mode) {}

    GizmoMode GetMode() const noexcept { return m_Mode; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "GizmoModeChangedEvent [ID:" << GetEventID() << ", Mode:";
        switch (m_Mode) {
            case GizmoMode::None:      ss << "None"; break;
            case GizmoMode::Translate: ss << "Translate"; break;
            case GizmoMode::Rotate:    ss << "Rotate"; break;
            case GizmoMode::Scale:     ss << "Scale"; break;
            default: ss << "Unknown"; break;
        }
        ss << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(GizmoModeChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Input)

private:
    GizmoMode m_Mode;
};

} // namespace Zgine
