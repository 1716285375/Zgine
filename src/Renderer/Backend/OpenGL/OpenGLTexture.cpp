#include "OpenGLTexture.h"
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Zgine {

    OpenGLTexture::OpenGLTexture(const std::string& path)
        : m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_BPP(0) {
        TextureSettings settings;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

        if (!data) {
            ZGINE_CORE_ERROR("Failed to load texture: {0}", path);
            return;
        }

        CreateTexture(m_Width, m_Height, data, settings);
        stbi_image_free(data);

        ZGINE_CORE_INFO("Loaded texture: {0} ({1}x{2})", path, m_Width, m_Height);
    }

    OpenGLTexture::OpenGLTexture(const std::string& path, const TextureSettings& settings)
        : m_RendererID(0), m_FilePath(path), m_Width(0), m_Height(0), m_BPP(0) {

        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

        if (!data) {
            ZGINE_CORE_ERROR("Failed to load texture: {0}", path);
            return;
        }

        CreateTexture(m_Width, m_Height, data, settings);
        stbi_image_free(data);

        ZGINE_CORE_INFO("Loaded texture: {0} ({1}x{2})", path, m_Width, m_Height);
    }

    OpenGLTexture::OpenGLTexture(const unsigned char* data, int size, const std::string& debugName)
        : m_RendererID(0), m_FilePath(debugName), m_Width(0), m_Height(0), m_BPP(0) {

        stbi_set_flip_vertically_on_load(1);
        unsigned char* decodedData = stbi_load_from_memory(data, size, &m_Width, &m_Height, &m_BPP, 4);

        if (!decodedData) {
            ZGINE_CORE_ERROR("Failed to load embedded texture: {0}", debugName);
            return;
        }

        CreateTexture(m_Width, m_Height, decodedData);
        stbi_image_free(decodedData);

        ZGINE_CORE_INFO("Loaded embedded texture: {0} ({1}x{2})", debugName, m_Width, m_Height);
    }

    OpenGLTexture::OpenGLTexture(const unsigned char* rgbaData, int width, int height, const std::string& debugName)
        : m_RendererID(0), m_FilePath(debugName), m_Width(width), m_Height(height), m_BPP(4) {
        CreateTexture(m_Width, m_Height, rgbaData);
        ZGINE_CORE_INFO("Loaded embedded texture: {0} ({1}x{2})", debugName, m_Width, m_Height);
    }

    OpenGLTexture::~OpenGLTexture() {
        if (m_RendererID != 0) {
            glDeleteTextures(1, &m_RendererID);
        }
    }

    void OpenGLTexture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::CreateTexture(int width, int height, const unsigned char* data) {
        TextureSettings settings;
        CreateTexture(width, height, data, settings);
    }

    void OpenGLTexture::CreateTexture(int width, int height, const unsigned char* data, const TextureSettings& settings) {
        if (!data || width <= 0 || height <= 0) {
            ZGINE_CORE_ERROR("Invalid texture data for: {0}", m_FilePath);
            return;
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        GLenum minFilter = GL_LINEAR;
        GLenum magFilter = settings.Linear ? GL_LINEAR : GL_NEAREST;
        if (settings.GenerateMipmaps) {
            minFilter = settings.Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
        }

        GLenum wrapMode = settings.ClampToEdge ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

        GLint internalFormat = settings.SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (settings.GenerateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
