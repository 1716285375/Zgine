#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <Zgine/World/Core/Entity.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when audio action is requested
 */
class AudioActionEvent : public EditorEvent {
public:
    AudioActionEvent(Entity entity, AudioAction action)
        : m_Entity(entity), m_Action(action) {}

    Entity GetEntity() const noexcept { return m_Entity; }
    AudioAction GetAction() const noexcept { return m_Action; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "AudioActionEvent [ID:" << GetEventID()
           << ", Entity:" << m_Entity.GetHandle().GetValue()
           << ", Action:" << static_cast<int>(m_Action) << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(AudioActionEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Audio)

private:
    Entity m_Entity;
    AudioAction m_Action;
};

} // namespace Zgine
