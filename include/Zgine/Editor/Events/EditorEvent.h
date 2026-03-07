#pragma once

#include <Zgine/Core/Base/Macro.h>
#include <string>
#include <string_view>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <chrono>
#include <atomic>
#include <cstdint>

namespace Zgine {

/**
 * @brief Event priority for ordering and scheduling
 *
 * Higher priority events may be processed before lower priority ones
 * in some event bus implementations.
 */
enum class EditorEventPriority : uint8_t {
    Low = 0,        ///< Low priority events (UI updates, cosmetic changes)
    Normal = 1,     ///< Normal priority (most events)
    High = 2,       ///< High priority (user actions, critical updates)
    Critical = 3    ///< Critical priority (errors, system events)
};

/**
 * @brief Event categories for filtering (bit flags)
 *
 * Multiple categories can be combined using bitwise OR.
 * Use BIT() macro for consistent bit flag definitions.
 */
enum class EditorEventCategory : uint32_t {
    None        = 0,
    Entity      = BIT(0),   ///< Entity lifecycle (create, delete, modify)
    World       = BIT(1),   ///< World operations (load, save, new)
    Selection   = BIT(2),   ///< Selection changes
    Transform   = BIT(3),   ///< Transform modifications
    Viewport    = BIT(4),   ///< Viewport state changes
    Editor      = BIT(5),   ///< General editor state
    Component   = BIT(6),   ///< Component add/remove/modify
    Asset       = BIT(7),   ///< Asset import/delete/modify
    Project     = BIT(8),   ///< Project operations
    UI          = BIT(9),   ///< UI events (panel resize, dock changes)
    Input       = BIT(10),  ///< Input events
    Render      = BIT(11),  ///< Rendering events
    Physics     = BIT(12),  ///< Physics events
    Audio       = BIT(13),  ///< Audio events
    Script      = BIT(14),  ///< Script events
    Network     = BIT(15),  ///< Network events
    Debug       = BIT(16),  ///< Debug/profiling events
    Undo        = BIT(17),  ///< Undo/Redo events
    Settings    = BIT(18),  ///< Settings changes
    Build       = BIT(19)   ///< Build/compilation events
};

/// Bitwise OR operator for event categories
inline EditorEventCategory operator|(EditorEventCategory a, EditorEventCategory b) {
    return static_cast<EditorEventCategory>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

/// Bitwise AND operator for event categories
inline EditorEventCategory operator&(EditorEventCategory a, EditorEventCategory b) {
    return static_cast<EditorEventCategory>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

/**
 * @brief Base class for all editor events
 *
 * Events represent immutable snapshots of what happened in the editor.
 * They are published through EditorEventBus and consumed by subscribers.
 *
 * **Thread Safety**: Events should be treated as immutable after construction.
 * **Lifetime**: Events are typically short-lived (frame or callback scope).
 * **Performance**: Events are lightweight and support move semantics.
 *
 * @note Derive from this class and use EDITOR_EVENT_CLASS_TYPE and
 *       EDITOR_EVENT_CLASS_CATEGORY macros to define concrete events.
 */
class EditorEvent {
public:
    using EventID = uint64_t;
    using TimePoint = std::chrono::steady_clock::time_point;

    virtual ~EditorEvent() = default;

    // Non-copyable but movable for performance
    ZGINE_NON_COPYABLE(EditorEvent)
    EditorEvent(EditorEvent&&) noexcept = default;
    EditorEvent& operator=(EditorEvent&&) noexcept = default;

    //=========================================================================
    // Type Information
    //=========================================================================

    /// Get the event category (may be multiple categories combined)
    virtual EditorEventCategory GetCategory() const noexcept = 0;

    /// Get the human-readable event name (zero-copy view)
    virtual std::string_view GetName() const noexcept = 0;

    /// Get the runtime type information for this event
    virtual std::type_index GetType() const noexcept = 0;

    //=========================================================================
    // Event Metadata
    //=========================================================================

    /// Get unique event ID (sequential, thread-safe)
    EventID GetEventID() const noexcept { return m_EventID; }

    /// Get event timestamp (time point when event was created)
    TimePoint GetTimestamp() const noexcept { return m_Timestamp; }

    /// Get event priority for scheduling
    EditorEventPriority GetPriority() const noexcept { return m_Priority; }

    /// Get elapsed time in seconds since event creation
    double GetElapsedSeconds() const noexcept {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - m_Timestamp).count();
    }

    /// Get elapsed time in milliseconds since event creation
    double GetElapsedMilliseconds() const noexcept {
        return GetElapsedSeconds() * 1000.0;
    }

    //=========================================================================
    // Handling State
    //=========================================================================

    /// Check if this event has been handled
    bool IsHandled() const noexcept { return m_Handled; }

    /// Mark this event as handled to prevent further processing
    void SetHandled(bool handled = true) noexcept { m_Handled = handled; }

    //=========================================================================
    // Debug & Logging
    //=========================================================================

    /**
     * @brief Get string representation for debugging/logging
     * @return Human-readable event description
     */
    virtual std::string ToString() const {
        std::string result;
        result.reserve(GetName().size() + 20); // Pre-allocate
        result += GetName();
        result += " [ID:";
        result += std::to_string(m_EventID);
        result += "]";
        return result;
    }

    /**
     * @brief Check if event belongs to a specific category
     * @param category Category to test (can be multiple categories OR'd together)
     * @return true if event has at least one of the specified categories
     */
    bool InCategory(EditorEventCategory category) const noexcept {
        return (static_cast<uint32_t>(GetCategory()) &
                static_cast<uint32_t>(category)) != 0;
    }

protected:
    /**
     * @brief Protected constructor for derived classes
     * @param priority Event priority (default: Normal)
     */
    explicit EditorEvent(EditorEventPriority priority = EditorEventPriority::Normal)
        : m_EventID(GenerateEventID())
        , m_Timestamp(std::chrono::steady_clock::now())
        , m_Priority(priority)
        , m_Handled(false)
    {}

private:
    /// Generate unique, thread-safe event IDs
    static EventID GenerateEventID() {
        static std::atomic<EventID> s_NextID{1};
        return s_NextID.fetch_add(1, std::memory_order_relaxed);
    }

    EventID m_EventID;
    TimePoint m_Timestamp;
    EditorEventPriority m_Priority;
    bool m_Handled;
};

//=============================================================================
// Helper Macros
//=============================================================================

/**
 * @brief Define static type information for an event class
 *
 * This macro generates:
 * - GetStaticType() for compile-time type queries
 * - GetType() override for runtime type identification
 * - GetName() override for human-readable event names (zero-copy string_view)
 *
 * @param type The event class name
 *
 * @example
 * class MyEvent : public EditorEvent {
 * public:
 *     ZGINE_EDITOR_EVENT_CLASS_TYPE(MyEvent)
 *     // ...
 * };
 */
#define ZGINE_EDITOR_EVENT_CLASS_TYPE(type) \
    static std::type_index GetStaticType() noexcept { \
        return std::type_index(typeid(type)); \
    } \
    virtual std::type_index GetType() const noexcept override { \
        return GetStaticType(); \
    } \
    virtual std::string_view GetName() const noexcept override { \
        return #type; \
    }

/**
 * @brief Define event category
 *
 * @param category EditorEventCategory enum value (without EditorEventCategory:: prefix)
 *
 * @example
 * ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Entity)
 */
#define ZGINE_EDITOR_EVENT_CLASS_CATEGORY(category) \
    virtual EditorEventCategory GetCategory() const noexcept override { \
        return EditorEventCategory::category; \
    }

/**
 * @brief Use default ToString() implementation from base class
 *
 * Default format: "EventName [ID:123]"
 * For events that don't need custom ToString(), this macro makes the intention explicit.
 *
 * @example
 * class SimpleEvent : public EditorEvent {
 * public:
 *     ZGINE_EDITOR_EVENT_CLASS_TYPE(SimpleEvent)
 *     ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Editor)
 *     ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
 * };
 */
#define ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING() \
    /* Use base class default ToString() implementation */

/**
 * @brief Generate standard ToString() implementation
 *
 * Generates: "EventName [ID:123]" format using GetName() and GetEventID()
 * This is functionally equivalent to the default, but makes it explicit in code.
 *
 * @note This is now redundant with ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING,
 *       but kept for backward compatibility and explicit intent.
 */
#define ZGINE_EDITOR_EVENT_TOSTRING() \
    std::string ToString() const override { \
        std::string result; \
        result.reserve(GetName().size() + 20); \
        result += GetName(); \
        result += " [ID:"; \
        result += std::to_string(GetEventID()); \
        result += "]"; \
        return result; \
    }

// Backward compatibility aliases
#define EDITOR_EVENT_CLASS_TYPE(type) ZGINE_EDITOR_EVENT_CLASS_TYPE(type)
#define EDITOR_EVENT_CLASS_CATEGORY(category) ZGINE_EDITOR_EVENT_CLASS_CATEGORY(category)
#define EDITOR_EVENT_DEFAULT_TOSTRING() ZGINE_EDITOR_EVENT_DEFAULT_TOSTRING()
#define EDITOR_EVENT_TOSTRING() ZGINE_EDITOR_EVENT_TOSTRING()

} // namespace Zgine
