#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <vector>
#include <memory>

namespace Zgine {

struct Vertex {
    Math::Vector3 Position;
    Math::Vector3 Normal;
    Math::Vector2 TexCoords;
    Math::Vector4 Color;
};

struct MeshData {
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    Math::Vector4 BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

class Mesh {
public:
    Mesh(const MeshData& data);
    ~Mesh();

    void Bind() const;
    void Unbind() const;
    void Draw() const;

    inline unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_Vertices.size()); }
    inline unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_Indices.size()); }
    inline const Math::Vector4& GetBaseColor() const { return m_BaseColor; }

private:
    void SetupMesh();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Math::Vector4 m_BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
};

} // namespace Zgine
