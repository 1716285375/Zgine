#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Zgine.h>
#include <Zgine/Core/Application/EntryPoint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Zgine/Core/Application/Application.h>

namespace Zgine {

    class SandboxLayer : public Layer {
    public:
        SandboxLayer()
            : Layer("SandboxLayer"),
              m_Camera(glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 1000.0f))
        {
            m_Camera.SetPosition(glm::vec3(0.0f, 5.0f, 15.0f));
            m_Camera.SetRotation(glm::vec3(-20.0f, -90.0f, 0.0f));
        }

        virtual void OnAttach() override {
            ZGINE_CORE_INFO("SandboxLayer::OnAttach");

            // Initialize required systems
            AssetManager::Get().Initialize();
            PBRMaterialPresetManager::Initialize();

            m_PhysicsSystem.Initialize();
            m_AudioSystem.Initialize();
            m_ScriptSystem.Initialize();
            // m_PhysicsSystem.SetPhysicsSystem(&m_PhysicsSystem); // Removed: not needed
            m_ScriptSystem.SetAudioSystem(&m_AudioSystem);

            // 初始化渲染系统 / Initialize rendering system
            m_RenderSystem.Initialize();

            // 设置基本渲染配置（禁用高级特性）/ Set basic rendering config (disable advanced features)
            RenderConfig config = RenderConfig::CreateBasic();
            m_RenderSystem.SetRenderConfig(config);

            // 注意：在ZGINE_BASIC_RENDERING_ONLY=1时，下面的高级系统不可用
            // Note: Advanced systems are not available when ZGINE_BASIC_RENDERING_ONLY=1
            // m_ShadowSystem.Initialize();
            // m_IBLSystem.Initialize();
            // m_RenderSystem.SetShadowSystem(&m_ShadowSystem);
            // m_RenderSystem.SetIBLSystem(&m_IBLSystem);

            // Set up test scene
            SetupTestScene();

            m_PhysicsSystem.OnSceneStart(&m_Scene);
            m_AudioSystem.OnSceneStart(&m_Scene);
            m_ScriptSystem.OnSceneStart(&m_Scene);
        }

        void SetupTestScene() {
            // Directional Light (Sun)
            auto sun = m_Scene.CreateEntity("Sun");
            auto& light = sun.AddComponent<DirectionalLightComponent>();
            light.Color = glm::vec3(1.0f, 0.95f, 0.9f);
            light.Intensity = 5.0f;
            sun.GetComponent<TransformComponent>().Rotation = glm::vec3(-45.0f, -45.0f, 0.0f);

            // Ground Plane
            auto ground = m_Scene.CreateEntity("Ground");
            ground.AddComponent<PrimitiveComponent>(PrimitiveType::Cube);
            auto& gt = ground.GetComponent<TransformComponent>();
            gt.Translation = glm::vec3(0.0f, -0.25f, 0.0f);
            gt.Scale = glm::vec3(50.0f, 0.5f, 50.0f);
            PBRMaterialPresetManager::ApplyPreset(ground, "Rough Concrete");

            // Spheres with different materials for PBR verification
            const char* presets[] = { "Gold", "Silver", "Iron", "Plastic Red", "Rubber Black" };
            for (int i = 0; i < 5; ++i) {
                auto sphere = m_Scene.CreateEntity(presets[i]);
                sphere.AddComponent<PrimitiveComponent>(PrimitiveType::Sphere);
                auto& st = sphere.GetComponent<TransformComponent>();
                st.Translation = glm::vec3(-8.0f + i * 4.0f, 1.0f, 0.0f);
                PBRMaterialPresetManager::ApplyPreset(sphere, presets[i]);
            }

            // Point Lights
            glm::vec3 lightColors[] = {
                {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}
            };
            for (int i = 0; i < 4; ++i) {
                auto pointLight = m_Scene.CreateEntity("Point Light " + std::to_string(i));
                auto& pl = pointLight.AddComponent<PointLightComponent>();
                pl.Color = lightColors[i];
                pl.Intensity = 10.0f;
                pl.Range = 20.0f;
                pointLight.GetComponent<TransformComponent>().Translation = glm::vec3(-10.0f + i * 6.0f, 3.0f, 5.0f);
            }
        }

        virtual void OnDetach() override {
            m_PhysicsSystem.OnSceneStop();
            m_AudioSystem.OnSceneStop();
            m_ScriptSystem.OnSceneStop();
            m_RenderSystem.Shutdown();
        }

        virtual void OnUpdate(Timestep ts) override {
            // Simple camera controller
            float mouseSensitivity = 0.1f;
            float moveSpeed = 10.0f * ts;

            if (Input::IsKeyPressed(GLFW_KEY_W)) m_Camera.MoveForward(moveSpeed);
            if (Input::IsKeyPressed(GLFW_KEY_S)) m_Camera.MoveForward(-moveSpeed);
            if (Input::IsKeyPressed(GLFW_KEY_A)) m_Camera.MoveRight(-moveSpeed);
            if (Input::IsKeyPressed(GLFW_KEY_D)) m_Camera.MoveRight(moveSpeed);
            if (Input::IsKeyPressed(GLFW_KEY_Q)) m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0, -moveSpeed, 0));
            if (Input::IsKeyPressed(GLFW_KEY_E)) m_Camera.SetPosition(m_Camera.GetPosition() + glm::vec3(0, moveSpeed, 0));

            // System updates
            m_PhysicsSystem.Step(ts);
            m_PhysicsSystem.SyncPhysicsToECS(&m_Scene);
            m_AudioSystem.Update(&m_Scene, ts);

            // Render to default framebuffer (screen)
            auto& window = Application::Get().GetWindow();
            glViewport(0, 0, window.GetWidth(), window.GetHeight());
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_RenderSystem.BeginFrame(); // Ensure light collection etc.
            m_RenderSystem.RenderScene(&m_Scene, &m_Camera);
        }

        virtual void OnGuiRender() override {
            // Keep it empty or add simple status
        }

        virtual void OnEvent(Event& e) override {
            // Camera aspect ratio update on resize would go here
        }

    private:
        Scene m_Scene;
        Camera m_Camera;
        PhysicsSystem m_PhysicsSystem;
        AudioSystem m_AudioSystem;
        ScriptSystem m_ScriptSystem;
        RenderSystem m_RenderSystem;

        // 高级渲染系统（在基本模式下不使用）/ Advanced systems (not used in basic mode)
        // ShadowSystem m_ShadowSystem;
        // IBLSystem m_IBLSystem;
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
