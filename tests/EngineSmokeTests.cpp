#include <gtest/gtest.h>
#include <Zgine/Scene/Components.h>
#include <Zgine/Scene/Entity.h>
#include <Zgine/Scene/Scene.h>

TEST(SceneEntityTests, CreateEntityHasDefaultComponents) {
    Zgine::Scene scene;
    Zgine::Entity entity = scene.CreateEntity("Player");

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
    Zgine::Scene scene;
    Zgine::Entity entity = scene.CreateEntity();

    entity.AddComponent<Zgine::AudioListenerComponent>();
    EXPECT_TRUE(entity.HasComponent<Zgine::AudioListenerComponent>());

    entity.RemoveComponent<Zgine::AudioListenerComponent>();
    EXPECT_FALSE(entity.HasComponent<Zgine::AudioListenerComponent>());
}
