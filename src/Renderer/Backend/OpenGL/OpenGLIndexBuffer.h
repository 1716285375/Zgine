#pragma once

#include <Zgine/Renderer/RHI/IndexBuffer.h>

namespace Zgine {

    class OpenGLIndexBuffer : public IndexBuffer {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return m_Count; }
        virtual uint32_t GetID() const override { return m_RendererID; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };

}
