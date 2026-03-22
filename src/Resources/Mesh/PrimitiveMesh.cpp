#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <glad/glad.h>
#include <cmath>
#include <vector>

namespace Zgine {

namespace {
    PrimitiveMesh s_CubeMesh;
    PrimitiveMesh s_PlaneMesh;
    PrimitiveMesh s_SphereMesh;

    // Vertex layout: position(3) + normal(3) + texcoord(2) = 8 floats, stride 32 bytes
    void ConfigureVertexLayout(const std::shared_ptr<VertexArray>& vertexArray,
                               const std::shared_ptr<VertexBuffer>& vertexBuffer,
                               const std::shared_ptr<IndexBuffer>& indexBuffer) {
        vertexArray->Bind();
        vertexBuffer->Bind();

        constexpr int stride = 8 * sizeof(float);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2); // texcoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        vertexArray->SetIndexBuffer(indexBuffer);
        vertexArray->Unbind();
    }
}

PrimitiveMesh PrimitiveMeshFactory::GetMesh(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube:
            return CreateCube();
        case PrimitiveType::Plane:
            return CreatePlane();
        case PrimitiveType::Sphere:
            return CreateSphere();
        default:
            return CreateCube();
    }
}

PrimitiveMesh PrimitiveMeshFactory::CreateCube() {
    if (s_CubeMesh.VertexArray && s_CubeMesh.VertexBuffer && s_CubeMesh.IndexBuffer) {
        return s_CubeMesh;
    }

    // 24 vertices: pos(3) + normal(3) + texcoord(2) = 8 floats each
    float vertices[] = {
        // Front face (z = +0.5)
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
        // Back face (z = -0.5)
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
        // Left face (x = -0.5)
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        // Right face (x = +0.5)
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        // Bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
        // Top face (y = +0.5)
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    };

    // All faces use CCW winding: 0,1,2, 2,3,0 pattern
    unsigned int indices[] = {
         0,  1,  2,   2,  3,  0,   // Front
         4,  5,  6,   6,  7,  4,   // Back
         8,  9, 10,  10, 11,  8,   // Left
        12, 13, 14,  14, 15, 12,   // Right
        16, 17, 18,  18, 19, 16,   // Bottom
        20, 21, 22,  22, 23, 20,   // Top
    };

    s_CubeMesh.VertexArray = VertexArray::Create();
    s_CubeMesh.VertexBuffer = VertexBuffer::Create(vertices, static_cast<unsigned int>(sizeof(vertices)));
    s_CubeMesh.IndexBuffer = IndexBuffer::Create(indices, 36);
    ConfigureVertexLayout(s_CubeMesh.VertexArray, s_CubeMesh.VertexBuffer, s_CubeMesh.IndexBuffer);

    return s_CubeMesh;
}

PrimitiveMesh PrimitiveMeshFactory::CreatePlane() {
    if (s_PlaneMesh.VertexArray && s_PlaneMesh.VertexBuffer && s_PlaneMesh.IndexBuffer) {
        return s_PlaneMesh;
    }

    // 4 vertices: pos(3) + normal(3) + texcoord(2) = 8 floats each
    // Vertices ordered CCW when viewed from +Y (top-down)
    float vertices[] = {
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    s_PlaneMesh.VertexArray = VertexArray::Create();
    s_PlaneMesh.VertexBuffer = VertexBuffer::Create(vertices, static_cast<unsigned int>(sizeof(vertices)));
    s_PlaneMesh.IndexBuffer = IndexBuffer::Create(indices, 6);
    ConfigureVertexLayout(s_PlaneMesh.VertexArray, s_PlaneMesh.VertexBuffer, s_PlaneMesh.IndexBuffer);

    return s_PlaneMesh;
}

PrimitiveMesh PrimitiveMeshFactory::CreateSphere() {
    if (s_SphereMesh.VertexArray && s_SphereMesh.VertexBuffer && s_SphereMesh.IndexBuffer) {
        return s_SphereMesh;
    }

    const int sectorCount = 32;
    const int stackCount = 16;
    const float radius = 0.5f;
    const float PI = 3.14159265359f;

    // pos(3) + normal(3) + texcoord(2) = 8 floats per vertex
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2.0f - PI * i / stackCount;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = 2.0f * PI * j / sectorCount;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;

            float u = (float)j / sectorCount;
            float v = (float)i / stackCount;

            // Position (Y-up: swap y and z)
            vertices.push_back(x);
            vertices.push_back(z);
            vertices.push_back(y);
            // Normal (Y-up: swap y and z)
            vertices.push_back(nx);
            vertices.push_back(nz);
            vertices.push_back(ny);
            // TexCoords
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    s_SphereMesh.VertexArray = VertexArray::Create();
    s_SphereMesh.VertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(float)));
    s_SphereMesh.IndexBuffer = IndexBuffer::Create(indices.data(), static_cast<unsigned int>(indices.size()));
    ConfigureVertexLayout(s_SphereMesh.VertexArray, s_SphereMesh.VertexBuffer, s_SphereMesh.IndexBuffer);

    return s_SphereMesh;
}

}
