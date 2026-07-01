/**
 * @file main.cpp
 * @brief Zgine Editor Application Entry Point
 *
 * This file creates the editor application that integrates all engine systems
 * with the Editor UI for game development.
 */

#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Zgine.h>
#include <Zgine/Core/Application/EntryPoint.h>
#include <Zgine/Core/Application/Application.h>
#include <Zgine/Editor/Core/Editor.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Serialization/WorldSerializer.h>
#include <Zgine/Renderer/Pipeline/RenderSystem.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/Resources/Core/AssetManager.h>
#include <Zgine/Resources/Material/PBRMaterialPreset.h>
#include <Zgine/World/Camera/Camera.h>
#include <Zgine/Renderer/RHI/Framebuffer.h>

// Event system includes
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/Events.h>
#include <Zgine/World/Serialization/WorldSerializer.h>
#include <Zgine/World/Components/Components.h>
#include <glad/glad.h>
#include <ImGuizmo.h>

namespace Zgine {

    class EditorLayer : public Layer {
    public:
        EditorLayer()
            : Layer("EditorLayer"),
              m_EditorCamera(Math::Matrix4::Perspective(Math::DegToRad(45.0f), 1280.0f/720.0f, 0.1f, 1000.0f))
        {
            m_EditorCamera.SetPosition(Math::Vector3(0.0f, 2.0f, 10.0f));
            m_EditorCamera.SetRotation(Math::Vector3(-10.0f, -90.0f, 0.0f));
        }

        virtual void OnAttach() override {
            ZGINE_CORE_INFO("EditorLayer::OnAttach - Initializing Editor");

            // Initialize asset manager and material presets
            AssetManager::Get().Initialize();
            // PBRMaterialPresetRegistry::Initialize();

            // Initialize physics system
            m_PhysicsSystem.Initialize();

            // Initialize audio system
            m_AudioSystem.Initialize();

            // Initialize scripting system
            m_ScriptSystem.Initialize();
            // m_ScriptSystem.SetPhysicsSystem(&m_PhysicsSystem); // Removed: SetPhysicsSystem not needed
            m_ScriptSystem.SetAudioSystem(&m_AudioSystem);

            // Initialize rendering systems
            const RendererAPI::API rendererAPI = RendererAPI::GetAPI();
            m_RenderingAvailable = rendererAPI == RendererAPI::API::OpenGL && RendererAPI::IsAvailable(rendererAPI);
            if (!m_RenderingAvailable) {
                ZGINE_CORE_WARN(
                    "ZgineEditor currently requires the OpenGL renderer. '{}' is selectable for backend study, "
                    "but editor rendering is disabled until that backend has a device, swapchain, and ImGui renderer.",
                    RendererAPI::ToString(rendererAPI));
                return;
            }

            m_RenderSystem.Initialize();

            // Create framebuffer for World rendering
            FramebufferSpec fbSpec;
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            fbSpec.HDR = true;
            fbSpec.DepthStencil = true;
            m_SceneFramebuffer = Framebuffer::Create(fbSpec);
            if (!m_SceneFramebuffer) {
                ZGINE_CORE_ERROR("Failed to create editor scene framebuffer for backend '{}'.",
                    RendererAPI::ToString(rendererAPI));
                m_RenderingAvailable = false;
                return;
            }

            // Initialize Editor UI
            auto& window = Application::Get().GetWindow();
            m_Editor.Initialize(&window);
            m_Editor.GetContext().SetPlayRuntimeConfigurator([this](World& runtimeWorld) {
                auto& systems = runtimeWorld.GetSystemManager();
                systems.RegisterExternalSystem(&m_PhysicsSystem);
                systems.RegisterExternalSystem(&m_AudioSystem);
                systems.RegisterExternalSystem(&m_ScriptSystem);
            });

            // Setup Editor callbacks
            SetupEditorCallbacks();

            // Create default World content
            SetupDefaultScene();

            // ==========================================
            // TEST: TimerManager integration
            // ==========================================
            ZGINE_IGNORE_RESULT(Application::Get().GetTimerManager().AddRepeatTimer(1.0, []() {
                static int ticks = 0;
                ++ticks;
                ZGINE_CORE_INFO("TimerManager Tick! (1.0s) - Count: {}", ticks);
            }));

            ZGINE_IGNORE_RESULT(Application::Get().GetTimerManager().AddTimer(2.5, []() {
                ZGINE_CORE_WARN("TimerManager OneShot! (2.5s) - Triggered exactly once.");
            }));
            // ==========================================

            ZGINE_CORE_INFO("EditorLayer initialization complete");
        }

        virtual void OnDetach() override {
            ZGINE_CORE_INFO("EditorLayer::OnDetach - Shutting down Editor");

            m_Editor.Shutdown();
            m_ScriptSystem.Shutdown();
            m_AudioSystem.Shutdown();
            m_PhysicsSystem.Shutdown();
            m_RenderSystem.Shutdown();
        }

        virtual void OnFixedUpdate(Timestep ts) override {
            if (m_Editor.GetMode() == EditorMode::Play) {
                m_Editor.GetContext().FixedUpdatePlayRuntime(ts.GetSecondsF());
            }
        }

        virtual void OnUpdate(Timestep ts) override {
            if (!m_RenderingAvailable || !m_SceneFramebuffer) {
                return;
            }

            // Handle viewport resize FIRST, before rendering to ensure new framebuffer gets content
            HandleViewportResize();

            // Handle camera input when viewport is focused and gizmo is not in use
            if (m_Editor.IsSceneViewportFocused() && m_Editor.GetMode() == EditorMode::Edit) {
                HandleCameraInput(ts);
            }

            if (m_Editor.GetMode() == EditorMode::Play) {
                m_Editor.GetContext().UpdatePlayRuntime(ts.GetSecondsF());
            }

            // Render World to framebuffer (now with correct size)
            RenderSceneToFramebuffer();

            // Update editor
            m_Editor.OnUpdate(&m_World);
        }

        virtual void OnGuiRender() override {
            if (!m_RenderingAvailable || !m_SceneFramebuffer) {
                return;
            }

            // Note: Application's GuiLayer handles ImGui Begin/End frame
            // We just render our editor UI content here

            // Post-process scene and provide final texture to editor viewport
            uint32_t finalTexture = m_RenderSystem.PostProcess(m_SceneFramebuffer->GetColorAttachmentID());
            m_Editor.SetSceneTexture(finalTexture);
            m_Editor.SetSceneViewProjection(m_EditorCamera.GetView(), m_EditorCamera.GetProjection());
            m_Editor.SetRenderStats(m_RenderSystem.GetStats());

            // Render editor UI (dockspace, panels, menus, etc.)
            m_Editor.Render(&m_World);
        }

        virtual void OnEvent(Event& e) override {
            if (!m_RenderingAvailable) {
                return;
            }

            // Handle window resize
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
                m_EditorCamera.SetProjection(
                    Math::Matrix4::Perspective(Math::DegToRad(45.0f),
                        (float)event.GetWidth() / (float)event.GetHeight(),
                        0.1f, 1000.0f)
                );
                return false;
            });
        }

    private:
        void RenderSceneToFramebuffer() {
            if (!m_RenderingAvailable || !m_SceneFramebuffer) {
                return;
            }

            // Save current viewport
            GLint prevViewport[4];
            glGetIntegerv(GL_VIEWPORT, prevViewport);

            // Bind scene framebuffer (sets viewport automatically)
            m_SceneFramebuffer->Bind();

            // Enable depth testing for World rendering
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            // Enable Face Culling
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            // Clear with editor theme color
            auto clearColor = m_Editor.GetClearColor();
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Begin frame and render World
            World* renderWorld = m_Editor.GetContext().GetSceneContext().GetActiveScene();
            if (!renderWorld) {
                renderWorld = &m_World;
            }
            m_RenderSystem.BeginFrame();
            m_RenderSystem.RenderScene(renderWorld, &m_EditorCamera);

            // Unbind framebuffer and restore state
            m_SceneFramebuffer->Unbind();

            // Restore viewport to window size for ImGui rendering
            glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

            // Clear the default framebuffer so floating windows don't have ghosting
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Disable depth testing for ImGui (ImGui manages its own depth)
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }

        void HandleViewportResize() {
            if (!m_RenderingAvailable || !m_SceneFramebuffer) {
                return;
            }

            auto viewportSize = m_Editor.GetSceneViewportSize();
            if (viewportSize.x > 0 && viewportSize.y > 0) {
                unsigned int newWidth = static_cast<unsigned int>(viewportSize.x);
                unsigned int newHeight = static_cast<unsigned int>(viewportSize.y);

                if (newWidth < 100 || newHeight < 100) return;

                auto& spec = m_SceneFramebuffer->GetSpec();
                if (newWidth != spec.Width || newHeight != spec.Height) {
                    float aspect = (float)newWidth / (float)newHeight;
                    ZGINE_CORE_INFO("Resizing framebuffer to {}x{} (Aspect: {:.2f})", newWidth, newHeight, aspect);

                    m_SceneFramebuffer->Resize(newWidth, newHeight);
                    m_RenderSystem.ResizePostProcess(newWidth, newHeight);

                    m_EditorCamera.SetProjection(
                        Math::Matrix4::Perspective(Math::DegToRad(45.0f), aspect, 0.1f, 1000.0f)
                    );
                }
            }
        }

        void HandleCameraInput(Timestep ts) {
            if (!m_RenderingAvailable) {
                return;
            }

            // Don't process camera input while gizmo is being used
            if (ImGuizmo::IsUsing()) return;

            float moveSpeed = 10.0f * (float)ts;
            float mouseSensitivity = 0.2f;

            // Right-click camera rotation (orbit)
            const Math::Vector2 mousePosition = Input::GetMousePosition();
            const float mouseX = mousePosition.x;
            const float mouseY = mousePosition.y;
            bool rightClick = Input::IsMouseButtonDown(MouseButton::Right);

            // WASD movement only when right mouse button is held (FPS-style camera)
            if (rightClick && m_Editor.IsSceneViewportFocused()) {
                if (Input::IsKeyDown(KeyCode::W)) m_EditorCamera.MoveForward(moveSpeed);
                if (Input::IsKeyDown(KeyCode::S)) m_EditorCamera.MoveForward(-moveSpeed);
                if (Input::IsKeyDown(KeyCode::A)) m_EditorCamera.MoveRight(-moveSpeed);
                if (Input::IsKeyDown(KeyCode::D)) m_EditorCamera.MoveRight(moveSpeed);
                if (Input::IsKeyDown(KeyCode::Q))
                    m_EditorCamera.SetPosition(m_EditorCamera.GetPosition() + Math::Vector3(0, -moveSpeed, 0));
                if (Input::IsKeyDown(KeyCode::E))
                    m_EditorCamera.SetPosition(m_EditorCamera.GetPosition() + Math::Vector3(0, moveSpeed, 0));
            }

            if (rightClick) {
                if (m_IsRightMousePressed) {
                    float deltaX = mouseX - m_LastMouseX;
                    float deltaY = mouseY - m_LastMouseY;
                    m_EditorCamera.Rotate(deltaX * mouseSensitivity, -deltaY * mouseSensitivity);
                }
                m_IsRightMousePressed = true;
            } else {
                m_IsRightMousePressed = false;
            }

            m_LastMouseX = mouseX;
            m_LastMouseY = mouseY;
        }

        void SetupEditorCallbacks() {
            // Access the event bus from editor context
            auto& eventBus = m_Editor.GetContext().GetEventBus();

            // Subscribe to entity creation events (for primitives)
            eventBus.Subscribe<EntityCreatedEvent>([this](EntityCreatedEvent& e) {
                auto entity = e.GetEntity();
                auto primitiveType = e.GetPrimitiveType();
                if (primitiveType != PrimitiveType::None && entity.IsValid()) {
                    entity.AddComponent<PrimitiveComponent>(primitiveType);
                    // PBRMaterialPresetRegistry presets can be copied into PBRMaterialComponent here.
                }
            });

            // Subscribe to entity deletion events
            eventBus.Subscribe<EntityDeletedEvent>([this](EntityDeletedEvent& e) {
                auto entity = e.GetEntity();
                if (entity.IsValid()) {
                    m_World.DestroyEntity(entity);
                }
            });

            // Subscribe to transform change events
            eventBus.Subscribe<TransformChangedEvent>([this](TransformChangedEvent& e) {
                auto entity = e.GetEntity();
                if (entity.IsValid() && entity.HasComponent<RigidbodyComponent>()) {
                    m_PhysicsSystem.UpdateBodyTransform(entity);
                }
            });

            // Subscribe to audio action events
            eventBus.Subscribe<AudioActionEvent>([this](AudioActionEvent& e) {
                auto entity = e.GetEntity();
                if (!entity.IsValid() || !entity.HasComponent<AudioSourceComponent>()) return;
                switch (e.GetAction()) {
                    case AudioAction::Play:
                        m_AudioSystem.PlayAudioSource(entity);
                        break;
                    case AudioAction::Stop:
                        m_AudioSystem.StopAudioSource(entity);
                        break;
                    case AudioAction::Pause:
                        m_AudioSystem.PauseAudioSource(entity);
                        break;
                    default:
                        break;
                }
            });

            // Subscribe to World save events
            eventBus.Subscribe<SceneSavedEvent>([this](SceneSavedEvent& e) {
                auto* World = e.GetScene();
                if (World) {
                    WorldSerializer serializer(World);
                    serializer.SerializeToFile("assets/scenes/default.json");
                    ZGINE_CORE_INFO("World saved to assets/scenes/default.json");
                }
            });

            // Subscribe to World load events
            eventBus.Subscribe<SceneLoadedEvent>([this](SceneLoadedEvent& e) {
                auto* World = e.GetScene();
                if (!World) return;

                // Stop systems before loading
                m_PhysicsSystem.OnSceneStop();
                m_AudioSystem.OnSceneStop();
                m_ScriptSystem.OnSceneStop();

                // Load World
                WorldSerializer serializer(World);
                if (serializer.DeserializeFromFile("assets/scenes/default.json")) {
                    ZGINE_CORE_INFO("World loaded from assets/scenes/default.json");
                }

                // Restart systems
                m_PhysicsSystem.OnSceneStart(World);
                m_AudioSystem.OnSceneStart(World);
                m_ScriptSystem.OnSceneStart(World);
            });

            // Subscribe to script reload events
            eventBus.Subscribe<ScriptReloadEvent>([this](ScriptReloadEvent& e) {
                m_ScriptSystem.ReloadScript(e.GetEntity());
            });

            // Subscribe to play mode change events
            eventBus.Subscribe<PlayModeChangedEvent>([this](PlayModeChangedEvent& e) {
                if (e.GetMode() == EditorMode::Play) {
                    ZGINE_CORE_INFO("Entered Play mode");
                } else if (e.GetMode() == EditorMode::Edit) {
                    ZGINE_CORE_INFO("Exited Play mode");
                }
            });

            // Subscribe to asset drop events
            eventBus.Subscribe<AssetDroppedEvent>([this](AssetDroppedEvent& e) {
                auto* World = e.GetScene();
                const auto& path = e.GetAssetPath();
                std::string ext = path.extension().string();
                if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb") {
                    // Load model - the MeshComponent uses AssetHandle for mesh loading
                    auto entity = World->CreateEntity(path.stem().string());
                    entity.AddComponent<MeshComponent>();
                    // TODO: Use AssetManager to load mesh and assign MeshHandle
                    ZGINE_CORE_INFO("Model entity created: {}", path.string());
                } else if (ext == ".lua") {
                    // Attach script to selected entity
                    auto selected = m_Editor.GetSelectedEntity();
                    if (selected) {
                        auto& script = selected.AddComponent<ScriptComponent>();
                        script.ScriptPath = path.string();
                        ZGINE_CORE_INFO("Attached script: {}", path.string());
                    }
                }
            });
        }

        void SetupDefaultScene() {
            // DEBUG: Simplified World setup - no skybox/IBL, just basic objects
            // Skip skybox loading for now
            /*
            auto skybox = std::make_shared<Skybox>();
            if (skybox->LoadFromEquirectangular("assets/materials/sky-box/qwantani_noon_puresky_8k.exr")) {
                m_RenderSystem.SetSkybox(skybox);
                m_IBLSystem.GenerateFromSkybox(skybox.get());
                m_RenderSystem.SetUseIBL(true);
            } else {
                ZGINE_CORE_WARN("Failed to load skybox, using default background");
            }
            */

            // Create directional light (sun) - essential for basic lighting
            auto sun = m_World.CreateEntity("Directional Light");
            auto& light = sun.AddComponent<DirectionalLightComponent>();
            light.Direction = Math::Vector3(-0.5f, -1.0f, -0.5f);
            light.Intensity = 0.8f;

            // Create ground plane
            auto ground = m_World.CreateEntity("Ground");
            ground.AddComponent<PrimitiveComponent>(PrimitiveType::Plane);
            auto& groundTransform = ground.GetComponent<TransformComponent>();
            groundTransform.Scale = Math::Vector3(20.0f, 1.0f, 20.0f);
            // Replace texture preset with manual material to avoid missing asset errors
            auto& groundMat = ground.AddComponent<PBRMaterialComponent>();
            groundMat.Albedo = Math::Vector3(0.5f, 0.5f, 0.5f);
            groundMat.Roughness = 0.8f;

            // Gold metal cube (high metallic, low roughness = sharp reflections)
            auto cube = m_World.CreateEntity("Gold Cube");
            cube.AddComponent<PrimitiveComponent>(PrimitiveType::Cube);
            auto& cubeTransform = cube.GetComponent<TransformComponent>();
            cubeTransform.Translation = Math::Vector3(0.0f, 2.0f, 0.0f);
            cubeTransform.Scale = Math::Vector3(2.0f, 2.0f, 2.0f);
            cubeTransform.Rotation = Math::Vector3(25.0f, 35.0f, 0.0f);
            auto& cubeMat = cube.AddComponent<PBRMaterialComponent>();
            cubeMat.Albedo = Math::Vector3(1.0f, 0.76f, 0.33f);
            cubeMat.Metallic = 1.0f;
            cubeMat.Roughness = 0.15f;

            // Blue plastic cube (non-metal, rough = soft highlights)
            auto cube2 = m_World.CreateEntity("Plastic Cube");
            cube2.AddComponent<PrimitiveComponent>(PrimitiveType::Cube);
            auto& cube2Transform = cube2.GetComponent<TransformComponent>();
            cube2Transform.Translation = Math::Vector3(4.0f, 1.5f, 0.0f);
            cube2Transform.Scale = Math::Vector3(1.5f, 1.5f, 1.5f);
            auto& cube2Mat = cube2.AddComponent<PBRMaterialComponent>();
            cube2Mat.Albedo = Math::Vector3(0.1f, 0.4f, 0.9f);
            cube2Mat.Metallic = 0.0f;
            cube2Mat.Roughness = 0.7f;

            // Chrome sphere (full metal, very smooth)
            auto sphere = m_World.CreateEntity("Chrome Sphere");
            sphere.AddComponent<PrimitiveComponent>(PrimitiveType::Sphere);
            auto& sphereTransform = sphere.GetComponent<TransformComponent>();
            sphereTransform.Translation = Math::Vector3(-4.0f, 1.5f, 0.0f);
            sphereTransform.Scale = Math::Vector3(2.0f, 2.0f, 2.0f);
            auto& sphereMat = sphere.AddComponent<PBRMaterialComponent>();
            sphereMat.Albedo = Math::Vector3(0.95f, 0.93f, 0.88f);
            sphereMat.Metallic = 1.0f;
            sphereMat.Roughness = 0.05f;

            // Point light - red, positioned near the cube
            auto pointLight = m_World.CreateEntity("Point Light");
            auto& pointLightTransform = pointLight.GetComponent<TransformComponent>();
            pointLightTransform.Translation = Math::Vector3(-3.0f, 3.0f, 2.0f);
            auto& pl = pointLight.AddComponent<PointLightComponent>();
            pl.Color = Math::Vector3(1.0f, 0.3f, 0.1f);
            pl.Intensity = 5.0f;
            pl.Constant = 1.0f;
            pl.Linear = 0.09f;
            pl.Quadratic = 0.032f;

            // Spot light - white, pointing down at the ground
            auto spotLight = m_World.CreateEntity("Spot Light");
            auto& spotLightTransform = spotLight.GetComponent<TransformComponent>();
            spotLightTransform.Translation = Math::Vector3(3.0f, 6.0f, -2.0f);
            auto& sl = spotLight.AddComponent<SpotLightComponent>();
            sl.Direction = Math::Vector3(0.0f, -1.0f, 0.2f);
            sl.Color = Math::Vector3(0.4f, 0.8f, 1.0f);
            sl.Intensity = 8.0f;
            sl.InnerCone = 15.0f;
            sl.OuterCone = 25.0f;

            ZGINE_CORE_INFO("Default World created with multi-light demo");
        }

    private:
        // Engine systems
        World m_World;
        Camera m_EditorCamera;
        PhysicsSystem m_PhysicsSystem;
        AudioSystem m_AudioSystem;
        ScriptSystem m_ScriptSystem;
        RenderSystem m_RenderSystem;

        // Editor
        Editor m_Editor;

        // Framebuffer for World rendering
        std::shared_ptr<Framebuffer> m_SceneFramebuffer;

        // Camera control state
        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        bool m_IsRightMousePressed = false;
        bool m_RenderingAvailable = false;
    };

    class ZgineEditorApp : public Application {
    public:
        ZgineEditorApp() : Application("Zgine Editor") {
            PushLayer(new EditorLayer());
        }
    };

    Application* CreateApplication() {
        return new ZgineEditorApp();
    }

}
