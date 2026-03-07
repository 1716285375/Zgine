#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <sstream>
#include <string>

namespace Zgine {

//=============================================================================
// Editor Lifecycle Events
//=============================================================================

/**
 * @brief Event fired when editor has finished initialization
 *
 * **When**: After all subsystems initialized, before first frame
 * **Who listens**: Plugins, Extension systems
 * **Priority**: High
 */
class EditorInitializedEvent : public EditorEvent {
public:
    explicit EditorInitializedEvent(EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority) {}

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorInitializedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

/**
 * @brief Event fired when editor is about to shut down
 *
 * **When**: Before subsystems cleanup, after last frame
 * **Who listens**: All systems for cleanup
 * **Priority**: Critical
 */
class EditorShutdownEvent : public EditorEvent {
public:
    explicit EditorShutdownEvent(EditorEventPriority priority = EditorEventPriority::Critical)
        : EditorEvent(priority) {}

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorShutdownEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

//=============================================================================
// Editor State Events
//=============================================================================

/**
 * @brief Event fired when editor play mode changes
 *
 * **When**: User presses Play/Pause/Stop buttons
 * **Who listens**: All systems, Viewport, Panels
 * **Priority**: Critical (affects all systems)
 */
class PlayModeChangedEvent : public EditorEvent {
public:
    explicit PlayModeChangedEvent(EditorMode mode, EditorEventPriority priority = EditorEventPriority::Critical)
        : EditorEvent(priority), m_Mode(mode) {}

    EditorMode GetMode() const noexcept { return m_Mode; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "PlayModeChangedEvent [ID:" << GetEventID() << ", Mode:";
        switch (m_Mode) {
            case EditorMode::Edit:  ss << "Edit"; break;
            case EditorMode::Play:  ss << "Play"; break;
            case EditorMode::Pause: ss << "Pause"; break;
            default: ss << "Unknown"; break;
        }
        ss << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(PlayModeChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)

private:
    EditorMode m_Mode;
};

/**
 * @brief Event fired when editor focus changes
 *
 * **When**: Editor window gains or loses focus
 * **Who listens**: Input system, Auto-save system
 * **Priority**: Normal
 */
class EditorFocusChangedEvent : public EditorEvent {
public:
    explicit EditorFocusChangedEvent(bool focused) : m_Focused(focused) {}

    bool IsFocused() const noexcept { return m_Focused; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EditorFocusChangedEvent [ID:" << GetEventID()
           << ", Focused:" << (m_Focused ? "true" : "false") << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorFocusChangedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)

private:
    bool m_Focused;
};

//=============================================================================
// Editor Command Events
//=============================================================================

/**
 * @brief Event fired when a command is executed
 *
 * **When**: User executes command (menu, shortcut, etc.)
 * **Who listens**: Command history, Undo system
 * **Priority**: Normal
 */
class CommandExecutedEvent : public EditorEvent {
public:
    explicit CommandExecutedEvent(const std::string& commandName)
        : m_CommandName(commandName) {}

    const std::string& GetCommandName() const noexcept { return m_CommandName; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "CommandExecutedEvent [ID:" << GetEventID()
           << ", Command:" << m_CommandName << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(CommandExecutedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)

private:
    std::string m_CommandName;
};

//=============================================================================
// Editor Window Events
//=============================================================================

/**
 * @brief Event fired when editor window is minimized
 *
 * **When**: User minimizes editor window
 * **Who listens**: Render system (can pause), Auto-save
 * **Priority**: Normal
 */
class EditorMinimizedEvent : public EditorEvent {
public:
    EditorMinimizedEvent() = default;

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorMinimizedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

/**
 * @brief Event fired when editor window is restored from minimized
 *
 * **When**: User restores editor window
 * **Who listens**: Render system (resume)
 * **Priority**: Normal
 */
class EditorRestoredEvent : public EditorEvent {
public:
    EditorRestoredEvent() = default;

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorRestoredEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

/**
 * @brief Event fired when editor window is maximized
 *
 * **When**: User maximizes editor window
 * **Who listens**: Layout manager, Panels
 * **Priority**: Low
 */
class EditorMaximizedEvent : public EditorEvent {
public:
    explicit EditorMaximizedEvent(EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority) {}

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorMaximizedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

//=============================================================================
// Editor Performance Events
//=============================================================================

/**
 * @brief Event fired when editor enters low performance mode
 *
 * **When**: FPS drops below threshold
 * **Who listens**: Render system, Profiler
 * **Priority**: High
 */
class LowPerformanceDetectedEvent : public EditorEvent {
public:
    explicit LowPerformanceDetectedEvent(float fps, EditorEventPriority priority = EditorEventPriority::High)
        : EditorEvent(priority), m_CurrentFPS(fps) {}

    float GetCurrentFPS() const noexcept { return m_CurrentFPS; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "LowPerformanceDetectedEvent [ID:" << GetEventID()
           << ", FPS:" << m_CurrentFPS << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(LowPerformanceDetectedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)

private:
    float m_CurrentFPS;
};

//=============================================================================
// Editor Frame Events
//=============================================================================

/**
 * @brief Event fired at the beginning of editor frame
 *
 * **When**: Start of each editor update cycle
 * **Who listens**: Systems that need to update each frame
 * **Priority**: Low (frequent event)
 */
class EditorFrameBeginEvent : public EditorEvent {
public:
    explicit EditorFrameBeginEvent(float deltaTime, EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority), m_DeltaTime(deltaTime) {}

    float GetDeltaTime() const noexcept { return m_DeltaTime; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "EditorFrameBeginEvent [ID:" << GetEventID()
           << ", DeltaTime:" << m_DeltaTime << "s]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorFrameBeginEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)

private:
    float m_DeltaTime;
};

/**
 * @brief Event fired at the end of editor frame
 *
 * **When**: End of each editor update cycle
 * **Who listens**: Profiler, Metrics collector
 * **Priority**: Low (frequent event)
 */
class EditorFrameEndEvent : public EditorEvent {
public:
    explicit EditorFrameEndEvent(EditorEventPriority priority = EditorEventPriority::Low)
        : EditorEvent(priority) {}

    ZGINE_EDITOR_EVENT_CLASS_TYPE(EditorFrameEndEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
    ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
};

} // namespace Zgine
