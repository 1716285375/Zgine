#include <Zgine/Resources/Core/AssetType.h>
#include <algorithm>
#include <cctype>

namespace Zgine {

namespace {
    std::string ToLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    bool PathContainsFolder(const std::filesystem::path& path, const char* folder) {
        for (const auto& part : path) {
            if (ToLower(part.string()) == ToLower(folder)) {
                return true;
            }
        }
        return false;
    }
}

const char* AssetTypeToString(AssetType type) {
    switch (type) {
        case AssetType::Folder: return "Folder";
        case AssetType::Texture: return "Texture";
        case AssetType::Mesh: return "Mesh";
        case AssetType::Audio: return "Audio";
        case AssetType::Shader: return "Shader";
        case AssetType::World: return "World";
        case AssetType::Material: return "Material";
        case AssetType::Prefab: return "Prefab";
        case AssetType::Script: return "Script";
        default: return "Unknown";
    }
}

AssetType AssetTypeFromString(const std::string& value) {
    std::string lower = ToLower(value);
    if (lower == "folder" || lower == "directory") return AssetType::Folder;
    if (lower == "texture") return AssetType::Texture;
    if (lower == "mesh") return AssetType::Mesh;
    if (lower == "audio") return AssetType::Audio;
    if (lower == "shader") return AssetType::Shader;
    if (lower == "world" || lower == "scene") return AssetType::World;
    if (lower == "material") return AssetType::Material;
    if (lower == "prefab") return AssetType::Prefab;
    if (lower == "script") return AssetType::Script;
    return AssetType::Unknown;
}

AssetType AssetTypeFromPath(const std::filesystem::path& path) {
    std::string ext = ToLower(path.extension().string());
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" ||
        ext == ".bmp" || ext == ".hdr" || ext == ".exr") {
        return AssetType::Texture;
    }
    if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" ||
        ext == ".blend" || ext == ".dae" || ext == ".3ds") {
        return AssetType::Mesh;
    }
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac") {
        return AssetType::Audio;
    }
    if (ext == ".vert" || ext == ".frag" || ext == ".glsl" || ext == ".comp" ||
        ext == ".geom" || ext == ".spv" || ext == ".hlsl") {
        return AssetType::Shader;
    }
    if ((ext == ".json" && PathContainsFolder(path, "scenes")) ||
        ext == ".zgscene" || ext == ".scene" || ext == ".world") {
        return AssetType::World;
    }
    if (ext == ".mat" || ext == ".material") {
        return AssetType::Material;
    }
    if (ext == ".prefab" || ext == ".zgprefab") {
        return AssetType::Prefab;
    }
    if (ext == ".lua") {
        return AssetType::Script;
    }
    return AssetType::Unknown;
}

}
