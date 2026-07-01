#include "VulkanContextAccess.h"

#include <Zgine/Core/Log/Log.h>

#include <atomic>

namespace Zgine::Vulkan {

namespace {

    DeviceContext g_DeviceContext;
    std::atomic<uint32_t> g_NextResourceID{1};

    void CheckVk(VkResult result, const char* message) {
        if (result != VK_SUCCESS) {
            ZGINE_CORE_THROW_RUNTIME("{}", message);
        }
    }

} // namespace

bool HasDeviceContext() {
    return g_DeviceContext.PhysicalDevice != VK_NULL_HANDLE &&
        g_DeviceContext.Device != VK_NULL_HANDLE &&
        g_DeviceContext.GraphicsQueue != VK_NULL_HANDLE &&
        g_DeviceContext.CommandPool != VK_NULL_HANDLE;
}

const DeviceContext& GetDeviceContext() {
    if (!HasDeviceContext()) {
        ZGINE_CORE_THROW_RUNTIME("Vulkan device context is not initialized.");
    }

    return g_DeviceContext;
}

void SetDeviceContext(const DeviceContext& context) {
    g_DeviceContext = context;
}

void ResetDeviceContext() {
    g_DeviceContext = {};
}

uint32_t AllocateResourceID() {
    return g_NextResourceID.fetch_add(1, std::memory_order_relaxed);
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    const DeviceContext& context = GetDeviceContext();

    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(context.PhysicalDevice, &memoryProperties);

    for (uint32_t index = 0; index < memoryProperties.memoryTypeCount; ++index) {
        const bool typeMatches = (typeFilter & (1u << index)) != 0;
        const bool propertiesMatch =
            (memoryProperties.memoryTypes[index].propertyFlags & properties) == properties;

        if (typeMatches && propertiesMatch) {
            return index;
        }
    }

    ZGINE_CORE_THROW_RUNTIME("Failed to find a suitable Vulkan memory type.");
}

void CreateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory)
{
    const DeviceContext& context = GetDeviceContext();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CheckVk(
        vkCreateBuffer(context.Device, &bufferInfo, nullptr, &buffer),
        "Failed to create Vulkan buffer.");

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(context.Device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    const VkResult allocationResult =
        vkAllocateMemory(context.Device, &allocateInfo, nullptr, &bufferMemory);
    if (allocationResult != VK_SUCCESS) {
        vkDestroyBuffer(context.Device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
        ZGINE_CORE_THROW_RUNTIME("Failed to allocate Vulkan buffer memory.");
    }

    const VkResult bindResult = vkBindBufferMemory(context.Device, buffer, bufferMemory, 0);
    if (bindResult != VK_SUCCESS) {
        vkFreeMemory(context.Device, bufferMemory, nullptr);
        vkDestroyBuffer(context.Device, buffer, nullptr);
        bufferMemory = VK_NULL_HANDLE;
        buffer = VK_NULL_HANDLE;
        ZGINE_CORE_THROW_RUNTIME("Failed to bind Vulkan buffer memory.");
    }
}

void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size) {
    const DeviceContext& context = GetDeviceContext();

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = context.CommandPool;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    CheckVk(
        vkAllocateCommandBuffers(context.Device, &allocateInfo, &commandBuffer),
        "Failed to allocate Vulkan transfer command buffer.");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        vkFreeCommandBuffers(context.Device, context.CommandPool, 1, &commandBuffer);
        ZGINE_CORE_THROW_RUNTIME("Failed to begin Vulkan transfer command buffer.");
    }

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copyRegion);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        vkFreeCommandBuffers(context.Device, context.CommandPool, 1, &commandBuffer);
        ZGINE_CORE_THROW_RUNTIME("Failed to end Vulkan transfer command buffer.");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(context.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        vkFreeCommandBuffers(context.Device, context.CommandPool, 1, &commandBuffer);
        ZGINE_CORE_THROW_RUNTIME("Failed to submit Vulkan buffer copy.");
    }
    if (vkQueueWaitIdle(context.GraphicsQueue) != VK_SUCCESS) {
        vkFreeCommandBuffers(context.Device, context.CommandPool, 1, &commandBuffer);
        ZGINE_CORE_THROW_RUNTIME("Failed to wait for Vulkan buffer copy.");
    }

    vkFreeCommandBuffers(context.Device, context.CommandPool, 1, &commandBuffer);
}

} // namespace Zgine::Vulkan
