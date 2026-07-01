#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/VertexBuffer.h>
#include <Zgine/Core/Log/Log.h>
#include <Renderer/Backend/OpenGL/OpenGLVertexBuffer.h>
#if ZGINE_HAS_VULKAN
#include <Renderer/Backend/Vulkan/VulkanContextAccess.h>
#include <Renderer/Backend/Vulkan/VulkanVertexBuffer.h>
#endif

namespace Zgine {

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(data, size);
            case RendererAPI::API::DirectX12:
                RendererAPI::ReportUnavailableBackend("VertexBuffer");
                return nullptr;
            case RendererAPI::API::Vulkan:
#if ZGINE_HAS_VULKAN
                if (!Vulkan::HasDeviceContext()) {
                    ZGINE_CORE_ERROR("VertexBuffer requires an initialized Vulkan renderer.");
                    return nullptr;
                }
                return std::make_shared<VulkanVertexBuffer>(data, size);
#else
                RendererAPI::ReportUnavailableBackend("VertexBuffer");
                return nullptr;
#endif
        }
        return nullptr;
    }

}
