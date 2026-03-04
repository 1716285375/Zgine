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
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Core/Entity.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Scene/Serialization/SceneSerializer.h>
#include <Zgine/Renderer/Core/RenderSystem.h>
#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/Resources/Core/AssetManager.h>
#include <Zgine/Resources/Material/PBRMaterialPreset.h>
#include <Zgine/Scene/Camera/Camera.h>

// Event system includes
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/EntityEvents.h>
#include <Zgine/Editor/Events/SceneEvents.h>
#include <Zgine/Editor/Events/EditorStateEvents.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Zgine {

    class EditorLayer : public Layer {
    public:
        EditorLayer()
            : Layer("EditorLayer"),
              m_EditorCamera(glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 1000.0f))
        {
            m_EditorCamera.SetPosition(glm::vec3(0.0f, 2.0f, 10.0f));
            m_EditorCamera.SetRotation(glm::vec3(-10.0f, 0.0f, 0.0f));
        }

        virtual void OnAttach() override {
            ZGINE_CORE_INFO("EditorLayer::OnAttach - Initializing Editor");

            // Initialize asset manager and material presets
            AssetManager::Get().Initialize();
            PBRMaterialPresetManager::Initialize();

            // Initialize physics system
            m_PhysicsSystem.Initialize();

            // Initialize audio system
            m_AudioSystem.Initialize();

            // Initialize scripting system
            m_ScriptSystem.Initialize();
            // m_ScriptSystem.SetPhysicsSystem(&m_PhysicsSystem); // Removed: SetPhysicsSystem not needed
            m_ScriptSystem.SetAudioSystem(&m_AudioSystem);

            // Initialize rendering systems
            m_RenderSystem.Initialize();

            // Create framebuffer for scene rendering
            CreateFramebuffer(1280, 720);

            // Initialize Editor UI
            auto& window = Application::Get().GetWindow();
            m_Editor.Initialize(&window);

            // Setup Editor callbacks
            SetupEditorCallbacks();

            // Create default scene content
            SetupDefaultScene();

            ZGINE_CORE_INFO("EditorLayer initialization complete");
        }

        virtual void OnDetach() override {
            ZGINE_CORE_INFO("EditorLayer::OnDetach - Shutting down Editor");

            // Stop systems
            m_PhysicsSystem.OnSceneStop();
            m_AudioSystem.OnSceneStop();
            m_ScriptSystem.OnSceneStop();

            // Shutdown
            m_Editor.Shutdown();
            m_RenderSystem.Shutdown();
        }

        virtual void OnUpdate(Timestep ts) override {
            // Handle viewport resize FIRST, before rendering to ensure new framebuffer gets content
            HandleViewportResize();

            // Handle camera input when viewport is focused
            if (m_Editor.IsSceneViewportFocused() && m_Editor.GetMode() == EditorMode::Edit) {
                HandleCameraInput(ts);
            }

            // Update systems based on editor mode
            if (m_Editor.GetMode() == EditorMode::Play) {
                m_PhysicsSystem.Step(ts);
                m_PhysicsSystem.SyncPhysicsToECS(&m_Scene);
                m_AudioSystem.Update(&m_Scene, ts);
                m_ScriptSystem.Update(&m_Scene, ts);
            }

            // Render scene to framebuffer (now with correct size)
            RenderSceneToFramebuffer();

            // Update editor
            m_Editor.OnUpdate(&m_Scene);
        }

        virtual void OnGuiRender() override {
            // Note: Application's GuiLayer handles ImGui Begin/End frame
            // We just render our editor UI content here

            // Provide scene texture to editor viewport (framebuffer already resized in OnUpdate)
            m_Editor.SetSceneTexture(m_SceneColorTexture);
            m_Editor.SetSceneViewProjection(m_EditorCamera.GetView(), m_EditorCamera.GetProjection());
            m_Editor.SetRenderStats(m_RenderSystem.GetStats());

            // Render editor UI (dockspace, panels, menus, etc.)
            m_Editor.Render(&m_Scene);
        }

        virtual void OnEvent(Event& e) override {
            // Handle window resize
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
                m_EditorCamera.SetProjection(
                    glm::perspective(glm::radians(45.0f),
                        (float)event.GetWidth() / (float)event.GetHeight(),
                        0.1f, 1000.0f)
                );
                return false;
            });
        }

    private:
        void CreateFramebuffer(unsigned int width, unsigned int height) {
            // Store old resources for deletion after new ones are created
            unsigned int oldFBO = m_SceneFBO;
            unsigned int oldColorTexture = m_SceneColorTexture;
            unsigned int oldDepthRBO = m_SceneDepthRBO;

            // Create new framebuffer first (before deleting old one to prevent flickering)
            unsigned int newFBO = 0;
            unsigned int newColorTexture = 0;
            unsigned int newDepthRBO = 0;

            glGenFramebuffers(1, &newFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, newFBO);

            // Color attachment
            glGenTextures(1, &newColorTexture);
            glBindTexture(GL_TEXTURE_2D, newColorTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newColorTexture, 0);

            // Depth attachment
            glGenRenderbuffers(1, &newDepthRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, newDepthRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, newDepthRBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                ZGINE_CORE_ERROR("Editor framebuffer is not complete!");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Now swap to the new resources
            m_SceneFBO = newFBO;
            m_SceneColorTexture = newColorTexture;
            m_SceneDepthRBO = newDepthRBO;
            m_FramebufferWidth = width;
            m_FramebufferHeight = height;

            // Delete old resources after swap (safe to do now)
            if (oldFBO != 0) {
                glDeleteFramebuffers(1, &oldFBO);
                glDeleteTextures(1, &oldColorTexture);
                glDeleteRenderbuffers(1, &oldDepthRBO);
            }
        }

        void RenderSceneToFramebuffer() {
            // Save current viewport
            GLint prevViewport[4];
            glGetIntegerv(GL_VIEWPORT, prevViewport);

            // Bind scene framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, m_SceneFBO);
            glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);

            // Enable depth testing for scene rendering
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);  // 确保使用正确的深度函数
            glDepthMask(GL_TRUE);  // 确保深度写入启用

            // Clear with editor theme color
            auto clearColor = m_Editor.GetClearColor();
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Begin frame and render scene
            m_RenderSystem.BeginFrame();
            m_RenderSystem.RenderScene(&m_Scene, &m_EditorCamera);

            // Unbind framebuffer and restore state
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Restore viewport to window size for ImGui rendering
            glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

            // Clear the default framebuffer so floating windows don't have ghosting
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Disable depth testing for ImGui (ImGui manages its own depth)
            glDisable(GL_DEPTH_TEST);
        }

        void HandleViewportResize() {
            auto viewportSize = m_Editor.GetSceneViewportSize();
            if (viewportSize.x > 0 && viewportSize.y > 0) {
                unsigned int newWidth = static_cast<unsigned int>(viewportSize.x);
                unsigned int newHeight = static_cast<unsigned int>(viewportSize.y);

                // Prevent resizing to tiny dimensions (e.g., during initialization)
                if (newWidth < 100 || newHeight < 100) return;

                if (newWidth != m_FramebufferWidth || newHeight != m_FramebufferHeight) {
                    float aspect = (float)newWidth / (float)newHeight;
                    ZGINE_CORE_INFO("Resizing framebuffer to {}x{} (Aspect: {:.2f})", newWidth, newHeight, aspect);

                    CreateFramebuffer(newWidth, newHeight);

                    // DEBUG: Force standard aspect ratio to rule out distortion
                    // aspect = 1.777f;

                    m_EditorCamera.SetProjection(
                        glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f)
                    );
                }
            }
        }

        void HandleCameraInput(Timestep ts) {
            float moveSpeed = 10.0f * ts;
            float mouseSensitivity = 0.2f;

            // WASD movement
            if (m_Editor.IsSceneViewportFocused()) {
                if (Input::IsKeyPressed(GLFW_KEY_W)) m_EditorCamera.MoveForward(moveSpeed);
                if (Input::IsKeyPressed(GLFW_KEY_S)) m_EditorCamera.MoveForward(-moveSpeed);
                if (Input::IsKeyPressed(GLFW_KEY_A)) m_EditorCamera.MoveRight(-moveSpeed);
                if (Input::IsKeyPressed(GLFW_KEY_D)) m_EditorCamera.MoveRight(moveSpeed);
                if (Input::IsKeyPressed(GLFW_KEY_Q))
                    m_EditorCamera.SetPosition(m_EditorCamera.GetPosition() + glm::vec3(0, -moveSpeed, 0));
                if (Input::IsKeyPressed(GLFW_KEY_E))
                    m_EditorCamera.SetPosition(m_EditorCamera.GetPosition() + glm::vec3(0, moveSpeed, 0));
            }

            // Right-click camera rotation (orbit)
            auto [mouseX, mouseY] = Input::GetMousePosition();

            // Only rotate if right click is held AND (we just started holding it OR we are already rotating)
            bool rightClick = Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

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
                    PBRMaterialPresetManager::ApplyPreset(entity, "Ceramic");
                }
            });

            // Subscribe to entity deletion events
            eventBus.Subscribe<EntityDeletedEvent>([this](EntityDeletedEvent& e) {
                auto entity = e.GetEntity();
                if (entity.IsValid()) {
                    m_Scene.DestroyEntity(entity);
                }
            });

            // Subscribe to transform change events
            eventBus.Subscribe<TransformChangedEvent>([this](TransformChangedEvent& e) {
                auto entity = e.GetEntity();
                if (entity.IsValid() && entity.HasComponent<RigidBodyComponent>()) {
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

            // Subscribe to scene save events
            eventBus.Subscribe<SceneSavedEvent>([this](SceneSavedEvent& e) {
                auto* scene = e.GetScene();
                if (scene) {
                    SceneSerializer serializer(scene);
                    serializer.SerializeToFile("assets/scenes/default.json");
                    ZGINE_CORE_INFO("Scene saved to assets/scenes/default.json");
                }
            });

            // Subscribe to scene load events
            eventBus.Subscribe<SceneLoadedEvent>([this](SceneLoadedEvent& e) {
                auto* scene = e.GetScene();
                if (!scene) return;

                // Stop systems before loading
                m_PhysicsSystem.OnSceneStop();
                m_AudioSystem.OnSceneStop();
                m_ScriptSystem.OnSceneStop();

                // Load scene
                SceneSerializer serializer(scene);
                if (serializer.DeserializeFromFile("assets/scenes/default.json")) {
                    ZGINE_CORE_INFO("Scene loaded from assets/scenes/default.json");
                }

                // Restart systems
                m_PhysicsSystem.OnSceneStart(scene);
                m_AudioSystem.OnSceneStart(scene);
                m_ScriptSystem.OnSceneStart(scene);
            });

            // Subscribe to script reload events
            eventBus.Subscribe<ScriptReloadEvent>([this](ScriptReloadEvent& e) {
                m_ScriptSystem.ReloadScript(e.GetEntity());
            });

            // Subscribe to play mode change events
            eventBus.Subscribe<PlayModeChangedEvent>([this](PlayModeChangedEvent& e) {
                if (e.GetMode() == EditorMode::Play) {
                    // Start play mode
                    m_PhysicsSystem.OnSceneStart(&m_Scene);
                    m_AudioSystem.OnSceneStart(&m_Scene);
                    m_ScriptSystem.OnSceneStart(&m_Scene);
                    ZGINE_CORE_INFO("Entered Play mode");
                } else {
                    // Stop play mode
                    m_PhysicsSystem.OnSceneStop();
                    m_AudioSystem.OnSceneStop();
                    m_ScriptSystem.OnSceneStop();
                    ZGINE_CORE_INFO("Exited Play mode");
                }
            });

            // Subscribe to asset drop events
            eventBus.Subscribe<AssetDroppedEvent>([this](AssetDroppedEvent& e) {
                auto* scene = e.GetScene();
                const auto& path = e.GetAssetPath();
                std::string ext = path.extension().string();
                if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb") {
                    // Load model - the MeshComponent uses AssetHandle for mesh loading
                    auto entity = scene->CreateEntity(path.stem().string());
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
            // DEBUG: Simplified scene setup - no skybox/IBL, just basic objects
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
            auto sun = m_Scene.CreateEntity("Directional Light");
            auto& light = sun.AddComponent<DirectionalLightComponent>();
            light.Direction = glm::vec3(-0.5f, -1.0f, -0.5f);
            light.Intensity = 3.0f;

            // Create ground plane
            auto ground = m_Scene.CreateEntity("Ground");
            ground.AddComponent<PrimitiveComponent>(PrimitiveType::Plane);
            auto& groundTransform = ground.GetComponent<TransformComponent>();
            groundTransform.Scale = glm::vec3(20.0f, 1.0f, 20.0f);
            // Replace texture preset with manual material to avoid missing asset errors
            auto& groundMat = ground.AddComponent<PBRMaterialComponent>();
            groundMat.Albedo = glm::vec3(0.5f, 0.5f, 0.5f);
            groundMat.Roughness = 0.8f;

            // Create sample cube - 增大尺寸和调整位置以显示3D效果
            auto cube = m_Scene.CreateEntity("Cube");
            cube.AddComponent<PrimitiveComponent>(PrimitiveType::Cube);
            auto& cubeTransform = cube.GetComponent<TransformComponent>();
            cubeTransform.Translation = glm::vec3(0.0f, 2.0f, 0.0f);  // 抬高位置
            cubeTransform.Scale = glm::vec3(2.0f, 2.0f, 2.0f);  // 增大尺寸，更明显
            cubeTransform.Rotation = glm::vec3(25.0f, 35.0f, 0.0f);  // 调整旋转角度
            // Replace texture preset with manual material
            // auto& cubeMat = cube.AddComponent<PBRMaterialComponent>();
            // cubeMat.Albedo = glm::vec3(1.0f, 0.8f, 0.0f); // Gold
            // cubeMat.Metallic = 0.9f;
            // cubeMat.Roughness = 0.2f;

            ZGINE_CORE_INFO("Default scene created (DEBUG MODE - simplified)");
        }

    private:
        // Engine systems
        Scene m_Scene;
        Camera m_EditorCamera;
        PhysicsSystem m_PhysicsSystem;
        AudioSystem m_AudioSystem;
        ScriptSystem m_ScriptSystem;
        RenderSystem m_RenderSystem;

        // Editor
        Editor m_Editor;

        // Framebuffer for scene rendering
        unsigned int m_SceneFBO = 0;
        unsigned int m_SceneColorTexture = 0;
        unsigned int m_SceneDepthRBO = 0;
        unsigned int m_FramebufferWidth = 1280;
        unsigned int m_FramebufferHeight = 720;

        // Camera control state
        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        bool m_IsRightMousePressed = false;
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
