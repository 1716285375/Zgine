#include <Zgine/Renderer/RHI/Shader.h>
#include "../Backend/OpenGL/OpenGLShader.h"

namespace Zgine {

std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
    return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc);
}

std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
    // For now, just use the two-parameter version
    // TODO: Implement file loading
    return nullptr;
}

} // namespace Zgine
