#include <Zgine/Renderer/Pipeline/RenderSystem.h>
#include <Zgine/Renderer/Pipeline/TextureDefaults.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Camera/Camera.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Renderer/RHI/Framebuffer.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <Zgine/Platform/IO/File.h>
#include <Zgine/Core/Math/Math.h>
#include <Zgine/Core/Math/Matrix3.h>
#include <Zgine/Core/Math/Matrix4.h>
#include <cmath>
#include <algorithm>
#include <exception>
#include <string>

namespace Zgine {

namespace {
    static std::unique_ptr<Zgine::RendererAPI> s_RendererAPI;
}

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() { Shutdown(); }

void RenderSystem::Initialize() {
    if (m_Initialized) {
        return;
    }

    const RendererAPI::API api = RendererAPI::GetAPI();
    if (!RendererAPI::IsAvailable(api)) {
        ZGINE_CORE_ERROR("RenderSystem cannot initialize renderer backend '{}'. Backend is known but not implemented yet.",
            RendererAPI::ToString(api));
        return;
    }

    if (!s_RendererAPI) {
        s_RendererAPI = RendererAPI::Create();
        if (!s_RendererAPI) {
            ZGINE_CORE_ERROR("RenderSystem failed to create renderer backend '{}'.", RendererAPI::ToString(api));
            return;
        }
        try {
            s_RendererAPI->Init();
        } catch (const std::exception& error) {
            ZGINE_CORE_ERROR("RenderSystem failed to initialize renderer backend '{}': {}",
                RendererAPI::ToString(api), error.what());
            s_RendererAPI.reset();
            return;
        }
    }

    if (api != RendererAPI::API::OpenGL) {
        m_Config.EnableShadows = false;
        m_Config.EnablePostProcess = false;
        m_Initialized = true;
        ZGINE_CORE_INFO(
            "RenderSystem initialized backend '{}' for clear-frame rendering. Scene pipelines and resources are not implemented yet.",
            RendererAPI::ToString(api));
        return;
    }

    // Default textures (1x1 white, black, flat normal)
    TextureDefaults::Initialize();

    // Blinn-Phong shader (Basic path)
    std::string simpleVert = File::ReadFile("assets/shaders/Simple.vert");
    std::string simpleFrag = File::ReadFile("assets/shaders/Simple.frag");
    if (!simpleVert.empty() && !simpleFrag.empty()) {
        m_SimpleShader = Shader::Create("Simple", simpleVert, simpleFrag);
    } else {
        ZGINE_CORE_ERROR("Failed to load Simple shaders.");
    }

    // PBR shader (Advanced path)
    std::string pbrVert = File::ReadFile("assets/shaders/PBR.vert");
    std::string pbrFrag = File::ReadFile("assets/shaders/PBR.frag");
    if (!pbrVert.empty() && !pbrFrag.empty()) {
        m_PBRShader = Shader::Create("PBR", pbrVert, pbrFrag);
        // Set texture sampler slots once
        m_PBRShader->Bind();
        m_PBRShader->SetUniform1i("u_AlbedoMap", 0);
        m_PBRShader->SetUniform1i("u_NormalMap", 1);
        m_PBRShader->SetUniform1i("u_MetallicMap", 2);
        m_PBRShader->SetUniform1i("u_RoughnessMap", 3);
        m_PBRShader->SetUniform1i("u_AOMap", 4);
        m_PBRShader->Unbind();
        // Default to PBR path
        m_Config.Path = RenderPath::Advanced;
    } else {
        ZGINE_CORE_WARN("Failed to load PBR shaders, falling back to Basic path.");
        m_Config.Path = RenderPath::Basic;
    }

    // Depth shader (shadow pass)
    std::string depthVert = File::ReadFile("assets/shaders/Depth.vert");
    std::string depthFrag = File::ReadFile("assets/shaders/Depth.frag");
    if (!depthVert.empty() && !depthFrag.empty()) {
        m_DepthShader = Shader::Create("Depth", depthVert, depthFrag);
    } else {
        ZGINE_CORE_WARN("Failed to load Depth shaders, shadows disabled.");
    }

    // Shadow map FBO (depth-only, 2048x2048)
    if (m_DepthShader) {
        FramebufferSpec shadowSpec;
        shadowSpec.Width = 2048;
        shadowSpec.Height = 2048;
        shadowSpec.HDR = false;
        shadowSpec.DepthStencil = false;
        shadowSpec.DepthTexture = true;
        m_ShadowMapFBO = Framebuffer::Create(shadowSpec);
        m_Config.EnableShadows = true;
    }

    // Set shadow map sampler slot for both shaders
    if (m_SimpleShader) {
        m_SimpleShader->Bind();
        m_SimpleShader->SetUniform1i("u_ShadowMap", 5);
        m_SimpleShader->Unbind();
    }
    if (m_PBRShader) {
        m_PBRShader->Bind();
        m_PBRShader->SetUniform1i("u_ShadowMap", 5);
        m_PBRShader->Unbind();
    }

    // Post-process pipeline
    m_PostProcess.Initialize(1280, 720);
    m_Config.EnablePostProcess = true;

    m_Initialized = true;

    ZGINE_CORE_INFO("RenderSystem initialized (backend: {}, path: {}, shadows: {}, postprocess: {})",
        RendererAPI::ToString(api),
        m_Config.Path == RenderPath::Advanced ? "PBR" : "Blinn-Phong",
        m_Config.EnableShadows ? "on" : "off",
        m_Config.EnablePostProcess ? "on" : "off");
}

void RenderSystem::Shutdown() {
    if (!m_Initialized) {
        return;
    }

    m_PostProcess.Shutdown();
    m_ShadowMapFBO.reset();
    m_DepthShader.reset();
    m_PBRShader.reset();
    m_SimpleShader.reset();
    TextureDefaults::Shutdown();
    s_RendererAPI.reset();

    m_Initialized = false;
    ZGINE_CORE_INFO("RenderSystem shut down");
}

uint32_t RenderSystem::PostProcess(uint32_t sceneColorTexture) {
    if (!m_Config.EnablePostProcess) return sceneColorTexture;
    return m_PostProcess.Process(sceneColorTexture);
}

void RenderSystem::ResizePostProcess(uint32_t width, uint32_t height) {
    if (!m_Config.EnablePostProcess) {
        return;
    }
    m_PostProcess.Resize(width, height);
}

void RenderSystem::BeginFrame() {
    m_FrameStats = RenderStats{};
}

Shader* RenderSystem::GetActiveShader() const {
    if (m_Config.Path == RenderPath::Advanced && m_PBRShader)
        return m_PBRShader.get();
    return m_SimpleShader.get();
}

void RenderSystem::RenderShadowPass(World* world) {
    if (!m_DepthShader || !m_ShadowMapFBO || !m_Config.EnableShadows) return;

    // Compute light-space matrix from directional light
    auto& dir = m_LightingData.directional;
    Math::Vector3 lightDir = Math::Normalize(dir.direction);

    // Orthographic projection centered at origin, covering the scene
    float orthoSize = 20.0f;
    Math::Matrix4 lightProjection = Math::Matrix4::Ortho(
        -orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 50.0f);

    // Look from the opposite of light direction toward origin
    Math::Vector3 lightPos = lightDir * (-25.0f);
    Math::Matrix4 lightView = Math::Matrix4::LookAt(
        lightPos, Math::Vector3(0.0f, 0.0f, 0.0f), Math::Vector3(0.0f, 1.0f, 0.0f));

    m_LightSpaceMatrix = lightProjection * lightView;

    // Render to shadow map
    m_ShadowMapFBO->Bind();
    s_RendererAPI->Clear();

    m_DepthShader->Bind();
    m_DepthShader->SetUniformMat4f("u_LightSpaceMatrix", m_LightSpaceMatrix);

    auto view = world->GetRegistry().view<TransformComponent, PrimitiveComponent>();
    for (auto entity : view) {
        auto [transform, primitive] = view.get<TransformComponent, PrimitiveComponent>(entity);
        PrimitiveMesh mesh = PrimitiveMeshFactory::GetMesh(primitive.Type);
        if (!mesh.VertexArray) continue;

        Math::Matrix4 transformMat = transform.GetTransform();
        m_DepthShader->SetUniformMat4f("u_Transform", transformMat);

        s_RendererAPI->DrawIndexed(mesh.VertexArray, mesh.IndexBuffer->GetCount());
        m_FrameStats.DrawCalls++;
    }

    m_DepthShader->Unbind();
    m_ShadowMapFBO->Unbind();
}

void RenderSystem::RenderScene(World* world, Camera* camera) {
    if (!world || !camera || !s_RendererAPI || !m_Initialized) return;

    if (RendererAPI::GetAPI() != RendererAPI::API::OpenGL) {
        s_RendererAPI->Clear();
        return;
    }

    // Collect lights first (needed by shadow pass)
    m_LightingData = LightingData{};
    CollectLights(*world, m_LightingData);

    // Shadow pass
    RenderShadowPass(world);

    Shader* shader = GetActiveShader();
    if (!shader) return;

    shader->Bind();

    // View-Projection
    Math::Matrix4 viewProj = camera->GetProjection() * camera->GetView();
    shader->SetUniformMat4f("u_ViewProjection", viewProj);

    // Camera position for specular
    auto camPos = camera->GetPosition();
    shader->SetUniform3f("u_CameraPos", camPos.x, camPos.y, camPos.z);

    // Light uniforms
    SetupLightUniforms(shader, m_LightingData);

    // Shadow uniforms
    shader->SetUniformMat4f("u_LightSpaceMatrix", m_LightSpaceMatrix);
    shader->SetUniform1i("u_EnableShadows", m_Config.EnableShadows ? 1 : 0);

    // Bind shadow map to slot 5
    if (m_ShadowMapFBO && m_Config.EnableShadows) {
        m_ShadowMapFBO->BindDepthTexture(5);
    }

    // Render entities
    auto view = world->GetRegistry().view<TransformComponent, PrimitiveComponent>();
    for (auto entity : view) {
        auto [transform, primitive] = view.get<TransformComponent, PrimitiveComponent>(entity);
        PrimitiveMesh mesh = PrimitiveMeshFactory::GetMesh(primitive.Type);

        if (!mesh.VertexArray) continue;

        Math::Matrix4 transformMat = transform.GetTransform();
        shader->SetUniformMat4f("u_Transform", transformMat);

        Math::Matrix3 normalMatrix = Math::Transpose(Math::Inverse(Math::ToMatrix3(transformMat)));
        shader->SetUniformMat3f("u_NormalMatrix", normalMatrix);

        // Material
        SetupMaterialUniforms(shader, *world, static_cast<uint32_t>(entity));

        s_RendererAPI->DrawIndexed(mesh.VertexArray, mesh.IndexBuffer->GetCount());
        m_FrameStats.DrawCalls++;
        m_FrameStats.Triangles += mesh.IndexBuffer->GetCount() / 3;
    }
}

void RenderSystem::CollectLights(World& world, LightingData& lightData) {
    // Directional light (use first found)
    auto dirView = world.GetRegistry().view<DirectionalLightComponent>();
    if (!dirView.empty()) {
        auto entity = dirView.front();
        auto& light = dirView.get<DirectionalLightComponent>(entity);
        lightData.directional.direction = light.Direction;
        lightData.directional.color = light.Color;
        lightData.directional.intensity = light.Intensity;
    } else {
        lightData.directional.direction = Math::Vector3(0.0f, -1.0f, -0.5f);
        lightData.directional.color = Math::Vector3(1.0f, 1.0f, 1.0f);
        lightData.directional.intensity = 1.0f;
    }

    // Point lights
    lightData.numPointLights = 0;
    auto pointView = world.GetRegistry().view<PointLightComponent>();
    for (auto entity : pointView) {
        if (lightData.numPointLights >= 8) break;
        auto& pl = pointView.get<PointLightComponent>(entity);
        auto& data = lightData.points[lightData.numPointLights];

        if (world.GetRegistry().all_of<TransformComponent>(entity)) {
            data.position = world.GetRegistry().get<TransformComponent>(entity).Translation;
        } else {
            data.position = pl.Position;
        }
        data.color = pl.Color;
        data.intensity = pl.Intensity;
        data.constant = pl.Constant;
        data.linear = pl.Linear;
        data.quadratic = pl.Quadratic;
        lightData.numPointLights++;
    }

    // Spot lights
    lightData.numSpotLights = 0;
    auto spotView = world.GetRegistry().view<SpotLightComponent>();
    for (auto entity : spotView) {
        if (lightData.numSpotLights >= 8) break;
        auto& sl = spotView.get<SpotLightComponent>(entity);
        auto& data = lightData.spots[lightData.numSpotLights];

        if (world.GetRegistry().all_of<TransformComponent>(entity)) {
            data.position = world.GetRegistry().get<TransformComponent>(entity).Translation;
        } else {
            data.position = sl.Position;
        }
        data.direction = sl.Direction;
        data.color = sl.Color;
        data.intensity = sl.Intensity;
        data.cutOff = std::cos(Math::DegToRad(sl.InnerCone));
        data.outerCutOff = std::cos(Math::DegToRad(sl.OuterCone));
        data.constant = 1.0f;
        data.linear = 0.09f;
        data.quadratic = 0.032f;
        lightData.numSpotLights++;
    }
}

void RenderSystem::SetupLightUniforms(Shader* shader, const LightingData& lightData) {
    auto& dir = lightData.directional;
    shader->SetUniform3f("u_DirLight.direction", dir.direction.x, dir.direction.y, dir.direction.z);
    shader->SetUniform3f("u_DirLight.color",
        dir.color.x * dir.intensity,
        dir.color.y * dir.intensity,
        dir.color.z * dir.intensity);

    shader->SetUniform1i("u_NumPointLights", lightData.numPointLights);
    for (int i = 0; i < lightData.numPointLights; i++) {
        auto& pl = lightData.points[i];
        std::string prefix = "u_PointLights[" + std::to_string(i) + "].";
        shader->SetUniform3f(prefix + "position", pl.position.x, pl.position.y, pl.position.z);
        shader->SetUniform3f(prefix + "color",
            pl.color.x * pl.intensity, pl.color.y * pl.intensity, pl.color.z * pl.intensity);
        shader->SetUniform1f(prefix + "constant", pl.constant);
        shader->SetUniform1f(prefix + "linear", pl.linear);
        shader->SetUniform1f(prefix + "quadratic", pl.quadratic);
    }

    shader->SetUniform1i("u_NumSpotLights", lightData.numSpotLights);
    for (int i = 0; i < lightData.numSpotLights; i++) {
        auto& sl = lightData.spots[i];
        std::string prefix = "u_SpotLights[" + std::to_string(i) + "].";
        shader->SetUniform3f(prefix + "position", sl.position.x, sl.position.y, sl.position.z);
        shader->SetUniform3f(prefix + "direction", sl.direction.x, sl.direction.y, sl.direction.z);
        shader->SetUniform3f(prefix + "color",
            sl.color.x * sl.intensity, sl.color.y * sl.intensity, sl.color.z * sl.intensity);
        shader->SetUniform1f(prefix + "cutOff", sl.cutOff);
        shader->SetUniform1f(prefix + "outerCutOff", sl.outerCutOff);
        shader->SetUniform1f(prefix + "constant", sl.constant);
        shader->SetUniform1f(prefix + "linear", sl.linear);
        shader->SetUniform1f(prefix + "quadratic", sl.quadratic);
    }
}

void RenderSystem::SetupMaterialUniforms(Shader* shader, World& world, uint32_t entity) {
    auto entityID = static_cast<entt::entity>(entity);
    bool isPBR = (m_Config.Path == RenderPath::Advanced);

    if (world.GetRegistry().all_of<PBRMaterialComponent>(entityID)) {
        auto& mat = world.GetRegistry().get<PBRMaterialComponent>(entityID);

        if (isPBR) {
            // PBR path: set scalar uniforms
            shader->SetUniform3f("u_Albedo", mat.Albedo.x, mat.Albedo.y, mat.Albedo.z);
            shader->SetUniform1f("u_Metallic", mat.Metallic);
            shader->SetUniform1f("u_Roughness", mat.Roughness);
            shader->SetUniform1f("u_AO", mat.AO);

            // Albedo texture (slot 0)
            if (mat.UseAlbedoTexture && mat.AlbedoTexture) {
                mat.AlbedoTexture->Bind(0);
                shader->SetUniform1i("u_UseAlbedoMap", 1);
            } else {
                TextureDefaults::White()->Bind(0);
                shader->SetUniform1i("u_UseAlbedoMap", 0);
            }

            // Normal map (slot 1)
            if (mat.UseNormalTexture && mat.NormalTexture) {
                mat.NormalTexture->Bind(1);
                shader->SetUniform1i("u_UseNormalMap", 1);
            } else {
                TextureDefaults::FlatNormal()->Bind(1);
                shader->SetUniform1i("u_UseNormalMap", 0);
            }

            // Metallic map (slot 2)
            if (mat.UseMetallicTexture && mat.MetallicTexture) {
                mat.MetallicTexture->Bind(2);
                shader->SetUniform1i("u_UseMetallicMap", 1);
            } else {
                TextureDefaults::Black()->Bind(2);
                shader->SetUniform1i("u_UseMetallicMap", 0);
            }

            // Roughness map (slot 3)
            if (mat.UseRoughnessTexture && mat.RoughnessTexture) {
                mat.RoughnessTexture->Bind(3);
                shader->SetUniform1i("u_UseRoughnessMap", 1);
            } else {
                TextureDefaults::White()->Bind(3);
                shader->SetUniform1i("u_UseRoughnessMap", 0);
            }

            // AO map (slot 4)
            if (mat.UseAOTexture && mat.AOTexture) {
                mat.AOTexture->Bind(4);
                shader->SetUniform1i("u_UseAOMap", 1);
            } else {
                TextureDefaults::White()->Bind(4);
                shader->SetUniform1i("u_UseAOMap", 0);
            }
        } else {
            // Blinn-Phong path: u_Color + u_Shininess
            shader->SetUniform3f("u_Color", mat.Albedo.x, mat.Albedo.y, mat.Albedo.z);
            float r = std::max(mat.Roughness, 0.01f);
            float shininess = 2.0f / (r * r * r * r) - 2.0f;
            shininess = std::clamp(shininess, 2.0f, 2048.0f);
            shader->SetUniform1f("u_Shininess", shininess);
        }
    } else {
        // No material component — defaults
        if (isPBR) {
            shader->SetUniform3f("u_Albedo", 0.8f, 0.8f, 0.8f);
            shader->SetUniform1f("u_Metallic", 0.0f);
            shader->SetUniform1f("u_Roughness", 0.5f);
            shader->SetUniform1f("u_AO", 1.0f);
            TextureDefaults::White()->Bind(0);
            shader->SetUniform1i("u_UseAlbedoMap", 0);
            TextureDefaults::FlatNormal()->Bind(1);
            shader->SetUniform1i("u_UseNormalMap", 0);
            TextureDefaults::Black()->Bind(2);
            shader->SetUniform1i("u_UseMetallicMap", 0);
            TextureDefaults::White()->Bind(3);
            shader->SetUniform1i("u_UseRoughnessMap", 0);
            TextureDefaults::White()->Bind(4);
            shader->SetUniform1i("u_UseAOMap", 0);
        } else {
            shader->SetUniform3f("u_Color", 0.8f, 0.8f, 0.8f);
            shader->SetUniform1f("u_Shininess", 32.0f);
        }
    }
}

} // namespace Zgine
