#include <Zgine/Resources/Import/AssetImportSettings.h>

namespace Zgine {

namespace {
    nlohmann::json SerializeTexture(const TextureImportSettings& settings) {
        nlohmann::json data;
        data["GenerateMipmaps"] = settings.GenerateMipmaps;
        data["SRGB"] = settings.SRGB;
        data["ClampToEdge"] = settings.ClampToEdge;
        data["Linear"] = settings.Linear;
        data["Compress"] = settings.Compress;
        return data;
    }

    nlohmann::json SerializeMesh(const MeshImportSettings& settings) {
        nlohmann::json data;
        data["Triangulate"] = settings.Triangulate;
        data["FlipUVs"] = settings.FlipUVs;
        data["CalcTangents"] = settings.CalcTangents;
        return data;
    }

    nlohmann::json SerializeAudio(const AudioImportSettings& settings) {
        nlohmann::json data;
        data["Stream"] = settings.Stream;
        return data;
    }

    nlohmann::json SerializeShader(const ShaderImportSettings& settings) {
        nlohmann::json data;
        data["VertexPath"] = settings.VertexPath;
        data["FragmentPath"] = settings.FragmentPath;
        data["Optimize"] = settings.Optimize;
        return data;
    }

    void DeserializeTexture(const nlohmann::json& data, TextureImportSettings& settings) {
        if (data.contains("GenerateMipmaps")) settings.GenerateMipmaps = data["GenerateMipmaps"].get<bool>();
        if (data.contains("SRGB")) settings.SRGB = data["SRGB"].get<bool>();
        if (data.contains("ClampToEdge")) settings.ClampToEdge = data["ClampToEdge"].get<bool>();
        if (data.contains("Linear")) settings.Linear = data["Linear"].get<bool>();
        if (data.contains("Compress")) settings.Compress = data["Compress"].get<bool>();
    }

    void DeserializeMesh(const nlohmann::json& data, MeshImportSettings& settings) {
        if (data.contains("Triangulate")) settings.Triangulate = data["Triangulate"].get<bool>();
        if (data.contains("FlipUVs")) settings.FlipUVs = data["FlipUVs"].get<bool>();
        if (data.contains("CalcTangents")) settings.CalcTangents = data["CalcTangents"].get<bool>();
    }

    void DeserializeAudio(const nlohmann::json& data, AudioImportSettings& settings) {
        if (data.contains("Stream")) settings.Stream = data["Stream"].get<bool>();
    }

    void DeserializeShader(const nlohmann::json& data, ShaderImportSettings& settings) {
        if (data.contains("VertexPath")) settings.VertexPath = data["VertexPath"].get<std::string>();
        if (data.contains("FragmentPath")) settings.FragmentPath = data["FragmentPath"].get<std::string>();
        if (data.contains("Optimize")) settings.Optimize = data["Optimize"].get<bool>();
    }
}

nlohmann::json AssetImportSettings::Serialize(AssetType type) const {
    switch (type) {
        case AssetType::Texture: return SerializeTexture(Texture);
        case AssetType::Mesh: return SerializeMesh(Mesh);
        case AssetType::Audio: return SerializeAudio(Audio);
        case AssetType::Shader: return SerializeShader(Shader);
        default: return nlohmann::json::object();
    }
}

void AssetImportSettings::Deserialize(AssetType type, const nlohmann::json& data) {
    switch (type) {
        case AssetType::Texture: DeserializeTexture(data, Texture); break;
        case AssetType::Mesh: DeserializeMesh(data, Mesh); break;
        case AssetType::Audio: DeserializeAudio(data, Audio); break;
        case AssetType::Shader: DeserializeShader(data, Shader); break;
        default: break;
    }
}

}
