#include <Zgine/Resources/Core/AssetManager.h>
#include <Zgine/Core/OS/File.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Macro.h>
#include <algorithm>
#include <fstream>

namespace Zgine {

AssetManager& AssetManager::Get() {
    static AssetManager instance;
    return instance;
}

void AssetManager::CacheEntryDeleter::operator()(CacheEntry* entry) const {
    if (!entry) {
        return;
    }
    entry->~CacheEntry();
    if (Pool) {
        Pool->Deallocate(entry);
    } else {
        ::operator delete(entry);
    }
}

void AssetManager::Initialize(const AssetManagerConfig& config) {
    m_Config = config;
    m_Metadata.clear();
    m_PathToHandle.clear();
    m_Cache.clear();
    m_DirtyAssets.clear();
    m_CurrentCacheBytes = 0;
    m_AccessCounter = 0;

    if (m_Config.CacheEntryPoolSize > 0) {
        m_CacheEntryPool = std::make_unique<MemoryPool>(sizeof(CacheEntry), m_Config.CacheEntryPoolSize);
    } else {
        m_CacheEntryPool.reset();
    }

    RegisterImporters();

    m_FileWatcher.SetCallback([this](const std::filesystem::path& path, FileStatus status) {
        OnFileChanged(path, status);
    });

    m_Initialized = true;
    ScanAssets();
}

void AssetManager::Shutdown() {
    m_FileWatcher.Clear();
    m_Importers.clear();
    m_Cache.clear();
    m_Metadata.clear();
    m_PathToHandle.clear();
    m_DirtyAssets.clear();
    m_CacheEntryPool.reset();
    m_CurrentCacheBytes = 0;
    m_AccessCounter = 0;
    m_Initialized = false;
}

void AssetManager::RegisterImporters() {
    m_Importers.clear();
    m_Importers[AssetType::Texture] = std::make_unique<TextureImporter>();
    m_Importers[AssetType::Mesh] = std::make_unique<MeshImporter>();
    m_Importers[AssetType::Audio] = std::make_unique<AudioImporter>();
    m_Importers[AssetType::Shader] = std::make_unique<ShaderImporter>();
}

AssetManager::CacheEntryPtr AssetManager::CreateCacheEntry(const std::shared_ptr<Asset>& asset) {
    CacheEntry* entry = nullptr;
    if (m_CacheEntryPool) {
        entry = static_cast<CacheEntry*>(m_CacheEntryPool->Allocate());
    } else {
        entry = static_cast<CacheEntry*>(::operator new(sizeof(CacheEntry)));
    }
    new (entry) CacheEntry();
    entry->AssetData = asset;
    entry->SizeBytes = asset ? asset->GetSizeBytes() : 0;
    entry->LastAccess = ++m_AccessCounter;
    return CacheEntryPtr(entry, CacheEntryDeleter{m_CacheEntryPool.get()});
}

std::string AssetManager::NormalizePath(const std::filesystem::path& path) const {
    std::error_code ec;
    auto normalized = std::filesystem::weakly_canonical(path, ec);
    if (ec) {
        return path.lexically_normal().string();
    }
    return normalized.string();
}

std::filesystem::path AssetManager::GetMetaPath(const std::filesystem::path& assetPath) const {
    return assetPath.string() + ".meta";
}

void AssetManager::SaveMetadata(const AssetMetadata& metadata) const {
    const auto metaPath = GetMetaPath(metadata.SourcePath);
    auto data = metadata.Serialize();
    File::WriteFile(metaPath.string(), data.dump(4));
}

std::optional<AssetMetadata> AssetManager::LoadMetadata(const std::filesystem::path& metaPath) const {
    std::ifstream file(metaPath);
    if (!file.is_open()) {
        return std::nullopt;
    }
    try {
        nlohmann::json data;
        file >> data;
        return AssetMetadata::Deserialize(data);
    } catch (...) {
        return std::nullopt;
    }
}

void AssetManager::WatchAssetPaths(const AssetMetadata& metadata) {
    m_FileWatcher.Watch(metadata.SourcePath);
    m_FileWatcher.Watch(GetMetaPath(metadata.SourcePath));
}

void AssetManager::ScanAssets() {
    if (!m_Initialized) {
        return;
    }

    std::error_code ec;
    if (!std::filesystem::exists(m_Config.AssetsRoot, ec)) {
        ZGINE_CORE_WARN("AssetManager: assets root not found: {}", m_Config.AssetsRoot.string());
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(m_Config.AssetsRoot, ec)) {
        if (ec) {
            continue;
        }
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto& path = entry.path();
        if (path.extension() == ".meta") {
            continue;
        }

        AssetType type = AssetTypeFromPath(path);
        if (type == AssetType::Unknown) {
            continue;
        }

        RegisterAsset(path, type);
    }
}

AssetHandle AssetManager::RegisterAsset(const std::filesystem::path& assetPath, AssetType type) {
    if (!m_Initialized) {
        return AssetHandle();
    }

    std::string normalized = NormalizePath(assetPath);
    auto existing = m_PathToHandle.find(normalized);
    if (existing != m_PathToHandle.end()) {
        return existing->second;
    }

    if (type == AssetType::Unknown) {
        type = AssetTypeFromPath(assetPath);
    }
    if (type == AssetType::Unknown) {
        return AssetHandle();
    }

    AssetMetadata metadata;
    auto metaPath = GetMetaPath(assetPath);
    auto loaded = LoadMetadata(metaPath);
    if (loaded) {
        metadata = *loaded;
        metadata.Type = type;
        metadata.SourcePath = assetPath;
        if (!metadata.Handle.IsValid()) {
            metadata.Handle = UUID::New();
        }
    } else {
        metadata.Handle = UUID::New();
        metadata.Type = type;
        metadata.SourcePath = assetPath;
    }

    SaveMetadata(metadata);

    m_Metadata[metadata.Handle] = metadata;
    m_PathToHandle[normalized] = metadata.Handle;
    WatchAssetPaths(metadata);

    return metadata.Handle;
}

AssetHandle AssetManager::ImportAsset(const std::filesystem::path& sourcePath,
                                      const std::filesystem::path& destPath) {
    if (!m_Initialized) {
        return AssetHandle();
    }

    AssetType type = AssetTypeFromPath(sourcePath);
    if (type == AssetType::Unknown) {
        ZGINE_CORE_WARN("AssetManager: unsupported asset type {}", sourcePath.string());
        return AssetHandle();
    }

    std::filesystem::path target = destPath.empty()
        ? (m_Config.AssetsRoot / sourcePath.filename())
        : destPath;

    std::error_code ec;
    std::filesystem::create_directories(target.parent_path(), ec);
    std::filesystem::copy_file(sourcePath, target,
                               std::filesystem::copy_options::overwrite_existing, ec);
    if (ec) {
        ZGINE_CORE_ERROR("AssetManager: failed to copy asset {} ({})", sourcePath.string(), ec.message());
        return AssetHandle();
    }

    return RegisterAsset(target, type);
}

AssetHandle AssetManager::GetHandleFromPath(const std::filesystem::path& path) const {
    std::string normalized = NormalizePath(path);
    auto it = m_PathToHandle.find(normalized);
    if (it != m_PathToHandle.end()) {
        return it->second;
    }
    return AssetHandle();
}

const AssetMetadata* AssetManager::GetMetadata(AssetHandle handle) const {
    auto it = m_Metadata.find(handle);
    if (it == m_Metadata.end()) {
        return nullptr;
    }
    return &it->second;
}

const AssetMetadata* AssetManager::GetMetadata(const std::filesystem::path& path) const {
    AssetHandle handle = GetHandleFromPath(path);
    if (!handle.IsValid()) {
        return nullptr;
    }
    return GetMetadata(handle);
}

AssetImportResult AssetManager::ImportAssetInternal(AssetMetadata& metadata) {
    AssetImportResult result;
    auto importerIt = m_Importers.find(metadata.Type);
    if (importerIt == m_Importers.end()) {
        ZGINE_CORE_WARN("AssetManager: no importer for type {}", AssetTypeToString(metadata.Type));
        return result;
    }

    AssetImportContext context;
    context.Manager = this;
    result = importerIt->second->Import(metadata, context);
    if (!result.AssetData) {
        return result;
    }

    if (result.Dependencies != metadata.Dependencies) {
        metadata.Dependencies = result.Dependencies;
        SaveMetadata(metadata);
    }

    return result;
}

std::shared_ptr<Asset> AssetManager::LoadAssetInternal(AssetHandle handle) {
    auto metaIt = m_Metadata.find(handle);
    if (metaIt == m_Metadata.end()) {
        return nullptr;
    }

    auto cacheIt = m_Cache.find(handle);
    if (cacheIt != m_Cache.end() && cacheIt->second && cacheIt->second->AssetData) {
        cacheIt->second->LastAccess = ++m_AccessCounter;
        return cacheIt->second->AssetData;
    }

    AssetMetadata& metadata = metaIt->second;
    AssetImportResult result = ImportAssetInternal(metadata);
    if (!result.AssetData) {
        return nullptr;
    }

    CacheEntryPtr entry = CreateCacheEntry(result.AssetData);
    m_CurrentCacheBytes += entry->SizeBytes;
    m_Cache[handle] = std::move(entry);
    TrimCache();
    return m_Cache[handle]->AssetData;
}

std::shared_ptr<Asset> AssetManager::LoadAsset(AssetHandle handle) {
    if (!m_Initialized || !handle.IsValid()) {
        return nullptr;
    }

    if (m_DirtyAssets.find(handle) != m_DirtyAssets.end()) {
        UnloadAsset(handle);
        m_DirtyAssets.erase(handle);
    }

    return LoadAssetInternal(handle);
}

std::future<std::shared_ptr<Asset>> AssetManager::LoadAssetAsync(AssetHandle handle) {
    if (!m_Initialized || !handle.IsValid()) {
        return std::async(std::launch::deferred, []() { return std::shared_ptr<Asset>(); });
    }

    const AssetMetadata* metadata = GetMetadata(handle);
    if (!metadata) {
        return std::async(std::launch::deferred, []() { return std::shared_ptr<Asset>(); });
    }

    if (!IsThreadSafeAsset(metadata->Type)) {
        return std::async(std::launch::deferred, [this, handle]() { return LoadAsset(handle); });
    }

    return std::async(std::launch::async, [this, handle]() { return LoadAsset(handle); });
}

void AssetManager::UnloadAsset(AssetHandle handle) {
    auto it = m_Cache.find(handle);
    if (it == m_Cache.end()) {
        return;
    }

    if (it->second && it->second->AssetData && it->second->AssetData.use_count() > 1) {
        return;
    }

    m_CurrentCacheBytes = (m_CurrentCacheBytes >= it->second->SizeBytes)
        ? (m_CurrentCacheBytes - it->second->SizeBytes)
        : 0;
    m_Cache.erase(it);
}

void AssetManager::TrimCache() {
    if (m_CurrentCacheBytes <= m_Config.MaxCacheSizeBytes) {
        return;
    }

    std::vector<std::pair<AssetHandle, uint64_t>> entries;
    entries.reserve(m_Cache.size());
    for (const auto& [handle, entry] : m_Cache) {
        if (!entry) {
            continue;
        }
        entries.emplace_back(handle, entry->LastAccess);
    }

    std::sort(entries.begin(), entries.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    for (const auto& [handle, access] : entries) {
        ZGINE_UNUSED(access);
        if (m_CurrentCacheBytes <= m_Config.MaxCacheSizeBytes) {
            break;
        }
        auto it = m_Cache.find(handle);
        if (it == m_Cache.end() || !it->second || !it->second->AssetData) {
            continue;
        }
        if (it->second->AssetData.use_count() > 1) {
            continue;
        }
        m_CurrentCacheBytes -= it->second->SizeBytes;
        m_Cache.erase(it);
    }
}

void AssetManager::OnFileChanged(const std::filesystem::path& path, FileStatus status) {
    if (!m_HotReloadEnabled) {
        return;
    }

    if (path.extension() == ".meta") {
        auto loaded = LoadMetadata(path);
        if (!loaded) {
            return;
        }
        AssetMetadata metadata = *loaded;
        std::string normalizedSource = NormalizePath(metadata.SourcePath);
        m_Metadata[metadata.Handle] = metadata;
        m_PathToHandle[normalizedSource] = metadata.Handle;
        WatchAssetPaths(metadata);
        MarkDirty(metadata.Handle);
        return;
    }

    AssetHandle handle = GetHandleFromPath(path);
    if (!handle.IsValid()) {
        if (status == FileStatus::Created) {
            RegisterAsset(path);
        }
        return;
    }

    if (status == FileStatus::Removed) {
        UnloadAsset(handle);
    }
    MarkDirty(handle);
}

void AssetManager::MarkDirty(AssetHandle handle) {
    if (!handle.IsValid()) {
        return;
    }
    m_DirtyAssets.insert(handle);
}

void AssetManager::Update() {
    if (!m_Initialized || !m_HotReloadEnabled) {
        return;
    }

    m_FileWatcher.Poll();

    if (m_DirtyAssets.empty()) {
        return;
    }

    std::unordered_set<AssetHandle> pending = std::move(m_DirtyAssets);
    m_DirtyAssets.clear();

    for (const auto& handle : pending) {
        auto metaIt = m_Metadata.find(handle);
        if (metaIt == m_Metadata.end()) {
            continue;
        }
        auto cacheIt = m_Cache.find(handle);
        if (cacheIt == m_Cache.end() || !cacheIt->second || !cacheIt->second->AssetData) {
            continue;
        }
        if (cacheIt->second->AssetData.use_count() > 1) {
            continue;
        }
        AssetImportResult result = ImportAssetInternal(metaIt->second);
        if (!result.AssetData) {
            continue;
        }
        m_CurrentCacheBytes = (m_CurrentCacheBytes >= cacheIt->second->SizeBytes)
            ? (m_CurrentCacheBytes - cacheIt->second->SizeBytes)
            : 0;
        cacheIt->second->AssetData = result.AssetData;
        cacheIt->second->SizeBytes = result.AssetData->GetSizeBytes();
        cacheIt->second->LastAccess = ++m_AccessCounter;
        m_CurrentCacheBytes += cacheIt->second->SizeBytes;
    }

    TrimCache();
}

bool AssetManager::IsThreadSafeAsset(AssetType type) const {
    switch (type) {
        case AssetType::Texture:
        case AssetType::Shader:
            return false;
        default:
            return true;
    }
}

}
