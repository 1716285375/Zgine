#include "zgpch.h"
#include <Zgine.h>

#include "imgui.h"

class PrimitiveTestLayer : public Zgine::Layer
{
public:
	PrimitiveTestLayer()
		: Layer("PrimitiveTest")
		, m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
		, m_CameraPosition(0.0f)
		, m_CameraSpeed(0.02f)
		, m_Time(0.0f)
		, m_LineThickness(0.05f)
		, m_CircleRadius(0.3f)
		, m_CircleSegments(32)
		, m_ShowLines(true)
		, m_ShowCircles(true)
		, m_ShowQuads(true)
		, m_AnimateCircles(true)
	{
		// Initialize batch renderer
		Zgine::BatchRenderer2D::Init();
	}

	virtual ~PrimitiveTestLayer()
	{
		Zgine::BatchRenderer2D::Shutdown();
	}

	virtual void OnUpdate() override
	{
		// Update camera position based on input
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_CameraPosition.x += m_CameraSpeed;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_CameraPosition.x -= m_CameraSpeed;

		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_CameraPosition.y -= m_CameraSpeed;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_CameraPosition.y += m_CameraSpeed;

		// Update time for animations
		m_Time += 0.016f; // Approximate 60 FPS

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
		
		ImGui::Separator();
		
		// Render options
		ImGui::Text("Render Options:");
		ImGui::Checkbox("Show Quads", &m_ShowQuads);
		ImGui::SameLine();
		ImGui::Checkbox("Show Lines", &m_ShowLines);
		ImGui::Checkbox("Show Circles", &m_ShowCircles);
		ImGui::SameLine();
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
		
		ImGui::End();
		
		// Performance stats window
		ImGui::Begin("Performance Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		auto stats = Zgine::BatchRenderer2D::GetStats();
		ImGui::Text("Performance Statistics:");
		ImGui::Separator();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.VertexCount);
		ImGui::Text("Index Count: %d", stats.IndexCount);
		
		ImGui::Separator();
		ImGui::Text("Time: %.2f seconds", m_Time);
		
		ImGui::Separator();
		if (ImGui::Button("Reset Stats", ImVec2(100, 30)))
		{
			Zgine::BatchRenderer2D::ResetStats();
		}
		
		ImGui::End();
		
		// Debug info window
		ImGui::Begin("Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("Debug Information:");
		ImGui::Separator();
		ImGui::Text("FPS: %.1f", 1.0f / 0.016f); // Approximate FPS
		ImGui::Text("Camera Speed: %.3f", m_CameraSpeed);
		ImGui::Text("Line Thickness: %.3f", m_LineThickness);
		ImGui::Text("Circle Radius: %.2f", m_CircleRadius);
		ImGui::Text("Circle Segments: %d", m_CircleSegments);
		
		ImGui::Separator();
		ImGui::Text("Render States:");
		ImGui::Text("Quads: %s", m_ShowQuads ? "ON" : "OFF");
		ImGui::Text("Lines: %s", m_ShowLines ? "ON" : "OFF");
		ImGui::Text("Circles: %s", m_ShowCircles ? "ON" : "OFF");
		ImGui::Text("Animation: %s", m_AnimateCircles ? "ON" : "OFF");
		
		ImGui::End();
	}

private:
	Zgine::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraSpeed;
	float m_Time;
	
	// Line settings
	float m_LineThickness;
	
	// Circle settings
	float m_CircleRadius;
	int m_CircleSegments;
	
	// Render options
	bool m_ShowLines;
	bool m_ShowCircles;
	bool m_ShowQuads;
	bool m_AnimateCircles;
};

class Sandbox : public Zgine::Application {
public:
	Sandbox()
	{
		PushLayer(new PrimitiveTestLayer());
	}
	~Sandbox() {}
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};