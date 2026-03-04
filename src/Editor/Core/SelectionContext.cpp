#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/SelectionEvents.h>
#include <Zgine/Scene/Core/Scene.h>
#include <algorithm>

namespace Zgine {

SelectionContext::SelectionContext(EditorEventBus* eventBus)
    : m_EventBus(eventBus)
{
}

void SelectionContext::Select(Entity entity, SelectionMode mode) {
    if (!entity) {
        Clear();
        return;
    }

    switch (mode) {
        case SelectionMode::Replace:
            m_Selection.clear();
            m_Selection.push_back(entity);
            m_Primary = entity;
            PublishSelectionChanged();
            break;

        case SelectionMode::Add:
            Add(entity);
            break;

        case SelectionMode::Subtract:
            Remove(entity);
            break;

        case SelectionMode::Toggle:
            Toggle(entity);
            break;
    }
}

void SelectionContext::Select(const std::vector<Entity>& entities, SelectionMode mode) {
    if (entities.empty()) {
        if (mode == SelectionMode::Replace) {
            Clear();
        }
        return;
    }

    switch (mode) {
        case SelectionMode::Replace:
            m_Selection = entities;
            m_Primary = entities.empty() ? Entity{} : entities[0];
            PublishSelectionChanged();
            break;

        case SelectionMode::Add:
            for (Entity e : entities) {
                if (!IsSelected(e)) {
                    m_Selection.push_back(e);
                }
            }
            if (!m_Primary && !m_Selection.empty()) {
                m_Primary = m_Selection[0];
            }
            PublishSelectionChanged();
            break;

        case SelectionMode::Subtract:
            for (Entity e : entities) {
                Remove(e);
            }
            break;

        case SelectionMode::Toggle:
            for (Entity e : entities) {
                Toggle(e);
            }
            break;
    }
}

void SelectionContext::Clear() {
    if (m_Selection.empty()) {
        return; // Already clear
    }

    m_Selection.clear();
    m_Primary = Entity{};
    PublishDeselected();
}

void SelectionContext::SetPrimary(Entity entity) {
    if (!entity) {
        return;
    }

    // Add to selection if not already selected
    if (!IsSelected(entity)) {
        m_Selection.push_back(entity);
    }

    m_Primary = entity;
    PublishSelectionChanged();
}

void SelectionContext::Add(Entity entity) {
    if (!entity || IsSelected(entity)) {
        return;
    }

    m_Selection.push_back(entity);

    // If no primary, set this as primary
    if (!m_Primary) {
        m_Primary = entity;
    }

    PublishSelectionChanged();
}

void SelectionContext::Remove(Entity entity) {
    auto it = std::find(m_Selection.begin(), m_Selection.end(), entity);
    if (it == m_Selection.end()) {
        return; // Not selected
    }

    m_Selection.erase(it);

    // If removed entity was primary, update primary
    if (m_Primary == entity) {
        m_Primary = m_Selection.empty() ? Entity{} : m_Selection[0];
    }

    if (m_Selection.empty()) {
        PublishDeselected();
    } else {
        PublishSelectionChanged();
    }
}

void SelectionContext::Toggle(Entity entity) {
    if (IsSelected(entity)) {
        Remove(entity);
    } else {
        Add(entity);
    }
}

bool SelectionContext::IsSelected(Entity entity) const {
    return std::find(m_Selection.begin(), m_Selection.end(), entity) != m_Selection.end();
}

void SelectionContext::Validate(Scene* scene) {
    if (!scene) {
        Clear();
        return;
    }

    // Remove entities that no longer exist
    m_Selection.erase(
        std::remove_if(m_Selection.begin(), m_Selection.end(),
            [scene](Entity e) {
                return !scene->IsEntityValid(e);
            }),
        m_Selection.end()
    );

    // Validate primary
    if (m_Primary && !scene->IsEntityValid(m_Primary)) {
        m_Primary = m_Selection.empty() ? Entity{} : m_Selection[0];
    }
}

void SelectionContext::PublishSelectionChanged() {
    if (!m_PublishEvents || !m_EventBus || !m_Primary) {
        return;
    }

    // Publish EntitySelectedEvent with primary entity
    EntitySelectedEvent event(m_Primary);
    m_EventBus->PublishImmediate(event);
}

void SelectionContext::PublishDeselected() {
    if (!m_PublishEvents || !m_EventBus) {
        return;
    }

    // Publish EntityDeselectedEvent
    EntityDeselectedEvent event;
    m_EventBus->PublishImmediate(event);
}

} // namespace Zgine
