#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Renderer/Backend/OpenGL/OpenGLVertexBuffer.h>

namespace Zgine {

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(data, size);
            case RendererAPI::API::DirectX12:
            case RendererAPI::API::Vulkan:
                RendererAPI::ReportUnavailableBackend("VertexBuffer");
                return nullptr;
        }
        return nullptr;
    }

}
