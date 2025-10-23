#include "MainControlLayer.h"
#include "Zgine/Renderer/Renderer.h"
#include "Zgine/Renderer/Lighting.h"
#include "Zgine/Renderer/Material.h"
#include "Zgine/Renderer/ResourceManager.h"
#include "Zgine/Log.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Application.h"

namespace Sandbox {

	MainControlLayer::MainControlLayer()
		: Layer("MainControlLayer"),
		m_2DCamera(-1.6f, 1.6f, -0.9f, 0.9f),
		m_3DCamera(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f),
		m_2DCameraPosition(0.0f, 0.0f, 0.0f),
		m_2DCameraSpeed(1.0f),
		m_3DCameraPosition(0.0f, 2.0f, 5.0f),
		m_3DCameraRotation(0.0f),
		m_3DCameraSpeed(5.0f),
		m_3DRotationSpeed(1.0f),
		m_Time(0.0f),
		m_Show2DTestWindow(true),
		m_Show3DTestWindow(true),
		m_ShowPerformanceWindow(true),
		m_ShowSettingsWindow(false),
		// 2D options
		m_2DShowQuads(true),
		m_2DShowLines(true),
		m_2DShowCircles(true),
		m_2DShowAdvanced(false),
		m_2DShowTriangles(false),
		m_2DShowEllipses(false),
		m_2DShowArcs(false),
		m_2DShowGradients(false),
		m_2DAnimateCircles(true),
		m_2DAnimateQuads(true),
		m_2DAnimationSpeed(1.0f),
		m_2DLineThickness(0.02f),
		m_2DCircleRadius(0.5f),
		m_2DCircleSegments(32),
		// 3D options
		m_3DShowCubes(true),
		m_3DShowSpheres(true),
		m_3DShowCylinders(true),
		m_3DShowPlanes(true),
		m_3DShowEnvironment(false),
		m_3DAnimateObjects(true),
		m_3DWireframeMode(false),
		m_3DLightIntensity(1.0f),
		m_3DLightPosition(2.0f, 2.0f, 2.0f),
		m_3DLightColor(1.0f, 1.0f, 1.0f),
		// Particle System
		m_ShowParticleSystem(true),
		m_ParticleSystemEnabled(true),
		m_ParticleSystemIntensity(1.0f),
		// Performance
		m_FPS(0.0f),
		m_FrameCount(0),
		m_FPSTimer(0.0f)
	{
	}

	MainControlLayer::~MainControlLayer()
	{
	}

	void MainControlLayer::OnAttach()
	{
		// Initialize camera positions
		m_2DCamera.SetPosition(m_2DCameraPosition);
		m_3DCamera.SetPosition(m_3DCameraPosition);
		m_3DCamera.SetRotation(m_3DCameraRotation);

		// ===== Test Complete Lighting System =====
		auto& lightingSystem = Zgine::LightingSystem::GetInstance();

		// 1. Directional Light (Sun) - Simulate sunlight
		auto sunLight = std::make_shared<Zgine::DirectionalLight>(
			glm::vec3(-0.5f, -1.0f, -0.3f), // Direction (from top-right)
			glm::vec3(1.0f, 0.95f, 0.8f),   // Warm sunlight color
			1.2f                             // Intensity
		);
		lightingSystem.AddLight(sunLight);

		// 2. Point Light - Simulate light bulb
		auto pointLight1 = std::make_shared<Zgine::PointLight>(
			glm::vec3(2.0f, 2.0f, 2.0f),    // Position
			glm::vec3(1.0f, 0.8f, 0.6f),    // Warm color
			1.5f,                           // Intensity
			8.0f                            // Range
		);
		lightingSystem.AddLight(pointLight1);

		auto pointLight2 = std::make_shared<Zgine::PointLight>(
			glm::vec3(-3.0f, 1.0f, -2.0f),  // Position
			glm::vec3(0.6f, 0.8f, 1.0f),    // Cool color
			1.0f,                           // Intensity
			6.0f                            // Range
		);
		lightingSystem.AddLight(pointLight2);

		// 3. Spot Light - Simulate flashlight
		auto spotLight = std::make_shared<Zgine::SpotLight>(
			glm::vec3(0.0f, 5.0f, 0.0f),    // Position
			glm::vec3(0.0f, -1.0f, 0.0f),   // Direction (downward)
			glm::vec3(1.0f, 1.0f, 0.9f),    // Color
			2.0f,                           // Intensity
			10.0f,                          // Range
			15.0f,                          // Inner cone angle
			25.0f                           // Outer cone angle
		);
		lightingSystem.AddLight(spotLight);

		// Set ambient light
		lightingSystem.SetAmbientLighting(glm::vec3(0.1f, 0.1f, 0.15f), 0.2f);

		// ===== Test Advanced Material System =====
		auto& materialLibrary = Zgine::MaterialLibrary::GetInstance();

		// Create different types of materials
		auto defaultMaterial = materialLibrary.CreateDefaultMaterial();
		auto metallicMaterial = materialLibrary.CreateMetallicMaterial();
		auto glassMaterial = materialLibrary.CreateGlassMaterial();
		auto emissiveMaterial = materialLibrary.CreateEmissiveMaterial();

		// ===== Test Resource Management System =====
		auto& resourceManager = Zgine::ResourceManager::GetInstance();

		// Create some test materials
		auto testMaterial1 = resourceManager.CreateMaterial("TestMaterial1");
		testMaterial1->SetAlbedo(glm::vec3(0.8f, 0.2f, 0.3f));
		testMaterial1->SetMetallic(0.8f);
		testMaterial1->SetRoughness(0.2f);

		auto testMaterial2 = resourceManager.CreateMaterial("TestMaterial2");
		testMaterial2->SetAlbedo(glm::vec3(0.2f, 0.8f, 0.3f));
		testMaterial2->SetMetallic(0.1f);
		testMaterial2->SetRoughness(0.8f);
		testMaterial2->SetEmissive(0.5f);
		testMaterial2->SetEmissiveColor(glm::vec3(0.0f, 1.0f, 0.0f));

		ZG_CORE_INFO("=== Zgine Engine Core Systems Test Initialization Complete ===");
		ZG_CORE_INFO("2D Camera Position: ({}, {}, {})",
			m_2DCameraPosition.x, m_2DCameraPosition.y, m_2DCameraPosition.z);
		ZG_CORE_INFO("3D Camera Position: ({}, {}, {})",
			m_3DCameraPosition.x, m_3DCameraPosition.y, m_3DCameraPosition.z);
		ZG_CORE_INFO("Lighting System: {} lights", lightingSystem.GetLightCount());
		ZG_CORE_INFO("Material Library: {} materials", materialLibrary.GetMaterialCount());
		ZG_CORE_INFO("Resource Manager: {} materials", resourceManager.GetMaterialCount());
		ZG_CORE_INFO("=== Starting Rendering Tests ===");

		// Initialize particle systems
		InitializeParticleSystems();
	}

	void MainControlLayer::OnEvent(Zgine::Event& e)
	{
		Zgine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Zgine::WindowResizeEvent>([this](Zgine::WindowResizeEvent& e) { return OnWindowResize(e); });
	}

	bool MainControlLayer::OnWindowResize(Zgine::WindowResizeEvent& e)
	{
		// Update 3D camera aspect ratio when window is resized
		float aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_3DCamera.SetAspectRatio(aspectRatio);

		// Update 2D camera projection to maintain aspect ratio
		float width = (float)e.GetWidth();
		float height = (float)e.GetHeight();
		float aspect = width / height;

		// Maintain the same scale but adjust for aspect ratio
		float left = -aspect;
		float right = aspect;
		float bottom = -1.0f;
		float top = 1.0f;

		m_2DCamera.SetProjection(left, right, bottom, top);

		ZG_CORE_INFO("Window resized to {}x{}, aspect ratio: {}", e.GetWidth(), e.GetHeight(), aspectRatio);
		return false; // Don't mark as handled, let other layers process it too
	}

	void MainControlLayer::OnUpdate(Zgine::Timestep ts)
	{
		// Debug output
		static int frameCount = 0;
		frameCount++;
		if (frameCount % 60 == 0) // Every 60 frames
		{
			ZG_CORE_INFO("MainControlLayer::OnUpdate called - Frame: {}", frameCount);
		}

		// Update time
		m_Time += ts;

		// Update FPS
		m_FrameCount++;
		m_FPSTimer += ts;
		if (m_FPSTimer >= 1.0f)
		{
			m_FPS = m_FrameCount / m_FPSTimer;
			m_FrameCount = 0;
			m_FPSTimer = 0.0f;
		}

		// Update cameras
		Update2DCamera(ts);
		Update3DCamera(ts);

		// Update particle systems
		UpdateParticleSystems(ts);

		// Render 2D scene if window is open
		if (m_Show2DTestWindow)
		{
			try
			{
				if (!Zgine::BatchRenderer2D::IsInitialized())
				{
					ZG_CORE_ERROR("BatchRenderer2D is not initialized!");
					return;
				}

				ZG_CORE_TRACE("MainControlLayer::OnUpdate - Starting 2D rendering");
				Zgine::BatchRenderer2D::BeginScene(m_2DCamera);

				// Render basic shapes based on UI state
				ZG_CORE_TRACE("MainControlLayer::OnUpdate - Rendering 2D basic shapes");
				Render2DBasicShapes();

				if (m_2DShowAdvanced)
				{
					ZG_CORE_TRACE("MainControlLayer::OnUpdate - Rendering 2D advanced shapes");
					Render2DAdvancedShapes();
				}

				if (m_2DAnimateCircles || m_2DAnimateQuads)
				{
					ZG_CORE_TRACE("MainControlLayer::OnUpdate - Rendering 2D animated shapes");
					Render2DAnimatedShapes();
				}

				ZG_CORE_TRACE("MainControlLayer::OnUpdate - Ending 2D rendering");
				Zgine::BatchRenderer2D::EndScene();
			}
			catch (const std::exception& e)
			{
				ZG_CORE_ERROR("2D Rendering error: {}", e.what());
			}
			catch (...)
			{
				ZG_CORE_ERROR("Unknown 2D rendering error");
			}
		}

		// Render 3D scene if window is open
		if (m_Show3DTestWindow)
		{
			try
			{
				if (!Zgine::BatchRenderer3D::IsInitialized())
				{
					ZG_CORE_ERROR("BatchRenderer3D is not initialized!");
					return;
				}

				Zgine::BatchRenderer3D::BeginScene(m_3DCamera);

				// Render basic shapes based on UI state
				if (m_3DShowCubes || m_3DShowSpheres || m_3DShowPlanes)
					Render3DBasicShapes();

				if (m_3DAnimateObjects)
					Render3DAnimatedShapes();

				if (m_3DShowEnvironment)
					Render3DEnvironment();

				Zgine::BatchRenderer3D::EndScene();
			}
			catch (const std::exception& e)
			{
				ZG_CORE_ERROR("3D Rendering error: {}", e.what());
			}
			catch (...)
			{
				ZG_CORE_ERROR("Unknown 3D rendering error");
			}
		}

		// Render particle systems
		RenderParticleSystems();
	}

	void MainControlLayer::OnImGuiRender()
	{
		// Debug output
		static int imguiFrameCount = 0;
		imguiFrameCount++;
		// if (imguiFrameCount % 60 == 0) // Every 60 frames
		// {
		//	ZG_CORE_INFO("MainControlLayer::OnImGuiRender called - Frame: {}", imguiFrameCount);
		// }

		// Main menu bar
		RenderMainMenu();

		// Test windows
		if (m_Show2DTestWindow)
			Render2DTestWindow();

		if (m_Show3DTestWindow)
			Render3DTestWindow();

		if (m_ShowParticleSystem)
			RenderParticleSystemWindow();

		if (m_ShowPerformanceWindow)
			RenderPerformanceWindow();

		if (m_ShowSettingsWindow)
			RenderSettingsWindow();
	}

	void MainControlLayer::RenderMainMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// File Menu
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					// TODO: Implement new scene
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
				{
					// TODO: Implement open scene
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{
					// TODO: Implement save scene
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt+F4"))
				{
					// Close the application by posting a window close event
					// This will be handled by the application's event system
					Zgine::WindowCloseEvent closeEvent;
					Zgine::Application::Get().OnEvent(closeEvent);
				}
				ImGui::EndMenu();
			}

			// View Menu
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("2D Rendering Test", nullptr, &m_Show2DTestWindow);
				ImGui::MenuItem("3D Rendering Test", nullptr, &m_Show3DTestWindow);
				ImGui::MenuItem("Particle Systems", nullptr, &m_ShowParticleSystem);
				ImGui::MenuItem("Performance Monitor", nullptr, &m_ShowPerformanceWindow);
				ImGui::MenuItem("Settings", nullptr, &m_ShowSettingsWindow);
				ImGui::EndMenu();
			}

			// Tools Menu
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Reset Camera"))
				{
					// Reset cameras to default positions
					m_2DCameraPosition = { 0.0f, 0.0f, 0.0f };
					m_3DCameraPosition = { 0.0f, 2.0f, 5.0f };
					m_3DCameraRotation = { 0.0f, 0.0f, 0.0f };
					m_2DCamera.SetPosition(m_2DCameraPosition);
					m_3DCamera.SetPosition(m_3DCameraPosition);
					m_3DCamera.SetRotation(m_3DCameraRotation);
				}
				if (ImGui::MenuItem("Clear All Particles"))
				{
					Zgine::ParticleSystemManager::ClearAll();
				}
				ImGui::EndMenu();
			}

			// Help Menu
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About Zgine Engine"))
				{
					// TODO: Show about dialog
				}
				if (ImGui::MenuItem("Documentation"))
				{
					// TODO: Open documentation
				}
				ImGui::EndMenu();
			}

			// Status Bar
			ImGui::SameLine(ImGui::GetWindowWidth() - 200);
			ImGui::Text("FPS: %.1f | Objects: %d", m_FPS, GetTotalObjectCount());

			ImGui::EndMainMenuBar();
		}
	}

	int MainControlLayer::GetTotalObjectCount() const
	{
		int count = 0;

		// Count 2D objects
		if (m_2DShowQuads) count += 7; // Main quad + rotated quad + 5 small quads
		if (m_2DShowLines) count += 10; // Grid pattern
		if (m_2DShowCircles) count += 3; // Basic circles
		if (m_2DShowAdvanced)
		{
			if (m_2DShowTriangles) count += 1;
			if (m_2DShowEllipses) count += 1;
			if (m_2DShowArcs) count += 1;
			if (m_2DShowGradients) count += 2;
		}
		if (m_2DAnimateCircles) count += 5; // Animated circles
		if (m_2DAnimateQuads) count += 3; // Animated quads

		// Count 3D objects
		if (m_3DShowCubes) count += 8; // Main cube + tall cube + small cube + 5 floating cubes
		if (m_3DShowSpheres) count += 6; // Main sphere + 5 floating spheres
		if (m_3DShowCylinders) count += 8; // Main cylinder + glass cylinder + emissive cylinder + 4 rotating + 3 stacked
		if (m_3DShowPlanes) count += 3; // Environment planes

		// Count particle systems
		if (m_ParticleSystemEnabled)
		{
			if (m_FireParticleSystem) count += m_FireParticleSystem->GetActiveParticleCount();
			if (m_SmokeParticleSystem) count += m_SmokeParticleSystem->GetActiveParticleCount();
			if (m_ExplosionParticleSystem) count += m_ExplosionParticleSystem->GetActiveParticleCount();
		}

		return count;
	}

	void MainControlLayer::Render2DTestWindow()
	{
		ImGui::Begin("2D Rendering Test", &m_Show2DTestWindow);

		// Debug info
		ImGui::Text("Debug: 2D Test Window is open");
		ImGui::Text("Show2DTestWindow: %s", m_Show2DTestWindow ? "true" : "false");
		ImGui::Text("ShowQuads: %s", m_2DShowQuads ? "true" : "false");
		ImGui::Text("Renderer2D Initialized: %s", Zgine::BatchRenderer2D::IsInitialized() ? "true" : "false");

		// Basic shapes
		ImGui::Text("Basic Shapes");
		ImGui::Checkbox("Quads", &m_2DShowQuads);
		ImGui::SameLine();
		ImGui::Checkbox("Lines", &m_2DShowLines);
		ImGui::SameLine();
		ImGui::Checkbox("Circles", &m_2DShowCircles);

		ImGui::Separator();

		// Advanced shapes
		ImGui::Text("Advanced Shapes");
		ImGui::Checkbox("Show Advanced", &m_2DShowAdvanced);
		if (m_2DShowAdvanced)
		{
			ImGui::Indent();
			ImGui::Checkbox("Triangles", &m_2DShowTriangles);
			ImGui::SameLine();
			ImGui::Checkbox("Ellipses", &m_2DShowEllipses);
			ImGui::Checkbox("Arcs", &m_2DShowArcs);
			ImGui::SameLine();
			ImGui::Checkbox("Gradients", &m_2DShowGradients);
			ImGui::Unindent();
		}

		ImGui::Separator();

		// Animation
		ImGui::Text("Animation");
		ImGui::Checkbox("Animate Circles", &m_2DAnimateCircles);
		ImGui::SameLine();
		ImGui::Checkbox("Animate Quads", &m_2DAnimateQuads);
		ImGui::SliderFloat("Animation Speed", &m_2DAnimationSpeed, 0.1f, 5.0f);

		ImGui::Separator();

		// Settings
		ImGui::Text("Settings");
		ImGui::SliderFloat("Line Thickness", &m_2DLineThickness, 0.001f, 0.1f);
		ImGui::SliderFloat("Circle Radius", &m_2DCircleRadius, 0.1f, 2.0f);
		ImGui::SliderInt("Circle Segments", &m_2DCircleSegments, 8, 64);

		ImGui::Separator();

		// Camera controls
		ImGui::Text("Camera Controls");
		ImGui::SliderFloat("Camera Speed", &m_2DCameraSpeed, 0.1f, 10.0f);
		ImGui::Text("Use WASD to move camera");

		ImGui::End();
	}

	void MainControlLayer::Render3DTestWindow()
	{
		ImGui::Begin("3D Rendering Test", &m_Show3DTestWindow);

		// Debug info
		ImGui::Text("Debug: 3D Test Window is open");
		ImGui::Text("Show3DTestWindow: %s", m_Show3DTestWindow ? "true" : "false");
		ImGui::Text("Renderer3D Initialized: %s", Zgine::BatchRenderer3D::IsInitialized() ? "true" : "false");
		ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", m_3DCameraPosition.x, m_3DCameraPosition.y, m_3DCameraPosition.z);

		// Basic shapes
		ImGui::Text("Basic Shapes");
		ImGui::Checkbox("Cubes", &m_3DShowCubes);
		ImGui::SameLine();
		ImGui::Checkbox("Spheres", &m_3DShowSpheres);
		ImGui::SameLine();
		ImGui::Checkbox("Cylinders", &m_3DShowCylinders);
		ImGui::SameLine();
		ImGui::Checkbox("Planes", &m_3DShowPlanes);

		ImGui::Separator();

		// Environment
		ImGui::Text("Environment");
		ImGui::Checkbox("Show Environment", &m_3DShowEnvironment);
		ImGui::Checkbox("Wireframe Mode", &m_3DWireframeMode);

		ImGui::Separator();

		// Animation
		ImGui::Text("Animation");
		ImGui::Checkbox("Animate Objects", &m_3DAnimateObjects);

		ImGui::Separator();

		// Lighting
		ImGui::Text("Lighting");
		ImGui::SliderFloat("Light Intensity", &m_3DLightIntensity, 0.1f, 3.0f);
		ImGui::SliderFloat3("Light Position", &m_3DLightPosition.x, -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", &m_3DLightColor.x);

		ImGui::Separator();

		// Camera controls
		ImGui::Text("Camera Controls");
		ImGui::SliderFloat("Camera Speed", &m_3DCameraSpeed, 0.1f, 20.0f);
		ImGui::SliderFloat("Rotation Speed", &m_3DRotationSpeed, 0.1f, 5.0f);
		ImGui::Text("Use WASD to move, Mouse to look around");

		ImGui::End();
	}

	void MainControlLayer::RenderPerformanceWindow()
	{
		ImGui::Begin("Zgine Engine Core Systems Test", &m_ShowPerformanceWindow);

		// ===== System Status Monitor =====
		ImGui::Text("=== System Status Monitor ===");
		ImGui::Separator();

		// Renderer status
		ImGui::Text("Renderer Status:");
		ImGui::Text("  2D Renderer: %s", Zgine::BatchRenderer2D::IsInitialized() ? "✓ Initialized" : "✗ Not Initialized");
		ImGui::Text("  3D Renderer: %s", Zgine::BatchRenderer3D::IsInitialized() ? "✓ Initialized" : "✗ Not Initialized");

		// Lighting system status
		auto& lightingSystem = Zgine::LightingSystem::GetInstance();
		ImGui::Text("Lighting System:");
		ImGui::Text("  Light Count: %d", lightingSystem.GetLightCount());
		ImGui::Text("  Ambient Intensity: %.2f", lightingSystem.GetAmbientIntensity());

		// Material system status
		auto& materialLibrary = Zgine::MaterialLibrary::GetInstance();
		ImGui::Text("Material System:");
		ImGui::Text("  Material Count: %d", materialLibrary.GetMaterialCount());

		// Resource manager status
		auto& resourceManager = Zgine::ResourceManager::GetInstance();
		ImGui::Text("Resource Manager:");
		ImGui::Text("  Material Count: %d", resourceManager.GetMaterialCount());
		ImGui::Text("  Texture Count: %d", resourceManager.GetTextureCount());
		ImGui::Text("  Shader Count: %d", resourceManager.GetShaderCount());

		ImGui::Separator();

		// ===== PBR Rendering Test Controls =====
		ImGui::Text("=== PBR Rendering Test ===");
		ImGui::Checkbox("Enable PBR Material Test", &m_3DAnimateObjects);
		ImGui::Checkbox("Show Metallic Cubes", &m_3DShowCubes);
		ImGui::Checkbox("Show Glass Spheres", &m_3DShowSpheres);
		ImGui::Checkbox("Show Environment Planes", &m_3DShowPlanes);

		ImGui::Separator();

		// ===== Lighting Test Controls =====
		ImGui::Text("=== Lighting Test ===");
		static float ambientIntensity = 0.2f;
		if (ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f))
		{
			lightingSystem.SetAmbientLighting(glm::vec3(0.1f, 0.1f, 0.15f), ambientIntensity);
		}

		ImGui::Separator();

		// ===== Performance Monitor =====
		ImGui::Text("=== Performance Monitor ===");
		ImGui::Text("FPS: %.1f", m_FPS);
		ImGui::Text("Runtime: %.2f seconds", m_Time);

		// 2D Stats
		if (m_Show2DTestWindow)
		{
			auto stats2D = Zgine::BatchRenderer2D::GetStats();
			ImGui::Text("2D Render Stats:");
			ImGui::Text("  Draw Calls: %d", stats2D.DrawCalls);
			ImGui::Text("  Quads: %d", stats2D.QuadCount);
			ImGui::Text("  Vertices: %d", stats2D.GetTotalVertexCount());
			ImGui::Text("  Indices: %d", stats2D.GetTotalIndexCount());
		}

		ImGui::Separator();

		// 3D Stats
		if (m_Show3DTestWindow)
		{
			auto stats3D = Zgine::BatchRenderer3D::GetStats();
			ImGui::Text("3D Render Stats:");
			ImGui::Text("  Draw Calls: %d", stats3D.DrawCalls);
			ImGui::Text("  Cubes: %d", stats3D.CubeCount);
			ImGui::Text("  Spheres: %d", stats3D.SphereCount);
			ImGui::Text("  Cylinders: %d", stats3D.CylinderCount);
			ImGui::Text("  Planes: %d", stats3D.PlaneCount);
		}

		ImGui::Separator();

		// Reset buttons
		if (ImGui::Button("Reset 2D Stats"))
			Zgine::BatchRenderer2D::ResetStats();

		ImGui::SameLine();
		if (ImGui::Button("Reset 3D Stats"))
			Zgine::BatchRenderer3D::ResetStats();

		ImGui::End();
	}

	void MainControlLayer::RenderSettingsWindow()
	{
		ImGui::Begin("Settings", &m_ShowSettingsWindow);

		ImGui::Text("Application Settings");
		ImGui::Separator();

		// Window management
		ImGui::Text("Window Management");
		if (ImGui::Button("Open All Windows"))
		{
			m_Show2DTestWindow = true;
			m_Show3DTestWindow = true;
			m_ShowPerformanceWindow = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close All Windows"))
		{
			m_Show2DTestWindow = false;
			m_Show3DTestWindow = false;
			m_ShowPerformanceWindow = false;
		}

		ImGui::Separator();

		// Default settings
		ImGui::Text("Default Settings");
		if (ImGui::Button("Reset to Defaults"))
		{
			// Reset 2D settings
			m_2DShowQuads = true;
			m_2DShowLines = true;
			m_2DShowCircles = true;
			m_2DShowAdvanced = false;
			m_2DAnimateCircles = true;
			m_2DAnimateQuads = true;
			m_2DAnimationSpeed = 1.0f;
			m_2DLineThickness = 0.02f;
			m_2DCircleRadius = 0.5f;
			m_2DCircleSegments = 32;

			// Reset 3D settings
			m_3DShowCubes = true;
			m_3DShowSpheres = true;
			m_3DShowCylinders = true;
			m_3DShowPlanes = true;
			m_3DShowEnvironment = false;
			m_3DAnimateObjects = true;
			m_3DWireframeMode = false;
			m_3DLightIntensity = 1.0f;
			m_3DLightPosition = glm::vec3(2.0f, 2.0f, 2.0f);
			m_3DLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		ImGui::End();
	}

	void MainControlLayer::Update2DCamera(Zgine::Timestep ts)
	{
		// Use arrow keys for 2D camera movement to avoid conflict with 3D camera
		if (Zgine::Input::IsKeyPressed(ZG_KEY_LEFT))
			m_2DCameraPosition.x -= m_2DCameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_RIGHT))
			m_2DCameraPosition.x += m_2DCameraSpeed * ts;

		if (Zgine::Input::IsKeyPressed(ZG_KEY_UP))
			m_2DCameraPosition.y += m_2DCameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_DOWN))
			m_2DCameraPosition.y -= m_2DCameraSpeed * ts;

		m_2DCamera.SetPosition(m_2DCameraPosition);
	}

	void MainControlLayer::Update3DCamera(Zgine::Timestep ts)
	{
		// Camera movement
		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_3DCamera.MoveForward(m_3DCameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_3DCamera.MoveForward(-m_3DCameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_3DCamera.MoveRight(-m_3DCameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_3DCamera.MoveRight(m_3DCameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_Q))
			m_3DCamera.MoveUp(m_3DCameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_E))
			m_3DCamera.MoveUp(-m_3DCameraSpeed * ts);

		// Mouse look
		if (Zgine::Input::IsMouseButtonPressed(ZG_MOUSE_BUTTON_RIGHT))
		{
			auto mousePos = Zgine::Input::GetMousePosition();
			static glm::vec2 lastMousePos = glm::vec2(mousePos.first, mousePos.second);
			glm::vec2 currentMousePos = glm::vec2(mousePos.first, mousePos.second);

			float deltaX = currentMousePos.x - lastMousePos.x;
			float deltaY = lastMousePos.y - currentMousePos.y; // Reversed for natural look

			m_3DCamera.Rotate(deltaX * m_3DRotationSpeed * ts, deltaY * m_3DRotationSpeed * ts);

			lastMousePos = currentMousePos;
		}
	}

	void MainControlLayer::Render2DBasicShapes()
	{
		ZG_CORE_TRACE("MainControlLayer::Render2DBasicShapes - Starting 2D basic shapes rendering");

		// Enhanced 2D rendering with Godot4-style effects

		// Basic quads with different colors and effects
		if (m_2DShowQuads)
		{
			ZG_CORE_TRACE("MainControlLayer::Render2DBasicShapes - Rendering quads");

			// Main quad with gradient effect
			Zgine::BatchRenderer2D::DrawQuadGradient(
				{ -1.0f, 0.0f, 0.0f }, { 0.8f, 0.8f },
				{ 0.8f, 0.2f, 0.3f, 1.0f }, // Top-left: Red
				{ 0.2f, 0.8f, 0.3f, 1.0f }, // Top-right: Green
				{ 0.2f, 0.3f, 0.8f, 1.0f }, // Bottom-left: Blue
				{ 0.8f, 0.8f, 0.2f, 1.0f }  // Bottom-right: Yellow
			);

			// Rotated quad with smooth rotation
			float rotation = m_Time * 0.5f; // Slow rotation
			Zgine::BatchRenderer2D::DrawRotatedQuad(
				{ 0.5f, -0.5f, 0.0f }, { 0.5f, 0.75f },
				rotation, { 0.2f, 0.3f, 0.8f, 1.0f }
			);

			// Multiple small quads with different effects
			for (int i = 0; i < 5; i++)
			{
				float x = -2.0f + i * 0.8f;
				float y = 1.0f + 0.3f * sin(m_Time + i);
				glm::vec4 color = {
					0.5f + 0.5f * sin(m_Time + i),
					0.5f + 0.5f * cos(m_Time + i * 1.2f),
					0.5f + 0.5f * sin(m_Time + i * 0.8f),
					1.0f
				};
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.3f, 0.3f }, color);
			}
		}

		// Enhanced lines with varying thickness
		if (m_2DShowLines)
		{
			// Grid pattern
			for (int i = -3; i <= 3; i++)
			{
				float pos = i * 0.5f;
				Zgine::BatchRenderer2D::DrawLine(
					{ pos, -1.5f, 0.0f }, { pos, 1.5f, 0.0f },
					{ 0.3f, 0.3f, 0.3f, 0.5f }, 0.02f
				);
				Zgine::BatchRenderer2D::DrawLine(
					{ -1.5f, pos, 0.0f }, { 1.5f, pos, 0.0f },
					{ 0.3f, 0.3f, 0.3f, 0.5f }, 0.02f
				);
			}

			// Animated lines
			for (int i = 0; i < 8; i++)
			{
				float angle = (m_Time + i * 0.5f) * 0.3f;
				float x1 = 0.5f * cos(angle);
				float y1 = 0.5f * sin(angle);
				float x2 = 1.0f * cos(angle + 0.5f);
				float y2 = 1.0f * sin(angle + 0.5f);

				glm::vec4 color = {
					0.8f + 0.2f * sin(m_Time + i),
					0.8f + 0.2f * cos(m_Time + i),
					0.8f + 0.2f * sin(m_Time + i * 1.5f),
					1.0f
				};
				Zgine::BatchRenderer2D::DrawLine(
					{ x1, y1, 0.0f }, { x2, y2, 0.0f },
					color, 0.03f + 0.02f * sin(m_Time + i)
				);
			}
		}

		// Enhanced circles with smooth animation
		if (m_2DShowCircles)
		{
			// Main circle with pulsing effect
			float pulse = 0.5f + 0.3f * sin(m_Time * 2.0f);
			Zgine::BatchRenderer2D::DrawCircle(
				{ 0.0f, 0.0f, 0.0f }, m_2DCircleRadius * pulse,
				{ 0.8f, 0.2f, 0.8f, 1.0f }, m_2DCircleSegments
			);

			// Circle outline with rotation
			float outlineRadius = 0.8f + 0.2f * sin(m_Time * 1.5f);
			Zgine::BatchRenderer2D::DrawCircleOutline(
				{ 0.0f, 0.0f, 0.0f }, outlineRadius,
				{ 0.2f, 0.8f, 0.8f, 1.0f }, 0.05f, 32
			);

			// Multiple small circles
			for (int i = 0; i < 6; i++)
			{
				float angle = (m_Time + i) * 0.8f;
				float radius = 1.2f;
				float x = radius * cos(angle);
				float y = radius * sin(angle);
				float size = 0.1f + 0.05f * sin(m_Time * 3.0f + i);

				glm::vec4 color = {
					0.5f + 0.5f * sin(m_Time + i),
					0.5f + 0.5f * cos(m_Time + i * 1.3f),
					0.5f + 0.5f * sin(m_Time + i * 0.7f),
					1.0f
				};
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, size, color, 16);
			}
		}
	}

	void MainControlLayer::Render2DAdvancedShapes()
	{
		// Triangles
		if (m_2DShowTriangles)
		{
			Zgine::BatchRenderer2D::DrawTriangle({ -1.5f, 0.0f, 0.0f }, { -0.5f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.5f, 0.0f, 1.0f });
		}

		// Ellipses
		if (m_2DShowEllipses)
		{
			Zgine::BatchRenderer2D::DrawEllipse({ 1.0f, 0.0f, 0.0f }, 0.6f, 0.3f, { 0.0f, 0.5f, 1.0f, 1.0f });
		}

		// Arcs
		if (m_2DShowArcs)
		{
			Zgine::BatchRenderer2D::DrawArc({ 0.0f, 1.0f, 0.0f }, 0.4f, 0.0f, 3.14159f, { 1.0f, 0.0f, 0.0f, 1.0f }, m_2DLineThickness);
		}

		// Gradients
		if (m_2DShowGradients)
		{
			Zgine::BatchRenderer2D::DrawQuadGradient(glm::vec3(-1.0f, 1.5f, 0.0f), glm::vec2(0.4f, 0.4f),
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

			Zgine::BatchRenderer2D::DrawRotatedQuadGradient(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec2(0.4f, 0.4f), 45.0f,
				glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
				glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}

	void MainControlLayer::Render2DAnimatedShapes()
	{
		float time = m_Time * m_2DAnimationSpeed;

		// Animated circles
		if (m_2DAnimateCircles)
		{
			for (int i = 0; i < 5; i++)
			{
				float angle = time + i * 1.256f; // 72 degrees apart
				float radius = 0.3f + i * 0.1f;
				glm::vec3 pos = { cos(angle) * radius, sin(angle) * radius, 0.0f };
				Zgine::BatchRenderer2D::DrawCircle(pos, 0.1f, { 0.8f, 0.2f, 0.8f, 1.0f }, m_2DCircleSegments);
			}
		}

		// Animated quads
		if (m_2DAnimateQuads)
		{
			for (int i = 0; i < 3; i++)
			{
				float scale = 0.5f + 0.3f * sin(time + i);
				glm::vec3 pos = { -0.8f + i * 0.8f, 0.5f + 0.2f * cos(time + i), 0.0f };
				Zgine::BatchRenderer2D::DrawQuad(pos, { scale, scale }, { 0.2f, 0.8f, 0.8f, 1.0f });
			}
		}
	}

	void MainControlLayer::Render3DBasicShapes()
	{
		// Enhanced 3D rendering with Unreal Engine4-style effects

		// Cubes with advanced materials and lighting
		if (m_3DShowCubes)
		{
			// Main cube with metallic material
			glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }) *
				glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
			Zgine::BatchRenderer3D::DrawCube({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, transform1, { 0.8f, 0.2f, 0.3f, 1.0f });

			// Tall cube with glass-like material
			glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), { 3.0f, 0.0f, 0.0f }) *
				glm::scale(glm::mat4(1.0f), { 0.8f, 1.5f, 0.8f });
			Zgine::BatchRenderer3D::DrawCube({ 3.0f, 0.0f, 0.0f }, { 0.8f, 1.5f, 0.8f }, transform2, { 0.2f, 0.8f, 0.3f, 0.8f });

			// Small cube with emissive material
			glm::mat4 transform3 = glm::translate(glm::mat4(1.0f), { -3.0f, 0.0f, 0.0f }) *
				glm::scale(glm::mat4(1.0f), { 0.6f, 0.6f, 0.6f });
			Zgine::BatchRenderer3D::DrawCube({ -3.0f, 0.0f, 0.0f }, { 0.6f, 0.6f, 0.6f }, transform3, { 1.0f, 0.8f, 0.2f, 1.0f });

			// Floating cubes with different materials
			for (int i = 0; i < 4; i++)
			{
				float x = -6.0f + i * 4.0f;
				float y = 2.0f + 0.5f * sin(m_Time + i * 0.8f);
				float z = 2.0f * cos(m_Time + i * 0.6f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), { x, y, z }) *
					glm::scale(glm::mat4(1.0f), { 0.4f, 0.4f, 0.4f });

				glm::vec4 color = {
					0.5f + 0.5f * sin(m_Time + i),
					0.5f + 0.5f * cos(m_Time + i * 1.2f),
					0.5f + 0.5f * sin(m_Time + i * 0.8f),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawCube({ x, y, z }, { 0.4f, 0.4f, 0.4f }, transform, color);
			}
		}

		// Spheres with realistic materials
		if (m_3DShowSpheres)
		{
			// Main sphere with metallic finish
			Zgine::BatchRenderer3D::DrawSphere({ 0.0f, 2.0f, 0.0f }, 0.8f, { 0.7f, 0.7f, 0.8f, 1.0f }, 24);

			// Glass sphere
			Zgine::BatchRenderer3D::DrawSphere({ 2.0f, 1.0f, 2.0f }, 0.6f, { 0.2f, 0.8f, 0.9f, 0.6f }, 20);

			// Emissive sphere
			Zgine::BatchRenderer3D::DrawSphere({ -2.0f, 1.0f, -2.0f }, 0.5f, { 1.0f, 0.3f, 0.3f, 1.0f }, 16);

			// Orbiting spheres
			for (int i = 0; i < 6; i++)
			{
				float angle = m_Time * 0.5f + i * 1.047f; // 60 degrees apart
				float radius = 4.0f;
				float x = radius * cos(angle);
				float z = radius * sin(angle);
				float y = 1.0f + 0.3f * sin(m_Time * 2.0f + i);

				glm::vec4 color = {
					0.3f + 0.7f * sin(m_Time + i),
					0.3f + 0.7f * cos(m_Time + i * 1.3f),
					0.3f + 0.7f * sin(m_Time + i * 0.7f),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawSphere({ x, y, z }, 0.3f, color, 16);
			}
		}

		// Cylinders with different materials and sizes
		if (m_3DShowCylinders)
		{
			// Main cylinder with metallic finish
			Zgine::BatchRenderer3D::DrawCylinder({ 0.0f, 0.0f, 3.0f }, 0.5f, 2.0f, { 0.7f, 0.7f, 0.8f, 1.0f }, 24);

			// Glass cylinder
			Zgine::BatchRenderer3D::DrawCylinder({ 2.0f, 0.0f, 3.0f }, 0.3f, 1.5f, { 0.2f, 0.8f, 0.9f, 0.6f }, 20);

			// Emissive cylinder
			Zgine::BatchRenderer3D::DrawCylinder({ -2.0f, 0.0f, 3.0f }, 0.4f, 1.8f, { 1.0f, 0.3f, 0.3f, 1.0f }, 16);

			// Rotating cylinders
			for (int i = 0; i < 4; i++)
			{
				float angle = m_Time * 0.3f + i * 1.57f; // 90 degrees apart
				float radius = 5.0f;
				float x = radius * cos(angle);
				float z = 3.0f + radius * sin(angle);
				float y = 0.5f + 0.2f * sin(m_Time * 1.5f + i);

				glm::vec4 color = {
					0.4f + 0.6f * sin(m_Time + i),
					0.4f + 0.6f * cos(m_Time + i * 1.2f),
					0.4f + 0.6f * sin(m_Time + i * 0.8f),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawCylinder({ x, y, z }, 0.2f, 1.0f, color, 16);
			}

			// Stacked cylinders
			for (int i = 0; i < 3; i++)
			{
				float y = -1.0f + i * 0.8f;
				float radius = 0.3f - i * 0.05f;
				float height = 0.6f;
				
				glm::vec4 color = {
					0.8f - i * 0.2f,
					0.6f + i * 0.1f,
					0.4f + i * 0.2f,
					1.0f
				};
				Zgine::BatchRenderer3D::DrawCylinder({ 4.0f, y, 0.0f }, radius, height, color, 20);
			}
		}

		// Enhanced ground plane with grid pattern
		if (m_3DShowPlanes)
		{
			// Main ground plane
			Zgine::BatchRenderer3D::DrawPlane({ 0.0f, -2.0f, 0.0f }, { 20.0f, 20.0f }, { 0.2f, 0.2f, 0.2f, 1.0f });

			// Additional planes for environment
			Zgine::BatchRenderer3D::DrawPlane({ 0.0f, 5.0f, 0.0f }, { 20.0f, 20.0f }, { 0.1f, 0.1f, 0.2f, 0.8f }); // Sky
			Zgine::BatchRenderer3D::DrawPlane({ -10.0f, 0.0f, 0.0f }, { 20.0f, 20.0f }, { 0.2f, 0.1f, 0.1f, 0.6f }); // Left wall
			Zgine::BatchRenderer3D::DrawPlane({ 10.0f, 0.0f, 0.0f }, { 20.0f, 20.0f }, { 0.1f, 0.2f, 0.1f, 0.6f }); // Right wall
		}
	}

	void MainControlLayer::Render3DAnimatedShapes()
	{
		if (!m_3DAnimateObjects)
			return;

		float time = m_Time;

		// ===== PBR Material Test - Different Material Cubes =====
		if (m_3DShowCubes)
		{
			// 1. Metallic material cube - High reflection
			glm::mat4 metallicRotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 metallicTransform = glm::translate(glm::mat4(1.0f), { -2.0f, 2.0f, 0.0f }) * metallicRotation;
			Zgine::BatchRenderer3D::DrawCube({ -2.0f, 2.0f, 0.0f }, { 0.6f, 0.6f, 0.6f }, metallicTransform, { 0.8f, 0.8f, 0.9f, 1.0f });

			// 2. Glass material cube - Transparent
			glm::mat4 glassRotation = glm::rotate(glm::mat4(1.0f), time * 0.7f, glm::vec3(1.0f, 0.0f, 1.0f));
			glm::mat4 glassTransform = glm::translate(glm::mat4(1.0f), { 0.0f, 2.0f, 0.0f }) * glassRotation;
			Zgine::BatchRenderer3D::DrawCube({ 0.0f, 2.0f, 0.0f }, { 0.6f, 0.6f, 0.6f }, glassTransform, { 0.2f, 0.8f, 0.9f, 0.6f });

			// 3. Emissive material cube - Self-illuminated
			glm::mat4 emissiveRotation = glm::rotate(glm::mat4(1.0f), time * 1.3f, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 emissiveTransform = glm::translate(glm::mat4(1.0f), { 2.0f, 2.0f, 0.0f }) * emissiveRotation;
			Zgine::BatchRenderer3D::DrawCube({ 2.0f, 2.0f, 0.0f }, { 0.6f, 0.6f, 0.6f }, emissiveTransform, { 1.0f, 0.3f, 0.3f, 1.0f });

			// 4. Dynamic material change test
			float materialTime = time * 0.5f;
			for (int i = 0; i < 3; i++)
			{
				float x = -4.0f + i * 4.0f;
				float y = 0.5f + 0.3f * sin(materialTime + i * 2.0f);

				// Dynamically change material properties
				float metallic = 0.5f + 0.5f * sin(materialTime + i);
				float roughness = 0.5f + 0.5f * cos(materialTime + i * 1.3f);

				glm::vec4 color = {
					0.5f + 0.5f * sin(materialTime + i),
					0.5f + 0.5f * cos(materialTime + i * 1.2f),
					0.5f + 0.5f * sin(materialTime + i * 0.8f),
					1.0f
				};

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), { x, y, 0.0f });
				Zgine::BatchRenderer3D::DrawCube({ x, y, 0.0f }, { 0.4f, 0.4f, 0.4f }, transform, color);
			}
		}

		// ===== Lighting Effect Test - Different Material Spheres =====
		if (m_3DShowSpheres)
		{
			// 1. Metallic sphere - High reflection
			float metallicAngle = time * 0.4f;
			float metallicX = 3.0f * cos(metallicAngle);
			float metallicZ = 3.0f * sin(metallicAngle);
			Zgine::BatchRenderer3D::DrawSphere({ metallicX, 1.5f, metallicZ }, 0.5f, { 0.9f, 0.9f, 0.95f, 1.0f }, 20);

			// 2. Glass sphere - Transparent
			float glassAngle = time * 0.6f + 2.0f;
			float glassX = 3.0f * cos(glassAngle);
			float glassZ = 3.0f * sin(glassAngle);
			Zgine::BatchRenderer3D::DrawSphere({ glassX, 1.5f, glassZ }, 0.4f, { 0.1f, 0.8f, 0.9f, 0.7f }, 18);

			// 3. Emissive sphere - Self-illuminated
			float emissiveAngle = time * 0.8f + 4.0f;
			float emissiveX = 3.0f * cos(emissiveAngle);
			float emissiveZ = 3.0f * sin(emissiveAngle);
			Zgine::BatchRenderer3D::DrawSphere({ emissiveX, 1.5f, emissiveZ }, 0.3f, { 1.0f, 0.2f, 0.2f, 1.0f }, 16);

			// 4. Dynamic lighting response sphere
			for (int i = 0; i < 6; i++)
			{
				float orbitRadius = 2.0f;
				float orbitSpeed = 0.3f + i * 0.1f;
				float orbitAngle = time * orbitSpeed + i * 1.047f; // 60 degrees apart

				float x = orbitRadius * cos(orbitAngle);
				float z = orbitRadius * sin(orbitAngle);
				float y = 0.5f + 0.5f * sin(time * 1.5f + i * 0.8f);

				// Dynamically change material properties based on position
				float distanceFromCenter = sqrt(x * x + z * z);
				float metallic = 0.1f + 0.9f * (distanceFromCenter / 2.0f);
				float roughness = 0.1f + 0.9f * (1.0f - distanceFromCenter / 2.0f);

				glm::vec4 color = {
					0.3f + 0.7f * sin(time + i),
					0.3f + 0.7f * cos(time + i * 1.3f),
					0.3f + 0.7f * sin(time + i * 0.7f),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawSphere({ x, y, z }, 0.25f, color, 14);
			}
		}
	}

	void MainControlLayer::Render3DEnvironment()
	{
		if (!m_3DShowEnvironment)
			return;

		// Ground grid
		for (int i = -5; i <= 5; i++)
		{
			Zgine::BatchRenderer3D::DrawCube({ i * 2.0f, -1.5f, 0.0f }, { 0.1f, 0.1f, 10.0f }, glm::mat4(1.0f), { 0.3f, 0.3f, 0.3f, 1.0f });
			Zgine::BatchRenderer3D::DrawCube({ 0.0f, -1.5f, i * 2.0f }, { 10.0f, 0.1f, 0.1f }, glm::mat4(1.0f), { 0.3f, 0.3f, 0.3f, 1.0f });
		}

		// Light source visualization
		Zgine::BatchRenderer3D::DrawSphere(m_3DLightPosition, 0.2f, glm::vec4(m_3DLightColor, 1.0f), 8);
	}

	// ===== Particle System Implementation =====

	void MainControlLayer::InitializeParticleSystems()
	{
		// Initialize ParticleSystemManager
		Zgine::ParticleSystemManager::Init();

		// Create Fire Particle System
		Zgine::ParticleEmitterConfig fireConfig;
		fireConfig.Position = { 0.0f, -1.0f, 0.0f };
		fireConfig.VelocityMin = { -0.5f, 1.0f, -0.5f };
		fireConfig.VelocityMax = { 0.5f, 3.0f, 0.5f };
		fireConfig.AccelerationMin = { -0.1f, 0.0f, -0.1f };
		fireConfig.AccelerationMax = { 0.1f, 0.0f, 0.1f };
		fireConfig.ColorStart = { 1.0f, 0.3f, 0.0f, 1.0f }; // Orange
		fireConfig.ColorEnd = { 1.0f, 0.0f, 0.0f, 0.0f };   // Red to transparent
		fireConfig.SizeStart = { 0.3f, 0.3f };
		fireConfig.SizeEnd = { 0.1f, 0.1f };
		fireConfig.LifeMin = 1.0f;
		fireConfig.LifeMax = 2.0f;
		fireConfig.EmissionRate = 50.0f;
		fireConfig.MaxParticles = 500;

		m_FireParticleSystem = Zgine::CreateRef<Zgine::ParticleSystem>(fireConfig);
		m_FireParticleSystem->StartEmission();
		Zgine::ParticleSystemManager::AddParticleSystem(m_FireParticleSystem);

		// Create Smoke Particle System
		Zgine::ParticleEmitterConfig smokeConfig;
		smokeConfig.Position = { 0.0f, -0.5f, 0.0f };
		smokeConfig.VelocityMin = { -0.3f, 0.5f, -0.3f };
		smokeConfig.VelocityMax = { 0.3f, 1.5f, 0.3f };
		smokeConfig.AccelerationMin = { -0.05f, 0.0f, -0.05f };
		smokeConfig.AccelerationMax = { 0.05f, 0.0f, 0.05f };
		smokeConfig.ColorStart = { 0.3f, 0.3f, 0.3f, 0.8f }; // Gray
		smokeConfig.ColorEnd = { 0.1f, 0.1f, 0.1f, 0.0f };  // Dark gray to transparent
		smokeConfig.SizeStart = { 0.5f, 0.5f };
		smokeConfig.SizeEnd = { 1.0f, 1.0f };
		smokeConfig.LifeMin = 2.0f;
		smokeConfig.LifeMax = 4.0f;
		smokeConfig.EmissionRate = 20.0f;
		smokeConfig.MaxParticles = 200;

		m_SmokeParticleSystem = Zgine::CreateRef<Zgine::ParticleSystem>(smokeConfig);
		m_SmokeParticleSystem->StartEmission();
		Zgine::ParticleSystemManager::AddParticleSystem(m_SmokeParticleSystem);

		// Create Explosion Particle System (initially inactive)
		Zgine::ParticleEmitterConfig explosionConfig;
		explosionConfig.Position = { 2.0f, 0.0f, 0.0f };
		explosionConfig.VelocityMin = { -2.0f, -2.0f, -2.0f };
		explosionConfig.VelocityMax = { 2.0f, 2.0f, 2.0f };
		explosionConfig.AccelerationMin = { -0.2f, -0.2f, -0.2f };
		explosionConfig.AccelerationMax = { 0.2f, 0.2f, 0.2f };
		explosionConfig.ColorStart = { 1.0f, 1.0f, 0.0f, 1.0f }; // Yellow
		explosionConfig.ColorEnd = { 1.0f, 0.0f, 0.0f, 0.0f };  // Red to transparent
		explosionConfig.SizeStart = { 0.2f, 0.2f };
		explosionConfig.SizeEnd = { 0.05f, 0.05f };
		explosionConfig.LifeMin = 0.5f;
		explosionConfig.LifeMax = 1.0f;
		explosionConfig.EmissionRate = 100.0f;
		explosionConfig.MaxParticles = 100;

		m_ExplosionParticleSystem = Zgine::CreateRef<Zgine::ParticleSystem>(explosionConfig);
		// Don't start emission immediately - will be triggered by UI

		ZG_CORE_INFO("Particle systems initialized successfully");
		ZG_CORE_INFO("Fire particles: {} max", fireConfig.MaxParticles);
		ZG_CORE_INFO("Smoke particles: {} max", smokeConfig.MaxParticles);
		ZG_CORE_INFO("Explosion particles: {} max", explosionConfig.MaxParticles);
	}

	void MainControlLayer::UpdateParticleSystems(Zgine::Timestep ts)
	{
		if (!m_ParticleSystemEnabled)
			return;

		// Update all particle systems
		Zgine::ParticleSystemManager::OnUpdate(ts);

		// Update fire position based on time
		if (m_FireParticleSystem)
		{
			float fireX = 0.5f * sin(m_Time * 0.5f);
			float fireZ = 0.3f * cos(m_Time * 0.3f);
			m_FireParticleSystem->SetPosition({ fireX, -1.0f, fireZ });
		}

		// Update smoke position to follow fire
		if (m_SmokeParticleSystem && m_FireParticleSystem)
		{
			auto firePos = m_FireParticleSystem->GetPosition();
			m_SmokeParticleSystem->SetPosition({ firePos.x, firePos.y + 0.5f, firePos.z });
		}
	}

	void MainControlLayer::RenderParticleSystems()
	{
		if (!m_ParticleSystemEnabled)
			return;

		// Render all particle systems using 2D camera for now
		Zgine::ParticleSystemManager::OnRender(m_2DCamera.GetViewProjectionMatrix());
	}

	void MainControlLayer::RenderParticleSystemWindow()
	{
		ImGui::Begin("Particle System", &m_ShowParticleSystem);

		// Debug info
		ImGui::Text("Particle System Debug Info");
		ImGui::Text("System Enabled: %s", m_ParticleSystemEnabled ? "true" : "false");
		ImGui::Text("Total Active Particles: %d", Zgine::ParticleSystemManager::GetTotalActiveParticleCount());

		if (m_FireParticleSystem)
			ImGui::Text("Fire Particles: %d", m_FireParticleSystem->GetActiveParticleCount());
		if (m_SmokeParticleSystem)
			ImGui::Text("Smoke Particles: %d", m_SmokeParticleSystem->GetActiveParticleCount());
		if (m_ExplosionParticleSystem)
			ImGui::Text("Explosion Particles: %d", m_ExplosionParticleSystem->GetActiveParticleCount());

		ImGui::Separator();

		// Controls
		ImGui::Checkbox("Enable Particle Systems", &m_ParticleSystemEnabled);
		ImGui::SliderFloat("Intensity", &m_ParticleSystemIntensity, 0.1f, 3.0f);

		ImGui::Separator();

		// Fire system controls
		if (ImGui::CollapsingHeader("Fire System"))
		{
			if (m_FireParticleSystem)
			{
				auto& config = m_FireParticleSystem->GetConfig();

				ImGui::Text("Emission Rate: %.1f particles/sec", config.EmissionRate);
				ImGui::Text("Max Particles: %d", config.MaxParticles);
				ImGui::Text("Emitting: %s", m_FireParticleSystem->IsEmitting() ? "Yes" : "No");

				if (ImGui::Button("Toggle Fire"))
				{
					if (m_FireParticleSystem->IsEmitting())
						m_FireParticleSystem->StopEmission();
					else
						m_FireParticleSystem->StartEmission();
				}

				if (ImGui::Button("Fire Burst"))
				{
					m_FireParticleSystem->EmitBurst(50);
				}
			}
		}

		// Smoke system controls
		if (ImGui::CollapsingHeader("Smoke System"))
		{
			if (m_SmokeParticleSystem)
			{
				auto& config = m_SmokeParticleSystem->GetConfig();

				ImGui::Text("Emission Rate: %.1f particles/sec", config.EmissionRate);
				ImGui::Text("Max Particles: %d", config.MaxParticles);
				ImGui::Text("Emitting: %s", m_SmokeParticleSystem->IsEmitting() ? "Yes" : "No");

				if (ImGui::Button("Toggle Smoke"))
				{
					if (m_SmokeParticleSystem->IsEmitting())
						m_SmokeParticleSystem->StopEmission();
					else
						m_SmokeParticleSystem->StartEmission();
				}
			}
		}

		// Explosion system controls
		if (ImGui::CollapsingHeader("Explosion System"))
		{
			if (m_ExplosionParticleSystem)
			{
				auto& config = m_ExplosionParticleSystem->GetConfig();

				ImGui::Text("Emission Rate: %.1f particles/sec", config.EmissionRate);
				ImGui::Text("Max Particles: %d", config.MaxParticles);
				ImGui::Text("Emitting: %s", m_ExplosionParticleSystem->IsEmitting() ? "Yes" : "No");

				if (ImGui::Button("Trigger Explosion"))
				{
					m_ExplosionParticleSystem->EmitBurst(100);
				}

				if (ImGui::Button("Toggle Continuous Explosion"))
				{
					if (m_ExplosionParticleSystem->IsEmitting())
						m_ExplosionParticleSystem->StopEmission();
					else
						m_ExplosionParticleSystem->StartEmission();
				}
			}
		}

		ImGui::Separator();

		// Global controls
		if (ImGui::Button("Clear All Particles"))
		{
			Zgine::ParticleSystemManager::ClearAll();
		}

		ImGui::End();
	}


}