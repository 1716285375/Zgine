#include <Zgine/Resources/Import/AssetImporter.h>
#include <Zgine/Resources/Core/AssetManager.h>
#include <Zgine/Platform/IO/File.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Macro.h>
#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Resources/Mesh/MeshLoader.h>
#include <filesystem>
#include <unordered_set>
#include <cctype>

namespace Zgine {

namespace {
    size_t CalculateMeshSize(const std::vector<std::shared_ptr<Mesh>>& meshes) {
        size_t total = 0;
        for (const auto& mesh : meshes) {
            if (!mesh) {
                continue;
            }
            total += mesh->GetVertexCount() * sizeof(Vertex);
            total += mesh->GetIndexCount() * sizeof(unsigned int);
        }
        return total;
    }
}

AssetImportResult TextureImporter::Import(const AssetMetadata& metadata, AssetImportContext& context) {
    ZGINE_UNUSED(context);
    AssetImportResult result;

    if (metadata.SourcePath.empty()) {
        ZGINE_CORE_WARN("TextureImporter: missing source path");
        return result;
    }

    TextureSettings settings;
    settings.GenerateMipmaps = metadata.ImportSettings.Texture.GenerateMipmaps;
    settings.SRGB = metadata.ImportSettings.Texture.SRGB;
    settings.ClampToEdge = metadata.ImportSettings.Texture.ClampToEdge;
    settings.Linear = metadata.ImportSettings.Texture.Linear;

    auto texture = Texture::Create(metadata.SourcePath.string(), settings);
    if (!texture || texture->GetID() == 0) {
        ZGINE_CORE_ERROR("TextureImporter: failed to load {}", metadata.SourcePath.string());
        return result;
    }

    size_t sizeBytes = static_cast<size_t>(texture->GetWidth()) *
                       static_cast<size_t>(texture->GetHeight()) * 4u;
    result.AssetData = std::make_shared<TextureAsset>(metadata.Handle, texture, sizeBytes);
    return result;
}

AssetImportResult MeshImporter::Import(const AssetMetadata& metadata, AssetImportContext& context) {
    AssetImportResult result;

    if (metadata.SourcePath.empty()) {
        ZGINE_CORE_WARN("MeshImporter: missing source path");
        return result;
    }

    auto meshes = MeshLoader::LoadModel(metadata.SourcePath.string(), metadata.ImportSettings.Mesh);
    if (meshes.empty()) {
        ZGINE_CORE_ERROR("MeshImporter: failed to load {}", metadata.SourcePath.string());
        return result;
    }

    size_t sizeBytes = CalculateMeshSize(meshes);
    result.AssetData = std::make_shared<MeshAsset>(metadata.Handle, meshes, sizeBytes);

    // Note: Mesh textures are no longer tracked at the mesh level
    // Texture dependencies should be managed separately if needed

    return result;
}

AssetImportResult AudioImporter::Import(const AssetMetadata& metadata, AssetImportContext& context) {
    ZGINE_UNUSED(context);
    AssetImportResult result;
    if (metadata.SourcePath.empty()) {
        ZGINE_CORE_WARN("AudioImporter: missing source path");
        return result;
    }

    std::error_code ec;
    size_t sizeBytes = 0;
    if (std::filesystem::exists(metadata.SourcePath, ec)) {
        sizeBytes = static_cast<size_t>(std::filesystem::file_size(metadata.SourcePath, ec));
    }
    result.AssetData = std::make_shared<AudioAsset>(metadata.Handle, metadata.SourcePath.string(), sizeBytes);
    return result;
}

AssetImportResult ShaderImporter::Import(const AssetMetadata& metadata, AssetImportContext& context) {
    AssetImportResult result;
    if (metadata.SourcePath.empty()) {
        ZGINE_CORE_WARN("ShaderImporter: missing source path");
        return result;
    }

    ShaderImportSettings settings = metadata.ImportSettings.Shader;
    std::filesystem::path source = metadata.SourcePath;
    std::string ext = source.extension().string();
    for (auto& c : ext) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    if (settings.VertexPath.empty() || settings.FragmentPath.empty()) {
        if (ext == ".vert") {
            settings.VertexPath = source.string();
            settings.FragmentPath = source.replace_extension(".frag").string();
        } else if (ext == ".frag") {
            settings.FragmentPath = source.string();
            settings.VertexPath = source.replace_extension(".vert").string();
        } else {
            ZGINE_CORE_WARN("ShaderImporter: cannot infer shader pair for {}", metadata.SourcePath.string());
            return result;
        }
    }

    std::string vertexSrc = File::ReadFile(settings.VertexPath);
    std::string fragmentSrc = File::ReadFile(settings.FragmentPath);
    if (vertexSrc.empty() || fragmentSrc.empty()) {
        ZGINE_CORE_ERROR("ShaderImporter: failed to read shader sources for {}", metadata.SourcePath.string());
        return result;
    }

    std::string shaderName = metadata.SourcePath.stem().string();
    auto shader = Shader::Create(shaderName, vertexSrc, fragmentSrc);
    size_t sizeBytes = vertexSrc.size() + fragmentSrc.size();
    result.AssetData = std::make_shared<ShaderAsset>(metadata.Handle, shader, sizeBytes);

    if (context.Manager) {
        AssetHandle vertexHandle = context.Manager->GetHandleFromPath(settings.VertexPath);
        if (vertexHandle.IsValid()) {
            result.Dependencies.push_back(vertexHandle);
        }
        AssetHandle fragmentHandle = context.Manager->GetHandleFromPath(settings.FragmentPath);
        if (fragmentHandle.IsValid()) {
            result.Dependencies.push_back(fragmentHandle);
        }
    }

    return result;
}

}
