#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRendererAPI.h"

#include "VulkanContextAccess.h"

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

constexpr size_t kMaxFramesInFlight = 2;

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

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        ZGINE_CORE_WARN("Vulkan validation: {}", callbackData->pMessage);
    } else {
        ZGINE_CORE_TRACE("Vulkan validation: {}", callbackData->pMessage);
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
        ZGINE_CORE_THROW_RUNTIME("GLFW did not report required Vulkan instance extensions.");
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

GLFWwindow* GetGlfwNativeWindow(const Window& window) {
    auto* nativeWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());
    if (!nativeWindow) {
        ZGINE_CORE_THROW_RUNTIME("Vulkan renderer requires a native window handle.");
    }

    return nativeWindow;
}

void CreateWindowSurface(VkInstance instance, const Window& window, VkSurfaceKHR& surface) {
    GLFWwindow* nativeWindow = GetGlfwNativeWindow(window);
    if (glfwCreateWindowSurface(instance, nativeWindow, nullptr, &surface) != VK_SUCCESS) {
        ZGINE_CORE_THROW_RUNTIME("Failed to create Vulkan window surface.");
    }
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent{
        static_cast<uint32_t>(std::max(window.GetWidth(), 1u)),
        static_cast<uint32_t>(std::max(window.GetHeight(), 1u)),
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

void CheckVk(VkResult result, const char* message) {
    if (result != VK_SUCCESS) {
        ZGINE_CORE_THROW_RUNTIME("{}", message);
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
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> SwapchainFramebuffers;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    std::array<VkCommandBuffer, kMaxFramesInFlight> CommandBuffers{};
    std::array<VkSemaphore, kMaxFramesInFlight> ImageAvailableSemaphores{};
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::array<VkFence, kMaxFramesInFlight> InFlightFences{};
    QueueFamilyIndices QueueFamilies;
    Math::Vector4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
    uint32_t CurrentFrame = 0;
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
    if (window.GetGraphicsAPI() != WindowGraphicsAPI::Vulkan) {
        ZGINE_CORE_THROW_RUNTIME("Vulkan renderer requires a Window created for the Vulkan graphics API.");
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
        ZGINE_CORE_THROW_RUNTIME("Failed to create Vulkan instance.");
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

    CreateWindowSurface(m_Context->Instance, window, m_Context->Surface);

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Context->Instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        ZGINE_CORE_THROW_RUNTIME("No Vulkan-capable GPU found.");
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
        ZGINE_CORE_THROW_RUNTIME("No suitable Vulkan physical device found.");
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
        ZGINE_CORE_THROW_RUNTIME("Failed to create Vulkan logical device.");
    }

    vkGetDeviceQueue(m_Context->Device, m_Context->QueueFamilies.GraphicsFamily.value(), 0, &m_Context->GraphicsQueue);
    vkGetDeviceQueue(m_Context->Device, m_Context->QueueFamilies.PresentFamily.value(), 0, &m_Context->PresentQueue);

    CreateSwapchainResources(window);

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = m_Context->QueueFamilies.GraphicsFamily.value();

    CheckVk(
        vkCreateCommandPool(m_Context->Device, &commandPoolInfo, nullptr, &m_Context->CommandPool),
        "Failed to create Vulkan command pool.");

    Vulkan::SetDeviceContext({
        m_Context->PhysicalDevice,
        m_Context->Device,
        m_Context->GraphicsQueue,
        m_Context->CommandPool,
    });

    VkCommandBufferAllocateInfo commandBufferInfo{};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = m_Context->CommandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_Context->CommandBuffers.size());

    CheckVk(
        vkAllocateCommandBuffers(m_Context->Device, &commandBufferInfo, m_Context->CommandBuffers.data()),
        "Failed to allocate Vulkan command buffers.");

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t frame = 0; frame < kMaxFramesInFlight; ++frame) {
        CheckVk(
            vkCreateSemaphore(m_Context->Device, &semaphoreInfo, nullptr, &m_Context->ImageAvailableSemaphores[frame]),
            "Failed to create Vulkan image-available semaphore.");
        CheckVk(
            vkCreateFence(m_Context->Device, &fenceInfo, nullptr, &m_Context->InFlightFences[frame]),
            "Failed to create Vulkan frame fence.");
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

void VulkanRendererAPI::CreateSwapchainResources(const Window& window) {
    const SwapchainSupportDetails swapchainSupport =
        QuerySwapchainSupport(m_Context->PhysicalDevice, m_Context->Surface);
    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.Formats);
    const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.PresentModes);
    const VkExtent2D extent = ChooseSwapExtent(swapchainSupport.Capabilities, window);

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

    CheckVk(
        vkCreateSwapchainKHR(m_Context->Device, &swapchainCreateInfo, nullptr, &m_Context->Swapchain),
        "Failed to create Vulkan swapchain.");

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

        CheckVk(
            vkCreateImageView(m_Context->Device, &viewCreateInfo, nullptr, &m_Context->SwapchainImageViews[index]),
            "Failed to create Vulkan swapchain image view.");
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_Context->SwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    CheckVk(
        vkCreateRenderPass(m_Context->Device, &renderPassInfo, nullptr, &m_Context->RenderPass),
        "Failed to create Vulkan render pass.");

    m_Context->SwapchainFramebuffers.resize(m_Context->SwapchainImageViews.size());
    for (size_t index = 0; index < m_Context->SwapchainImageViews.size(); ++index) {
        VkImageView attachments[] = {
            m_Context->SwapchainImageViews[index],
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_Context->RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Context->SwapchainExtent.width;
        framebufferInfo.height = m_Context->SwapchainExtent.height;
        framebufferInfo.layers = 1;

        CheckVk(
            vkCreateFramebuffer(m_Context->Device, &framebufferInfo, nullptr, &m_Context->SwapchainFramebuffers[index]),
            "Failed to create Vulkan framebuffer.");
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    m_Context->RenderFinishedSemaphores.resize(m_Context->SwapchainImages.size(), VK_NULL_HANDLE);
    for (VkSemaphore& semaphore : m_Context->RenderFinishedSemaphores) {
        CheckVk(
            vkCreateSemaphore(m_Context->Device, &semaphoreInfo, nullptr, &semaphore),
            "Failed to create Vulkan render-finished semaphore.");
    }
}

void VulkanRendererAPI::CleanupSwapchainResources() {
    if (!m_Context || m_Context->Device == VK_NULL_HANDLE) {
        return;
    }

    for (VkSemaphore semaphore : m_Context->RenderFinishedSemaphores) {
        if (semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(m_Context->Device, semaphore, nullptr);
        }
    }
    m_Context->RenderFinishedSemaphores.clear();

    for (VkFramebuffer framebuffer : m_Context->SwapchainFramebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(m_Context->Device, framebuffer, nullptr);
        }
    }
    m_Context->SwapchainFramebuffers.clear();

    if (m_Context->RenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_Context->Device, m_Context->RenderPass, nullptr);
        m_Context->RenderPass = VK_NULL_HANDLE;
    }

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

    m_Context->SwapchainImages.clear();
    m_Context->SwapchainImageFormat = VK_FORMAT_UNDEFINED;
    m_Context->SwapchainExtent = {};
}

void VulkanRendererAPI::RecreateSwapchain(const Window& window) {
    if (!m_Context || m_Context->Device == VK_NULL_HANDLE) {
        return;
    }

    if (window.GetWidth() == 0 || window.GetHeight() == 0) {
        ZGINE_CORE_TRACE("Skipping Vulkan swapchain recreation for minimized window.");
        return;
    }

    vkDeviceWaitIdle(m_Context->Device);
    CleanupSwapchainResources();
    CreateSwapchainResources(window);
    m_Context->CurrentFrame = 0;

    ZGINE_CORE_INFO(
        "Vulkan swapchain recreated: {}x{} images={}",
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

        for (VkFence fence : m_Context->InFlightFences) {
            if (fence != VK_NULL_HANDLE) {
                vkDestroyFence(m_Context->Device, fence, nullptr);
            }
        }
        m_Context->InFlightFences = {};

        for (VkSemaphore semaphore : m_Context->ImageAvailableSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(m_Context->Device, semaphore, nullptr);
            }
        }
        m_Context->ImageAvailableSemaphores = {};

        if (m_Context->CommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_Context->Device, m_Context->CommandPool, nullptr);
            m_Context->CommandPool = VK_NULL_HANDLE;
            m_Context->CommandBuffers = {};
        }

        CleanupSwapchainResources();

        Vulkan::ResetDeviceContext();

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
    if (!m_Context || m_Context->Device == VK_NULL_HANDLE || m_Context->Swapchain == VK_NULL_HANDLE) {
        return;
    }

    Window& window = Application::Get().GetWindow();

    const uint32_t frame = m_Context->CurrentFrame;
    const VkFence inFlightFence = m_Context->InFlightFences[frame];
    const VkSemaphore imageAvailableSemaphore = m_Context->ImageAvailableSemaphores[frame];
    const VkCommandBuffer commandBuffer = m_Context->CommandBuffers[frame];

    vkWaitForFences(m_Context->Device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult acquireResult = vkAcquireNextImageKHR(
        m_Context->Device,
        m_Context->Swapchain,
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain(window);
        return;
    }
    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        ZGINE_CORE_THROW_RUNTIME("Failed to acquire Vulkan swapchain image.");
    }

    bool shouldRecreateSwapchain = acquireResult == VK_SUBOPTIMAL_KHR;
    const VkSemaphore renderFinishedSemaphore = m_Context->RenderFinishedSemaphores[imageIndex];

    vkResetFences(m_Context->Device, 1, &inFlightFence);
    CheckVk(vkResetCommandBuffer(commandBuffer, 0), "Failed to reset Vulkan command buffer.");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CheckVk(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin Vulkan command buffer.");

    VkClearValue clearColor{};
    clearColor.color = {{
        m_Context->ClearColor.r,
        m_Context->ClearColor.g,
        m_Context->ClearColor.b,
        m_Context->ClearColor.a,
    }};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_Context->RenderPass;
    renderPassInfo.framebuffer = m_Context->SwapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_Context->SwapchainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);

    CheckVk(vkEndCommandBuffer(commandBuffer), "Failed to record Vulkan command buffer.");

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

    CheckVk(vkQueueSubmit(m_Context->GraphicsQueue, 1, &submitInfo, inFlightFence), "Failed to submit Vulkan clear frame.");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_Context->Swapchain;
    presentInfo.pImageIndices = &imageIndex;

    const VkResult presentResult = vkQueuePresentKHR(m_Context->PresentQueue, &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
        shouldRecreateSwapchain = true;
    } else if (presentResult != VK_SUCCESS) {
        ZGINE_CORE_THROW_RUNTIME("Failed to present Vulkan clear frame.");
    }

    if (shouldRecreateSwapchain) {
        RecreateSwapchain(window);
    } else {
        m_Context->CurrentFrame = (m_Context->CurrentFrame + 1) % kMaxFramesInFlight;
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
