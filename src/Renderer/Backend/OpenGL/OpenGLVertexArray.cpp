#include "OpenGLVertexArray.h"
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>

namespace Zgine {

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

        m_VertexBuffers.push_back(vertexBuffer);
        m_VertexBufferIndex++;
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
        Bind();
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

}
