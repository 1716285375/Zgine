#pragma once

#include <Zgine/Renderer/RHI/VertexBuffer.h>

#include <vulkan/vulkan.h>

namespace Zgine {

class VulkanVertexBuffer final : public VertexBuffer {
public:
    VulkanVertexBuffer(const void* data, uint32_t size);
    ~VulkanVertexBuffer() override;

    void Bind() const override;
    void Unbind() const override;

    uint32_t GetID() const override { return m_RendererID; }
    void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
    const BufferLayout& GetLayout() const override { return m_Layout; }

    [[nodiscard]] VkBuffer GetBuffer() const { return m_Buffer; }
    [[nodiscard]] VkDeviceSize GetSize() const { return m_Size; }

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkDeviceSize m_Size = 0;
    uint32_t m_RendererID = 0;
    BufferLayout m_Layout;
};

} // namespace Zgine
