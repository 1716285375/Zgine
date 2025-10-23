#include "MainControlLayer.h"
#include "Zgine/Renderer/Renderer.h"
#include "Zgine/Log.h"
#include "Zgine/Events/ApplicationEvent.h"

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
		m_3DShowPlanes(true),
		m_3DShowEnvironment(false),
		m_3DAnimateObjects(true),
		m_3DWireframeMode(false),
		m_3DLightIntensity(1.0f),
		m_3DLightPosition(2.0f, 2.0f, 2.0f),
		m_3DLightColor(1.0f, 1.0f, 1.0f),
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
		
		ZG_CORE_INFO("MainControlLayer attached - 2D Camera: ({}, {}, {})", 
			m_2DCameraPosition.x, m_2DCameraPosition.y, m_2DCameraPosition.z);
		ZG_CORE_INFO("MainControlLayer attached - 3D Camera: ({}, {}, {})", 
			m_3DCameraPosition.x, m_3DCameraPosition.y, m_3DCameraPosition.z);
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
		// if (frameCount % 60 == 0) // Every 60 frames
		// {
		//	ZG_CORE_INFO("MainControlLayer::OnUpdate called - Frame: {}", frameCount);
		// }

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
				
				Zgine::BatchRenderer2D::BeginScene(m_2DCamera);
				
				// Render basic shapes based on UI state
				Render2DBasicShapes();
				
				if (m_2DShowAdvanced)
					Render2DAdvancedShapes();
				
				if (m_2DAnimateCircles || m_2DAnimateQuads)
					Render2DAnimatedShapes();
				
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
		
		if (m_ShowPerformanceWindow)
			RenderPerformanceWindow();
		
		if (m_ShowSettingsWindow)
			RenderSettingsWindow();
	}

	void MainControlLayer::RenderMainMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// Debug info in menu bar
			ImGui::Text("Debug: MainControlLayer Active");
			
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("2D Test Window", nullptr, &m_Show2DTestWindow);
				ImGui::MenuItem("3D Test Window", nullptr, &m_Show3DTestWindow);
				ImGui::MenuItem("Performance", nullptr, &m_ShowPerformanceWindow);
				ImGui::MenuItem("Settings", nullptr, &m_ShowSettingsWindow);
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Presets"))
			{
				if (ImGui::MenuItem("2D Only"))
				{
					m_Show2DTestWindow = true;
					m_Show3DTestWindow = false;
				}
				if (ImGui::MenuItem("3D Only"))
				{
					m_Show2DTestWindow = false;
					m_Show3DTestWindow = true;
				}
				if (ImGui::MenuItem("Both Windows"))
				{
					m_Show2DTestWindow = true;
					m_Show3DTestWindow = true;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
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
		ImGui::Begin("Performance", &m_ShowPerformanceWindow);
		
		// FPS
		ImGui::Text("FPS: %.1f", m_FPS);
		
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
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_2DCameraPosition.x -= m_2DCameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_2DCameraPosition.x += m_2DCameraSpeed * ts;

		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_2DCameraPosition.y += m_2DCameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
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
		// Enhanced 2D rendering with Godot4-style effects
		
		// Basic quads with different colors and effects
		if (m_2DShowQuads)
		{
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

		// Advanced rotating cube with complex transformations
		if (m_3DShowCubes)
		{
			// Main rotating cube with multiple rotation axes
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f)) *
								 glm::rotate(glm::mat4(1.0f), time * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)) *
								 glm::rotate(glm::mat4(1.0f), time * 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
			
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.0f, 3.0f, 0.0f }) * rotation;
			Zgine::BatchRenderer3D::DrawCube({ 0.0f, 3.0f, 0.0f }, { 0.8f, 0.8f, 0.8f }, transform, { 1.0f, 0.5f, 0.0f, 1.0f });
			
			// Pulsing cube with scale animation
			float scale = 0.5f + 0.3f * sin(time * 2.0f);
			glm::mat4 scaleTransform = glm::translate(glm::mat4(1.0f), { 4.0f, 2.0f, 0.0f }) * 
									   glm::scale(glm::mat4(1.0f), { scale, scale, scale });
			Zgine::BatchRenderer3D::DrawCube({ 4.0f, 2.0f, 0.0f }, { scale, scale, scale }, scaleTransform, { 0.0f, 1.0f, 0.5f, 1.0f });
		}

		// Advanced sphere animations
		if (m_3DShowSpheres)
		{
			// Orbiting spheres with complex paths
			for (int i = 0; i < 4; i++)
			{
				float orbitRadius = 3.0f + i * 0.5f;
				float orbitSpeed = 0.3f + i * 0.1f;
				float orbitAngle = time * orbitSpeed + i * 1.57f; // 90 degrees apart
				
				float x = orbitRadius * cos(orbitAngle);
				float z = orbitRadius * sin(orbitAngle);
				float y = 2.0f + 1.0f * sin(time * 1.5f + i * 0.8f);
				
				// Add rotation to spheres
				glm::mat4 sphereRotation = glm::rotate(glm::mat4(1.0f), time * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 sphereTransform = glm::translate(glm::mat4(1.0f), { x, y, z }) * sphereRotation;
				
				glm::vec4 color = {
					0.4f + 0.6f * sin(time + i),
					0.4f + 0.6f * cos(time + i * 1.3f),
					0.4f + 0.6f * sin(time + i * 0.7f),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawSphere({ x, y, z }, 0.4f, color, 16);
			}
			
			// Spiral spheres
			for (int i = 0; i < 8; i++)
			{
				float spiralRadius = 2.0f;
				float spiralHeight = i * 0.3f;
				float spiralAngle = time * 0.8f + i * 0.5f;
				
				float x = spiralRadius * cos(spiralAngle);
				float z = spiralRadius * sin(spiralAngle);
				float y = -1.0f + spiralHeight + 0.5f * sin(time * 2.0f + i);
				
				glm::vec4 color = {
					0.6f + 0.4f * sin(time * 1.5f + i),
					0.6f + 0.4f * cos(time * 1.2f + i),
					0.6f + 0.4f * sin(time * 0.8f + i),
					1.0f
				};
				Zgine::BatchRenderer3D::DrawSphere({ x, y, z }, 0.2f, color, 12);
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

}
