#include <gtest/gtest.h>
#include <Zgine/Editor/Core/AssetSelectionContext.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Events/EditorEvents.h>
#include <Zgine/Editor/Events/SelectionEvents.h>
#include <Zgine/Resources/Core/AssetDatabase.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Systems/ISystem.h>

#include <vector>

using namespace Zgine;

namespace {

AssetRecord MakeAssetRecord() {
    AssetRecord record;
    record.Handle = AssetHandle::New();
    record.Type = AssetType::Texture;
    record.SourcePath = "C:/Project/assets/Textures/albedo.png";
    record.RelativePath = "Textures/albedo.png";
    record.HasMetadata = true;
    return record;
}

Entity FindByTag(World& world, const char* tag) {
    for (Entity entity : world.GetAllEntities()) {
        if (entity.HasComponent<TagComponent>() &&
            entity.GetComponent<TagComponent>().Tag == tag) {
            return entity;
        }
    }
    return Entity();
}

class RuntimeConfiguratorTestSystem final : public ISystem {
public:
    void Initialize() override {}
    void Shutdown() override {}
    void OnSceneStart(World* world) override {
        StartedWorld = world;
        ++SceneStarted;
    }
    void OnSceneStop() override {
        ++SceneStopped;
        StartedWorld = nullptr;
    }
    void Update(World*, float) override {}
    const char* GetName() const override { return "RuntimeConfiguratorTestSystem"; }

    World* StartedWorld = nullptr;
    int SceneStarted = 0;
    int SceneStopped = 0;
};

} // namespace

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
    EXPECT_NE(&m_Context->GetAssetSelectionContext(), nullptr);
    EXPECT_NE(&m_Context->GetEventBus(), nullptr);
    EXPECT_NE(&m_Context->GetCommandHistory(), nullptr);
    EXPECT_NE(&m_Context->GetViewportContext(), nullptr);
    EXPECT_NE(&m_Context->GetSceneContext(), nullptr);
}

TEST_F(EditorContextTest, AssetSelectionContextAccess) {
    auto& assetSelection = m_Context->GetAssetSelectionContext();

    EXPECT_FALSE(assetSelection.HasSelection());
    EXPECT_FALSE(assetSelection.GetSelection().IsValid());
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
    Entity testEntity(EntityHandle::FromValue(123), nullptr);
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

TEST_F(EditorContextTest, EntitySelectionClearsAssetSelection) {
    World world;
    Entity entity = world.CreateEntity("Selected");

    auto& assetSelection = m_Context->GetAssetSelectionContext();
    auto& entitySelection = m_Context->GetSelectionContext();

    assetSelection.Select(MakeAssetRecord());
    ASSERT_TRUE(assetSelection.HasSelection());

    entitySelection.Select(entity);

    EXPECT_TRUE(entitySelection.HasSelection());
    EXPECT_EQ(entitySelection.GetPrimary(), entity);
    EXPECT_FALSE(assetSelection.HasSelection());
}

TEST_F(EditorContextTest, AssetSelectionClearsEntitySelection) {
    World world;
    Entity entity = world.CreateEntity("Selected");

    auto& assetSelection = m_Context->GetAssetSelectionContext();
    auto& entitySelection = m_Context->GetSelectionContext();

    entitySelection.Select(entity);
    ASSERT_TRUE(entitySelection.HasSelection());

    assetSelection.Select(MakeAssetRecord());

    EXPECT_TRUE(assetSelection.HasSelection());
    EXPECT_FALSE(entitySelection.HasSelection());
    EXPECT_FALSE(entitySelection.GetPrimary());
}

TEST_F(EditorContextTest, EnterPauseAndExitPlayModeRestoreEditScene) {
    World editWorld;
    Entity editEntity = editWorld.CreateEntity("Player");
    editEntity.GetComponent<TransformComponent>().Translation = {1.0f, 2.0f, 3.0f};

    m_Context->SetActiveScene(&editWorld);

    std::vector<EditorMode> observedModes;
    m_Context->GetEventBus().Subscribe<PlayModeChangedEvent>(
        [&](PlayModeChangedEvent& event) {
            observedModes.push_back(event.GetMode());
        });

    ASSERT_TRUE(m_Context->EnterPlayMode());
    EXPECT_EQ(m_Context->GetMode(), EditorMode::Play);
    ASSERT_NE(m_Context->GetSceneContext().GetActiveScene(), nullptr);
    EXPECT_NE(m_Context->GetSceneContext().GetActiveScene(), &editWorld);

    World* runtimeWorld = m_Context->GetSceneContext().GetActiveScene();
    Entity runtimeEntity = FindByTag(*runtimeWorld, "Player");
    ASSERT_TRUE(runtimeEntity);
    runtimeEntity.GetComponent<TransformComponent>().Translation = {10.0f, 20.0f, 30.0f};
    EXPECT_EQ(editEntity.GetComponent<TransformComponent>().Translation,
              Math::Vector3(1.0f, 2.0f, 3.0f));

    m_Context->PausePlayMode();
    EXPECT_EQ(m_Context->GetMode(), EditorMode::Pause);
    EXPECT_EQ(m_Context->GetSceneContext().GetActiveScene(), runtimeWorld);

    ASSERT_TRUE(m_Context->EnterPlayMode());
    EXPECT_EQ(m_Context->GetMode(), EditorMode::Play);
    EXPECT_EQ(m_Context->GetSceneContext().GetActiveScene(), runtimeWorld);

    m_Context->ExitPlayMode();
    EXPECT_EQ(m_Context->GetMode(), EditorMode::Edit);
    EXPECT_EQ(m_Context->GetSceneContext().GetActiveScene(), &editWorld);

    ASSERT_EQ(observedModes.size(), 4);
    EXPECT_EQ(observedModes[0], EditorMode::Play);
    EXPECT_EQ(observedModes[1], EditorMode::Pause);
    EXPECT_EQ(observedModes[2], EditorMode::Play);
    EXPECT_EQ(observedModes[3], EditorMode::Edit);
}

TEST_F(EditorContextTest, EnterPlayModeFailsWithoutActiveScene) {
    EXPECT_FALSE(m_Context->EnterPlayMode());
    EXPECT_EQ(m_Context->GetMode(), EditorMode::Edit);
    EXPECT_EQ(m_Context->GetSceneContext().GetActiveScene(), nullptr);
}

TEST_F(EditorContextTest, RuntimeConfiguratorRegistersSystemsBeforePlaySceneStarts) {
    World editWorld;
    editWorld.CreateEntity("Player");
    RuntimeConfiguratorTestSystem system;

    m_Context->SetActiveScene(&editWorld);
    m_Context->SetPlayRuntimeConfigurator([&](World& runtimeWorld) {
        runtimeWorld.GetSystemManager().RegisterExternalSystem(&system);
    });

    ASSERT_TRUE(m_Context->EnterPlayMode());
    World* runtimeWorld = m_Context->GetSceneContext().GetActiveScene();
    ASSERT_NE(runtimeWorld, nullptr);
    EXPECT_NE(runtimeWorld, &editWorld);
    EXPECT_EQ(system.SceneStarted, 1);
    EXPECT_EQ(system.StartedWorld, runtimeWorld);
    EXPECT_TRUE(runtimeWorld->GetSystemManager().IsSceneRunning());

    m_Context->ExitPlayMode();
    EXPECT_EQ(system.SceneStopped, 1);
    EXPECT_EQ(system.StartedWorld, nullptr);
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
    EXPECT_NE(&ctx1.GetAssetSelectionContext(), &ctx2.GetAssetSelectionContext());
    EXPECT_NE(&ctx1.GetEventBus(), &ctx2.GetEventBus());
    EXPECT_NE(&ctx1.GetCommandHistory(), &ctx2.GetCommandHistory());

    ctx1.Shutdown();
    ctx2.Shutdown();
}
