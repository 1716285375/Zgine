#pragma once

#include <Zgine/Renderer/RHI/Shader.h>
#include <glad/glad.h>
#include <unordered_map>

namespace Zgine {

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetUniform1i(const std::string& name, int value) override;
        virtual void SetUniform1f(const std::string& name, float value) override;
        virtual void SetUniform2f(const std::string& name, float v0, float v1) override;
        virtual void SetUniform3f(const std::string& name, float v0, float v1, float v2) override;
        virtual void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) override;
        virtual void SetUniformMat3f(const std::string& name, const Math::Matrix3& matrix) override;
        virtual void SetUniformMat4f(const std::string& name, const Math::Matrix4& matrix) override;

        virtual uint32_t GetID() const override { return m_RendererID; }

    private:
        uint32_t CompileShader(uint32_t type, const std::string& source);
        uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
        int GetUniformLocation(const std::string& name);

        uint32_t m_RendererID;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };

}
