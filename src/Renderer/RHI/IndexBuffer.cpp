#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Core/Log/Log.h>
#include <Renderer/Backend/OpenGL/OpenGLIndexBuffer.h>
#if ZGINE_HAS_VULKAN
#include <Renderer/Backend/Vulkan/VulkanContextAccess.h>
#include <Renderer/Backend/Vulkan/VulkanIndexBuffer.h>
#endif

namespace Zgine {

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(indices, count);
            case RendererAPI::API::DirectX12:
                RendererAPI::ReportUnavailableBackend("IndexBuffer");
                return nullptr;
            case RendererAPI::API::Vulkan:
#if ZGINE_HAS_VULKAN
                if (!Vulkan::HasDeviceContext()) {
                    if (auto& logger = Log::GetCoreLogger()) {
                        logger->error("IndexBuffer requires an initialized Vulkan renderer.");
                    }
                    return nullptr;
                }
                return std::make_shared<VulkanIndexBuffer>(indices, count);
#else
                RendererAPI::ReportUnavailableBackend("IndexBuffer");
                return nullptr;
#endif
        }
        return nullptr;
    }

}
