#include <gtest/gtest.h>

#include <Zgine/Resources/Core/AssetDatabase.h>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace {

class AssetDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        m_Root = std::filesystem::temp_directory_path() /
            ("zgine-asset-database-test-" + std::to_string(unique));

        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Textures", ec));
        ASSERT_FALSE(ec);
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Models", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Audio", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Materials", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Prefabs", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Scenes", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Scripts", ec));
        ASSERT_TRUE(std::filesystem::create_directories(m_Root / "Notes", ec));
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
    }

    std::filesystem::path WriteFile(const std::filesystem::path& relativePath, std::string_view content) const {
        const std::filesystem::path path = m_Root / relativePath;
        std::ofstream file(path, std::ios::binary);
        file << content;
        return path;
    }

    void WriteMetadata(const std::filesystem::path& sourcePath, Zgine::AssetHandle handle, Zgine::AssetType type) const {
        Zgine::AssetMetadata metadata;
        metadata.Handle = handle;
        metadata.Type = type;
        metadata.SourcePath = sourcePath;

        std::ofstream file(sourcePath.string() + ".meta", std::ios::trunc);
        file << metadata.Serialize().dump(4);
    }

    std::filesystem::path m_Root;
};

} // namespace

TEST_F(AssetDatabaseTest, ScansAssetRecordsWithStableClassification) {
    const auto texturePath = WriteFile("Textures/albedo.png", "texture");
    WriteFile("Models/cube.glb", "mesh");
    WriteFile("Audio/music.ogg", "audio");
    WriteFile("Materials/basic.material", "{}");
    WriteFile("Prefabs/player.prefab", "{}");
    WriteFile("Scenes/main.json", "{}");
    WriteFile("Scripts/player.lua", "function OnUpdate() end");
    WriteFile("Notes/readme.txt", "notes");

    const Zgine::AssetHandle textureHandle = Zgine::AssetHandle::New();
    WriteMetadata(texturePath, textureHandle, Zgine::AssetType::Texture);

    Zgine::AssetDatabase database;
    database.Scan({ .AssetsRoot = m_Root });

    EXPECT_TRUE(database.IsScanned());
    EXPECT_NE(database.GetRecordByPath("Textures"), nullptr);

    const Zgine::AssetRecord* texture = database.GetRecordByPath("Textures/albedo.png");
    ASSERT_NE(texture, nullptr);
    EXPECT_EQ(texture->Type, Zgine::AssetType::Texture);
    EXPECT_TRUE(texture->HasMetadata);
    EXPECT_EQ(texture->Handle, textureHandle);
    EXPECT_EQ(texture->RelativePath.generic_string(), "Textures/albedo.png");

    const Zgine::AssetRecord* textureByAbsolutePath = database.GetRecordByPath(texturePath);
    ASSERT_NE(textureByAbsolutePath, nullptr);
    EXPECT_EQ(textureByAbsolutePath->SourcePath, texture->SourcePath);

    const Zgine::AssetRecord* textureByHandle = database.GetRecordByHandle(textureHandle);
    ASSERT_NE(textureByHandle, nullptr);
    EXPECT_EQ(textureByHandle->SourcePath, texture->SourcePath);

    const Zgine::AssetRecord* mesh = database.GetRecordByPath("Models/cube.glb");
    ASSERT_NE(mesh, nullptr);
    EXPECT_EQ(mesh->Type, Zgine::AssetType::Mesh);

    const Zgine::AssetRecord* audio = database.GetRecordByPath("Audio/music.ogg");
    ASSERT_NE(audio, nullptr);
    EXPECT_EQ(audio->Type, Zgine::AssetType::Audio);

    const Zgine::AssetRecord* material = database.GetRecordByPath("Materials/basic.material");
    ASSERT_NE(material, nullptr);
    EXPECT_EQ(material->Type, Zgine::AssetType::Material);

    const Zgine::AssetRecord* prefab = database.GetRecordByPath("Prefabs/player.prefab");
    ASSERT_NE(prefab, nullptr);
    EXPECT_EQ(prefab->Type, Zgine::AssetType::Prefab);

    const Zgine::AssetRecord* scene = database.GetRecordByPath("Scenes/main.json");
    ASSERT_NE(scene, nullptr);
    EXPECT_EQ(scene->Type, Zgine::AssetType::World);

    const Zgine::AssetRecord* script = database.GetRecordByPath("Scripts/player.lua");
    ASSERT_NE(script, nullptr);
    EXPECT_EQ(script->Type, Zgine::AssetType::Script);

    const Zgine::AssetRecord* unknown = database.GetRecordByPath("Notes/readme.txt");
    ASSERT_NE(unknown, nullptr);
    EXPECT_EQ(unknown->Type, Zgine::AssetType::Unknown);
}

TEST_F(AssetDatabaseTest, CanExcludeDirectoriesAndUnknownFiles) {
    WriteFile("Textures/albedo.png", "texture");
    WriteFile("Notes/readme.txt", "notes");

    Zgine::AssetDatabaseConfig config;
    config.AssetsRoot = m_Root;
    config.IncludeDirectories = false;
    config.IncludeUnknownFiles = false;

    Zgine::AssetDatabase database;
    database.Scan(config);

    EXPECT_EQ(database.GetRecordByPath("Textures"), nullptr);
    EXPECT_EQ(database.GetRecordByPath("Notes/readme.txt"), nullptr);

    const auto textures = database.GetRecordsByType(Zgine::AssetType::Texture);
    ASSERT_EQ(textures.size(), 1u);
    EXPECT_EQ(textures.front()->RelativePath.generic_string(), "Textures/albedo.png");
}

TEST_F(AssetDatabaseTest, ReturnsRecordsByTypeInPathOrder) {
    WriteFile("Textures/z.png", "z");
    WriteFile("Textures/a.png", "a");

    Zgine::AssetDatabase database;
    database.Scan({ .AssetsRoot = m_Root });

    const auto textures = database.GetRecordsByType(Zgine::AssetType::Texture);
    ASSERT_EQ(textures.size(), 2u);
    EXPECT_EQ(textures[0]->RelativePath.generic_string(), "Textures/a.png");
    EXPECT_EQ(textures[1]->RelativePath.generic_string(), "Textures/z.png");
}

TEST_F(AssetDatabaseTest, MissingRootScansToEmptyDatabase) {
    Zgine::AssetDatabase database;
    database.Scan({ .AssetsRoot = m_Root / "Missing" });

    EXPECT_TRUE(database.IsScanned());
    EXPECT_TRUE(database.GetRecords().empty());
    EXPECT_EQ(database.GetRecordByPath("anything.png"), nullptr);
}
