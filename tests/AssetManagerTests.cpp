#include <gtest/gtest.h>

#include <Zgine/Resources/Core/AssetManager.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>

namespace {

class AssetManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        m_Root = std::filesystem::temp_directory_path() /
            ("zgine-asset-manager-test-" + std::to_string(unique));

        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
        ASSERT_TRUE(std::filesystem::create_directories(m_Root, ec));
        ASSERT_FALSE(ec);

        Zgine::AssetManagerConfig config;
        config.AssetsRoot = m_Root;
        config.MaxCacheSizeBytes = 1024 * 1024;

        auto& manager = Zgine::AssetManager::Get();
        manager.Shutdown();
        manager.Initialize(config);
    }

    void TearDown() override {
        Zgine::AssetManager::Get().Shutdown();

        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
    }

    std::filesystem::path WriteAudioAsset(std::string_view name, std::string_view bytes) const {
        const auto path = m_Root / name;
        std::ofstream stream(path, std::ios::binary);
        stream << bytes;
        stream.close();
        return path;
    }

private:
    std::filesystem::path m_Root;
};

} // namespace

TEST_F(AssetManagerTest, ConcurrentAsyncLoadsShareCachedAudioAsset) {
    auto& manager = Zgine::AssetManager::Get();
    const auto path = WriteAudioAsset("tone.wav", "RIFF....WAVEfmt ");
    const Zgine::AssetHandle handle = manager.RegisterAsset(path, Zgine::AssetType::Audio);
    ASSERT_TRUE(handle.IsValid());

    std::vector<std::future<std::shared_ptr<Zgine::Asset>>> futures;
    for (int i = 0; i < 8; ++i) {
        futures.push_back(manager.LoadAssetAsync(handle));
    }

    std::vector<std::shared_ptr<Zgine::Asset>> assets;
    for (auto& future : futures) {
        assets.push_back(future.get());
    }

    ASSERT_FALSE(assets.empty());
    ASSERT_NE(assets.front(), nullptr);
    EXPECT_EQ(assets.front()->GetType(), Zgine::AssetType::Audio);

    for (const auto& asset : assets) {
        ASSERT_NE(asset, nullptr);
        EXPECT_EQ(asset.get(), assets.front().get());
    }

    auto cached = manager.LoadAsset(handle);
    ASSERT_NE(cached, nullptr);
    EXPECT_EQ(cached.get(), assets.front().get());
}

TEST_F(AssetManagerTest, InvalidAsyncLoadReturnsNullAsset) {
    auto future = Zgine::AssetManager::Get().LoadAssetAsync(Zgine::AssetHandle());
    EXPECT_EQ(future.get(), nullptr);
}
