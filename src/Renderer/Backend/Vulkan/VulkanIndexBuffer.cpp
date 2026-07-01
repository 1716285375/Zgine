#include "VulkanIndexBuffer.h"

#include "VulkanContextAccess.h"

#include <Zgine/Core/Log/Log.h>

#include <cstring>

namespace Zgine {

namespace {

    void UploadIndices(
        const uint32_t* indices,
        VkDeviceSize size,
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
            ZGINE_CORE_THROW_RUNTIME("Failed to map Vulkan index staging buffer.");
        }
        std::memcpy(mappedData, indices, static_cast<size_t>(size));
        vkUnmapMemory(context.Device, stagingMemory);

        try {
            Vulkan::CreateBuffer(
                size,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
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

VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
    : m_Device(Vulkan::GetDeviceContext().Device)
    , m_Count(count)
    , m_RendererID(Vulkan::AllocateResourceID())
{
    if (count == 0) {
        ZGINE_CORE_THROW_INVALID_ARGUMENT("VulkanIndexBuffer requires a non-zero index count.");
    }

    const VkDeviceSize size = static_cast<VkDeviceSize>(count) * sizeof(uint32_t);
    if (indices) {
        UploadIndices(indices, size, m_Buffer, m_Memory);
    } else {
        Vulkan::CreateBuffer(
            size,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_Buffer,
            m_Memory);
    }
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
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

void VulkanIndexBuffer::Bind() const {
    // Vulkan binds index buffers while recording a command buffer.
}

void VulkanIndexBuffer::Unbind() const {
    // Vulkan does not have global index-buffer binding state.
}

} // namespace Zgine
