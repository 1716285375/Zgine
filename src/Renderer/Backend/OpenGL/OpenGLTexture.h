#pragma once

#include <Zgine/Renderer/RHI/Texture.h>
#include <glad/glad.h>

namespace Zgine {

    class OpenGLTexture : public Texture {
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(const std::string& path, const TextureSettings& settings);
        OpenGLTexture(const unsigned char* data, int size, const std::string& debugName);
        OpenGLTexture(const unsigned char* rgbaData, int width, int height, const std::string& debugName);
        virtual ~OpenGLTexture();

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void Unbind() const override;

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetID() const override { return m_RendererID; }
        virtual const std::string& GetFilePath() const override { return m_FilePath; }

    private:
        void CreateTexture(int width, int height, const unsigned char* data);
        void CreateTexture(int width, int height, const unsigned char* data, const TextureSettings& settings);

        uint32_t m_RendererID;
        std::string m_FilePath;
        int m_Width, m_Height, m_BPP;
    };

}
