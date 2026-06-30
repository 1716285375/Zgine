#include "VulkanVertexBuffer.h"

#include "VulkanContextAccess.h"

#include <cstring>
#include <stdexcept>

namespace Zgine {

namespace {

    void UploadToDeviceLocalBuffer(
        const void* data,
        VkDeviceSize size,
        VkBufferUsageFlags targetUsage,
        VkBuffer& buffer,
        VkDeviceMemory& memory)
    {
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

        Vulkan::CreateBuffer(
            size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingMemory);

        const Vulkan::DeviceContext& context = Vulkan::GetDeviceContext();

        void* mappedData = nullptr;
        if (vkMapMemory(context.Device, stagingMemory, 0, size, 0, &mappedData) != VK_SUCCESS) {
            vkDestroyBuffer(context.Device, stagingBuffer, nullptr);
            vkFreeMemory(context.Device, stagingMemory, nullptr);
            throw std::runtime_error("Failed to map Vulkan vertex staging buffer.");
        }
        std::memcpy(mappedData, data, static_cast<size_t>(size));
        vkUnmapMemory(context.Device, stagingMemory);

        try {
            Vulkan::CreateBuffer(
                size,
                targetUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                buffer,
                memory);

            Vulkan::CopyBuffer(stagingBuffer, buffer, size);
        } catch (...) {
            if (buffer != VK_NULL_HANDLE) {
                vkDestroyBuffer(context.Device, buffer, nullptr);
                buffer = VK_NULL_HANDLE;
            }
            if (memory != VK_NULL_HANDLE) {
                vkFreeMemory(context.Device, memory, nullptr);
                memory = VK_NULL_HANDLE;
            }
            vkDestroyBuffer(context.Device, stagingBuffer, nullptr);
            vkFreeMemory(context.Device, stagingMemory, nullptr);
            throw;
        }

        vkDestroyBuffer(context.Device, stagingBuffer, nullptr);
        vkFreeMemory(context.Device, stagingMemory, nullptr);
    }

} // namespace

VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size)
    : m_Device(Vulkan::GetDeviceContext().Device)
    , m_Size(size)
    , m_RendererID(Vulkan::AllocateResourceID())
{
    if (size == 0) {
        throw std::invalid_argument("VulkanVertexBuffer requires a non-zero size.");
    }

    if (data) {
        UploadToDeviceLocalBuffer(
            data,
            m_Size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            m_Buffer,
            m_Memory);
    } else {
        Vulkan::CreateBuffer(
            m_Size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_Buffer,
            m_Memory);
    }
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    if (m_Device == VK_NULL_HANDLE) {
        return;
    }

    if (!Vulkan::HasDeviceContext() || Vulkan::GetDeviceContext().Device != m_Device) {
        return;
    }

    if (m_Buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_Device, m_Buffer, nullptr);
        m_Buffer = VK_NULL_HANDLE;
    }

    if (m_Memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_Device, m_Memory, nullptr);
        m_Memory = VK_NULL_HANDLE;
    }
}

void VulkanVertexBuffer::Bind() const {
    // Vulkan binds vertex buffers while recording a command buffer.
}

void VulkanVertexBuffer::Unbind() const {
    // Vulkan does not have global vertex-buffer binding state.
}

} // namespace Zgine
