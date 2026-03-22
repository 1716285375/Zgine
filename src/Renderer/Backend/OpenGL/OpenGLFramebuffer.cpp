#include "OpenGLFramebuffer.h"
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>

namespace Zgine {

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpec& spec)
    : m_Spec(spec) {
    Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() {
    Cleanup();
}

void OpenGLFramebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Spec.Width, m_Spec.Height);
}

void OpenGLFramebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0 || width > 8192 || height > 8192) {
        ZGINE_CORE_WARN("Invalid framebuffer size: {}x{}", width, height);
        return;
    }
    m_Spec.Width = width;
    m_Spec.Height = height;
    Invalidate();
}

void OpenGLFramebuffer::Invalidate() {
    // Clean up old resources if they exist
    Cleanup();

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Color attachment (skip for depth-only FBOs like shadow maps)
    bool hasColor = !m_Spec.DepthTexture || m_Spec.HDR; // shadow maps: DepthTexture=true, HDR=false
    if (m_Spec.DepthTexture && !m_Spec.DepthStencil) {
        // Depth-only FBO (shadow map): no color attachment
        hasColor = false;
    }

    if (hasColor && !m_Spec.DepthTexture) {
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

        GLenum internalFormat = m_Spec.HDR ? GL_RGBA16F : GL_RGBA8;
        GLenum dataType = m_Spec.HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Spec.Width, m_Spec.Height,
                     0, GL_RGBA, dataType, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
    }

    // Depth attachment
    if (m_Spec.DepthTexture) {
        // Depth as texture (for shadow maps - readable in shader)
        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Spec.Width, m_Spec.Height,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

        if (!hasColor) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    } else if (m_Spec.DepthStencil) {
        // Depth+stencil as renderbuffer (not readable in shader, but faster)
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Spec.Width, m_Spec.Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ZGINE_CORE_ERROR("Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Cleanup() {
    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
    if (m_ColorAttachment) {
        glDeleteTextures(1, &m_ColorAttachment);
        m_ColorAttachment = 0;
    }
    if (m_DepthAttachment) {
        if (m_Spec.DepthTexture) {
            glDeleteTextures(1, &m_DepthAttachment);
        } else if (m_Spec.DepthStencil) {
            glDeleteRenderbuffers(1, &m_DepthAttachment);
        }
        m_DepthAttachment = 0;
    }
}

} // namespace Zgine
