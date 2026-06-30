#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zgine::Vulkan {

struct DeviceContext {
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
};

[[nodiscard]] bool HasDeviceContext();
[[nodiscard]] const DeviceContext& GetDeviceContext();
void SetDeviceContext(const DeviceContext& context);
void ResetDeviceContext();

[[nodiscard]] uint32_t AllocateResourceID();
[[nodiscard]] uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

void CreateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory);

void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size);

} // namespace Zgine::Vulkan
