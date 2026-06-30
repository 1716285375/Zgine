#pragma once

#include <Zgine/Renderer/RHI/IndexBuffer.h>

#include <vulkan/vulkan.h>

namespace Zgine {

class VulkanIndexBuffer final : public IndexBuffer {
public:
    VulkanIndexBuffer(uint32_t* indices, uint32_t count);
    ~VulkanIndexBuffer() override;

    void Bind() const override;
    void Unbind() const override;

    uint32_t GetCount() const override { return m_Count; }
    uint32_t GetID() const override { return m_RendererID; }

    [[nodiscard]] VkBuffer GetBuffer() const { return m_Buffer; }

private:
    VkDevice m_Device = VK_NULL_HANDLE;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    uint32_t m_Count = 0;
    uint32_t m_RendererID = 0;
};

} // namespace Zgine
