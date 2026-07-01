#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Core/Log/Log.h>
#include <Renderer/Backend/OpenGL/OpenGLRendererAPI.h>
#if ZGINE_HAS_VULKAN
#include <Renderer/Backend/Vulkan/VulkanRendererAPI.h>
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <string>

#ifndef ZGINE_DEFAULT_RENDERER_BACKEND
#define ZGINE_DEFAULT_RENDERER_BACKEND "OpenGL"
#endif

namespace Zgine {

    namespace {
        std::string NormalizeBackendName(std::string_view name) {
            std::string normalized;
            normalized.reserve(name.size());

            for (char c : name) {
                if (c == '_' || c == '-' || c == ' ') {
                    continue;
                }

                normalized.push_back(static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c))));
            }

            return normalized;
        }
    }

    RendererAPI::API RendererAPI::s_API = RendererAPI::FromString(ZGINE_DEFAULT_RENDERER_BACKEND);

    void RendererAPI::SetAPI(API api) {
        s_API = api;
    }

    bool RendererAPI::IsAvailable(API api) {
        switch (api) {
            case API::None:
                return false;
            case API::OpenGL:
                return true;
            case API::DirectX12:
                return false;
            case API::Vulkan:
#if ZGINE_HAS_VULKAN
                return true;
#else
                return false;
#endif
        }

        return false;
    }

    const char* RendererAPI::ToString(API api) {
        switch (api) {
            case API::None:
                return "None";
            case API::OpenGL:
                return "OpenGL";
            case API::DirectX12:
                return "DirectX12";
            case API::Vulkan:
                return "Vulkan";
        }

        return "Unknown";
    }

    RendererAPI::API RendererAPI::FromString(std::string_view name) {
        const std::string normalized = NormalizeBackendName(name);

        if (normalized == "none" || normalized == "null") {
            return API::None;
        }
        if (normalized == "opengl" || normalized == "gl") {
            return API::OpenGL;
        }
        if (normalized == "directx12" || normalized == "d3d12" || normalized == "dx12") {
            return API::DirectX12;
        }
        if (normalized == "vulkan" || normalized == "vk") {
            return API::Vulkan;
        }

        return API::OpenGL;
    }

    void RendererAPI::ReportUnavailableBackend(std::string_view resourceType) {
        ZGINE_CORE_ERROR("{} is not implemented for renderer backend '{}'.",
            resourceType, ToString(s_API));
    }

    std::unique_ptr<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case API::None:
                return nullptr;
            case API::OpenGL:
                return std::make_unique<OpenGLRendererAPI>();
            case API::DirectX12:
                ReportUnavailableBackend("RendererAPI");
                return nullptr;
            case API::Vulkan:
#if ZGINE_HAS_VULKAN
                return std::make_unique<VulkanRendererAPI>();
#else
                ReportUnavailableBackend("RendererAPI");
                return nullptr;
#endif
        }

        return nullptr;
    }

}
