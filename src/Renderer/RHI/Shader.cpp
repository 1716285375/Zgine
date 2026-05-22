#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include "../Backend/OpenGL/OpenGLShader.h"

namespace Zgine {

std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc);
        case RendererAPI::API::DirectX12:
        case RendererAPI::API::Vulkan:
            RendererAPI::ReportUnavailableBackend("Shader");
            return nullptr;
    }

    return nullptr;
}

std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
    // For now, just use the two-parameter version
    // TODO: Implement file loading
    return nullptr;
}

} // namespace Zgine
