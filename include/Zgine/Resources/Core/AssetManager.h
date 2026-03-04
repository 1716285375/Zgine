#pragma once

#include <filesystem>
#include <future>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <Zgine/Resources/Core/AssetMetadata.h>
#include <Zgine/Resources/Import/AssetImporter.h>
#include <Zgine/Core/OS/FileWatcher.h>
#include <Zgine/Core/Memory/MemoryPool.h>

namespace Zgine {

struct AssetManagerConfig {
    std::filesystem::path AssetsRoot = "assets";
    size_t MaxCacheSizeBytes = 256 * 1024 * 1024;
    size_t CacheEntryPoolSize = 1024;
};

class AssetManager {
public:
    static AssetManager& Get();

    void Initialize(const AssetManagerConfig& config = {});
    void Shutdown();

    bool IsInitialized() const { return m_Initialized; }

    void ScanAssets();
    AssetHandle ImportAsset(const std::filesystem::path& sourcePath,
                            const std::filesystem::path& destPath = {});
    AssetHandle RegisterAsset(const std::filesystem::path& assetPath, AssetType type = AssetType::Unknown);

    AssetHandle GetHandleFromPath(const std::filesystem::path& path) const;
    const AssetMetadata* GetMetadata(AssetHandle handle) const;
    const AssetMetadata* GetMetadata(const std::filesystem::path& path) const;

    std::shared_ptr<Asset> LoadAsset(AssetHandle handle);

    template <typename T>
    std::shared_ptr<T> LoadAsset(AssetHandle handle) {
        auto asset = LoadAsset(handle);
        return std::dynamic_pointer_cast<T>(asset);
    }

    std::future<std::shared_ptr<Asset>> LoadAssetAsync(AssetHandle handle);

    void UnloadAsset(AssetHandle handle);
    void TrimCache();
    void Update();

    void SetHotReloadEnabled(bool enabled) { m_HotReloadEnabled = enabled; }
    bool IsHotReloadEnabled() const { return m_HotReloadEnabled; }

    const std::filesystem::path& GetAssetsRoot() const { return m_Config.AssetsRoot; }

private:
    struct CacheEntry {
        std::shared_ptr<Asset> AssetData;
        size_t SizeBytes = 0;
        uint64_t LastAccess = 0;
    };

    struct CacheEntryDeleter {
        MemoryPool* Pool = nullptr;
        void operator()(CacheEntry* entry) const;
    };

    using CacheEntryPtr = std::unique_ptr<CacheEntry, CacheEntryDeleter>;

    AssetManager() = default;

    void RegisterImporters();
    CacheEntryPtr CreateCacheEntry(const std::shared_ptr<Asset>& asset);
    std::shared_ptr<Asset> LoadAssetInternal(AssetHandle handle);
    AssetImportResult ImportAssetInternal(AssetMetadata& metadata);
    void SaveMetadata(const AssetMetadata& metadata) const;
    std::optional<AssetMetadata> LoadMetadata(const std::filesystem::path& metaPath) const;
    std::filesystem::path GetMetaPath(const std::filesystem::path& assetPath) const;
    std::string NormalizePath(const std::filesystem::path& path) const;
    void WatchAssetPaths(const AssetMetadata& metadata);
    void OnFileChanged(const std::filesystem::path& path, FileStatus status);
    void MarkDirty(AssetHandle handle);
    bool IsThreadSafeAsset(AssetType type) const;

    AssetManagerConfig m_Config;
    bool m_Initialized = false;
    bool m_HotReloadEnabled = true;

    std::unordered_map<AssetHandle, AssetMetadata> m_Metadata;
    std::unordered_map<std::string, AssetHandle> m_PathToHandle;

    std::unordered_map<AssetHandle, CacheEntryPtr> m_Cache;
    size_t m_CurrentCacheBytes = 0;
    uint64_t m_AccessCounter = 0;

    std::unique_ptr<MemoryPool> m_CacheEntryPool;
    FileWatcher m_FileWatcher;
    std::unordered_set<AssetHandle> m_DirtyAssets;
    std::unordered_map<AssetType, std::unique_ptr<AssetImporter>> m_Importers;
};

}
