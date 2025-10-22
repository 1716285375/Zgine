#include "MainControlLayer.h"
#include "Zgine/Renderer/Renderer.h"

namespace Sandbox {

	MainControlLayer::MainControlLayer()
		: Layer("MainControlLayer"),
		m_2DCamera(-1.6f, 1.6f, -0.9f, 0.9f),
		m_3DCamera(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f),
		m_2DCameraPosition(0.0f, 0.0f, 0.0f),
		m_2DCameraSpeed(1.0f),
		m_3DCameraPosition(0.0f, 0.0f, 3.0f),
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
		// Initialize 3D camera
		m_3DCamera.SetPosition(m_3DCameraPosition);
	}

	MainControlLayer::~MainControlLayer()
	{
	}

	void MainControlLayer::OnAttach()
	{
		// Initialize camera positions
		m_2DCamera.SetPosition(m_2DCameraPosition);
		m_3DCamera.SetPosition(m_3DCameraPosition);
	}

	void MainControlLayer::OnUpdate(Zgine::Timestep ts)
	{
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
			Zgine::Renderer::BeginScene(m_2DCamera);
			
			if (m_2DShowQuads)
				Render2DBasicShapes();
			
			if (m_2DShowAdvanced)
				Render2DAdvancedShapes();
			
			if (m_2DAnimateCircles || m_2DAnimateQuads)
				Render2DAnimatedShapes();
			
			Zgine::Renderer::EndScene();
		}

		// Render 3D scene if window is open
		if (m_Show3DTestWindow)
		{
			Zgine::Renderer::BeginScene(m_3DCamera);
			
			if (m_3DShowCubes || m_3DShowSpheres || m_3DShowPlanes)
				Render3DBasicShapes();
			
			if (m_3DAnimateObjects)
				Render3DAnimatedShapes();
			
			if (m_3DShowEnvironment)
				Render3DEnvironment();
			
			Zgine::Renderer::EndScene();
		}
	}

	void MainControlLayer::OnImGuiRender()
	{
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
		// Basic quads
		Zgine::BatchRenderer2D::DrawQuad({ -1.0f, 0.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Zgine::BatchRenderer2D::DrawQuad({ 0.5f, -0.5f, 0.0f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Zgine::BatchRenderer2D::DrawRotatedQuad({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f }, 45.0f, { 0.8f, 0.8f, 0.2f, 1.0f });

		// Lines
		if (m_2DShowLines)
		{
			Zgine::BatchRenderer2D::DrawLine({ -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, m_2DLineThickness);
			Zgine::BatchRenderer2D::DrawLine({ 1.0f, -1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, m_2DLineThickness);
		}

		// Circles
		if (m_2DShowCircles)
		{
			Zgine::BatchRenderer2D::DrawCircle({ 0.0f, 0.0f, 0.0f }, m_2DCircleRadius, { 0.2f, 0.8f, 0.3f, 1.0f }, m_2DCircleSegments);
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
		// Cubes
		if (m_3DShowCubes)
		{
			Zgine::BatchRenderer3D::DrawCube({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, glm::mat4(1.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
			Zgine::BatchRenderer3D::DrawCube({ 2.0f, 0.0f, 0.0f }, { 0.5f, 1.5f, 0.5f }, glm::mat4(1.0f), { 0.2f, 0.8f, 0.3f, 1.0f });
		}

		// Spheres
		if (m_3DShowSpheres)
		{
			Zgine::BatchRenderer3D::DrawSphere({ -2.0f, 0.0f, 0.0f }, 0.8f, { 0.2f, 0.3f, 0.8f, 1.0f }, 16);
			Zgine::BatchRenderer3D::DrawSphere({ 0.0f, 2.0f, 0.0f }, 0.5f, { 0.8f, 0.8f, 0.2f, 1.0f }, 12);
		}

		// Planes
		if (m_3DShowPlanes)
		{
			Zgine::BatchRenderer3D::DrawPlane({ 0.0f, -1.0f, 0.0f }, { 10.0f, 10.0f }, { 0.5f, 0.5f, 0.5f, 1.0f });
		}
	}

	void MainControlLayer::Render3DAnimatedShapes()
	{
		if (!m_3DAnimateObjects)
			return;

		float time = m_Time;

		// Rotating cube
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));
		Zgine::BatchRenderer3D::DrawCube({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, rotation, { 1.0f, 0.5f, 0.0f, 1.0f });

		// Floating spheres
		for (int i = 0; i < 3; i++)
		{
			float y = 1.0f + 0.5f * sin(time + i * 2.0f);
			glm::vec3 pos = { -3.0f + i * 3.0f, y, 0.0f };
			Zgine::BatchRenderer3D::DrawSphere(pos, 0.3f, { 0.8f, 0.2f, 0.8f, 1.0f }, 12);
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
