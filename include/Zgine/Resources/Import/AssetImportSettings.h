#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <Zgine/Resources/Core/AssetType.h>

namespace Zgine {

struct TextureImportSettings {
    bool GenerateMipmaps = true;
    bool SRGB = true;
    bool ClampToEdge = false;
    bool Linear = true;
    bool Compress = false;
};

struct MeshImportSettings {
    bool Triangulate = true;
    bool FlipUVs = true;
    bool CalcTangents = true;
};

struct AudioImportSettings {
    bool Stream = true;
};

struct ShaderImportSettings {
    std::string VertexPath;
    std::string FragmentPath;
    bool Optimize = false;
};

struct AssetImportSettings {
    TextureImportSettings Texture;
    MeshImportSettings Mesh;
    AudioImportSettings Audio;
    ShaderImportSettings Shader;

    nlohmann::json Serialize(AssetType type) const;
    void Deserialize(AssetType type, const nlohmann::json& data);
};

}
