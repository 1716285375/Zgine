#pragma once

#include <cstdint>
#include <memory>
#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>

namespace Zgine {

enum class PrimitiveType : uint8_t {
    None = 0,
    Cube,
    Plane,
    Sphere,
    Cylinder,
    Cone
};

struct PrimitiveMesh {
    std::shared_ptr<VertexArray> VertexArray;
    std::shared_ptr<VertexBuffer> VertexBuffer;
    std::shared_ptr<IndexBuffer> IndexBuffer;
};

class PrimitiveMeshFactory {
public:
    static PrimitiveMesh GetMesh(PrimitiveType type);

private:
    static PrimitiveMesh CreateCube();
    static PrimitiveMesh CreatePlane();
    static PrimitiveMesh CreateSphere();
};

}
