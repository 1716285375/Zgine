#include <Zgine/Renderer/PostProcess/PostProcessPipeline.h>
#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Renderer/RHI/Framebuffer.h>
#include <Zgine/Platform/IO/File.h>
#include <Zgine/Core/Log/Log.h>
#include <glad/glad.h>

namespace Zgine {

PostProcessPipeline::PostProcessPipeline() {}

PostProcessPipeline::~PostProcessPipeline() {
    if (m_QuadVAO) glDeleteVertexArrays(1, &m_QuadVAO);
    if (m_QuadVBO) glDeleteBuffers(1, &m_QuadVBO);
}

void PostProcessPipeline::Initialize(uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;

    CreateFullscreenQuad();

    // Load shaders
    std::string screenVert = File::ReadFile("assets/shaders/postprocess/Screen.vert");

    std::string brightFrag = File::ReadFile("assets/shaders/postprocess/BrightPass.frag");
    if (!screenVert.empty() && !brightFrag.empty()) {
        m_BrightPassShader = Shader::Create("BrightPass", screenVert, brightFrag);
        m_BrightPassShader->Bind();
        m_BrightPassShader->SetUniform1i("u_SceneTexture", 0);
        m_BrightPassShader->Unbind();
    }

    std::string blurFrag = File::ReadFile("assets/shaders/postprocess/GaussianBlur.frag");
    if (!screenVert.empty() && !blurFrag.empty()) {
        m_BlurShader = Shader::Create("GaussianBlur", screenVert, blurFrag);
        m_BlurShader->Bind();
        m_BlurShader->SetUniform1i("u_Image", 0);
        m_BlurShader->Unbind();
    }

    std::string compositeFrag = File::ReadFile("assets/shaders/postprocess/Composite.frag");
    if (!screenVert.empty() && !compositeFrag.empty()) {
        m_CompositeShader = Shader::Create("Composite", screenVert, compositeFrag);
        m_CompositeShader->Bind();
        m_CompositeShader->SetUniform1i("u_SceneTexture", 0);
        m_CompositeShader->SetUniform1i("u_BloomTexture", 1);
        m_CompositeShader->Unbind();
    }

    if (!m_BrightPassShader || !m_BlurShader || !m_CompositeShader) {
        ZGINE_CORE_ERROR("Failed to load post-process shaders.");
        return;
    }

    // Create FBOs for bloom (half resolution for blur to save bandwidth)
    uint32_t bloomW = width / 2;
    uint32_t bloomH = height / 2;

    FramebufferSpec brightSpec;
    brightSpec.Width = bloomW;
    brightSpec.Height = bloomH;
    brightSpec.HDR = true;
    brightSpec.DepthStencil = false;
    m_BrightPassFBO = Framebuffer::Create(brightSpec);

    FramebufferSpec pingSpec = brightSpec;
    m_PingFBO = Framebuffer::Create(pingSpec);

    FramebufferSpec pongSpec = brightSpec;
    m_PongFBO = Framebuffer::Create(pongSpec);

    // Output FBO (full resolution, LDR — final composited result for display)
    FramebufferSpec outputSpec;
    outputSpec.Width = width;
    outputSpec.Height = height;
    outputSpec.HDR = false;
    outputSpec.DepthStencil = false;
    m_OutputFBO = Framebuffer::Create(outputSpec);

    m_Initialized = true;
    ZGINE_CORE_INFO("PostProcessPipeline initialized ({}x{}, bloom: {})",
        width, height, m_BloomEnabled ? "on" : "off");
}

void PostProcessPipeline::Resize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_Width = width;
    m_Height = height;

    uint32_t bloomW = width / 2;
    uint32_t bloomH = height / 2;

    if (m_BrightPassFBO) m_BrightPassFBO->Resize(bloomW, bloomH);
    if (m_PingFBO) m_PingFBO->Resize(bloomW, bloomH);
    if (m_PongFBO) m_PongFBO->Resize(bloomW, bloomH);
    if (m_OutputFBO) m_OutputFBO->Resize(width, height);
}

void PostProcessPipeline::CreateFullscreenQuad() {
    // NDC quad: 2 triangles covering the screen
    float quadVertices[] = {
        // pos(2)    texcoord(2)
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);

    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void PostProcessPipeline::DrawFullscreenQuad() {
    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

uint32_t PostProcessPipeline::Process(uint32_t sceneColorTexture) {
    if (!m_Initialized) return sceneColorTexture;

    glDisable(GL_DEPTH_TEST);

    uint32_t bloomTexture = 0;

    if (m_BloomEnabled) {
        // 1. Bright pass: extract highlights from scene
        m_BrightPassFBO->Bind();
        m_BrightPassShader->Bind();
        m_BrightPassShader->SetUniform1f("u_Threshold", m_BloomThreshold);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
        DrawFullscreenQuad();

        // 2. Gaussian blur (ping-pong)
        bool horizontal = true;
        uint32_t inputTexture = m_BrightPassFBO->GetColorAttachmentID();

        m_BlurShader->Bind();
        for (int i = 0; i < m_BlurPasses * 2; i++) {
            auto& targetFBO = horizontal ? m_PingFBO : m_PongFBO;
            targetFBO->Bind();

            m_BlurShader->SetUniform1i("u_Horizontal", horizontal ? 1 : 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, inputTexture);
            DrawFullscreenQuad();

            inputTexture = targetFBO->GetColorAttachmentID();
            horizontal = !horizontal;
        }

        // After even number of passes, result is in PongFBO (last was horizontal=false -> PongFBO)
        bloomTexture = inputTexture;
    }

    // 3. Composite: tone map + gamma + bloom merge → output FBO
    m_OutputFBO->Bind();

    m_CompositeShader->Bind();
    m_CompositeShader->SetUniform1i("u_EnableBloom", m_BloomEnabled ? 1 : 0);
    m_CompositeShader->SetUniform1f("u_BloomStrength", m_BloomStrength);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneColorTexture);

    if (m_BloomEnabled && bloomTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomTexture);
    }

    DrawFullscreenQuad();

    m_OutputFBO->Unbind();
    glEnable(GL_DEPTH_TEST);

    return m_OutputFBO->GetColorAttachmentID();
}

} // namespace Zgine
