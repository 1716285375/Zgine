#include <gtest/gtest.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/EntityEvents.h>
#include <Zgine/Editor/Events/SelectionEvents.h>
#include <Zgine/Editor/Events/SceneEvents.h>
#include <Zgine/Editor/Events/TransformEvents.h>
#include <Zgine/Editor/Events/EditorEvents.h>
#include <thread>
#include <chrono>

using namespace Zgine;

// ============================================================================
// EditorEventBus Tests
// ============================================================================

class EditorEventBusTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_EventBus = std::make_unique<EditorEventBus>();
    }

    void TearDown() override {
        m_EventBus.reset();
    }

    std::unique_ptr<EditorEventBus> m_EventBus;
};

// ============================================================================
// Basic Event Publishing and Subscription
// ============================================================================

TEST_F(EditorEventBusTest, ImmediateEventDispatch) {
    bool eventReceived = false;
    Entity testEntity(EntityHandle::FromValue(42), nullptr);

    m_EventBus->Subscribe<EntitySelectedEvent>([&](EntitySelectedEvent& e) {
        eventReceived = true;
        EXPECT_EQ(e.GetEntity(), testEntity);
    });

    EntitySelectedEvent event(testEntity);
    m_EventBus->PublishImmediate(event);
    EXPECT_TRUE(eventReceived);
}

TEST_F(EditorEventBusTest, QueuedEventDispatch) {
    std::atomic<int> eventCount{0};

    m_EventBus->Subscribe<EntityCreatedEvent>([&](EntityCreatedEvent& e) {
        eventCount++;
    });

    // Publish to queue
    Entity e1(EntityHandle::FromValue(1), nullptr);
    Entity e2(EntityHandle::FromValue(2), nullptr);
    Entity e3(EntityHandle::FromValue(3), nullptr);

    m_EventBus->Publish(EntityCreatedEvent(e1, PrimitiveType::Cube));
    m_EventBus->Publish(EntityCreatedEvent(e2, PrimitiveType::Sphere));
    m_EventBus->Publish(EntityCreatedEvent(e3, PrimitiveType::Plane));

    // Events not dispatched yet
    EXPECT_EQ(eventCount, 0);

    // Dispatch events
    m_EventBus->DispatchEvents();
    EXPECT_EQ(eventCount, 3);
}

TEST_F(EditorEventBusTest, MultipleSubscribers) {
    int subscriber1Count = 0;
    int subscriber2Count = 0;
    int subscriber3Count = 0;

    m_EventBus->Subscribe<EntityDeletedEvent>([&](EntityDeletedEvent& e) {
        subscriber1Count++;
    });

    m_EventBus->Subscribe<EntityDeletedEvent>([&](EntityDeletedEvent& e) {
        subscriber2Count++;
    });

    m_EventBus->Subscribe<EntityDeletedEvent>([&](EntityDeletedEvent& e) {
        subscriber3Count++;
    });

    Entity entity(EntityHandle::FromValue(100), nullptr);
    EntityDeletedEvent event(entity);
    m_EventBus->PublishImmediate(event);

    EXPECT_EQ(subscriber1Count, 1);
    EXPECT_EQ(subscriber2Count, 1);
    EXPECT_EQ(subscriber3Count, 1);
}

TEST_F(EditorEventBusTest, Unsubscribe) {
    int callCount = 0;

    auto listenerId = m_EventBus->Subscribe<TransformChangedEvent>(
        [&](TransformChangedEvent& e) {
            callCount++;
        });

    Entity entity(EntityHandle::FromValue(10), nullptr);
    TransformChangedEvent event(entity);

    m_EventBus->PublishImmediate(event);
    EXPECT_EQ(callCount, 1);

    // Unsubscribe
    m_EventBus->Unsubscribe(listenerId);

    // Event should not be received
    m_EventBus->PublishImmediate(event);
    EXPECT_EQ(callCount, 1);  // Still 1, not 2
}

// ============================================================================
// Event Type and Category Tests
// ============================================================================

TEST_F(EditorEventBusTest, EntitySelectedEvent) {
    Entity selected(EntityHandle::FromValue(999), nullptr);
    bool received = false;

    m_EventBus->Subscribe<EntitySelectedEvent>([&](EntitySelectedEvent& e) {
        received = true;
        EXPECT_EQ(e.GetEntity(), selected);
        EXPECT_EQ(e.GetCategory(), EditorEventCategory::Selection);
        // Type check using typeid
        EXPECT_EQ(e.GetType(), EntitySelectedEvent::GetStaticType());
    });

    EntitySelectedEvent event(selected);
    m_EventBus->PublishImmediate(event);
    EXPECT_TRUE(received);
}

TEST_F(EditorEventBusTest, TransformChangedEvent) {
    Entity entity(EntityHandle::FromValue(50), nullptr);
    bool received = false;

    m_EventBus->Subscribe<TransformChangedEvent>([&](TransformChangedEvent& e) {
        received = true;
        EXPECT_EQ(e.GetEntity(), entity);
        EXPECT_EQ(e.GetCategory(), EditorEventCategory::Transform);
        EXPECT_EQ(e.GetType(), TransformChangedEvent::GetStaticType());
    });

    TransformChangedEvent event(entity);
    m_EventBus->PublishImmediate(event);
    EXPECT_TRUE(received);
}

TEST_F(EditorEventBusTest, SceneSavedEvent) {
    // World* World = new World(); // Avoid raw optional pointers if possible, but keeping test simple
    World World; // Stack allocate to avoid leak concerns in test
    std::string path = "/path/to/World.zscene";
    bool received = false;

    m_EventBus->Subscribe<SceneSavedEvent>([&](SceneSavedEvent& e) {
        received = true;
        EXPECT_EQ(e.GetScene(), &World);
        EXPECT_EQ(e.GetPath(), path);
        EXPECT_EQ(e.GetCategory(), EditorEventCategory::World);
    });

    SceneSavedEvent event(&World, path);
    m_EventBus->PublishImmediate(event);
    EXPECT_TRUE(received);
}

TEST_F(EditorEventBusTest, PlayModeChangedEvent) {
    bool received = false;
    EditorMode newMode = EditorMode::Play;

    m_EventBus->Subscribe<PlayModeChangedEvent>([&](PlayModeChangedEvent& e) {
        received = true;
        EXPECT_EQ(e.GetMode(), newMode);
        EXPECT_EQ(e.GetCategory(), EditorEventCategory::Editor);
    });

    PlayModeChangedEvent event(newMode);
    m_EventBus->PublishImmediate(event);
    EXPECT_TRUE(received);
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

TEST_F(EditorEventBusTest, ThreadSafePublish) {
    std::atomic<int> eventCount{0};

    m_EventBus->Subscribe<EntityCreatedEvent>([&](EntityCreatedEvent& e) {
        eventCount++;
    });

    // Publish from multiple threads
    const int numThreads = 10;
    const int eventsPerThread = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < eventsPerThread; ++j) {
                Entity entity(EntityHandle::FromValue(i * eventsPerThread + j), nullptr);
                m_EventBus->Publish(EntityCreatedEvent(entity, PrimitiveType::Cube));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Dispatch all queued events
    m_EventBus->DispatchEvents();

    EXPECT_EQ(eventCount, numThreads * eventsPerThread);
}

// ============================================================================
// Event Queue Tests
// ============================================================================

TEST_F(EditorEventBusTest, EventQueueOrder) {
    std::vector<int> receivedOrder;

    m_EventBus->Subscribe<EntityCreatedEvent>([&](EntityCreatedEvent& e) {
        receivedOrder.push_back(static_cast<int>(e.GetEntity().GetHandle().GetValue()));
    });

    // Publish in order
    for (int i = 0; i < 10; ++i) {
        Entity entity(EntityHandle::FromValue(i), nullptr);
        m_EventBus->Publish(EntityCreatedEvent(entity, PrimitiveType::Cube));
    }

    m_EventBus->DispatchEvents();

    // Should maintain order
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(receivedOrder[i], i);
    }
}

TEST_F(EditorEventBusTest, ClearQueue) {
    int eventCount = 0;

    m_EventBus->Subscribe<EntityDeletedEvent>([&](EntityDeletedEvent& e) {
        eventCount++;
    });

    // Add events to queue
    for (int i = 0; i < 5; ++i) {
        Entity entity(EntityHandle::FromValue(i), nullptr);
        m_EventBus->Publish(EntityDeletedEvent(entity));
    }

    // Clear queue before dispatch
    m_EventBus->ClearQueue();
    m_EventBus->DispatchEvents();

    // No events should be dispatched
    EXPECT_EQ(eventCount, 0);
}

// ============================================================================
// Mixed Event Types
// ============================================================================

TEST_F(EditorEventBusTest, MultipleEventTypes) {
    int entityEventCount = 0;
    int sceneEventCount = 0;
    int stateEventCount = 0;

    m_EventBus->Subscribe<EntitySelectedEvent>([&](EntitySelectedEvent& e) {
        entityEventCount++;
    });

    m_EventBus->Subscribe<SceneSavedEvent>([&](SceneSavedEvent& e) {
        sceneEventCount++;
    });

    m_EventBus->Subscribe<PlayModeChangedEvent>([&](PlayModeChangedEvent& e) {
        stateEventCount++;
    });

    // Publish different event types
    Entity entity(EntityHandle::FromValue(1), nullptr);
    World World;

    EntitySelectedEvent e1(entity);
    m_EventBus->PublishImmediate(e1);

    SceneSavedEvent e2(&World, "test.World");
    m_EventBus->PublishImmediate(e2);

    PlayModeChangedEvent e3(EditorMode::Play);
    m_EventBus->PublishImmediate(e3);

    EXPECT_EQ(entityEventCount, 1);
    EXPECT_EQ(sceneEventCount, 1);
    EXPECT_EQ(stateEventCount, 1);
}

// ============================================================================
// Performance Test
// ============================================================================

TEST_F(EditorEventBusTest, PerformanceTest) {
    const int numEvents = 10000;
    std::atomic<int> eventCount{0};

    m_EventBus->Subscribe<EntityCreatedEvent>([&](EntityCreatedEvent& e) {
        eventCount++;
    });

    auto start = std::chrono::high_resolution_clock::now();

    // Queue many events
    for (int i = 0; i < numEvents; ++i) {
        Entity entity(EntityHandle::FromValue(i), nullptr);
        m_EventBus->Publish(EntityCreatedEvent(entity, PrimitiveType::Cube));
    }

    // Dispatch
    m_EventBus->DispatchEvents();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(eventCount, numEvents);

    // Should complete in reasonable time
    std::cout << "Dispatched " << numEvents << " events in "
              << duration.count() << "ms" << std::endl;
    EXPECT_LT(duration.count(), 100);
}
