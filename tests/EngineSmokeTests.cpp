#include <gtest/gtest.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/EntityManager.h>
#include <Zgine/World/Core/World.h>

#include <algorithm>
#include <vector>

TEST(LogTests, ProvidesFallbackLoggerBeforeInitialization) {
    ASSERT_TRUE(Zgine::Log::GetCoreLogger());
    ASSERT_TRUE(Zgine::Log::GetClientLogger());

    EXPECT_NO_THROW({
        ZGINE_CORE_DEBUG("Core fallback logger smoke test");
        ZGINE_DEBUG("Client fallback logger smoke test");
    });
}

TEST(SceneEntityTests, CreateEntityHasDefaultComponents) {
    Zgine::World World;
    Zgine::Entity entity = World.CreateEntity("Player");

    ASSERT_TRUE(entity);
    EXPECT_TRUE(entity.HasComponent<Zgine::TagComponent>());
    EXPECT_TRUE(entity.HasComponent<Zgine::TransformComponent>());

    const auto& tag = entity.GetComponent<Zgine::TagComponent>();
    EXPECT_EQ(tag.Tag, "Player");

    const auto& transform = entity.GetComponent<Zgine::TransformComponent>();
    EXPECT_FLOAT_EQ(transform.Translation.x, 0.0f);
    EXPECT_FLOAT_EQ(transform.Translation.y, 0.0f);
    EXPECT_FLOAT_EQ(transform.Translation.z, 0.0f);
    EXPECT_FLOAT_EQ(transform.Scale.x, 1.0f);
    EXPECT_FLOAT_EQ(transform.Scale.y, 1.0f);
    EXPECT_FLOAT_EQ(transform.Scale.z, 1.0f);
}

TEST(SceneEntityTests, AddAndRemoveComponent) {
    Zgine::World World;
    Zgine::Entity entity = World.CreateEntity();

    entity.AddComponent<Zgine::AudioListenerComponent>();
    EXPECT_TRUE(entity.HasComponent<Zgine::AudioListenerComponent>());

    entity.RemoveComponent<Zgine::AudioListenerComponent>();
    EXPECT_FALSE(entity.HasComponent<Zgine::AudioListenerComponent>());
}

TEST(SceneEntityTests, ClearDestroysEntityHierarchy) {
    Zgine::World world;
    Zgine::Entity root = world.CreateEntity("Root");
    Zgine::Entity child = world.CreateEntity("Child", root);

    const Zgine::EntityHandle rootHandle = root.GetHandle();
    const Zgine::EntityHandle childHandle = child.GetHandle();
    std::vector<Zgine::EntityHandle> destroyed;
    world.GetEntityManager().SetEntityDestroyedCallback(
        [&](Zgine::EntityHandle handle) {
            destroyed.push_back(handle);
        });

    world.Clear();

    EXPECT_EQ(world.GetEntityCount(), 0u);
    EXPECT_FALSE(root.IsValid());
    EXPECT_FALSE(child.IsValid());
    EXPECT_NE(std::find(destroyed.begin(), destroyed.end(), rootHandle), destroyed.end());
    EXPECT_NE(std::find(destroyed.begin(), destroyed.end(), childHandle), destroyed.end());
}
