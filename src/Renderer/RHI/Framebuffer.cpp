#include <Zgine/Renderer/RHI/Framebuffer.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include "../Backend/OpenGL/OpenGLFramebuffer.h"

namespace Zgine {

std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpec& spec) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLFramebuffer>(spec);
        case RendererAPI::API::DirectX12:
        case RendererAPI::API::Vulkan:
            RendererAPI::ReportUnavailableBackend("Framebuffer");
            return nullptr;
    }

    return nullptr;
}

} // namespace Zgine
