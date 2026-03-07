#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace Zgine {

enum class AssetType : uint8_t {
    Unknown = 0,
    Texture,
    Mesh,
    Audio,
    Shader,
    World,
    Material
};

const char* AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(const std::string& value);
AssetType AssetTypeFromPath(const std::filesystem::path& path);

}
