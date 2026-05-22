#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRendererAPI.h"

#include <Zgine/Core/Application/Application.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Platform/Window.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace Zgine {

namespace {

constexpr std::array<const char*, 1> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

constexpr std::array<const char*, 1> kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#if defined(NDEBUG)
constexpr bool kEnableValidationLayers = false;
#else
constexpr bool kEnableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    [[nodiscard]] bool IsComplete() const {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities{};
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

VkBool32 VKAPI_PTR VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    ZGINE_UNUSED(messageType);
    ZGINE_UNUSED(userData);

    if (auto& logger = Log::GetCoreLogger()) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            logger->warn("Vulkan validation: {}", callbackData->pMessage);
        } else {
            logger->trace("Vulkan validation: {}", callbackData->pMessage);
        }
    }

    return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func) {
        return func(instance, createInfo, allocator, debugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* allocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func) {
        func(instance, debugMessenger, allocator);
    }
}

bool HasInstanceExtension(std::string_view extensionName) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return std::any_of(extensions.begin(), extensions.end(), [extensionName](const VkExtensionProperties& extension) {
        return extensionName == extension.extensionName;
    });
}

bool CheckValidationLayerSupport() {
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : kValidationLayers) {
        const bool found = std::any_of(availableLayers.begin(), availableLayers.end(),
            [layerName](const VkLayerProperties& layer) {
                return std::strcmp(layerName, layer.layerName) == 0;
            });

        if (!found) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> GetRequiredInstanceExtensions(bool enableValidationLayers, bool& enablePortabilityEnumeration) {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensions || glfwExtensionCount == 0) {
        throw std::runtime_error("GLFW did not report required Vulkan instance extensions.");
    }

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    enablePortabilityEnumeration = HasInstanceExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    if (enablePortabilityEnumeration) {
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }

    return extensions;
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t index = 0; index < queueFamilyCount; ++index) {
        if (queueFamilies[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.GraphicsFamily = index;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);
        if (presentSupport) {
            indices.PresentFamily = index;
        }

        if (indices.IsComplete()) {
            break;
        }
    }

    return indices;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string_view> requiredExtensions(kDeviceExtensions.begin(), kDeviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
    }

    return details;
}

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    const QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    const bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapchainAdequate = false;
    if (extensionsSupported) {
        const SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device, surface);
        swapchainAdequate = !swapchainSupport.Formats.empty() && !swapchainSupport.PresentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && swapchainAdequate;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& format : availableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return availableFormats.front();
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& presentMode : availablePresentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent{
        static_cast<uint32_t>(std::max(width, 1)),
        static_cast<uint32_t>(std::max(height, 1)),
    };

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);

    return actualExtent;
}

const char* DeviceTypeToString(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            return "Unknown";
    }
}

} // namespace

struct VulkanRendererAPI::VulkanContext {
    VkInstance Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    VkQueue PresentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    VkFormat SwapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D SwapchainExtent{};
    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
    QueueFamilyIndices QueueFamilies;
    Math::Vector4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
    bool ClearWarningIssued = false;
    bool DrawWarningIssued = false;
    bool ValidationLayersEnabled = false;
    bool PortabilityEnumerationEnabled = false;
};

VulkanRendererAPI::VulkanRendererAPI()
    : m_Context(std::make_unique<VulkanContext>())
{
}

VulkanRendererAPI::~VulkanRendererAPI() {
    Shutdown();
}

void VulkanRendererAPI::Init() {
    Window& window = Application::Get().GetWindow();
    auto* glfwWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());
    if (!glfwWindow) {
        throw std::runtime_error("Vulkan renderer requires a GLFW native window.");
    }

    if (m_Context->Instance != VK_NULL_HANDLE) {
        return;
    }

    m_Context->ValidationLayersEnabled = kEnableValidationLayers && CheckValidationLayerSupport();
    if (kEnableValidationLayers && !m_Context->ValidationLayersEnabled) {
        ZGINE_CORE_WARN("Vulkan validation layer VK_LAYER_KHRONOS_validation is unavailable. Continuing without validation.");
    }

    const std::vector<const char*> instanceExtensions =
        GetRequiredInstanceExtensions(m_Context->ValidationLayersEnabled, m_Context->PortabilityEnumerationEnabled);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Zgine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Zgine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    if (m_Context->PortabilityEnumerationEnabled) {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_Context->ValidationLayersEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
        createInfo.ppEnabledLayerNames = kValidationLayers.data();
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_Context->Instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }

    if (m_Context->ValidationLayersEnabled) {
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
        PopulateDebugMessengerCreateInfo(messengerCreateInfo);
        if (CreateDebugUtilsMessengerEXT(
                m_Context->Instance,
                &messengerCreateInfo,
                nullptr,
                &m_Context->DebugMessenger) != VK_SUCCESS) {
            ZGINE_CORE_WARN("Failed to create Vulkan debug messenger.");
        }
    }

    if (glfwCreateWindowSurface(m_Context->Instance, glfwWindow, nullptr, &m_Context->Surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan window surface.");
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Context->Instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("No Vulkan-capable GPU found.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Context->Instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (IsDeviceSuitable(device, m_Context->Surface)) {
            m_Context->PhysicalDevice = device;
            break;
        }
    }

    if (m_Context->PhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable Vulkan physical device found.");
    }

    m_Context->QueueFamilies = FindQueueFamilies(m_Context->PhysicalDevice, m_Context->Surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    const std::set<uint32_t> uniqueQueueFamilies = {
        m_Context->QueueFamilies.GraphicsFamily.value(),
        m_Context->QueueFamilies.PresentFamily.value(),
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();

    if (vkCreateDevice(m_Context->PhysicalDevice, &deviceCreateInfo, nullptr, &m_Context->Device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan logical device.");
    }

    vkGetDeviceQueue(m_Context->Device, m_Context->QueueFamilies.GraphicsFamily.value(), 0, &m_Context->GraphicsQueue);
    vkGetDeviceQueue(m_Context->Device, m_Context->QueueFamilies.PresentFamily.value(), 0, &m_Context->PresentQueue);

    const SwapchainSupportDetails swapchainSupport =
        QuerySwapchainSupport(m_Context->PhysicalDevice, m_Context->Surface);
    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.Formats);
    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.PresentModes);
    const VkExtent2D extent = ChooseSwapExtent(swapchainSupport.Capabilities, glfwWindow);

    uint32_t imageCount = swapchainSupport.Capabilities.minImageCount + 1;
    if (swapchainSupport.Capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.Capabilities.maxImageCount) {
        imageCount = swapchainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_Context->Surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t queueFamilyIndices[] = {
        m_Context->QueueFamilies.GraphicsFamily.value(),
        m_Context->QueueFamilies.PresentFamily.value(),
    };

    if (m_Context->QueueFamilies.GraphicsFamily != m_Context->QueueFamilies.PresentFamily) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchainCreateInfo.preTransform = swapchainSupport.Capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_Context->Device, &swapchainCreateInfo, nullptr, &m_Context->Swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan swapchain.");
    }

    vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, nullptr);
    m_Context->SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Context->Device, m_Context->Swapchain, &imageCount, m_Context->SwapchainImages.data());

    m_Context->SwapchainImageFormat = surfaceFormat.format;
    m_Context->SwapchainExtent = extent;

    m_Context->SwapchainImageViews.resize(m_Context->SwapchainImages.size());
    for (size_t index = 0; index < m_Context->SwapchainImages.size(); ++index) {
        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = m_Context->SwapchainImages[index];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = m_Context->SwapchainImageFormat;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Context->Device, &viewCreateInfo, nullptr, &m_Context->SwapchainImageViews[index]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan swapchain image view.");
        }
    }

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_Context->PhysicalDevice, &properties);

    ZGINE_CORE_INFO(
        "Vulkan initialized: {} ({}) swapchain={}x{} images={}",
        properties.deviceName,
        DeviceTypeToString(properties.deviceType),
        m_Context->SwapchainExtent.width,
        m_Context->SwapchainExtent.height,
        m_Context->SwapchainImages.size());
}

void VulkanRendererAPI::Shutdown() {
    if (!m_Context || m_Context->Instance == VK_NULL_HANDLE) {
        return;
    }

    if (m_Context->Device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_Context->Device);

        for (VkImageView imageView : m_Context->SwapchainImageViews) {
            if (imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(m_Context->Device, imageView, nullptr);
            }
        }
        m_Context->SwapchainImageViews.clear();

        if (m_Context->Swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_Context->Device, m_Context->Swapchain, nullptr);
            m_Context->Swapchain = VK_NULL_HANDLE;
        }

        vkDestroyDevice(m_Context->Device, nullptr);
        m_Context->Device = VK_NULL_HANDLE;
    }

    if (m_Context->Surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_Context->Instance, m_Context->Surface, nullptr);
        m_Context->Surface = VK_NULL_HANDLE;
    }

    if (m_Context->DebugMessenger != VK_NULL_HANDLE) {
        DestroyDebugUtilsMessengerEXT(m_Context->Instance, m_Context->DebugMessenger, nullptr);
        m_Context->DebugMessenger = VK_NULL_HANDLE;
    }

    vkDestroyInstance(m_Context->Instance, nullptr);
    m_Context->Instance = VK_NULL_HANDLE;
}

void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    ZGINE_UNUSED(x);
    ZGINE_UNUSED(y);
    ZGINE_UNUSED(width);
    ZGINE_UNUSED(height);
}

void VulkanRendererAPI::SetClearColor(const Math::Vector4& color) {
    m_Context->ClearColor = color;
}

void VulkanRendererAPI::Clear() {
    if (!m_Context->ClearWarningIssued) {
        ZGINE_CORE_WARN("Vulkan Clear() is not implemented yet. Device and swapchain initialization are available first.");
        m_Context->ClearWarningIssued = true;
    }
}

void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) {
    ZGINE_UNUSED(vertexArray);
    ZGINE_UNUSED(indexCount);

    if (!m_Context->DrawWarningIssued) {
        ZGINE_CORE_WARN("Vulkan DrawIndexed() is not implemented yet. Add command buffers and pipelines before scene rendering.");
        m_Context->DrawWarningIssued = true;
    }
}

} // namespace Zgine
