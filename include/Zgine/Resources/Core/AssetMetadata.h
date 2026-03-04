#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>
#include <Zgine/Resources/Core/AssetHandle.h>
#include <Zgine/Resources/Core/AssetType.h>
#include <Zgine/Resources/Import/AssetImportSettings.h>

namespace Zgine {

struct AssetMetadata {
    AssetHandle Handle;
    AssetType Type = AssetType::Unknown;
    std::filesystem::path SourcePath;
    std::filesystem::path ImportedPath;
    AssetImportSettings ImportSettings;
    std::vector<AssetHandle> Dependencies;
    uint32_t Version = 1;

    nlohmann::json Serialize() const;
    static std::optional<AssetMetadata> Deserialize(const nlohmann::json& data);
};

}
