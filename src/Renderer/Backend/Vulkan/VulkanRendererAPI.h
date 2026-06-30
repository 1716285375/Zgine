#pragma once

#include <Zgine/Renderer/RHI/RendererAPI.h>

#include <memory>

struct GLFWwindow;

namespace Zgine {

class VulkanRendererAPI final : public RendererAPI {
public:
    VulkanRendererAPI();
    ~VulkanRendererAPI() override;

    void Init() override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void SetClearColor(const Math::Vector4& color) override;
    void Clear() override;
    void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

private:
    struct VulkanContext;

    void Shutdown();
    void CreateSwapchainResources(GLFWwindow* window);
    void CleanupSwapchainResources();
    void RecreateSwapchain(GLFWwindow* window);

    std::unique_ptr<VulkanContext> m_Context;
};

} // namespace Zgine
