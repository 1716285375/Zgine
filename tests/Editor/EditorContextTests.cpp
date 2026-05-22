#include <gtest/gtest.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Events/SelectionEvents.h>
#include <Zgine/World/Core/World.h>

using namespace Zgine;

// ============================================================================
// EditorContext Tests
// ============================================================================

class EditorContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_Context = std::make_unique<EditorContext>();
        m_Context->Initialize();
    }

    void TearDown() override {
        m_Context->Shutdown();
        m_Context.reset();
    }

    std::unique_ptr<EditorContext> m_Context;
};

TEST_F(EditorContextTest, InitializeAndShutdown) {
    // Context should be initialized
    EXPECT_NE(&m_Context->GetSelectionContext(), nullptr);
    EXPECT_NE(&m_Context->GetEventBus(), nullptr);
    EXPECT_NE(&m_Context->GetCommandHistory(), nullptr);
    EXPECT_NE(&m_Context->GetViewportContext(), nullptr);
    EXPECT_NE(&m_Context->GetSceneContext(), nullptr);
}

TEST_F(EditorContextTest, SelectionContextAccess) {
    auto& selection = m_Context->GetSelectionContext();

    // Initial state should be empty
    EXPECT_TRUE(selection.GetSelection().empty());
    EXPECT_FALSE(selection.GetPrimary());
    EXPECT_EQ(selection.GetSelection().size(), 0);
}

TEST_F(EditorContextTest, EventBusAccess) {
    auto& eventBus = m_Context->GetEventBus();

    bool eventReceived = false;
    eventBus.Subscribe<EntitySelectedEvent>([&](EntitySelectedEvent& e) {
        eventReceived = true;
    });

    // Should be able to publish and receive events
    Entity testEntity(static_cast<entt::entity>(123), nullptr);
    EntitySelectedEvent event(testEntity);
    eventBus.PublishImmediate(event);

    EXPECT_TRUE(eventReceived);
}

TEST_F(EditorContextTest, CommandHistoryAccess) {
    auto& cmdHistory = m_Context->GetCommandHistory();

    // Initial state
    EXPECT_FALSE(cmdHistory.CanUndo());
    EXPECT_FALSE(cmdHistory.CanRedo());
    EXPECT_EQ(cmdHistory.GetHistorySize(), 0);
}

TEST_F(EditorContextTest, ViewportContextAccess) {
    auto& viewport = m_Context->GetViewportContext();

    // Test viewport state
    viewport.SetFocused(true);
    EXPECT_TRUE(viewport.IsFocused());

    viewport.SetHovered(true);
    EXPECT_TRUE(viewport.IsHovered());

    // Test viewport size
    Math::Vector2 size(1920.0f, 1080.0f);
    viewport.SetViewportSize(size);
    EXPECT_EQ(viewport.GetViewportSize(), size);
}

TEST_F(EditorContextTest, SceneContextAccess) {
    auto& sceneCtx = m_Context->GetSceneContext();

    World* testScene = new World();
    sceneCtx.SetActiveScene(testScene);
    EXPECT_EQ(sceneCtx.GetActiveScene(), testScene);

    std::string path = "test_World.zscene";
    sceneCtx.SetScenePath(path);
    EXPECT_EQ(sceneCtx.GetScenePath(), path);

    delete testScene;
}

// ============================================================================
// Multiple Context Instances Test
// ============================================================================

TEST(EditorContextMultiInstanceTest, MultipleContextsIndependent) {
    EditorContext ctx1, ctx2;
    ctx1.Initialize();
    ctx2.Initialize();

    // Each context should have independent services
    EXPECT_NE(&ctx1.GetSelectionContext(), &ctx2.GetSelectionContext());
    EXPECT_NE(&ctx1.GetEventBus(), &ctx2.GetEventBus());
    EXPECT_NE(&ctx1.GetCommandHistory(), &ctx2.GetCommandHistory());

    ctx1.Shutdown();
    ctx2.Shutdown();
}
