#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <filesystem>
#include <sstream>

namespace Zgine {

// Forward declaration
class Scene;

/**
 * @brief Event fired when a new scene is created
 */
class SceneCreatedEvent : public EditorEvent {
public:
    explicit SceneCreatedEvent(Scene* scene, EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_Scene(scene) {}

    Scene* GetScene() const noexcept { return m_Scene; }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneCreatedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Scene)
    ZGINE_EDITOR_EVENT_TOSTRING()

private:
    Scene* m_Scene;
};

/**
 * @brief Event fired when a scene is saved
 */
class SceneSavedEvent : public EditorEvent {
public:
    SceneSavedEvent(Scene* scene, const std::filesystem::path& path)
        : m_Scene(scene), m_Path(path) {}

    Scene* GetScene() const noexcept { return m_Scene; }
    const std::filesystem::path& GetPath() const noexcept { return m_Path; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "SceneSavedEvent [ID:" << GetEventID()
           << ", Path:" << m_Path.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneSavedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Scene)

private:
    Scene* m_Scene;
    std::filesystem::path m_Path;
};

/**
 * @brief Event fired when a scene is loaded
 */
class SceneLoadedEvent : public EditorEvent {
public:
    SceneLoadedEvent(Scene* scene, const std::filesystem::path& path,
                     EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_Scene(scene), m_Path(path) {}

    Scene* GetScene() const noexcept { return m_Scene; }
    const std::filesystem::path& GetPath() const noexcept { return m_Path; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "SceneLoadedEvent [ID:" << GetEventID()
           << ", Path:" << m_Path.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneLoadedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Scene)

private:
    Scene* m_Scene;
    std::filesystem::path m_Path;
};

/**
 * @brief Event fired when the scene has been modified
 */
class SceneModifiedEvent : public EditorEvent {
public:
    explicit SceneModifiedEvent(Scene* scene = nullptr, EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority), m_Scene(scene) {}

    Scene* GetScene() const noexcept { return m_Scene; }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(SceneModifiedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Scene)
    ZGINE_EDITOR_EVENT_TOSTRING()

private:
    Scene* m_Scene;
};

} // namespace Zgine
