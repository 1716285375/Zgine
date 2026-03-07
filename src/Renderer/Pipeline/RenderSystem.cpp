#include <Zgine/Renderer/Pipeline/RenderSystem.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Camera/Camera.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Renderer/RHI/Shader.h>
#include <Zgine/Renderer/RHI/VertexArray.h>
#include <Zgine/Renderer/RHI/IndexBuffer.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <Zgine/Platform/IO/File.h>
#include <Zgine/Core/Math/Math.h>
#include <Zgine/Core/Math/Matrix3.h>
#include <Zgine/Core/Math/Matrix4.h>

namespace Zgine {

namespace {
    static std::unique_ptr<Zgine::RendererAPI> s_RendererAPI;
}

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() { Shutdown(); }

void RenderSystem::Initialize() {
    if (!s_RendererAPI) {
        s_RendererAPI = RendererAPI::Create();
        if (s_RendererAPI) {
            s_RendererAPI->Init();
        }
    }

    std::string simpleVert = File::ReadFile("assets/shaders/Simple.vert");
    std::string simpleFrag = File::ReadFile("assets/shaders/Simple.frag");
    if (!simpleVert.empty() && !simpleFrag.empty()) {
        m_SimpleShader = Shader::Create("Simple", simpleVert, simpleFrag);
    } else {
        ZGINE_CORE_ERROR("Failed to load Simple shaders.");
    }

    ZGINE_CORE_INFO("RenderSystem initialized");
}

void RenderSystem::Shutdown() {
    ZGINE_CORE_INFO("RenderSystem shut down");
}

void RenderSystem::BeginFrame() {
    // Basic begin frame logic
    m_FrameStats = RenderStats{}; // Reset stats
}

void RenderSystem::RenderScene(World* world, Camera* camera) {
    if (!world || !camera || !s_RendererAPI) return;

    if (m_SimpleShader) {
        m_SimpleShader->Bind();
        Math::Matrix4 viewProj = camera->GetProjection() * camera->GetView();
        m_SimpleShader->SetUniformMat4f("u_ViewProjection", viewProj);

        // Setup Directional Light (only taking the first one for the Simple shader)
        auto lightView = world->GetRegistry().view<DirectionalLightComponent>();
        if (!lightView.empty()) {
            auto entity = lightView.front();
            auto& light = lightView.get<DirectionalLightComponent>(entity);
            m_SimpleShader->SetUniform3f("u_LightDirection", light.Direction.x, light.Direction.y, light.Direction.z);
            m_SimpleShader->SetUniform3f("u_LightColor", light.Color.x * light.Intensity, light.Color.y * light.Intensity, light.Color.z * light.Intensity);
        } else {
            // Default light if none exists
            m_SimpleShader->SetUniform3f("u_LightDirection", 0.0f, -1.0f, -0.5f);
            m_SimpleShader->SetUniform3f("u_LightColor", 1.0f, 1.0f, 1.0f);
        }
    }

    auto view = world->GetRegistry().view<TransformComponent, PrimitiveComponent>();
    for (auto entity : view) {
        auto [transform, primitive] = view.get<TransformComponent, PrimitiveComponent>(entity);
        PrimitiveMesh mesh = PrimitiveMeshFactory::GetMesh(primitive.Type);

        if (!mesh.VertexArray) continue;

        Math::Matrix4 transformMat = transform.GetTransform();
        if (m_SimpleShader) {
            m_SimpleShader->SetUniformMat4f("u_Transform", transformMat);

            Math::Matrix3 normalMatrix = Math::Transpose(Math::Inverse(Math::ToMatrix3(transformMat)));
            m_SimpleShader->SetUniformMat3f("u_NormalMatrix", normalMatrix);

            if (world->GetRegistry().all_of<PBRMaterialComponent>(entity)) {
                auto& mat = world->GetRegistry().get<PBRMaterialComponent>(entity);
                m_SimpleShader->SetUniform3f("u_Color", mat.Albedo.x, mat.Albedo.y, mat.Albedo.z);
            } else {
                m_SimpleShader->SetUniform3f("u_Color", 0.8f, 0.8f, 0.8f);
            }
        }

        s_RendererAPI->DrawIndexed(mesh.VertexArray, mesh.IndexBuffer->GetCount());
        m_FrameStats.DrawCalls++;
        m_FrameStats.Triangles += mesh.IndexBuffer->GetCount() / 3;
    }
}

void RenderSystem::CollectLights(World& world, LightingData& lightData) {}
void RenderSystem::SetupLightUniforms(Shader* shader, const LightingData& lightData) {}
void RenderSystem::SetupMaterialUniforms(Shader* shader, class Entity entity) {}

} // namespace Zgine
