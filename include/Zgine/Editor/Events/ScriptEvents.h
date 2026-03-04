#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/Scene/Core/Entity.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when script reload is requested
 */
class ScriptReloadEvent : public EditorEvent {
public:
    explicit ScriptReloadEvent(Entity entity, EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_Entity(entity) {}

    Entity GetEntity() const noexcept { return m_Entity; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ScriptReloadEvent [ID:" << GetEventID()
           << ", Entity:" << static_cast<uint32_t>(m_Entity) << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ScriptReloadEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Script)

private:
    Entity m_Entity;
};

} // namespace Zgine
