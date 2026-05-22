#pragma once

#include <memory>

namespace Zgine {

class Shader;
class Framebuffer;

class PostProcessPipeline {
public:
    PostProcessPipeline();
    ~PostProcessPipeline();

    void Initialize(uint32_t width, uint32_t height);
    void Shutdown();
    void Resize(uint32_t width, uint32_t height);

    // Run full post-process chain on the scene framebuffer's color attachment
    // Returns the final texture ID to display
    uint32_t Process(uint32_t sceneColorTexture);

    void SetBloomEnabled(bool enabled) { m_BloomEnabled = enabled; }
    void SetBloomStrength(float strength) { m_BloomStrength = strength; }
    void SetBloomThreshold(float threshold) { m_BloomThreshold = threshold; }

private:
    void CreateFullscreenQuad();
    void DrawFullscreenQuad();

    std::shared_ptr<Shader> m_BrightPassShader;
    std::shared_ptr<Shader> m_BlurShader;
    std::shared_ptr<Shader> m_CompositeShader;

    std::shared_ptr<Framebuffer> m_OutputFBO;     // final composited result (LDR)
    std::shared_ptr<Framebuffer> m_BrightPassFBO;
    std::shared_ptr<Framebuffer> m_PingFBO;
    std::shared_ptr<Framebuffer> m_PongFBO;

    uint32_t m_QuadVAO = 0;
    uint32_t m_QuadVBO = 0;

    bool  m_BloomEnabled = true;
    float m_BloomStrength = 0.3f;
    float m_BloomThreshold = 1.0f;
    int   m_BlurPasses = 5;

    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    bool m_Initialized = false;
};

} // namespace Zgine
