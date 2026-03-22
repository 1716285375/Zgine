#pragma once

#include <memory>
#include <vector>
#include <Zgine/Renderer/Pipeline/RenderStats.h>
#include <Zgine/Renderer/Pipeline/RenderConfig.h>
#include <Zgine/Renderer/Lighting/LightData.h>
#include <Zgine/Core/Math/Matrix4.h>

namespace Zgine {
    class World;
    class Shader;
    class Texture;
    class Camera;
    class Framebuffer;
    class VertexArray;
    class VertexBuffer;
    class Entity;

    class RenderSystem {
    public:
        RenderSystem();
        ~RenderSystem();

        void Initialize();
        void Shutdown();
        void BeginFrame();

        void RenderScene(World* world, Camera* camera);

        void SetRenderConfig(const RenderConfig& config) { m_Config = config; }
        [[nodiscard]] const RenderConfig& GetRenderConfig() const { return m_Config; }
        [[nodiscard]] const RenderStats&  GetStats()        const { return m_FrameStats; }

    private:
        void CollectLights(World& world, LightingData& lightData);
        void SetupLightUniforms(Shader* shader, const LightingData& lightData);
        void SetupMaterialUniforms(Shader* shader, World& world, uint32_t entity);
        void RenderShadowPass(World* world);

        Shader* GetActiveShader() const;

        RenderConfig               m_Config;
        std::shared_ptr<Shader>    m_SimpleShader;
        std::shared_ptr<Shader>    m_PBRShader;
        std::shared_ptr<Shader>    m_DepthShader;
        std::shared_ptr<Framebuffer> m_ShadowMapFBO;
        Math::Matrix4              m_LightSpaceMatrix;
        LightingData               m_LightingData{};
        bool                       m_Initialized = false;
        RenderStats                m_FrameStats{};
    };
}
