#pragma once

#include <string>
#include <vector>
#include <memory>
#include <Zgine/Resources/Mesh/Mesh.h>
#include <Zgine/Resources/Import/AssetImportSettings.h>

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace Zgine {

class Texture;

class MeshLoader {
public:
    static std::vector<std::shared_ptr<Mesh>> LoadModel(const std::string& path,
                                                        const MeshImportSettings& settings = {});
    static std::shared_ptr<Mesh> LoadMesh(const std::string& path,
                                          const MeshImportSettings& settings = {});

private:
    static void ProcessNode(const ::aiNode* node, const ::aiScene* scene,
                           const std::string& directory, std::vector<std::shared_ptr<Mesh>>& meshes);
    static MeshData ProcessMesh(const ::aiMesh* mesh, const ::aiScene* scene,
                               const std::string& directory);
    static std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const ::aiMaterial* mat,
                                                                      const ::aiScene* scene,
                                                                      int type, const std::string& typeName,
                                                                      const std::string& directory);
};

}

