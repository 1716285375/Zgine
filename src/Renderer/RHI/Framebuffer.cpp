#include <Zgine/Renderer/RHI/Framebuffer.h>
#include "../Backend/OpenGL/OpenGLFramebuffer.h"

namespace Zgine {

std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpec& spec) {
    return std::make_shared<OpenGLFramebuffer>(spec);
}

} // namespace Zgine
