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

    void ConfigureVertexLayout(const std::shared_ptr<VertexArray>& vertexArray,
                               const std::shared_ptr<VertexBuffer>& vertexBuffer,
                               const std::shared_ptr<IndexBuffer>& indexBuffer,
                               int strideFloats, bool hasTangents = false) {
        vertexArray->Bind();
        vertexBuffer->Bind();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideFloats * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideFloats * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideFloats * sizeof(float), (void*)(6 * sizeof(float)));

        if (hasTangents) {
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, strideFloats * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, strideFloats * sizeof(float), (void*)(11 * sizeof(float)));
        } else {
            // 如果没有切线，设置默认值
            glDisableVertexAttribArray(3);
            glVertexAttrib3f(3, 1.0f, 0.0f, 0.0f);
            glDisableVertexAttribArray(4);
            glVertexAttrib3f(4, 0.0f, 1.0f, 0.0f);
        }

        // Color attribute (location 3 in old shader, but we'll use it for compatibility)
        glDisableVertexAttribArray(5);
        glVertexAttrib4f(5, 1.0f, 1.0f, 1.0f, 1.0f);

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

    float vertices[] = {
        // Front
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        // Back
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        // Left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        // Right
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        // Bottom
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        // Top
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0,  1,  2,   2,  3,  0,
        4,  5,  6,   6,  7,  4,
        8,  9,  10,  10, 11, 8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20
    };

    s_CubeMesh.VertexArray = VertexArray::Create();
    s_CubeMesh.IndexBuffer = IndexBuffer::Create(indices, 36);

    // 计算切线和副切线
    // 对于立方体，我们需要为每个面计算切线
    // 简化版本：使用默认切线（1,0,0）和副切线（0,1,0）
    // 实际应用中应该根据纹理坐标计算正确的切线
    float verticesWithTangents[24 * 14]; // 24 vertices * 14 floats (pos(3) + normal(3) + tex(2) + tangent(3) + bitangent(3))
    for (int i = 0; i < 24; i++) {
        int srcIdx = i * 8;
        int dstIdx = i * 14;
        // Position
        verticesWithTangents[dstIdx + 0] = vertices[srcIdx + 0];
        verticesWithTangents[dstIdx + 1] = vertices[srcIdx + 1];
        verticesWithTangents[dstIdx + 2] = vertices[srcIdx + 2];
        // Normal
        verticesWithTangents[dstIdx + 3] = vertices[srcIdx + 3];
        verticesWithTangents[dstIdx + 4] = vertices[srcIdx + 4];
        verticesWithTangents[dstIdx + 5] = vertices[srcIdx + 5];
        // TexCoords
        verticesWithTangents[dstIdx + 6] = vertices[srcIdx + 6];
        verticesWithTangents[dstIdx + 7] = vertices[srcIdx + 7];
        // Tangent (简化：使用默认值)
        verticesWithTangents[dstIdx + 8] = 1.0f;
        verticesWithTangents[dstIdx + 9] = 0.0f;
        verticesWithTangents[dstIdx + 10] = 0.0f;
        // Bitangent
        verticesWithTangents[dstIdx + 11] = 0.0f;
        verticesWithTangents[dstIdx + 12] = 1.0f;
        verticesWithTangents[dstIdx + 13] = 0.0f;
    }

    s_CubeMesh.VertexBuffer = VertexBuffer::Create(verticesWithTangents, static_cast<unsigned int>(sizeof(verticesWithTangents)));
    ConfigureVertexLayout(s_CubeMesh.VertexArray, s_CubeMesh.VertexBuffer, s_CubeMesh.IndexBuffer, 14, true);

    return s_CubeMesh;
}

PrimitiveMesh PrimitiveMeshFactory::CreatePlane() {
    if (s_PlaneMesh.VertexArray && s_PlaneMesh.VertexBuffer && s_PlaneMesh.IndexBuffer) {
        return s_PlaneMesh;
    }

    float vertices[] = {
        -0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    s_PlaneMesh.VertexArray = VertexArray::Create();
    s_PlaneMesh.IndexBuffer = IndexBuffer::Create(indices, 6);

    // 为平面计算切线和副切线
    float verticesWithTangents[4 * 14];
    for (int i = 0; i < 4; i++) {
        int srcIdx = i * 8;
        int dstIdx = i * 14;
        // Position
        verticesWithTangents[dstIdx + 0] = vertices[srcIdx + 0];
        verticesWithTangents[dstIdx + 1] = vertices[srcIdx + 1];
        verticesWithTangents[dstIdx + 2] = vertices[srcIdx + 2];
        // Normal
        verticesWithTangents[dstIdx + 3] = vertices[srcIdx + 3];
        verticesWithTangents[dstIdx + 4] = vertices[srcIdx + 4];
        verticesWithTangents[dstIdx + 5] = vertices[srcIdx + 5];
        // TexCoords
        verticesWithTangents[dstIdx + 6] = vertices[srcIdx + 6];
        verticesWithTangents[dstIdx + 7] = vertices[srcIdx + 7];
        // Tangent (对于平面，切线是 (1,0,0))
        verticesWithTangents[dstIdx + 8] = 1.0f;
        verticesWithTangents[dstIdx + 9] = 0.0f;
        verticesWithTangents[dstIdx + 10] = 0.0f;
        // Bitangent (对于平面，副切线是 (0,0,1))
        verticesWithTangents[dstIdx + 11] = 0.0f;
        verticesWithTangents[dstIdx + 12] = 0.0f;
        verticesWithTangents[dstIdx + 13] = 1.0f;
    }

    s_PlaneMesh.VertexBuffer = VertexBuffer::Create(verticesWithTangents, static_cast<unsigned int>(sizeof(verticesWithTangents)));
    ConfigureVertexLayout(s_PlaneMesh.VertexArray, s_PlaneMesh.VertexBuffer, s_PlaneMesh.IndexBuffer, 14, true);

    return s_PlaneMesh;
}

PrimitiveMesh PrimitiveMeshFactory::CreateSphere() {
    if (s_SphereMesh.VertexArray && s_SphereMesh.VertexBuffer && s_SphereMesh.IndexBuffer) {
        return s_SphereMesh;
    }

    const int sectorCount = 32;  // horizontal segments
    const int stackCount = 16;   // vertical stacks
    const float radius = 0.5f;
    const float PI = 3.14159265359f;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2.0f - PI * i / stackCount;  // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = 2.0f * PI * j / sectorCount;  // from 0 to 2pi

            // Position
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            
            // Normal (normalized position for unit sphere)
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;

            // Texture coordinates
            float u = (float)j / sectorCount;
            float v = (float)i / stackCount;

            // Tangent (derivative of position with respect to sector angle)
            float tx = -sinf(sectorAngle);
            float ty = cosf(sectorAngle);
            float tz = 0.0f;

            // Bitangent (derivative of position with respect to stack angle)
            float bx = -cosf(sectorAngle) * sinf(stackAngle);
            float by = -sinf(sectorAngle) * sinf(stackAngle);
            float bz = cosf(stackAngle);

            // Position (3)
            vertices.push_back(x);
            vertices.push_back(z);  // Swap y and z for Y-up coordinate system
            vertices.push_back(y);
            // Normal (3)
            vertices.push_back(nx);
            vertices.push_back(nz);
            vertices.push_back(ny);
            // TexCoords (2)
            vertices.push_back(u);
            vertices.push_back(v);
            // Tangent (3)
            vertices.push_back(tx);
            vertices.push_back(tz);
            vertices.push_back(ty);
            // Bitangent (3)
            vertices.push_back(bx);
            vertices.push_back(bz);
            vertices.push_back(by);
        }
    }

    // Generate indices
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
    s_SphereMesh.IndexBuffer = IndexBuffer::Create(indices.data(), static_cast<unsigned int>(indices.size()));
    s_SphereMesh.VertexBuffer = VertexBuffer::Create(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(float)));
    ConfigureVertexLayout(s_SphereMesh.VertexArray, s_SphereMesh.VertexBuffer, s_SphereMesh.IndexBuffer, 14, true);

    return s_SphereMesh;
}

}
