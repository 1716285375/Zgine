#include <gtest/gtest.h>

#include <Zgine/Resources/Core/AssetType.h>
#include <Zgine/Resources/Prefab/PrefabSerializer.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/World.h>

#include <chrono>
#include <filesystem>

namespace {

Zgine::Entity FindByTag(Zgine::World& world, const std::string& tag) {
    for (Zgine::Entity entity : world.GetAllEntities()) {
        if (entity.HasComponent<Zgine::TagComponent>() &&
            entity.GetComponent<Zgine::TagComponent>().Tag == tag) {
            return entity;
        }
    }
    return {};
}

} // namespace

TEST(PrefabTest, AssetTypeRecognizesPrefabExtensions) {
    EXPECT_EQ(Zgine::AssetTypeFromPath("assets/Prefabs/player.prefab"), Zgine::AssetType::Prefab);
    EXPECT_EQ(Zgine::AssetTypeFromPath("assets/Prefabs/player.zgprefab"), Zgine::AssetType::Prefab);
    EXPECT_STREQ(Zgine::AssetTypeToString(Zgine::AssetType::Prefab), "Prefab");
    EXPECT_EQ(Zgine::AssetTypeFromString("prefab"), Zgine::AssetType::Prefab);
}

TEST(PrefabTest, CreatesPrefabFromEntityHierarchy) {
    Zgine::World world;
    Zgine::Entity root = world.CreateEntity("Player");
    Zgine::Entity child = world.CreateEntity("Weapon", root);
    root.GetComponent<Zgine::TransformComponent>().Translation = {1.0f, 2.0f, 3.0f};
    child.GetComponent<Zgine::TransformComponent>().Translation = {4.0f, 5.0f, 6.0f};

    auto prefab = Zgine::PrefabSerializer::CreateFromEntityHierarchy(root);

    ASSERT_TRUE(prefab.has_value());
    EXPECT_TRUE(prefab->IsValid());
    EXPECT_EQ(prefab->Name, "Player");
    EXPECT_EQ(prefab->Entities.size(), 2u);
    EXPECT_EQ(prefab->TemplateRootUUID, root.GetComponent<Zgine::IDComponent>().ID.ToString());

    bool foundChildParent = false;
    for (const auto& entityJson : prefab->Entities) {
        if (entityJson.value("Tag", "") == "Weapon") {
            foundChildParent = entityJson.contains("Parent") &&
                entityJson["Parent"].get<std::string>() == root.GetComponent<Zgine::IDComponent>().ID.ToString();
        }
    }
    EXPECT_TRUE(foundChildParent);
}

TEST(PrefabTest, SerializesAndInstantiatesWithFreshUUIDsAndHierarchy) {
    Zgine::World source;
    Zgine::Entity root = source.CreateEntity("Player");
    Zgine::Entity child = source.CreateEntity("Weapon", root);
    root.GetComponent<Zgine::TransformComponent>().Translation = {1.0f, 2.0f, 3.0f};
    child.GetComponent<Zgine::TransformComponent>().Translation = {4.0f, 5.0f, 6.0f};

    auto prefab = Zgine::PrefabSerializer::CreateFromEntityHierarchy(root);
    ASSERT_TRUE(prefab.has_value());

    const std::string serialized = Zgine::PrefabSerializer::SerializeToString(*prefab);
    auto deserialized = Zgine::PrefabSerializer::DeserializeFromString(serialized);
    ASSERT_TRUE(deserialized.has_value());

    Zgine::World target;
    Zgine::Entity instanceRoot = Zgine::PrefabSerializer::Instantiate(*deserialized, target);

    ASSERT_TRUE(instanceRoot);
    EXPECT_EQ(target.GetEntityCount(), 2u);
    EXPECT_EQ(instanceRoot.GetComponent<Zgine::TagComponent>().Tag, "Player");
    EXPECT_FALSE(instanceRoot.GetComponent<Zgine::IDComponent>().ID ==
                 root.GetComponent<Zgine::IDComponent>().ID);

    const auto children = target.GetChildren(instanceRoot);
    ASSERT_EQ(children.size(), 1u);
    Zgine::Entity instanceChild = children.front();
    EXPECT_EQ(instanceChild.GetComponent<Zgine::TagComponent>().Tag, "Weapon");
    EXPECT_EQ(instanceChild.GetComponent<Zgine::RelationshipComponent>().Parent, instanceRoot.GetHandle());
    EXPECT_FALSE(instanceChild.GetComponent<Zgine::IDComponent>().ID ==
                 child.GetComponent<Zgine::IDComponent>().ID);

    const auto& rootTransform = instanceRoot.GetComponent<Zgine::TransformComponent>();
    EXPECT_FLOAT_EQ(rootTransform.Translation.x, 1.0f);
    EXPECT_FLOAT_EQ(rootTransform.Translation.y, 2.0f);
    EXPECT_FLOAT_EQ(rootTransform.Translation.z, 3.0f);
}

TEST(PrefabTest, SavesAndLoadsPrefabFileWithoutVFS) {
    Zgine::World source;
    Zgine::Entity root = source.CreateEntity("Player");
    root.GetComponent<Zgine::TransformComponent>().Translation = {7.0f, 8.0f, 9.0f};

    auto prefab = Zgine::PrefabSerializer::CreateFromEntityHierarchy(root);
    ASSERT_TRUE(prefab.has_value());

    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() /
        ("ZginePrefabTest_" + std::to_string(uniqueId) + ".prefab");

    EXPECT_TRUE(Zgine::PrefabSerializer::SaveToFile(*prefab, path));
    auto loaded = Zgine::PrefabSerializer::LoadFromFile(path);
    ASSERT_TRUE(loaded.has_value());

    EXPECT_EQ(loaded->Name, "Player");
    EXPECT_EQ(loaded->TemplateRootUUID, prefab->TemplateRootUUID);
    EXPECT_EQ(loaded->Entities.size(), 1u);

    std::error_code ec;
    std::filesystem::remove(path, ec);
}

TEST(PrefabTest, InvalidPrefabDoesNotInstantiate) {
    Zgine::PrefabAsset prefab;
    Zgine::World world;

    Zgine::Entity instance = Zgine::PrefabSerializer::Instantiate(prefab, world);

    EXPECT_FALSE(instance);
    EXPECT_EQ(world.GetEntityCount(), 0u);
}
