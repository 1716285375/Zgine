#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Renderer/Backend/OpenGL/OpenGLTexture.h>

namespace Zgine {

    std::shared_ptr<Texture> Texture::Create(const std::string& path) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture>(path);
        }
        return nullptr;
    }

    std::shared_ptr<Texture> Texture::Create(const std::string& path, const TextureSettings& settings) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture>(path, settings);
        }
        return nullptr;
    }

    std::shared_ptr<Texture> Texture::Create(const unsigned char* data, int size, const std::string& debugName) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture>(data, size, debugName);
        }
        return nullptr;
    }

    std::shared_ptr<Texture> Texture::Create(const unsigned char* rgbaData, int width, int height, const std::string& debugName) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture>(rgbaData, width, height, debugName);
        }
        return nullptr;
    }

}
