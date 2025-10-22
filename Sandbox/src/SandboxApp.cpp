#include "zgpch.h"
#include <Zgine.h>

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

class PrimitiveTestLayer : public Zgine::Layer
{
public:
	PrimitiveTestLayer()
		: Layer("PrimitiveTest")
		, m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
		, m_CameraPosition(0.0f)
		, m_CameraSpeed(2.0f)
		, m_Time(0.0f)
		, m_FPS(0.0f)
		, m_FrameCount(0)
		, m_FPSTimer(0.0f)
		, m_LineThickness(0.05f)
		, m_CircleRadius(0.3f)
		, m_CircleSegments(32)
		, m_ShowLines(true)
		, m_ShowCircles(true)
		, m_ShowQuads(true)
		, m_ShowAdvanced(true)
		, m_AnimateCircles(true)
	{
		// Initialize batch renderer
		Zgine::BatchRenderer2D::Init();
	}

	virtual ~PrimitiveTestLayer()
	{
		Zgine::BatchRenderer2D::Shutdown();
	}

	virtual void OnUpdate(Zgine::Timestep ts) override
	{
		// Update camera position based on input
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_CameraPosition.x -= m_CameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_CameraPosition.x += m_CameraSpeed * ts;

		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_CameraPosition.y += m_CameraSpeed * ts;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_CameraPosition.y -= m_CameraSpeed * ts;

		// Update time for animations
		m_Time += ts;
		
		// Calculate FPS
		m_FrameCount++;
		m_FPSTimer += ts;
		if (m_FPSTimer >= 1.0f)
		{
			m_FPS = m_FrameCount / m_FPSTimer;
			m_FrameCount = 0;
			m_FPSTimer = 0.0f;
		}

		// Render with batch renderer
		Zgine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Zgine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		Zgine::BatchRenderer2D::BeginScene(m_Camera);

		// Draw test quads
		if (m_ShowQuads)
		{
			for (int i = 0; i < 5; i++)
			{
				float x = (i - 2) * 0.4f;
				float y = sin(m_Time + i) * 0.2f;
				
				glm::vec4 color = glm::vec4(
					(float)(i % 3) / 2.0f,
					(float)((i + 1) % 3) / 2.0f,
					(float)((i + 2) % 3) / 2.0f,
					1.0f
				);

				Zgine::BatchRenderer2D::DrawQuad(
					{ x, y, 0.0f }, 
					{ 0.3f, 0.3f }, 
					color
				);
			}
		}

		// Draw test lines
		if (m_ShowLines)
		{
			// Draw a grid of lines
			for (int i = -3; i <= 3; i++)
			{
				float pos = i * 0.3f;
				
				// Vertical lines
				Zgine::BatchRenderer2D::DrawLine(
					{ pos, -1.0f, 0.0f },
					{ pos, 1.0f, 0.0f },
					{ 0.5f, 0.5f, 0.5f, 0.8f },
					m_LineThickness
				);
				
				// Horizontal lines
				Zgine::BatchRenderer2D::DrawLine(
					{ -1.0f, pos, 0.0f },
					{ 1.0f, pos, 0.0f },
					{ 0.5f, 0.5f, 0.5f, 0.8f },
					m_LineThickness
				);
			}

			// Draw animated lines
			for (int i = 0; i < 8; i++)
			{
				float angle = m_Time + i * 0.5f;
				float radius = 0.8f;
				
				glm::vec3 start = { cos(angle) * radius, sin(angle) * radius, 0.0f };
				glm::vec3 end = { cos(angle + 1.0f) * radius * 0.5f, sin(angle + 1.0f) * radius * 0.5f, 0.0f };
				
				glm::vec4 color = glm::vec4(
					sin(angle) * 0.5f + 0.5f,
					cos(angle) * 0.5f + 0.5f,
					0.8f,
					1.0f
				);

				Zgine::BatchRenderer2D::DrawLine(start, end, color, m_LineThickness * 2.0f);
			}
		}

		// Draw test circles
		if (m_ShowCircles)
		{
			// Draw static filled circles
			for (int i = 0; i < 3; i++)
			{
				float x = (i - 1) * 0.6f;
				float y = 0.0f;
				
				glm::vec4 color = glm::vec4(
					(float)(i % 3) / 2.0f,
					(float)((i + 1) % 3) / 2.0f,
					(float)((i + 2) % 3) / 2.0f,
					0.8f
				);

				Zgine::BatchRenderer2D::DrawCircle(
					{ x, y, 0.0f },
					m_CircleRadius,
					color,
					m_CircleSegments
				);
			}

			// Draw circle outlines
			for (int i = 0; i < 3; i++)
			{
				float x = (i - 1) * 0.6f;
				float y = 0.0f;
				
				glm::vec4 outlineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.9f); // White outline

				Zgine::BatchRenderer2D::DrawCircleOutline(
					{ x, y, 0.0f },
					m_CircleRadius,
					outlineColor,
					0.02f, // Thin outline
					m_CircleSegments
				);
			}

			// Draw animated circles
			if (m_AnimateCircles)
			{
				for (int i = 0; i < 6; i++)
				{
					float angle = m_Time * 0.5f + i * 1.0f;
					float radius = 0.4f + sin(m_Time + i) * 0.2f;
					
					float x = cos(angle) * 0.8f;
					float y = sin(angle) * 0.8f;
					
					glm::vec4 color = glm::vec4(
						sin(angle) * 0.5f + 0.5f,
						cos(angle) * 0.5f + 0.5f,
						0.6f,
						0.7f
					);

					Zgine::BatchRenderer2D::DrawCircle(
						{ x, y, 0.0f },
						radius,
						color,
						m_CircleSegments
					);
					
					// Add outline to animated circles
					glm::vec4 outlineColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);
					Zgine::BatchRenderer2D::DrawCircleOutline(
						{ x, y, 0.0f },
						radius,
						outlineColor,
						0.015f, // Thin outline
						m_CircleSegments
					);
				}
			}
		}

		// Draw new 2D primitives
		if (m_ShowAdvanced)
		{
			// Draw triangles
			for (int i = 0; i < 3; i++)
			{
				float x = (i - 1) * 0.8f;
				float y = -1.2f;
				
				glm::vec3 p0 = { x - 0.2f, y - 0.2f, 0.0f };
				glm::vec3 p1 = { x + 0.2f, y - 0.2f, 0.0f };
				glm::vec3 p2 = { x, y + 0.2f, 0.0f };
				
				glm::vec4 color = glm::vec4(
					(float)(i % 3) / 2.0f,
					(float)((i + 1) % 3) / 2.0f,
					(float)((i + 2) % 3) / 2.0f,
					0.8f
				);

				Zgine::BatchRenderer2D::DrawTriangle(p0, p1, p2, color);
			}

			// Draw ellipses
			for (int i = 0; i < 3; i++)
			{
				float x = (i - 1) * 0.6f;
				float y = 1.5f;
				
				glm::vec4 color = glm::vec4(0.8f, 0.2f, 0.8f, 0.7f);

				Zgine::BatchRenderer2D::DrawEllipse(
					{ x, y, 0.0f },
					0.3f, 0.2f, // radiusX, radiusY
					color,
					16
				);

				// Draw ellipse outline
				Zgine::BatchRenderer2D::DrawEllipseOutline(
					{ x, y, 0.0f },
					0.3f, 0.2f,
					glm::vec4(1.0f, 1.0f, 1.0f, 0.9f),
					0.02f,
					16
				);
			}

			// Draw arcs
			for (int i = 0; i < 4; i++)
			{
				float x = -1.0f + i * 0.6f;
				float y = -1.5f;
				
				float startAngle = i * 3.14159f / 2.0f;
				float endAngle = startAngle + 3.14159f / 2.0f;
				
				glm::vec4 color = glm::vec4(
					(float)i / 3.0f,
					1.0f - (float)i / 3.0f,
					0.5f,
					0.8f
				);

				Zgine::BatchRenderer2D::DrawArc(
					{ x, y, 0.0f },
					0.25f,
					startAngle,
					endAngle,
					color,
					0.03f,
					16
				);
			}

			// Draw gradient quads
			for (int i = 0; i < 2; i++)
			{
				float x = (i - 0.5f) * 1.2f;
				float y = 0.0f;
				
				glm::vec4 topLeft = glm::vec4(1.0f, 0.0f, 0.0f, 0.8f);     // Red
				glm::vec4 topRight = glm::vec4(0.0f, 1.0f, 0.0f, 0.8f);   // Green
				glm::vec4 bottomLeft = glm::vec4(0.0f, 0.0f, 1.0f, 0.8f); // Blue
				glm::vec4 bottomRight = glm::vec4(1.0f, 1.0f, 0.0f, 0.8f); // Yellow

				Zgine::BatchRenderer2D::DrawQuadGradient(
					{ x, y, 0.0f },
					{ 0.4f, 0.4f },
					topLeft, topRight, bottomLeft, bottomRight
				);
			}
		}

		Zgine::BatchRenderer2D::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		// Main control panel
		ImGui::Begin("Primitive Test Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("Primitive Rendering Test");
		ImGui::Separator();
		
		// Camera controls
		ImGui::Text("Camera Controls:");
		ImGui::Text("WASD - Move camera");
		ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
			m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
		
		// Camera speed control
		ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 0.5f, 10.0f, "%.1f");
		
		ImGui::Separator();
		
		// Render options
		ImGui::Text("Render Options:");
		ImGui::Checkbox("Show Quads", &m_ShowQuads);
		ImGui::SameLine();
		ImGui::Checkbox("Show Lines", &m_ShowLines);
		ImGui::Checkbox("Show Circles", &m_ShowCircles);
		ImGui::SameLine();
		ImGui::Checkbox("Show Advanced", &m_ShowAdvanced);
		ImGui::Checkbox("Animate Circles", &m_AnimateCircles);
		
		ImGui::Separator();
		
		// Line settings
		if (m_ShowLines)
		{
			ImGui::Text("Line Settings:");
			ImGui::SliderFloat("Line Thickness", &m_LineThickness, 0.01f, 0.2f, "%.3f");
		}
		
		// Circle settings
		if (m_ShowCircles)
		{
			ImGui::Text("Circle Settings:");
			ImGui::SliderFloat("Circle Radius", &m_CircleRadius, 0.1f, 0.8f, "%.2f");
			ImGui::SliderInt("Circle Segments", &m_CircleSegments, 8, 64);
			
			// Show circle info
			ImGui::Text("Circle Info:");
			ImGui::Text("Current segments: %d", m_CircleSegments);
			ImGui::Text("Current radius: %.2f", m_CircleRadius);
		}
		
		// Advanced settings
		if (m_ShowAdvanced)
		{
			ImGui::Separator();
			ImGui::Text("Advanced Settings:");
			ImGui::Text("Advanced primitives are enabled");
			ImGui::Text("Includes: Triangles, Ellipses, Arcs, Gradients");
		}
		
		ImGui::End();
		
		// Performance stats window
		ImGui::Begin("Performance Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		auto stats = Zgine::BatchRenderer2D::GetStats();
		ImGui::Text("Performance Statistics:");
		ImGui::Separator();
		
		// FPS display with color coding
		if (m_FPS >= 60.0f)
			ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "FPS: %.1f", m_FPS);
		else if (m_FPS >= 30.0f)
			ImGui::TextColored({1.0f, 1.0f, 0.0f, 1.0f}, "FPS: %.1f", m_FPS);
		else
			ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "FPS: %.1f", m_FPS);
		
		ImGui::Separator();
		ImGui::Text("Rendering Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.VertexCount);
		ImGui::Text("Index Count: %d", stats.IndexCount);
		
		ImGui::Separator();
		ImGui::Text("Time: %.2f seconds", m_Time);
		
		ImGui::Separator();
		if (ImGui::Button("Reset Stats", ImVec2(120, 30)))
		{
			Zgine::BatchRenderer2D::ResetStats();
		}
		
		ImGui::End();
		
		// Debug info window
		ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("Debug Information:");
		ImGui::Separator();
		
		ImGui::Text("Camera Speed: %.3f", m_CameraSpeed);
		ImGui::Text("Line Thickness: %.3f", m_LineThickness);
		ImGui::Text("Circle Radius: %.2f", m_CircleRadius);
		ImGui::Text("Circle Segments: %d", m_CircleSegments);
		
		ImGui::Separator();
		ImGui::Text("Render States:");
		ImGui::Text("Quads: %s", m_ShowQuads ? "ON" : "OFF");
		ImGui::Text("Lines: %s", m_ShowLines ? "ON" : "OFF");
		ImGui::Text("Circles: %s", m_ShowCircles ? "ON" : "OFF");
		ImGui::Text("Advanced: %s", m_ShowAdvanced ? "ON" : "OFF");
		ImGui::Text("Animation: %s", m_AnimateCircles ? "ON" : "OFF");
		
		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Text("WASD - Move Camera");
		ImGui::Text("Mouse - Interact with UI");
		ImGui::Text("ESC - Exit Application");
		
		ImGui::End();
		
		// Preset configurations window
		ImGui::Begin("Preset Configurations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("Quick Presets:");
		ImGui::Separator();
		
		if (ImGui::Button("Basic Shapes", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = false;
			m_ShowCircles = false;
			m_ShowAdvanced = false;
			m_AnimateCircles = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Lines Only", ImVec2(120, 30)))
		{
			m_ShowQuads = false;
			m_ShowLines = true;
			m_ShowCircles = false;
			m_ShowAdvanced = false;
			m_AnimateCircles = false;
		}
		
		if (ImGui::Button("Circles Only", ImVec2(120, 30)))
		{
			m_ShowQuads = false;
			m_ShowLines = false;
			m_ShowCircles = true;
			m_ShowAdvanced = false;
			m_AnimateCircles = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Advanced", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = true;
			m_ShowCircles = true;
			m_ShowAdvanced = true;
			m_AnimateCircles = true;
		}
		
		if (ImGui::Button("All Features", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = true;
			m_ShowCircles = true;
			m_ShowAdvanced = true;
			m_AnimateCircles = true;
			m_CircleRadius = 0.3f;
			m_CircleSegments = 32;
			m_LineThickness = 0.05f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Performance Test", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = true;
			m_ShowCircles = true;
			m_ShowAdvanced = true;
			m_AnimateCircles = true;
			m_CircleSegments = 64; // High detail for performance testing
		}
		
		ImGui::End();
	}

private:
	Zgine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraSpeed;
	float m_Time;
	float m_FPS;
	int m_FrameCount;
	float m_FPSTimer;
	
	// Line settings
	float m_LineThickness;
	
	// Circle settings
	float m_CircleRadius;
	int m_CircleSegments;
	
	// Render options
	bool m_ShowLines;
	bool m_ShowCircles;
	bool m_ShowQuads;
	bool m_ShowAdvanced;
	bool m_AnimateCircles;
};

class Test3DLayer : public Zgine::Layer
{
public:
	Test3DLayer()
		: Layer("Test3D")
		, m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f)
		, m_CameraPosition(0.0f, 5.0f, 10.0f)
		, m_CameraRotation(0.0f, 0.0f, 0.0f)
		, m_CameraSpeed(5.0f)
		, m_RotationSpeed(45.0f)
		, m_Time(0.0f)
		, m_ShowCubes(true)
		, m_ShowSpheres(true)
		, m_ShowPlanes(true)
		, m_AnimateObjects(true)
		, m_WireframeMode(false)
	{
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}

	virtual void OnUpdate(Zgine::Timestep ts) override
	{
		// Update time for animations
		m_Time += ts;

		// Camera movement
		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_Camera.MoveForward(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_Camera.MoveForward(-m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_Camera.MoveRight(-m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_Camera.MoveRight(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_Q))
			m_Camera.MoveUp(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_E))
			m_Camera.MoveUp(-m_CameraSpeed * ts);

		// Camera rotation with mouse
		if (Zgine::Input::IsMouseButtonPressed(ZG_MOUSE_BUTTON_RIGHT))
		{
			auto mousePos = Zgine::Input::GetMousePosition();
			static glm::vec2 lastMousePos = mousePos;
			
			if (lastMousePos != glm::vec2(-1.0f))
			{
				float deltaX = mousePos.x - lastMousePos.x;
				float deltaY = mousePos.y - lastMousePos.y;
				
				m_Camera.Rotate(deltaX * 0.1f, -deltaY * 0.1f);
			}
			
			lastMousePos = mousePos;
		}

		// Render 3D scene
		Zgine::BatchRenderer3D::BeginScene(m_Camera);

		if (m_ShowCubes)
		{
			// Animated cubes
			for (int i = 0; i < 5; i++)
			{
				float angle = m_Time * m_RotationSpeed + i * 72.0f;
				glm::vec3 position = glm::vec3(
					cos(glm::radians(angle)) * 3.0f,
					sin(m_Time * 2.0f + i) * 0.5f,
					sin(glm::radians(angle)) * 3.0f
				);
				
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
									 glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(m_RotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
				
				Zgine::BatchRenderer3D::DrawCube(position, glm::vec3(0.5f), transform, 
					glm::vec4(0.2f + i * 0.15f, 0.3f + i * 0.1f, 0.8f - i * 0.1f, 1.0f));
			}
		}

		if (m_ShowSpheres)
		{
			// Animated spheres
			for (int i = 0; i < 3; i++)
			{
				float angle = m_Time * m_RotationSpeed * 0.5f + i * 120.0f;
				glm::vec3 position = glm::vec3(
					cos(glm::radians(angle)) * 5.0f,
					cos(m_Time * 1.5f + i) * 2.0f,
					sin(glm::radians(angle)) * 5.0f
				);
				
				Zgine::BatchRenderer3D::DrawSphere(position, 0.8f, 
					glm::vec4(0.8f - i * 0.2f, 0.2f + i * 0.3f, 0.4f + i * 0.2f, 1.0f), 32);
			}
		}

		if (m_ShowPlanes)
		{
			// Ground plane
			Zgine::BatchRenderer3D::DrawPlane(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec2(20.0f, 20.0f), 
				glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
			
			// Wall planes
			Zgine::BatchRenderer3D::DrawPlane(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec2(20.0f, 10.0f), 
				glm::vec4(0.2f, 0.4f, 0.2f, 1.0f));
		}

		Zgine::BatchRenderer3D::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		// 3D Controls Window
		ImGui::Begin("3D Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("3D Rendering Controls");
		ImGui::Separator();
		
		ImGui::Checkbox("Show Cubes", &m_ShowCubes);
		ImGui::SameLine();
		ImGui::Checkbox("Show Spheres", &m_ShowSpheres);
		ImGui::SameLine();
		ImGui::Checkbox("Show Planes", &m_ShowPlanes);
		
		ImGui::Checkbox("Animate Objects", &m_AnimateObjects);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe Mode", &m_WireframeMode);
		
		ImGui::Separator();
		ImGui::Text("Camera Settings");
		ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 1.0f, 20.0f);
		ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 10.0f, 180.0f);
		
		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Text("WASD - Move camera");
		ImGui::Text("QE - Move up/down");
		ImGui::Text("Right Mouse + Drag - Rotate camera");
		
		ImGui::End();

		// 3D Performance Stats
		ImGui::Begin("3D Performance Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		auto stats = Zgine::BatchRenderer3D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Triangles: %d", stats.TriangleCount);
		ImGui::Text("Vertices: %d", stats.VertexCount);
		ImGui::Text("Indices: %d", stats.IndexCount);
		
		if (ImGui::Button("Reset Stats", ImVec2(120, 30)))
		{
			Zgine::BatchRenderer3D::ResetStats();
		}
		
		ImGui::End();
	}

private:
	Zgine::PerspectiveCamera m_Camera;
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraRotation;
	float m_CameraSpeed;
	float m_RotationSpeed;
	float m_Time;
	
	bool m_ShowCubes;
	bool m_ShowSpheres;
	bool m_ShowPlanes;
	bool m_AnimateObjects;
	bool m_WireframeMode;
};

class Sandbox : public Zgine::Application {
public:
	Sandbox()
	{
		PushLayer(new PrimitiveTestLayer());
		PushLayer(new Test3DLayer());
	}
	~Sandbox() {}
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};