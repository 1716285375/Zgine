#pragma once

#include <Zgine/Renderer/RHI/Framebuffer.h>

namespace Zgine {

class OpenGLFramebuffer : public Framebuffer {
public:
    explicit OpenGLFramebuffer(const FramebufferSpec& spec);
    ~OpenGLFramebuffer() override;

    void Bind() override;
    void Unbind() override;
    void Resize(uint32_t width, uint32_t height) override;

    uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }
    uint32_t GetDepthAttachmentID() const override { return m_DepthAttachment; }
    const FramebufferSpec& GetSpec() const override { return m_Spec; }

private:
    void Invalidate();
    void Cleanup();

    FramebufferSpec m_Spec;
    uint32_t m_RendererID = 0;
    uint32_t m_ColorAttachment = 0;
    uint32_t m_DepthAttachment = 0; // renderbuffer or texture depending on spec
};

} // namespace Zgine
