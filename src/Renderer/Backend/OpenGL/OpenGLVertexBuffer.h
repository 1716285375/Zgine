#pragma once

#include <Zgine/Renderer/RHI/VertexBuffer.h>

namespace Zgine {

    class OpenGLVertexBuffer : public VertexBuffer {
    public:
        OpenGLVertexBuffer(const void* data, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetID() const override { return m_RendererID; }

    private:
        uint32_t m_RendererID;
    };

}
