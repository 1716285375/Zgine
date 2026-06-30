#include <Zgine/Resources/Core/AssetDatabase.h>

#include <algorithm>
#include <fstream>

namespace Zgine {

namespace {

    std::string PathSortKey(const std::filesystem::path& path) {
        return path.generic_string();
    }

} // namespace

void AssetDatabase::Scan(const AssetDatabaseConfig& config) {
    Clear();
    m_AssetsRoot = NormalizeAbsolutePath(config.AssetsRoot);
    m_Scanned = true;

    std::error_code ec;
    if (!std::filesystem::exists(m_AssetsRoot, ec) ||
        !std::filesystem::is_directory(m_AssetsRoot, ec)) {
        return;
    }

    std::vector<AssetRecord> records;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(m_AssetsRoot, ec)) {
        if (ec) {
            break;
        }

        const std::filesystem::path sourcePath = entry.path();
        if (entry.is_directory(ec)) {
            if (!config.IncludeDirectories) {
                continue;
            }

            AssetRecord record;
            record.Type = AssetType::Folder;
            record.SourcePath = NormalizeAbsolutePath(sourcePath);
            record.RelativePath = NormalizeRelativePath(m_AssetsRoot, record.SourcePath);
            record.IsDirectory = true;
            records.push_back(std::move(record));
            continue;
        }

        if (!entry.is_regular_file(ec)) {
            continue;
        }

        if (sourcePath.extension() == ".meta") {
            continue;
        }

        AssetType type = AssetTypeFromPath(sourcePath);
        if (type == AssetType::Unknown && !config.IncludeUnknownFiles) {
            continue;
        }

        AssetRecord record;
        record.Type = type;
        record.SourcePath = NormalizeAbsolutePath(sourcePath);
        record.RelativePath = NormalizeRelativePath(m_AssetsRoot, record.SourcePath);

        if (auto metadata = LoadMetadata(GetMetaPath(sourcePath))) {
            record.Handle = metadata->Handle;
            record.Type = metadata->Type == AssetType::Unknown ? record.Type : metadata->Type;
            record.HasMetadata = true;
        }

        records.push_back(std::move(record));
    }

    std::sort(records.begin(), records.end(), [](const AssetRecord& left, const AssetRecord& right) {
        if (left.RelativePath == right.RelativePath) {
            return static_cast<uint8_t>(left.Type) < static_cast<uint8_t>(right.Type);
        }
        return PathSortKey(left.RelativePath) < PathSortKey(right.RelativePath);
    });

    for (AssetRecord& record : records) {
        AddRecord(std::move(record));
    }
}

void AssetDatabase::Clear() {
    m_AssetsRoot.clear();
    m_Records.clear();
    m_PathToRecord.clear();
    m_HandleToRecord.clear();
    m_Scanned = false;
}

const AssetRecord* AssetDatabase::GetRecordByPath(const std::filesystem::path& path) const {
    auto it = m_PathToRecord.find(MakePathKey(path));
    if (it == m_PathToRecord.end()) {
        return nullptr;
    }

    return &m_Records[it->second];
}

const AssetRecord* AssetDatabase::GetRecordByHandle(AssetHandle handle) const {
    if (!handle.IsValid()) {
        return nullptr;
    }

    auto it = m_HandleToRecord.find(handle);
    if (it == m_HandleToRecord.end()) {
        return nullptr;
    }

    return &m_Records[it->second];
}

std::vector<const AssetRecord*> AssetDatabase::GetRecordsByType(AssetType type) const {
    std::vector<const AssetRecord*> records;
    for (const AssetRecord& record : m_Records) {
        if (record.Type == type) {
            records.push_back(&record);
        }
    }
    return records;
}

std::filesystem::path AssetDatabase::NormalizeAbsolutePath(const std::filesystem::path& path) {
    std::error_code ec;
    const std::filesystem::path absolute = path.is_absolute()
        ? path
        : std::filesystem::absolute(path, ec);
    if (ec) {
        return path.lexically_normal();
    }

    const std::filesystem::path canonical = std::filesystem::weakly_canonical(absolute, ec);
    return ec ? absolute.lexically_normal() : canonical.lexically_normal();
}

std::filesystem::path AssetDatabase::NormalizeRelativePath(const std::filesystem::path& root,
                                                           const std::filesystem::path& path) {
    std::error_code ec;
    const std::filesystem::path relative = std::filesystem::relative(path, root, ec);
    return ec ? path.filename() : relative.lexically_normal();
}

std::filesystem::path AssetDatabase::GetMetaPath(const std::filesystem::path& sourcePath) {
    return sourcePath.string() + ".meta";
}

std::optional<AssetMetadata> AssetDatabase::LoadMetadata(const std::filesystem::path& metaPath) {
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

std::string AssetDatabase::MakePathKey(const std::filesystem::path& path) const {
    const std::filesystem::path absolutePath = path.is_absolute()
        ? path
        : (m_AssetsRoot / path);
    return NormalizeAbsolutePath(absolutePath).generic_string();
}

void AssetDatabase::AddRecord(AssetRecord record) {
    const size_t index = m_Records.size();
    const std::string sourceKey = NormalizeAbsolutePath(record.SourcePath).generic_string();
    const std::string relativeKey = MakePathKey(record.RelativePath);

    m_Records.push_back(std::move(record));
    m_PathToRecord[sourceKey] = index;
    m_PathToRecord[relativeKey] = index;

    const AssetHandle handle = m_Records[index].Handle;
    if (handle.IsValid()) {
        m_HandleToRecord[handle] = index;
    }
}

} // namespace Zgine
