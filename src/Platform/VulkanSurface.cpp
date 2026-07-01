#if ZGINE_HAS_VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Zgine/Platform/VulkanSurface.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Platform/Window.h>

namespace Zgine::Platform {

std::vector<const char*> GetRequiredVulkanInstanceExtensions() {
    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (!extensions || extensionCount == 0) {
        ZGINE_CORE_THROW_RUNTIME("Window platform did not report required Vulkan instance extensions.");
    }

    return {extensions, extensions + extensionCount};
}

void CreateVulkanSurface(const Window& window, VkInstance instance, VkSurfaceKHR& surface) {
    auto* nativeWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());
    if (!nativeWindow) {
        ZGINE_CORE_THROW_RUNTIME("Window platform cannot create a Vulkan surface without a native window handle.");
    }

    if (glfwCreateWindowSurface(instance, nativeWindow, nullptr, &surface) != VK_SUCCESS) {
        ZGINE_CORE_THROW_RUNTIME("Window platform failed to create a Vulkan surface.");
    }
}

} // namespace Zgine::Platform

#endif
