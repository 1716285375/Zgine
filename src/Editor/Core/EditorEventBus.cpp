#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Core/Log/Log.h>
#include <algorithm>

namespace Zgine {

EditorEventBus::EditorEventBus()
    : m_NextListenerID(1)
{
}

EditorEventBus::~EditorEventBus() {
    ClearQueue();
    m_Listeners.clear();
}

void EditorEventBus::Unsubscribe(ListenerID id) {
    for (auto& [typeIndex, listeners] : m_Listeners) {
        auto it = std::remove_if(listeners.begin(), listeners.end(),
            [id](const ListenerEntry& entry) {
                return entry.id == id;
            });

        if (it != listeners.end()) {
            listeners.erase(it, listeners.end());
            return;
        }
    }
}

void EditorEventBus::DispatchEvents() {
    std::queue<std::unique_ptr<EditorEvent>> localQueue;

    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        std::swap(localQueue, m_EventQueue);
    }

    while (!localQueue.empty()) {
        auto& event = localQueue.front();
        DispatchEvent(*event);
        localQueue.pop();
    }
}

void EditorEventBus::DispatchEvent(EditorEvent& event) {
    std::type_index typeIndex = event.GetType();

    auto it = m_Listeners.find(typeIndex);
    if (it != m_Listeners.end()) {
        // Create a copy of the listener list in case callbacks modify subscriptions
        auto listenersCopy = it->second;

        for (auto& entry : listenersCopy) {
            if (event.IsHandled()) {
                break; // Stop propagation if event is handled
            }
            entry.callback(event);
        }
    }
}

void EditorEventBus::ClearQueue() {
    std::lock_guard<std::mutex> lock(m_QueueMutex);
    while (!m_EventQueue.empty()) {
        m_EventQueue.pop();
    }
}

size_t EditorEventBus::GetQueueSize() const {
    std::lock_guard<std::mutex> lock(m_QueueMutex);
    return m_EventQueue.size();
}

} // namespace Zgine
