#include <gtest/gtest.h>

#include <Zgine/Runtime/SceneRuntime.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Systems/ISystem.h>

#include <algorithm>
#include <memory>

using namespace Zgine;

namespace {

Entity FindByTag(World& world, const char* tag) {
    for (Entity entity : world.GetAllEntities()) {
        if (entity.HasComponent<TagComponent>() &&
            entity.GetComponent<TagComponent>().Tag == tag) {
            return entity;
        }
    }
    return Entity();
}

struct LifecycleCounters {
    int Initialized = 0;
    int Shutdown = 0;
    int SceneStarted = 0;
    int SceneStopped = 0;
    int Updated = 0;
    int FixedUpdated = 0;
};

class LifecycleTestSystem final : public ISystem {
public:
    explicit LifecycleTestSystem(LifecycleCounters* counters)
        : m_Counters(counters) {}

    void Initialize() override { ++m_Counters->Initialized; }
    void Shutdown() override { ++m_Counters->Shutdown; }
    void OnSceneStart(World*) override { ++m_Counters->SceneStarted; }
    void OnSceneStop() override { ++m_Counters->SceneStopped; }
    void Update(World*, float) override { ++m_Counters->Updated; }
    void FixedUpdate(World*, float) override { ++m_Counters->FixedUpdated; }
    const char* GetName() const override { return "LifecycleTestSystem"; }

private:
    LifecycleCounters* m_Counters = nullptr;
};

} // namespace

TEST(SceneRuntimeTest, CloneForRuntimeIsolatesEditWorldState) {
    World editWorld;
    Entity root = editWorld.CreateEntity("Player");
    root.GetComponent<TransformComponent>().Translation = {1.0f, 2.0f, 3.0f};

    auto& rigidBody = root.AddComponent<RigidbodyComponent>();
    rigidBody.RuntimeBody.Set(reinterpret_cast<void*>(0x1));

    auto& audio = root.AddComponent<AudioSourceComponent>();
    audio.IsPlaying = true;
    audio.RuntimeSourcePtr = reinterpret_cast<void*>(0x2);

    auto& script = root.AddComponent<ScriptComponent>();
    script.IsInitialized = true;

    Entity child = editWorld.CreateEntity("Child", root);
    child.GetComponent<TransformComponent>().Translation = {4.0f, 5.0f, 6.0f};

    std::unique_ptr<World> runtimeWorld = editWorld.CloneForRuntime();
    ASSERT_NE(runtimeWorld, nullptr);
    EXPECT_EQ(runtimeWorld->GetEntityCount(), editWorld.GetEntityCount());

    Entity runtimeRoot = FindByTag(*runtimeWorld, "Player");
    Entity runtimeChild = FindByTag(*runtimeWorld, "Child");
    ASSERT_TRUE(runtimeRoot);
    ASSERT_TRUE(runtimeChild);
    EXPECT_NE(runtimeRoot.GetWorld(), root.GetWorld());

    const auto& runtimeRootRel = runtimeRoot.GetComponent<RelationshipComponent>();
    const auto& runtimeChildRel = runtimeChild.GetComponent<RelationshipComponent>();
    EXPECT_EQ(runtimeChildRel.Parent, runtimeRoot.GetHandle());
    EXPECT_NE(std::find(runtimeRootRel.Children.begin(), runtimeRootRel.Children.end(),
                        runtimeChild.GetHandle()),
              runtimeRootRel.Children.end());

    EXPECT_FALSE(runtimeRoot.GetComponent<RigidbodyComponent>().RuntimeBody.IsValid());
    EXPECT_EQ(runtimeRoot.GetComponent<AudioSourceComponent>().RuntimeSourcePtr, nullptr);
    EXPECT_FALSE(runtimeRoot.GetComponent<AudioSourceComponent>().IsPlaying);
    EXPECT_FALSE(runtimeRoot.GetComponent<ScriptComponent>().IsInitialized);

    runtimeRoot.GetComponent<TransformComponent>().Translation = {10.0f, 20.0f, 30.0f};
    EXPECT_EQ(root.GetComponent<TransformComponent>().Translation, Math::Vector3(1.0f, 2.0f, 3.0f));
    EXPECT_TRUE(root.GetComponent<RigidbodyComponent>().RuntimeBody.IsValid());
    EXPECT_EQ(root.GetComponent<AudioSourceComponent>().RuntimeSourcePtr, reinterpret_cast<void*>(0x2));
    EXPECT_TRUE(root.GetComponent<AudioSourceComponent>().IsPlaying);
    EXPECT_TRUE(root.GetComponent<ScriptComponent>().IsInitialized);
}

TEST(SceneRuntimeTest, StartUpdateAndStopUseSceneLifecycle) {
    SceneRuntime runtime;
    LifecycleCounters counters;

    auto runtimeWorld = std::make_unique<World>();
    runtimeWorld->GetSystemManager().RegisterSystem<LifecycleTestSystem>(&counters);

    EXPECT_TRUE(runtime.Start(std::move(runtimeWorld)));
    EXPECT_TRUE(runtime.IsRunning());
    EXPECT_NE(runtime.GetWorld(), nullptr);
    EXPECT_EQ(counters.Initialized, 0);
    EXPECT_EQ(counters.SceneStarted, 1);

    runtime.Update(0.016f);
    runtime.FixedUpdate(1.0f / 60.0f);
    EXPECT_EQ(counters.Updated, 1);
    EXPECT_EQ(counters.FixedUpdated, 1);

    EXPECT_TRUE(runtime.Start(std::make_unique<World>()));
    EXPECT_EQ(counters.SceneStarted, 1);

    runtime.Stop();
    EXPECT_FALSE(runtime.IsRunning());
    EXPECT_EQ(runtime.GetWorld(), nullptr);
    EXPECT_EQ(counters.SceneStopped, 1);
    EXPECT_EQ(counters.Shutdown, 0);

    runtime.Stop();
    EXPECT_EQ(counters.SceneStopped, 1);
}

TEST(SceneRuntimeTest, StartRejectsNullWorldWithoutChangingState) {
    SceneRuntime runtime;

    EXPECT_FALSE(runtime.Start(nullptr));
    EXPECT_FALSE(runtime.IsRunning());
    EXPECT_EQ(runtime.GetWorld(), nullptr);
}
