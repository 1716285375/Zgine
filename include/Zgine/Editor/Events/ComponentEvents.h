#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/World/Core/Entity.h>
#include <sstream>
#include <typeindex>

namespace Zgine {

/**
 * @brief Event fired when a component is added to an entity
 */
class ComponentAddedEvent : public EditorEvent {
public:
    ComponentAddedEvent(Entity entity, std::type_index componentType)
        : m_Entity(entity), m_ComponentType(componentType) {}

    Entity GetEntity() const noexcept { return m_Entity; }
    std::type_index GetComponentType() const noexcept { return m_ComponentType; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ComponentAddedEvent [ID:" << GetEventID()
           << ", Entity:" << static_cast<uint32_t>(m_Entity)
           << ", Component:" << m_ComponentType.name() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ComponentAddedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Component)

private:
    Entity m_Entity;
    std::type_index m_ComponentType;
};

/**
 * @brief Event fired when a component is removed from an entity
 */
class ComponentRemovedEvent : public EditorEvent {
public:
    ComponentRemovedEvent(Entity entity, std::type_index componentType)
        : m_Entity(entity), m_ComponentType(componentType) {}

    Entity GetEntity() const noexcept { return m_Entity; }
    std::type_index GetComponentType() const noexcept { return m_ComponentType; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "ComponentRemovedEvent [ID:" << GetEventID()
           << ", Entity:" << static_cast<uint32_t>(m_Entity)
           << ", Component:" << m_ComponentType.name() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(ComponentRemovedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Component)

private:
    Entity m_Entity;
    std::type_index m_ComponentType;
};

} // namespace Zgine
