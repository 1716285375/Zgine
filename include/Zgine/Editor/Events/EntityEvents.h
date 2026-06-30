#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <sstream>

namespace Zgine {

/**
 * @brief Event fired when an entity is created
 */
class EntityCreatedEvent : public EditorEvent {
public:
    EntityCreatedEvent(Entity entity, PrimitiveType type = PrimitiveType::None,
                       EditorEventPriority priority = EditorEventPriority::Normal)
        : EditorEvent(priority), m_Entity(entity), m_PrimitiveType(type) {}

    Entity GetEntity() const noexcept { return m_Entity; }
    PrimitiveType GetPrimitiveType() const noexcept { return m_PrimitiveType; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EntityCreatedEvent [ID:" << GetEventID()
           << ", Entity:" << m_Entity.GetHandle().GetValue() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EntityCreatedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Entity)

private:
    Entity m_Entity;
    PrimitiveType m_PrimitiveType;
};

/**
 * @brief Event fired when an entity is about to be deleted
 */
class EntityDeletedEvent : public EditorEvent {
public:
    explicit EntityDeletedEvent(Entity entity, EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_Entity(entity) {}

    Entity GetEntity() const noexcept { return m_Entity; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EntityDeletedEvent [ID:" << GetEventID()
           << ", Entity:" << m_Entity.GetHandle().GetValue() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EntityDeletedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Entity)

private:
    Entity m_Entity;
};

/**
 * @brief Event fired after an entity has been destroyed
 */
class EntityDestroyedEvent : public EditorEvent {
public:
    explicit EntityDestroyedEvent(Entity entity) : m_Entity(entity) {}

    Entity GetEntity() const noexcept { return m_Entity; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EntityDestroyedEvent [ID:" << GetEventID()
           << ", Entity:" << m_Entity.GetHandle().GetValue() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EntityDestroyedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Entity)

private:
    Entity m_Entity;
};

} // namespace Zgine
