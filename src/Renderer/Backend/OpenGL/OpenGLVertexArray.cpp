#include "OpenGLVertexArray.h"
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>

namespace Zgine {

namespace {

    GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
                return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return GL_INT;
            case ShaderDataType::Bool:
                return GL_INT;
            case ShaderDataType::None:
                return GL_NONE;
        }

        return GL_NONE;
    }

    bool IsIntegerType(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
                return true;
            default:
                return false;
        }
    }

} // namespace

    OpenGLVertexArray::OpenGLVertexArray() {
        glGenVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
        Bind();
        vertexBuffer->Bind();

        const BufferLayout& layout = vertexBuffer->GetLayout();
        if (layout.GetElements().empty()) {
            ZGINE_CORE_WARN("OpenGLVertexArray received a vertex buffer without a layout.");
            m_VertexBuffers.push_back(vertexBuffer);
            return;
        }

        for (const BufferElement& element : layout) {
            if (element.Type == ShaderDataType::Mat3 || element.Type == ShaderDataType::Mat4) {
                const uint32_t columnCount = element.Type == ShaderDataType::Mat3 ? 3 : 4;
                const uint32_t columnSize = columnCount * sizeof(float);

                for (uint32_t column = 0; column < columnCount; ++column) {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(
                        m_VertexBufferIndex,
                        static_cast<GLint>(columnCount),
                        GL_FLOAT,
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        static_cast<GLsizei>(layout.GetStride()),
                        reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset + column * columnSize)));
                    ++m_VertexBufferIndex;
                }

                continue;
            }

            glEnableVertexAttribArray(m_VertexBufferIndex);
            if (IsIntegerType(element.Type)) {
                glVertexAttribIPointer(
                    m_VertexBufferIndex,
                    static_cast<GLint>(element.GetComponentCount()),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    static_cast<GLsizei>(layout.GetStride()),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset)));
            } else {
                glVertexAttribPointer(
                    m_VertexBufferIndex,
                    static_cast<GLint>(element.GetComponentCount()),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    static_cast<GLsizei>(layout.GetStride()),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset)));
            }
            ++m_VertexBufferIndex;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
        Bind();
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

}
