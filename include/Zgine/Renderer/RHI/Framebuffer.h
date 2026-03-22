#pragma once

#include <memory>
#include <cstdint>

namespace Zgine {

struct FramebufferSpec {
    uint32_t Width = 1280;
    uint32_t Height = 720;
    bool HDR = true;            // GL_RGBA16F vs GL_RGBA8
    bool DepthStencil = true;   // depth+stencil renderbuffer
    bool DepthTexture = false;  // depth as texture (for shadow maps, readable in shader)
};

class Framebuffer {
public:
    virtual ~Framebuffer() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    virtual uint32_t GetColorAttachmentID() const = 0;
    virtual uint32_t GetDepthAttachmentID() const = 0;
    virtual const FramebufferSpec& GetSpec() const = 0;

    static std::shared_ptr<Framebuffer> Create(const FramebufferSpec& spec);
};

} // namespace Zgine
