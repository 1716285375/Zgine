#pragma once

#include <Zgine/Resources/Prefab/PrefabAsset.h>
#include <Zgine/World/Core/Entity.h>

#include <filesystem>
#include <optional>
#include <string>

namespace Zgine {

class World;

class PrefabSerializer {
public:
    [[nodiscard]] static std::optional<PrefabAsset> CreateFromEntityHierarchy(Entity root);
    [[nodiscard]] static Entity Instantiate(const PrefabAsset& prefab, World& targetWorld);

    [[nodiscard]] static std::string SerializeToString(const PrefabAsset& prefab);
    [[nodiscard]] static std::optional<PrefabAsset> DeserializeFromString(std::string_view data);

    [[nodiscard]] static bool SaveToFile(const PrefabAsset& prefab, const std::filesystem::path& path);
    [[nodiscard]] static std::optional<PrefabAsset> LoadFromFile(const std::filesystem::path& path);
};

} // namespace Zgine
