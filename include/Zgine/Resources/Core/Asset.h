#pragma once

#include <memory>
#include <vector>
#include <Zgine/Resources/Core/AssetHandle.h>
#include <Zgine/Resources/Core/AssetType.h>

namespace Zgine {

class Texture;
class Mesh;
class Shader;

class Asset {
public:
    explicit Asset(AssetHandle handle) : m_Handle(handle) {}
    virtual ~Asset() = default;

    AssetHandle GetHandle() const { return m_Handle; }
    virtual AssetType GetType() const = 0;
    virtual size_t GetSizeBytes() const = 0;

private:
    AssetHandle m_Handle;
};

class TextureAsset final : public Asset {
public:
    TextureAsset(AssetHandle handle, std::shared_ptr<Texture> texture, size_t sizeBytes)
        : Asset(handle), m_Texture(std::move(texture)), m_SizeBytes(sizeBytes) {}

    AssetType GetType() const override { return AssetType::Texture; }
    size_t GetSizeBytes() const override { return m_SizeBytes; }
    const std::shared_ptr<Texture>& GetTexture() const { return m_Texture; }

private:
    std::shared_ptr<Texture> m_Texture;
    size_t m_SizeBytes = 0;
};

class MeshAsset final : public Asset {
public:
    MeshAsset(AssetHandle handle, std::vector<std::shared_ptr<Mesh>> meshes, size_t sizeBytes)
        : Asset(handle), m_Meshes(std::move(meshes)), m_SizeBytes(sizeBytes) {}

    AssetType GetType() const override { return AssetType::Mesh; }
    size_t GetSizeBytes() const override { return m_SizeBytes; }
    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }

private:
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
    size_t m_SizeBytes = 0;
};

class AudioAsset final : public Asset {
public:
    AudioAsset(AssetHandle handle, std::string sourcePath, size_t sizeBytes)
        : Asset(handle), m_SourcePath(std::move(sourcePath)), m_SizeBytes(sizeBytes) {}

    AssetType GetType() const override { return AssetType::Audio; }
    size_t GetSizeBytes() const override { return m_SizeBytes; }
    const std::string& GetSourcePath() const { return m_SourcePath; }

private:
    std::string m_SourcePath;
    size_t m_SizeBytes = 0;
};

class ShaderAsset final : public Asset {
public:
    ShaderAsset(AssetHandle handle, std::shared_ptr<Shader> shader, size_t sizeBytes)
        : Asset(handle), m_Shader(std::move(shader)), m_SizeBytes(sizeBytes) {}

    AssetType GetType() const override { return AssetType::Shader; }
    size_t GetSizeBytes() const override { return m_SizeBytes; }
    const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }

private:
    std::shared_ptr<Shader> m_Shader;
    size_t m_SizeBytes = 0;
};

}
