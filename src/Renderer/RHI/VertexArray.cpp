#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Renderer/Backend/OpenGL/OpenGLVertexArray.h>

namespace Zgine {

    std::shared_ptr<VertexArray> VertexArray::Create() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
            case RendererAPI::API::DirectX12:
            case RendererAPI::API::Vulkan:
                RendererAPI::ReportUnavailableBackend("VertexArray");
                return nullptr;
        }
        return nullptr;
    }

}
