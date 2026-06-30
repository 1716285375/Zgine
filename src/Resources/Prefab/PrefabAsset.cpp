#include <Zgine/Resources/Prefab/PrefabAsset.h>
#include <Zgine/Resources/Core/AssetType.h>

namespace Zgine {

bool PrefabAsset::IsValid() const {
    return !TemplateRootUUID.empty() && Entities.is_array() && !Entities.empty();
}

nlohmann::json PrefabAsset::Serialize() const {
    nlohmann::json data;
    data["Version"] = FormatVersion;
    data["Type"] = AssetTypeToString(AssetType::Prefab);
    data["Handle"] = Handle.ToString();
    data["Name"] = Name;
    data["TemplateRoot"] = TemplateRootUUID;
    data["Entities"] = Entities;
    return data;
}

std::optional<PrefabAsset> PrefabAsset::Deserialize(const nlohmann::json& data) {
    if (!data.is_object() || !data.contains("Entities") || !data["Entities"].is_array()) {
        return std::nullopt;
    }

    PrefabAsset prefab;
    if (data.contains("Handle") && data["Handle"].is_string()) {
        prefab.Handle = AssetHandle::FromString(data["Handle"].get<std::string>());
    }
    if (data.contains("Name") && data["Name"].is_string()) {
        prefab.Name = data["Name"].get<std::string>();
    }
    if (data.contains("TemplateRoot") && data["TemplateRoot"].is_string()) {
        prefab.TemplateRootUUID = data["TemplateRoot"].get<std::string>();
    }
    prefab.Entities = data["Entities"];

    if (!prefab.IsValid()) {
        return std::nullopt;
    }

    return prefab;
}

} // namespace Zgine
