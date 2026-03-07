#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <filesystem>
#include <sstream>

namespace Zgine {

// Forward declaration
class World;

/**
 * @brief Event fired when a new World is created
 */
class SceneCreatedEvent : public EditorEvent {
public:
    explicit SceneCreatedEvent(World* World, EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_World(World) {}

    World* GetScene() const noexcept { return m_World; }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneCreatedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(World)
    ZGINE_EDITOR_EVENT_TOSTRING()

private:
    World* m_World;
};

/**
 * @brief Event fired when a World is saved
 */
class SceneSavedEvent : public EditorEvent {
public:
    SceneSavedEvent(World* World, const std::filesystem::path& path)
        : m_World(World), m_Path(path) {}

    World* GetScene() const noexcept { return m_World; }
    const std::filesystem::path& GetPath() const noexcept { return m_Path; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "SceneSavedEvent [ID:" << GetEventID()
           << ", Path:" << m_Path.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneSavedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(World)

private:
    World* m_World;
    std::filesystem::path m_Path;
};

/**
 * @brief Event fired when a World is loaded
 */
class SceneLoadedEvent : public EditorEvent {
public:
    SceneLoadedEvent(World* World, const std::filesystem::path& path,
                     EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_World(World), m_Path(path) {}

    World* GetScene() const noexcept { return m_World; }
    const std::filesystem::path& GetPath() const noexcept { return m_Path; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "SceneLoadedEvent [ID:" << GetEventID()
           << ", Path:" << m_Path.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneLoadedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(World)

private:
    World* m_World;
    std::filesystem::path m_Path;
};

/**
 * @brief Event fired when the World has been modified
 */
class SceneModifiedEvent : public EditorEvent {
public:
    explicit SceneModifiedEvent(World* World = nullptr, EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority), m_World(World) {}

    World* GetScene() const noexcept { return m_World; }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneModifiedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(World)
    ZGINE_EDITOR_EVENT_TOSTRING()

private:
    World* m_World;
};

} // namespace Zgine
