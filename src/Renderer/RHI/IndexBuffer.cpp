#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Renderer/Backend/OpenGL/OpenGLIndexBuffer.h>

namespace Zgine {

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(indices, count);
            case RendererAPI::API::DirectX12:
            case RendererAPI::API::Vulkan:
                RendererAPI::ReportUnavailableBackend("IndexBuffer");
                return nullptr;
        }
        return nullptr;
    }

}
