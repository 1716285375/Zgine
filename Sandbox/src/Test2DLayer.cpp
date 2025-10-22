#include "Test2DLayer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Sandbox {

	Test2DLayer::Test2DLayer()
		: Layer("Test2D")
		, m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
		, m_CameraPosition(0.0f)
		, m_CameraSpeed(2.0f)
		, m_Time(0.0f)
		, m_ShowQuads(true)
		, m_ShowLines(true)
		, m_ShowCircles(true)
		, m_ShowAdvanced(true)
		, m_ShowTriangles(true)
		, m_ShowEllipses(true)
		, m_ShowArcs(true)
		, m_ShowGradients(true)
		, m_AnimateCircles(true)
		, m_AnimateQuads(true)
		, m_LineThickness(0.05f)
		, m_CircleRadius(0.3f)
		, m_CircleSegments(32)
		, m_AnimationSpeed(1.0f)
		, m_FPS(0.0f)
		, m_FrameCount(0)
		, m_FPSTimer(0.0f)
	{
		// Initialize batch renderer
		Zgine::BatchRenderer2D::Init();
	}

	Test2DLayer::~Test2DLayer()
	{
		Zgine::BatchRenderer2D::Shutdown();
	}

	void Test2DLayer::OnUpdate(Zgine::Timestep ts)
	{
		UpdateCamera(ts);
		UpdateAnimations(ts);

		// Calculate FPS
		m_FrameCount++;
		m_FPSTimer += ts;
		if (m_FPSTimer >= 1.0f)
		{
			m_FPS = m_FrameCount / m_FPSTimer;
			m_FrameCount = 0;
			m_FPSTimer = 0.0f;
		}

		// Begin rendering
		Zgine::BatchRenderer2D::BeginScene(m_Camera);

		RenderBasicShapes();
		if (m_ShowAdvanced)
		{
			RenderAdvancedShapes();
		}
		RenderAnimatedShapes();

		Zgine::BatchRenderer2D::EndScene();
	}

	void Test2DLayer::UpdateCamera(Zgine::Timestep ts)
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

		m_Camera.SetPosition(m_CameraPosition);
	}

	void Test2DLayer::UpdateAnimations(Zgine::Timestep ts)
	{
		m_Time += ts * m_AnimationSpeed;
	}

	void Test2DLayer::RenderBasicShapes()
	{
		if (m_ShowQuads)
		{
			// Basic quads
			Zgine::BatchRenderer2D::DrawQuad({-1.0f, 0.0f}, {0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawQuad({0.0f, 0.0f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawQuad({1.0f, 0.0f}, {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f});

			// Rotated quads
			Zgine::BatchRenderer2D::DrawRotatedQuad({-1.0f, 1.0f}, {0.3f, 0.3f}, 45.0f, {1.0f, 1.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawRotatedQuad({0.0f, 1.0f}, {0.3f, 0.3f}, 90.0f, {1.0f, 0.0f, 1.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawRotatedQuad({1.0f, 1.0f}, {0.3f, 0.3f}, 135.0f, {0.0f, 1.0f, 1.0f, 1.0f});
		}

		if (m_ShowLines)
		{
			// Lines
			Zgine::BatchRenderer2D::DrawLine({-1.5f, -1.0f, 0.0f}, {1.5f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, m_LineThickness);
			Zgine::BatchRenderer2D::DrawLine({-1.5f, -0.5f, 0.0f}, {1.5f, -0.5f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, m_LineThickness);
		}

		if (m_ShowCircles)
		{
			// Circles
			Zgine::BatchRenderer2D::DrawCircle({-1.0f, -1.0f, 0.0f}, m_CircleRadius, {1.0f, 0.5f, 0.0f, 1.0f}, m_CircleSegments);
			Zgine::BatchRenderer2D::DrawCircle({0.0f, -1.0f, 0.0f}, m_CircleRadius, {0.5f, 1.0f, 0.0f, 1.0f}, m_CircleSegments);
			Zgine::BatchRenderer2D::DrawCircle({1.0f, -1.0f, 0.0f}, m_CircleRadius, {0.0f, 0.5f, 1.0f, 1.0f}, m_CircleSegments);
		}
	}

	void Test2DLayer::RenderAdvancedShapes()
	{
		if (m_ShowTriangles)
		{
			// Triangles
			Zgine::BatchRenderer2D::DrawTriangle({-1.5f, 0.5f, 0.0f}, {-1.0f, 0.5f, 0.0f}, {-1.25f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawTriangle({-0.5f, 0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}, {-0.25f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawTriangle({0.5f, 0.5f, 0.0f}, {1.0f, 0.5f, 0.0f}, {0.75f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f});
		}

		if (m_ShowEllipses)
		{
			// Ellipses
			Zgine::BatchRenderer2D::DrawEllipse({-1.0f, -0.5f, 0.0f}, 0.3f, 0.2f, {1.0f, 0.0f, 1.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawEllipse({0.0f, -0.5f, 0.0f}, 0.2f, 0.3f, {0.0f, 1.0f, 1.0f, 1.0f});
			Zgine::BatchRenderer2D::DrawEllipse({1.0f, -0.5f, 0.0f}, 0.25f, 0.25f, {1.0f, 1.0f, 0.0f, 1.0f});

			// Ellipse outlines
			Zgine::BatchRenderer2D::DrawEllipseOutline({-1.0f, -0.8f, 0.0f}, 0.2f, 0.15f, {1.0f, 0.5f, 0.0f, 1.0f}, 0.02f);
			Zgine::BatchRenderer2D::DrawEllipseOutline({0.0f, -0.8f, 0.0f}, 0.15f, 0.2f, {0.5f, 1.0f, 0.0f, 1.0f}, 0.02f);
			Zgine::BatchRenderer2D::DrawEllipseOutline({1.0f, -0.8f, 0.0f}, 0.18f, 0.18f, {0.0f, 0.5f, 1.0f, 1.0f}, 0.02f);
		}

		if (m_ShowArcs)
		{
			// Arcs
			Zgine::BatchRenderer2D::DrawArc({-1.0f, 0.2f, 0.0f}, 0.2f, 0.0f, 3.14159f, {1.0f, 0.0f, 0.0f, 1.0f}, 0.03f);
			Zgine::BatchRenderer2D::DrawArc({0.0f, 0.2f, 0.0f}, 0.2f, 3.14159f, 2 * 3.14159f, {0.0f, 1.0f, 0.0f, 1.0f}, 0.03f);
			Zgine::BatchRenderer2D::DrawArc({1.0f, 0.2f, 0.0f}, 0.2f, 3.14159f / 2, 3 * 3.14159f / 2, {0.0f, 0.0f, 1.0f, 1.0f}, 0.03f);
		}

		if (m_ShowGradients)
		{
			// Gradient quads
			Zgine::BatchRenderer2D::DrawQuadGradient(glm::vec3(-1.0f, 1.5f, 0.0f), glm::vec2(0.4f, 0.4f), 
				glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			
			Zgine::BatchRenderer2D::DrawRotatedQuadGradient(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec2(0.4f, 0.4f), 45.0f,
				glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), 
				glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}

	void Test2DLayer::RenderAnimatedShapes()
	{
		if (m_AnimateCircles)
		{
			// Animated circles
			for (int i = 0; i < 5; i++)
			{
				float angle = m_Time + i * 0.5f;
				float x = cos(angle) * 0.5f;
				float y = sin(angle) * 0.5f;
				float radius = 0.1f + sin(m_Time * 2.0f + i) * 0.05f;
				
				Zgine::BatchRenderer2D::DrawCircle({x, y, 0.0f}, radius, 
					{sin(m_Time + i), cos(m_Time + i), sin(m_Time * 0.5f + i), 1.0f}, 16);
			}
		}

		if (m_AnimateQuads)
		{
			// Animated quads
			for (int i = 0; i < 3; i++)
			{
				float scale = 0.3f + sin(m_Time * 1.5f + i) * 0.1f;
				float rotation = m_Time * 30.0f + i * 60.0f;
				
				Zgine::BatchRenderer2D::DrawRotatedQuad({-1.5f + i * 1.5f, -1.5f}, 
					{scale, scale}, rotation, 
					{sin(m_Time + i), cos(m_Time + i), sin(m_Time * 0.7f + i), 1.0f});
			}
		}
	}

	void Test2DLayer::OnImGuiRender()
	{
		// 2D Controls Window
		ImGui::Begin("2D Rendering Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("2D Rendering Controls");
		ImGui::Separator();
		
		ImGui::Checkbox("Show Quads", &m_ShowQuads);
		ImGui::SameLine();
		ImGui::Checkbox("Show Lines", &m_ShowLines);
		ImGui::SameLine();
		ImGui::Checkbox("Show Circles", &m_ShowCircles);
		
		ImGui::Checkbox("Show Advanced", &m_ShowAdvanced);
		if (m_ShowAdvanced)
		{
			ImGui::Indent();
			ImGui::Checkbox("Show Triangles", &m_ShowTriangles);
			ImGui::SameLine();
			ImGui::Checkbox("Show Ellipses", &m_ShowEllipses);
			ImGui::SameLine();
			ImGui::Checkbox("Show Arcs", &m_ShowArcs);
			ImGui::Checkbox("Show Gradients", &m_ShowGradients);
			ImGui::Unindent();
		}
		
		ImGui::Separator();
		ImGui::Text("Animation");
		ImGui::Checkbox("Animate Circles", &m_AnimateCircles);
		ImGui::SameLine();
		ImGui::Checkbox("Animate Quads", &m_AnimateQuads);
		ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.1f, 3.0f);
		
		ImGui::Separator();
		ImGui::Text("Settings");
		ImGui::SliderFloat("Line Thickness", &m_LineThickness, 0.01f, 0.2f);
		ImGui::SliderFloat("Circle Radius", &m_CircleRadius, 0.1f, 0.5f);
		ImGui::SliderInt("Circle Segments", &m_CircleSegments, 8, 64);
		
		ImGui::Separator();
		ImGui::Text("Camera Settings");
		ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 0.5f, 10.0f);
		
		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Text("WASD - Move camera");
		
		ImGui::End();

		// 2D Performance Stats
		ImGui::Begin("2D Performance Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		auto stats = Zgine::BatchRenderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.VertexCount);
		ImGui::Text("Indices: %d", stats.IndexCount);
		
		// FPS display with color coding
		ImVec4 fpsColor = {0.0f, 1.0f, 0.0f, 1.0f}; // Green
		if (m_FPS < 30.0f) fpsColor = {1.0f, 0.0f, 0.0f, 1.0f}; // Red
		else if (m_FPS < 60.0f) fpsColor = {1.0f, 1.0f, 0.0f, 1.0f}; // Yellow
		
		ImGui::TextColored(fpsColor, "FPS: %.1f", m_FPS);
		
		if (ImGui::Button("Reset Stats", ImVec2(120, 30)))
		{
			Zgine::BatchRenderer2D::ResetStats();
		}
		
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
			m_AnimateQuads = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Lines Only", ImVec2(120, 30)))
		{
			m_ShowQuads = false;
			m_ShowLines = true;
			m_ShowCircles = false;
			m_ShowAdvanced = false;
			m_AnimateCircles = false;
			m_AnimateQuads = false;
		}
		
		if (ImGui::Button("Circles Only", ImVec2(120, 30)))
		{
			m_ShowQuads = false;
			m_ShowLines = false;
			m_ShowCircles = true;
			m_ShowAdvanced = false;
			m_AnimateCircles = true;
			m_AnimateQuads = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Advanced", ImVec2(120, 30)))
		{
			m_ShowQuads = false;
			m_ShowLines = false;
			m_ShowCircles = false;
			m_ShowAdvanced = true;
			m_AnimateCircles = false;
			m_AnimateQuads = true;
		}
		
		if (ImGui::Button("All Features", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = true;
			m_ShowCircles = true;
			m_ShowAdvanced = true;
			m_AnimateCircles = true;
			m_AnimateQuads = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Performance", ImVec2(120, 30)))
		{
			m_ShowQuads = true;
			m_ShowLines = false;
			m_ShowCircles = false;
			m_ShowAdvanced = false;
			m_AnimateCircles = false;
			m_AnimateQuads = false;
		}
		
		ImGui::End();
	}

}
