#pragma once

#include <memory>
#include <vector>
#include <Zgine/Resources/Core/Asset.h>
#include <Zgine/Resources/Core/AssetMetadata.h>

namespace Zgine {

class AssetManager;

struct AssetImportContext {
    AssetManager* Manager = nullptr;
};

struct AssetImportResult {
    std::shared_ptr<Asset> AssetData;
    std::vector<AssetHandle> Dependencies;
};

class AssetImporter {
public:
    virtual ~AssetImporter() = default;
    virtual AssetImportResult Import(const AssetMetadata& metadata, AssetImportContext& context) = 0;
};

class TextureImporter final : public AssetImporter {
public:
    AssetImportResult Import(const AssetMetadata& metadata, AssetImportContext& context) override;
};

class MeshImporter final : public AssetImporter {
public:
    AssetImportResult Import(const AssetMetadata& metadata, AssetImportContext& context) override;
};

class AudioImporter final : public AssetImporter {
public:
    AssetImportResult Import(const AssetMetadata& metadata, AssetImportContext& context) override;
};

class ShaderImporter final : public AssetImporter {
public:
    AssetImportResult Import(const AssetMetadata& metadata, AssetImportContext& context) override;
};

}
