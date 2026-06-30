#pragma once

#include <Zgine/Resources/Core/AssetHandle.h>
#include <Zgine/Resources/Core/AssetMetadata.h>
#include <Zgine/Resources/Core/AssetType.h>

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Zgine {

struct AssetDatabaseConfig {
    std::filesystem::path AssetsRoot = "assets";
    bool IncludeDirectories = true;
    bool IncludeUnknownFiles = true;
};

struct AssetRecord {
    AssetHandle Handle;
    AssetType Type = AssetType::Unknown;
    std::filesystem::path SourcePath;
    std::filesystem::path RelativePath;
    bool IsDirectory = false;
    bool HasMetadata = false;
};

class AssetDatabase {
public:
    void Scan(const AssetDatabaseConfig& config = {});
    void Clear();

    [[nodiscard]] bool IsScanned() const { return m_Scanned; }
    [[nodiscard]] const std::filesystem::path& GetAssetsRoot() const { return m_AssetsRoot; }
    [[nodiscard]] const std::vector<AssetRecord>& GetRecords() const { return m_Records; }

    [[nodiscard]] const AssetRecord* GetRecordByPath(const std::filesystem::path& path) const;
    [[nodiscard]] const AssetRecord* GetRecordByHandle(AssetHandle handle) const;
    [[nodiscard]] std::vector<const AssetRecord*> GetRecordsByType(AssetType type) const;

private:
    static std::filesystem::path NormalizeAbsolutePath(const std::filesystem::path& path);
    static std::filesystem::path NormalizeRelativePath(const std::filesystem::path& root,
                                                       const std::filesystem::path& path);
    static std::filesystem::path GetMetaPath(const std::filesystem::path& sourcePath);
    static std::optional<AssetMetadata> LoadMetadata(const std::filesystem::path& metaPath);

    [[nodiscard]] std::string MakePathKey(const std::filesystem::path& path) const;
    void AddRecord(AssetRecord record);

    std::filesystem::path m_AssetsRoot;
    std::vector<AssetRecord> m_Records;
    std::unordered_map<std::string, size_t> m_PathToRecord;
    std::unordered_map<AssetHandle, size_t> m_HandleToRecord;
    bool m_Scanned = false;
};

} // namespace Zgine
