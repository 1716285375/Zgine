#pragma once

#include <memory>
#include <vector>
#include <Zgine/Renderer/Pipeline/RenderStats.h>
#include <Zgine/Renderer/Pipeline/RenderConfig.h>
#include <Zgine/Renderer/Lighting/LightData.h>

namespace Zgine {
    class World;
    class Shader;
    class Texture;
    class Camera;
    class VertexArray;
    class VertexBuffer;
    class Entity;

    /**
     * @brief Orchestrates a complete render frame.
     *
     * RenderSystem sits above the RHI layer: it knows *how* to draw a World
     * (submit draw calls, bind shaders, set uniforms) but delegates the actual
     * GPU commands to the RHI abstractions in Renderer/RHI/.
     */
    class RenderSystem {
    public:
        RenderSystem();
        ~RenderSystem();

        void Initialize();
        void Shutdown();
        void BeginFrame();

        void RenderScene(World* World, class Camera* camera);

        void SetRenderConfig(const RenderConfig& config) { m_Config = config; }
        [[nodiscard]] const RenderConfig& GetRenderConfig() const { return m_Config; }
        [[nodiscard]] const RenderStats&  GetStats()        const { return m_FrameStats; }

    private:
        void CollectLights(World& world, LightingData& lightData);
        void SetupLightUniforms(Shader* shader, const LightingData& lightData);
        void SetupMaterialUniforms(Shader* shader, class Entity entity);

        RenderConfig               m_Config;
        std::shared_ptr<Shader>    m_SimpleShader;
        bool                       m_Initialized = false;
        RenderStats                m_FrameStats{};
    };
}
