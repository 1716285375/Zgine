#pragma once

#include <memory>
#include <Zgine/Resources/Core/AssetHandle.h>

namespace Zgine {

class VertexArray;
class VertexBuffer;
class IndexBuffer;

/**
 * @brief Mesh component for renderable geometry
 */
struct MeshComponent {
    std::shared_ptr<VertexArray> VertexArray;
    std::shared_ptr<VertexBuffer> VertexBuffer;
    std::shared_ptr<IndexBuffer> IndexBuffer;
    AssetHandle MeshHandle;

    MeshComponent() = default;
    MeshComponent(const MeshComponent&) = default;
};

} //namespace Zgine
