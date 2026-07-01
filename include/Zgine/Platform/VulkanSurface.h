#pragma once

#include <vector>

#if ZGINE_HAS_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace Zgine {

class Window;

namespace Platform {

#if ZGINE_HAS_VULKAN
[[nodiscard]] std::vector<const char*> GetRequiredVulkanInstanceExtensions();
void CreateVulkanSurface(const Window& window, VkInstance instance, VkSurfaceKHR& surface);
#endif

} // namespace Platform

} // namespace Zgine
