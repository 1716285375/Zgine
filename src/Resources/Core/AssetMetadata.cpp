#include <Zgine/Resources/Core/AssetMetadata.h>
#include <string>

namespace Zgine {

nlohmann::json AssetMetadata::Serialize() const {
    nlohmann::json data;
    data["Version"] = Version;
    data["UUID"] = Handle.ToString();
    data["Type"] = AssetTypeToString(Type);
    data["SourcePath"] = SourcePath.string();
    if (!ImportedPath.empty()) {
        data["ImportedPath"] = ImportedPath.string();
    }
    data["ImportSettings"] = ImportSettings.Serialize(Type);

    nlohmann::json deps = nlohmann::json::array();
    for (const auto& dep : Dependencies) {
        deps.push_back(dep.ToString());
    }
    data["Dependencies"] = deps;

    return data;
}

std::optional<AssetMetadata> AssetMetadata::Deserialize(const nlohmann::json& data) {
    if (!data.contains("UUID") || !data.contains("Type") || !data.contains("SourcePath")) {
        return std::nullopt;
    }

    AssetMetadata metadata;
    if (data.contains("Version")) {
        metadata.Version = data["Version"].get<uint32_t>();
    }
    metadata.Handle = AssetHandle::FromString(data["UUID"].get<std::string>());
    metadata.Type = AssetTypeFromString(data["Type"].get<std::string>());
    metadata.SourcePath = data["SourcePath"].get<std::string>();
    if (data.contains("ImportedPath")) {
        metadata.ImportedPath = data["ImportedPath"].get<std::string>();
    }
    if (data.contains("ImportSettings")) {
        metadata.ImportSettings.Deserialize(metadata.Type, data["ImportSettings"]);
    }
    if (data.contains("Dependencies")) {
        for (const auto& entry : data["Dependencies"]) {
            if (!entry.is_string()) {
                continue;
            }
            metadata.Dependencies.push_back(AssetHandle::FromString(entry.get<std::string>()));
        }
    }

    return metadata;
}

}
