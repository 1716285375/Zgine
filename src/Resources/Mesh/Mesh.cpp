#include <Zgine/Resources/Mesh/Mesh.h>
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>

namespace Zgine {

Mesh::Mesh(const MeshData& data)
    : m_Vertices(data.Vertices),
      m_Indices(data.Indices),
      m_BaseColor(data.BaseColor) {
    SetupMesh();
}

Mesh::~Mesh() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
}

void Mesh::Bind() const {
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind() const {
    glBindVertexArray(0);
}

void Mesh::SetupMesh() {
    // Create and bind VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // 创建顶点缓冲区数据(位置 + 法线 + 纹理坐标 + 颜色)
    std::vector<float> vertexData;
    for (const auto& vertex : m_Vertices) {
        vertexData.push_back(vertex.Position.x);
        vertexData.push_back(vertex.Position.y);
        vertexData.push_back(vertex.Position.z);
        vertexData.push_back(vertex.Normal.x);
        vertexData.push_back(vertex.Normal.y);
        vertexData.push_back(vertex.Normal.z);
        vertexData.push_back(vertex.TexCoords.x);
        vertexData.push_back(vertex.TexCoords.y);
        vertexData.push_back(vertex.Color.r);
        vertexData.push_back(vertex.Color.g);
        vertexData.push_back(vertex.Color.b);
        vertexData.push_back(vertex.Color.w);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    // 手动设置顶点属性(位置+法线+纹理坐标+颜色布局)
    // 位置属性(layout 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);

    // 法线属性(layout 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));

    // 纹理坐标属性(layout 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));

    // 颜色属性(layout 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    // 绘制网格
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

}
