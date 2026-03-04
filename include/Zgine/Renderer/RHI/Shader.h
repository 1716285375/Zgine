#pragma once

#include <Zgine/Core/Math/MathTypes.h>

namespace Zgine {

/**
 * @brief Shader interface for the renderer
 */
class Shader {
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Uniform setters
    virtual void SetUniform1i(const std::string& name, int value) = 0;
    virtual void SetUniform1f(const std::string& name, float value) = 0;
    virtual void SetUniform2f(const std::string& name, float v0, float v1) = 0;
    virtual void SetUniform3f(const std::string& name, float v0, float v1, float v2) = 0;
    virtual void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) = 0;
    virtual void SetUniformMat3f(const std::string& name, const Math::Matrix3& matrix) = 0;
    virtual void SetUniformMat4f(const std::string& name, const Math::Matrix4& matrix) = 0;

    virtual uint32_t GetID() const = 0;

    static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    static std::shared_ptr<Shader> Create(const std::string& filepath);
};

} // namespace Zgine
