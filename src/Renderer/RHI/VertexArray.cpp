#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Renderer/Backend/OpenGL/OpenGLVertexArray.h>
#if ZGINE_HAS_VULKAN
#include <Renderer/Backend/Vulkan/VulkanVertexArray.h>
#endif

namespace Zgine {

    std::shared_ptr<VertexArray> VertexArray::Create() {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
            case RendererAPI::API::DirectX12:
                RendererAPI::ReportUnavailableBackend("VertexArray");
                return nullptr;
            case RendererAPI::API::Vulkan:
#if ZGINE_HAS_VULKAN
                return std::make_shared<VulkanVertexArray>();
#else
                RendererAPI::ReportUnavailableBackend("VertexArray");
                return nullptr;
#endif
        }
        return nullptr;
    }

}
