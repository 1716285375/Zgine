#pragma once

#include <Zgine/Resources/Core/AssetHandle.h>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <optional>
#include <string>

namespace Zgine {

struct PrefabAsset {
    static constexpr uint32_t FormatVersion = 1;

    AssetHandle Handle;
    std::string Name;
    std::string TemplateRootUUID;
    nlohmann::json Entities = nlohmann::json::array();

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] nlohmann::json Serialize() const;

    static std::optional<PrefabAsset> Deserialize(const nlohmann::json& data);
};

} // namespace Zgine
