#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/World/Core/Entity.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when an entity's transform is modified
 */
class TransformChangedEvent : public EditorEvent {
public:
    explicit TransformChangedEvent(Entity entity) : m_Entity(entity) {}

    Entity GetEntity() const noexcept { return m_Entity; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "TransformChangedEvent [ID:" << GetEventID()
           << ", Entity:" << m_Entity.GetHandle().GetValue() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(TransformChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Transform)

private:
    Entity m_Entity;
};

} // namespace Zgine
