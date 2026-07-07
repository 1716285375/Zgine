#include <Zgine/Core/Foundation/Prerequisites.h>
#include <Zgine/Zgine.h>
#include <Zgine/Core/Application/EntryPoint.h>
#include <Zgine/Core/Math/Math.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <glad/glad.h>
#include <Zgine/Core/Application/Application.h>

#include <string>

namespace Zgine {

    class SandboxLayer : public Layer {
    public:
        SandboxLayer()
            : Layer("SandboxLayer"),
              m_Camera(Math::Matrix4::Perspective(Math::DegToRad(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f))
        {
            m_Camera.SetPosition(Math::Vector3(0.0f, 5.0f, 15.0f));
            m_Camera.SetRotation(Math::Vector3(-20.0f, -90.0f, 0.0f));
        }

        virtual void OnAttach() override {
            ZGINE_CORE_INFO("SandboxLayer::OnAttach");

            // Initialize required systems
            AssetManager::Get().Initialize();
            PBRMaterialPresetRegistry::Initialize();

            m_ScriptSystem.SetAudioSystem(&m_AudioSystem);
            auto& systems = m_World.GetSystemManager();
            systems.RegisterExternalSystem(&m_PhysicsSystem);
            systems.RegisterExternalSystem(&m_AudioSystem);
            systems.RegisterExternalSystem(&m_ScriptSystem);
            systems.InitializeAll();

            // 初始化渲染系统 / Initialize rendering system
            const RendererAPI::API rendererAPI = RendererAPI::GetAPI();
            m_RenderingAvailable = RendererAPI::IsAvailable(rendererAPI);
            m_OpenGLSceneRendering = rendererAPI == RendererAPI::API::OpenGL && m_RenderingAvailable;
            if (m_RenderingAvailable) {
                m_RenderSystem.Initialize();
            }

            if (m_OpenGLSceneRendering) {
                // 设置基本渲染配置（禁用高级特性）/ Set basic rendering config (disable advanced features)
                RenderConfig config = RenderConfig::CreateBasic();
                m_RenderSystem.SetRenderConfig(config);
            } else if (m_RenderingAvailable) {
                ZGINE_CORE_WARN(
                    "Sandbox scene rendering currently requires OpenGL. '{}' renders clear frames while "
                    "pipelines and resource bindings are being implemented.",
                    RendererAPI::ToString(rendererAPI));
            } else {
                ZGINE_CORE_WARN("Sandbox rendering backend '{}' is unavailable.", RendererAPI::ToString(rendererAPI));
            }

            // 注意：在ZGINE_BASIC_RENDERING_ONLY=1时，下面的高级系统不可用
            // Note: Advanced systems are not available when ZGINE_BASIC_RENDERING_ONLY=1
            // m_ShadowSystem.Initialize();
            // m_IBLSystem.Initialize();
            // m_RenderSystem.SetShadowSystem(&m_ShadowSystem);
            // m_RenderSystem.SetIBLSystem(&m_IBLSystem);

            // Set up test World
            SetupTestScene();

            m_World.GetSystemManager().StartScene(&m_World);
        }

        void SetupTestScene() {
            // Directional Light (Sun)
            auto sun = m_World.CreateEntity("Sun");
            auto& light = sun.AddComponent<DirectionalLightComponent>();
            light.Color = Math::Vector3(1.0f, 0.95f, 0.9f);
            light.Intensity = 5.0f;
            sun.GetComponent<TransformComponent>().Rotation = Math::Vector3(-45.0f, -45.0f, 0.0f);

            // Ground Plane
            auto ground = m_World.CreateEntity("Ground");
            ground.AddComponent<PrimitiveComponent>(PrimitiveType::Cube);
            auto& gt = ground.GetComponent<TransformComponent>();
            gt.Translation = Math::Vector3(0.0f, -0.25f, 0.0f);
            gt.Scale = Math::Vector3(50.0f, 0.5f, 50.0f);
            ApplyMaterialPreset(ground, "Rough Concrete");

            // Spheres with different materials for PBR verification
            const char* presets[] = { "Gold", "Silver", "Iron", "Plastic Red", "Rubber Black" };
            for (int i = 0; i < 5; ++i) {
                auto sphere = m_World.CreateEntity(presets[i]);
                sphere.AddComponent<PrimitiveComponent>(PrimitiveType::Sphere);
                auto& st = sphere.GetComponent<TransformComponent>();
                st.Translation = Math::Vector3(-8.0f + i * 4.0f, 1.0f, 0.0f);
                ApplyMaterialPreset(sphere, presets[i]);
            }

            // Point Lights
            Math::Vector3 lightColors[] = {
                {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}
            };
            for (int i = 0; i < 4; ++i) {
                auto pointLight = m_World.CreateEntity("Point Light " + std::to_string(i));
                auto& pl = pointLight.AddComponent<PointLightComponent>();
                pl.Color = lightColors[i];
                pl.Intensity = 10.0f;
                pl.Range = 20.0f;
                pointLight.GetComponent<TransformComponent>().Translation = Math::Vector3(-10.0f + i * 6.0f, 3.0f, 5.0f);
            }
        }

        virtual void OnDetach() override {
            m_World.GetSystemManager().ShutdownAll();
            m_RenderSystem.Shutdown();
        }

        virtual void OnFixedUpdate(Timestep ts) override {
            m_World.GetSystemManager().FixedUpdateAll(&m_World, ts.GetSecondsF());
        }

        virtual void OnUpdate(Timestep ts) override {
            // Simple camera controller
            float mouseSensitivity = 0.1f;
            float moveSpeed = 10.0f * ts.GetSecondsF();

            if (Input::IsKeyDown(KeyCode::W)) m_Camera.MoveForward(moveSpeed);
            if (Input::IsKeyDown(KeyCode::S)) m_Camera.MoveForward(-moveSpeed);
            if (Input::IsKeyDown(KeyCode::A)) m_Camera.MoveRight(-moveSpeed);
            if (Input::IsKeyDown(KeyCode::D)) m_Camera.MoveRight(moveSpeed);
            if (Input::IsKeyDown(KeyCode::Q)) m_Camera.SetPosition(m_Camera.GetPosition() + Math::Vector3(0, -moveSpeed, 0));
            if (Input::IsKeyDown(KeyCode::E)) m_Camera.SetPosition(m_Camera.GetPosition() + Math::Vector3(0, moveSpeed, 0));

            m_World.GetSystemManager().UpdateAll(&m_World, ts.GetSecondsF());

            if (!m_RenderingAvailable) {
                return;
            }

            if (!m_OpenGLSceneRendering) {
                m_RenderSystem.BeginFrame();
                m_RenderSystem.RenderScene(&m_World, &m_Camera);
                return;
            }

            // Render to default framebuffer (screen)
            auto& window = Application::Get().GetWindow();
            glViewport(0, 0, window.GetWidth(), window.GetHeight());
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_RenderSystem.BeginFrame(); // Ensure light collection etc.
            m_RenderSystem.RenderScene(&m_World, &m_Camera);
        }

        virtual void OnGuiRender() override {
            // Keep it empty or add simple status
        }

        virtual void OnEvent(Event& e) override {
            // Camera aspect ratio update on resize would go here
        }

    private:
        void ApplyMaterialPreset(Entity entity, const std::string& presetName) {
            const PBRMaterialPreset* preset = PBRMaterialPresetRegistry::GetPreset(presetName);
            if (!preset) {
                ZGINE_CORE_WARN("Unknown PBR material preset: {}", presetName);
                return;
            }

            auto& material = entity.HasComponent<PBRMaterialComponent>()
                ? entity.GetComponent<PBRMaterialComponent>()
                : entity.AddComponent<PBRMaterialComponent>();
            material.Albedo = preset->Albedo;
            material.Metallic = preset->Metallic;
            material.Roughness = preset->Roughness;
            material.AO = preset->AO;
        }

        World m_World;
        Camera m_Camera;
        PhysicsSystem m_PhysicsSystem;
        AudioSystem m_AudioSystem;
        ScriptSystem m_ScriptSystem;
        RenderSystem m_RenderSystem;

        // 高级渲染系统（在基本模式下不使用）/ Advanced systems (not used in basic mode)
        // ShadowSystem m_ShadowSystem;
        // IBLSystem m_IBLSystem;
        bool m_RenderingAvailable = false;
        bool m_OpenGLSceneRendering = false;
    };

    class Sandbox : public Application {
    public:
        Sandbox() : Application("Zgine Sandbox") {
            PushLayer(new SandboxLayer());
        }
    };

    Application* CreateApplication() {
        return new Sandbox();
    }
}
