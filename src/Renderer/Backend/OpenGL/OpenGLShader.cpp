#include "OpenGLShader.h"
#include <Zgine/Core/Log/Log.h>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Zgine {

    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        m_RendererID = CreateShader(vertexSrc, fragmentSrc);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }

    uint32_t OpenGLShader::CompileShader(uint32_t type, const std::string& source) {
        uint32_t id = glCreateShader(type);
        if (id == 0) {
            ZGINE_CORE_ERROR("Failed to create shader object!");
            return 0;
        }

        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> message(length);
            glGetShaderInfoLog(id, length, &length, message.data());

            const char* shaderType =
                (type == GL_VERTEX_SHADER) ? "vertex" :
                (type == GL_FRAGMENT_SHADER) ? "fragment" :
                (type == GL_GEOMETRY_SHADER) ? "geometry" : "unknown";

            ZGINE_CORE_ERROR("Failed to compile {0} shader!", shaderType);
            ZGINE_CORE_ERROR("{0}", message.data());
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    uint32_t OpenGLShader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
        uint32_t program = glCreateProgram();
        if (program == 0) {
            ZGINE_CORE_ERROR("Failed to create shader program!");
            return 0;
        }

        uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        // Check if shader compilation failed
        if (vs == 0 || fs == 0) {
            ZGINE_CORE_ERROR("Shader compilation failed, cannot create program!");
            if (vs != 0) glDeleteShader(vs);
            if (fs != 0) glDeleteShader(fs);
            glDeleteProgram(program);
            return 0;
        }

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        // Check for linking errors
        int linkSuccess;
        glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess == GL_FALSE) {
            int length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> message(length);
            glGetProgramInfoLog(program, length, &length, message.data());
            ZGINE_CORE_ERROR("Failed to link shader program!");
            ZGINE_CORE_ERROR("{0}", message.data());

            glDeleteShader(vs);
            glDeleteShader(fs);
            glDeleteProgram(program);
            return 0;
        }

#ifndef NDEBUG
        // Validate program only in Debug builds (has performance cost)
        glValidateProgram(program);

        // Check validation status (helpful for debugging)
        int validateSuccess;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &validateSuccess);
        if (validateSuccess == GL_FALSE) {
            int length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            if (length > 0) {
                std::vector<char> message(length);
                glGetProgramInfoLog(program, length, &length, message.data());
                ZGINE_CORE_WARN("Shader program validation warning:");
                ZGINE_CORE_WARN("{0}", message.data());
            }
        }
#endif

        // Shaders are now linked into program, no longer needed
        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    int OpenGLShader::GetUniformLocation(const std::string& name) {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1) {
            ZGINE_CORE_WARN("Warning: uniform '{0}' doesn't exist!", name);
        }

        m_UniformLocationCache[name] = location;
        return location;
    }

    void OpenGLShader::SetUniform1i(const std::string& name, int value) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            glUniform1i(location, value);
        }
    }

    void OpenGLShader::SetUniform1f(const std::string& name, float value) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void OpenGLShader::SetUniform2f(const std::string& name, float v0, float v1) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            glUniform2f(location, v0, v1);
        }
    }

    void OpenGLShader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            glUniform3f(location, v0, v1, v2);
        }
    }

    void OpenGLShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
        int location = GetUniformLocation(name);
        if (location != -1) {
            glUniform4f(location, v0, v1, v2, v3);
        }
    }

    void OpenGLShader::SetUniformMat3f(const std::string& name, const Math::Matrix3& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location != -1) {
            glUniformMatrix3fv(location, 1, GL_FALSE, Math::ValuePtr(matrix));
        }
    }

    void OpenGLShader::SetUniformMat4f(const std::string& name, const Math::Matrix4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, Math::ValuePtr(matrix));
        }
    }

}
