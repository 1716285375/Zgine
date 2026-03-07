#include <Zgine/Resources/Mesh/MeshLoader.h>
#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Macro.h>
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/postprocess.h>
#include <filesystem>

namespace Zgine {

namespace {

std::shared_ptr<Texture> LoadEmbeddedTexture(const aiTexture* texture, const std::string& debugName) {
    if (!texture) {
        return nullptr;
    }

    if (texture->mHeight == 0) {
        const unsigned char* data = reinterpret_cast<const unsigned char*>(texture->pcData);
        int size = static_cast<int>(texture->mWidth);
        return Texture::Create(data, size, debugName);
    }

    const unsigned int texelCount = texture->mWidth * texture->mHeight;
    std::vector<unsigned char> rgba;
    rgba.resize(static_cast<size_t>(texelCount) * 4u);

    for (unsigned int i = 0; i < texelCount; ++i) {
        const aiTexel& texel = texture->pcData[i];
        const size_t offset = static_cast<size_t>(i) * 4u;
        rgba[offset + 0] = texel.r;
        rgba[offset + 1] = texel.g;
        rgba[offset + 2] = texel.b;
        rgba[offset + 3] = texel.a;
    }

    return Texture::Create(rgba.data(),
                           static_cast<int>(texture->mWidth),
                           static_cast<int>(texture->mHeight),
                           debugName);
}

}

std::vector<std::shared_ptr<Mesh>> MeshLoader::LoadModel(const std::string& path,
                                                         const MeshImportSettings& settings) {
    std::vector<std::shared_ptr<Mesh>> meshes;

    Assimp::Importer importer;
    unsigned int flags = 0;
    if (settings.Triangulate) {
        flags |= aiProcess_Triangulate;
    }
    if (settings.FlipUVs) {
        flags |= aiProcess_FlipUVs;
    }
    if (settings.CalcTangents) {
        flags |= aiProcess_CalcTangentSpace;
    }

    const aiScene* World = importer.ReadFile(path, flags);

    if (!World || World->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !World->mRootNode) {
        ZGINE_CORE_ERROR("Assimp error: {0}", importer.GetErrorString());
        return meshes;
    }

    std::filesystem::path filePath(path);
    std::string directory = filePath.parent_path().string();
    if (directory.empty()) {
        directory = ".";
    }

    ProcessNode(World->mRootNode, World, directory, meshes);
    return meshes;
}

std::shared_ptr<Mesh> MeshLoader::LoadMesh(const std::string& path,
                                           const MeshImportSettings& settings) {
    auto meshes = LoadModel(path, settings);
    if (meshes.empty()) {
        return nullptr;
    }
    return meshes[0]; // 返回第一个网�?
}

void MeshLoader::ProcessNode(const aiNode* node, const aiScene* World,
                             const std::string& directory, std::vector<std::shared_ptr<Mesh>>& meshes) {
    // 处理当前节点的所有网�?
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = World->mMeshes[node->mMeshes[i]];
        MeshData meshData = ProcessMesh(mesh, World, directory);
        meshes.push_back(std::make_shared<Mesh>(meshData));
    }

    // 递归处理子节�?
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], World, directory, meshes);
    }
}

MeshData MeshLoader::ProcessMesh(const aiMesh* mesh, const aiScene* World, const std::string& directory) {
    ZGINE_UNUSED(directory);
    MeshData data;

    // 处理顶点
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // 位置
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        // 法线
        if (mesh->mNormals) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        } else {
            vertex.Normal = Math::Vector3(0.0f);
        }

        // 纹理坐标
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.TexCoords = Math::Vector2(0.0f);
        }

        // 顶点颜色
        if (mesh->mColors[0]) {
            vertex.Color.r = mesh->mColors[0][i].r;
            vertex.Color.g = mesh->mColors[0][i].g;
            vertex.Color.b = mesh->mColors[0][i].b;
            vertex.Color.a = mesh->mColors[0][i].a;
        } else {
            vertex.Color = Math::Vector4(1.0f);
        }

        data.Vertices.push_back(vertex);
    }

    // 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            data.Indices.push_back(face.mIndices[j]);
        }
    }

    // 处理材质
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = World->mMaterials[mesh->mMaterialIndex];

        aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
        if (aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &baseColor) == AI_SUCCESS ||
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &baseColor) == AI_SUCCESS) {
            data.BaseColor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };
        }
        // Note: Textures are no longer loaded at mesh data level
    }

    return data;
}

std::vector<std::shared_ptr<Texture>> MeshLoader::LoadMaterialTextures(const aiMaterial* mat,
                                                                       const aiScene* World,
                                                                       int type, const std::string& typeName,
                                                                       const std::string& directory) {
    std::vector<std::shared_ptr<Texture>> textures;
    ZGINE_UNUSED(typeName);

    for (unsigned int i = 0; i < mat->GetTextureCount((aiTextureType)type); i++) {
        aiString str;
        mat->GetTexture((aiTextureType)type, i, &str);

        std::string filename = std::string(str.C_Str());
        if (filename.empty()) {
            continue;
        }

        const aiTexture* embedded = World ? World->GetEmbeddedTexture(str.C_Str()) : nullptr;
        if (embedded) {
            auto texture = LoadEmbeddedTexture(embedded, filename);
            if (texture) {
                textures.push_back(texture);
            } else {
                ZGINE_CORE_WARN("Failed to load embedded texture: {0}", filename);
            }
            continue;
        }

        std::filesystem::path fullPath = std::filesystem::path(directory) / filename;
        fullPath = fullPath.lexically_normal();

        auto texture = Texture::Create(fullPath.string());
        if (texture) {
            textures.push_back(texture);
        } else {
            ZGINE_CORE_WARN("Failed to load texture: {0}", fullPath.string());
        }
    }

    return textures;
}

}

