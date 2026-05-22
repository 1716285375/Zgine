#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <mutex>
#include <type_traits>
#include <utility>

namespace Zgine {

/**
 * @brief Event listener handle for unsubscribing
 */
using ListenerID = size_t;

/**
 * @brief Event callback function type
 */
using EventCallback = std::function<void(EditorEvent&)>;

/**
 * @brief Event bus for publish-subscribe pattern
 *
 * The EditorEventBus provides a centralized event system for
 * the editor. It supports:
 * - Type-safe event subscriptions
 * - Event queuing and deferred dispatch
 * - Immediate event dispatch
 * - Priority-based event handling
 * - Thread-safe event publishing
 *
 * Design Pattern: Observer/Pub-Sub
 *
 * Usage:
 * @code
 *   // Subscribe to events
 *   auto id = eventBus.Subscribe<EntitySelectedEvent>([](EditorEvent& e) {
 *       auto& event = static_cast<EntitySelectedEvent&>(e);
 *       // Handle event...
 *   });
 *
 *   // Publish immediate event
 *   eventBus.PublishImmediate(EntitySelectedEvent(entity));
 *
 *   // Queue event for later
 *   eventBus.Publish(EntityCreatedEvent(entity));
 *   // ... later ...
 *   eventBus.DispatchEvents();
 *
 *   // Unsubscribe
 *   eventBus.Unsubscribe(id);
 * @endcode
 */
class EditorEventBus {
public:
    EditorEventBus();
    ~EditorEventBus();

    /**
     * @brief Subscribe to events of a specific type
     * @tparam T Event type to subscribe to
     * @param callback Function to call when event is published
     * @return Listener ID for unsubscribing
     */
    template<typename T>
    ListenerID Subscribe(std::function<void(T&)> callback) {
        static_assert(std::is_base_of_v<EditorEvent, T>, "T must derive from EditorEvent");

        std::type_index typeIndex = T::GetStaticType();
        ListenerID id = m_NextListenerID++;

        // Wrap the typed callback in a generic callback
        EventCallback genericCallback = [callback](EditorEvent& event) {
            callback(static_cast<T&>(event));
        };

        m_Listeners[typeIndex].emplace_back(id, genericCallback);
        return id;
    }

    /**
     * @brief Unsubscribe from events
     * @param id Listener ID returned from Subscribe()
     */
    void Unsubscribe(ListenerID id);

    /**
     * @brief Publish event to queue for deferred dispatch
     * @param event Event to publish (will be moved into the queue)
     */
    template<typename T>
    void Publish(T&& event) {
        using EventType = std::remove_cv_t<std::remove_reference_t<T>>;
        static_assert(std::is_base_of_v<EditorEvent, EventType>, "T must derive from EditorEvent");

        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_EventQueue.push(std::make_unique<EventType>(std::forward<T>(event)));
    }

    /**
     * @brief Publish event immediately (not queued)
     * @param event Event to publish
     */
    template<typename T>
    void PublishImmediate(T&& event) {
        using EventType = std::remove_cv_t<std::remove_reference_t<T>>;
        static_assert(std::is_base_of_v<EditorEvent, EventType>, "T must derive from EditorEvent");
        DispatchEvent(event);
    }

    /**
     * @brief Dispatch all queued events
     *
     * This should be called once per frame to process all
     * events that were queued during the frame.
     */
    void DispatchEvents();

    /**
     * @brief Clear all queued events without dispatching
     */
    void ClearQueue();

    /**
     * @brief Get the number of queued events
     */
    size_t GetQueueSize() const;

private:
    void DispatchEvent(EditorEvent& event);

    struct ListenerEntry {
        ListenerID id;
        EventCallback callback;

        ListenerEntry(ListenerID id, EventCallback callback)
            : id(id), callback(std::move(callback)) {}
    };

    // Type -> List of listeners
    std::unordered_map<std::type_index, std::vector<ListenerEntry>> m_Listeners;

    // Event queue for deferred dispatch
    std::queue<std::unique_ptr<EditorEvent>> m_EventQueue;
    mutable std::mutex m_QueueMutex;  // mutable for const methods

    ListenerID m_NextListenerID = 1;
};

} // namespace Zgine
