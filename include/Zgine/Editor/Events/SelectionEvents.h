#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/Scene/Core/Entity.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when an entity is selected
 */
class EntitySelectedEvent : public EditorEvent {
public:
    explicit EntitySelectedEvent(Entity entity) : m_Entity(entity) {}

    Entity GetEntity() const noexcept { return m_Entity; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EntitySelectedEvent [ID:" << GetEventID()
           << ", Entity:" << static_cast<uint32_t>(m_Entity) << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EntitySelectedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Selection)

private:
    Entity m_Entity;
};

/**
 * @brief Event fired when entity selection is cleared
 */
class EntityDeselectedEvent : public EditorEvent {
public:
    EntityDeselectedEvent() = default;

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EntityDeselectedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Selection)
    ZGINE_EDITOR_EVENT_TOSTRING()
};

} // namespace Zgine
