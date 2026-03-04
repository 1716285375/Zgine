#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Renderer/Backend/OpenGL/OpenGLRendererAPI.h>

namespace Zgine {

    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    std::unique_ptr<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLRendererAPI>();
        }
        return nullptr;
    }

}
