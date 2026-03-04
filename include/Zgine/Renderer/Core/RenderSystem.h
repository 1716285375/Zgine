#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <Zgine/Renderer/Core/RenderStats.h>
#include <Zgine/Renderer/Core/RenderConfig.h>
#include <Zgine/Renderer/Lighting/LightData.h>

namespace Zgine {
    class Scene;
    class Shader;
    class Texture;
    class Camera;
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

        // 渲染场景
        void RenderScene(Scene* scene, class Camera* camera);

        // 运行时配置 / Runtime configuration
        void SetRenderConfig(const RenderConfig& config) { m_Config = config; }
        const RenderConfig& GetRenderConfig() const { return m_Config; }

        const RenderStats& GetStats() const { return m_FrameStats; }

    private:
        void CollectLights(Scene& scene, LightData& lightData);
        void SetupLightUniforms(Shader* shader, const LightData& lightData);
        void SetupMaterialUniforms(Shader* shader, class Entity entity);

        // 渲染配置 / Rendering configuration
        RenderConfig m_Config;

        // 核心资源 / Core resources
        std::shared_ptr<Shader> m_PBRShader;
        std::shared_ptr<Shader> m_SimpleShader;  // 简化着色器，用于无材质渲染

        // 状态 / State
        bool m_Initialized = false;
        RenderStats m_FrameStats{};
    };
}
