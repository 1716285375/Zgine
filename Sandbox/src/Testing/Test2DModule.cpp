#include "sandbox_pch.h"
#include "Test2DModule.h"
#include "Zgine/Core.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Events/KeyEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/ImGui/ImGuiWrapper.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <GL/gl.h>

namespace Sandbox {

	Test2DModule::Test2DModule()
		: m_Camera(-2.0f, 2.0f, -1.5f, 1.5f)
		, m_Time(0.0f)
		, m_ActiveScene("Basic Shapes")
	{
		ZG_CORE_INFO("Test2DModule created");
	}

	Test2DModule::~Test2DModule()
	{
		ZG_CORE_INFO("Test2DModule destroyed");
	}

	void Test2DModule::OnAttach()
	{
		ZG_CORE_INFO("Test2DModule attached");
		
		// Register built-in scenes
		RegisterScene("Basic Shapes", [this](const Test2DConfig& config) {
			RenderBasicShapesScene(config);
		});
		
		RegisterScene("Advanced Shapes", [this](const Test2DConfig& config) {
			RenderAdvancedShapesScene(config);
		});
		
		RegisterScene("Animated Shapes", [this](const Test2DConfig& config) {
			RenderAnimatedShapesScene(config);
		});
		
		RegisterScene("Performance Test", [this](const Test2DConfig& config) {
			RenderPerformanceTestScene(config);
		});
	}

	void Test2DModule::OnUpdate(float ts)
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

		// Reset object count for this frame
		ResetObjectCount();
	}

	void Test2DModule::OnImGuiRender()
	{
		// UI is now handled by UIManager to avoid duplicate windows
		// This method is kept for compatibility but does nothing
	}

	void Test2DModule::OnEvent(Zgine::Event& e)
	{
		Zgine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Zgine::WindowResizeEvent>([this](Zgine::WindowResizeEvent& e) {
			float aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
			m_Camera.SetProjection(-2.0f * aspectRatio, 2.0f * aspectRatio, -1.5f, 1.5f);
			return false;
		});
	}

	void Test2DModule::RegisterScene(const std::string& name, std::function<void(const Test2DConfig&)> renderFunc)
	{
		m_Scenes.emplace_back(name, renderFunc);
		ZG_CORE_INFO("Registered 2D scene: {}", name);
	}

	void Test2DModule::SetActiveScene(const std::string& name)
	{
		m_ActiveScene = name;
		ZG_CORE_INFO("Active 2D scene set to: {}", name);
	}

	void Test2DModule::BeginScene()
	{
		Zgine::BatchRenderer2D::BeginScene(m_Camera);
	}

	void Test2DModule::EndScene()
	{
		Zgine::BatchRenderer2D::EndScene();
	}

	void Test2DModule::RenderActiveScene()
	{
		// Note: BeginScene() is called by RenderManager::Render2D()
		// so we don't call it here to avoid double BeginScene()
		
		// Check if any rendering options are enabled
		bool hasAnyEnabled = m_Config.showQuads || m_Config.showLines || m_Config.showCircles || 
		                     m_Config.showAdvanced || m_Config.showTriangles || m_Config.showEllipses || 
		                     m_Config.showArcs || m_Config.showGradients;
		
		// ZG_CORE_TRACE("RenderActiveScene called - ActiveScene: {}, HasAnyEnabled: {}", m_ActiveScene, hasAnyEnabled);
		
		if (!hasAnyEnabled)
		{
			// Render a default test pattern when no options are selected
			// ZG_CORE_TRACE("Rendering default test pattern");
			RenderDefaultTestPattern();
		}
		else
		{
			// Render the active scene
			// ZG_CORE_TRACE("Rendering active scene: {}", m_ActiveScene);
			for (const auto& scene : m_Scenes)
			{
				if (scene.GetName() == m_ActiveScene)
				{
					// ZG_CORE_TRACE("Found scene: {}, calling Render", scene.GetName());
					scene.Render(m_Config);
					break;
				}
			}
		}
		
		// Note: EndScene() is called by RenderManager::Render2D()
		// so we don't call it here to avoid double EndScene()
	}

	void Test2DModule::UpdateCamera(float ts)
	{
		// Update camera position based on input
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_Config.cameraPosition.x -= m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_Config.cameraPosition.x += m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_Config.cameraPosition.y += m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_Config.cameraPosition.y -= m_Config.cameraSpeed * ts;

		// Update camera position
		m_Camera.SetPosition(m_Config.cameraPosition);
	}

	void Test2DModule::UpdateAnimations(float ts)
	{
		// Only update time if animation is not paused
		if (!m_Config.animationPaused)
		{
			m_Time += ts;
		}
	}

	void Test2DModule::RenderUI()
	{
		if (m_ShowConfigWindow)
			RenderConfigWindow();
		if (m_ShowPerformanceWindow)
			RenderPerformanceWindow();
		if (m_ShowSceneSelector)
			RenderSceneSelector();
	}

	void Test2DModule::RenderConfigWindow()
	{
		if (Zgine::IG::Begin("2D Test Configuration", &m_ShowConfigWindow))
		{
			Zgine::IG::Text("Render Options");
			Zgine::IG::Separator();
			Zgine::IG::Checkbox("Show Quads", &m_Config.showQuads);
			Zgine::IG::Checkbox("Show Lines", &m_Config.showLines);
			Zgine::IG::Checkbox("Show Circles", &m_Config.showCircles);
			Zgine::IG::Checkbox("Show Advanced", &m_Config.showAdvanced);
			Zgine::IG::Checkbox("Show Triangles", &m_Config.showTriangles);
			Zgine::IG::Checkbox("Show Ellipses", &m_Config.showEllipses);
			Zgine::IG::Checkbox("Show Arcs", &m_Config.showArcs);
			Zgine::IG::Checkbox("Show Gradients", &m_Config.showGradients);

			Zgine::IG::Separator();
			Zgine::IG::Text("Animation Options");
			Zgine::IG::Checkbox("Animate Circles", &m_Config.animateCircles);
			Zgine::IG::Checkbox("Animate Quads", &m_Config.animateQuads);
			Zgine::IG::SliderFloat("Animation Speed", &m_Config.animationSpeed, 0.1f, 5.0f);

			Zgine::IG::Separator();
			Zgine::IG::Text("Settings");
			Zgine::IG::SliderFloat("Line Thickness", &m_Config.lineThickness, 0.01f, 0.2f);
			Zgine::IG::SliderFloat("Circle Radius", &m_Config.circleRadius, 0.1f, 1.0f);
			Zgine::IG::SliderInt("Circle Segments", &m_Config.circleSegments, 8, 64);
			Zgine::IG::SliderInt("Ellipse Segments", &m_Config.ellipseSegments, 8, 32);

			Zgine::IG::Separator();
			Zgine::IG::Text("Camera Settings");
			Zgine::IG::SliderFloat("Camera Speed", &m_Config.cameraSpeed, 0.5f, 10.0f);
			Zgine::IG::SliderFloat3("Camera Position", &m_Config.cameraPosition.x, -5.0f, 5.0f);
		}
		Zgine::IG::End();
	}

	void Test2DModule::RenderPerformanceWindow()
	{
		if (Zgine::IG::Begin("2D Performance", &m_ShowPerformanceWindow))
		{
			Zgine::IG::Text("Performance Metrics");
			Zgine::IG::Separator();
			Zgine::IG::Text("FPS: %.1f", m_FPS);
			Zgine::IG::Text("Objects Rendered: %d", m_ObjectCount);
			Zgine::IG::Text("Active Scene: %s", m_ActiveScene.c_str());
		}
		Zgine::IG::End();
	}

	void Test2DModule::RenderSceneSelector()
	{
		if (Zgine::IG::Begin("2D Scene Selector", &m_ShowSceneSelector))
		{
			Zgine::IG::Text("Select 2D Test Scene");
			Zgine::IG::Separator();

			for (const auto& scene : m_Scenes)
			{
				bool isSelected = (scene.GetName() == m_ActiveScene);
				if (Zgine::IG::Selectable(scene.GetName().c_str(), isSelected))
				{
					SetActiveScene(scene.GetName());
				}
			}
		}
		Zgine::IG::End();
	}

	void Test2DModule::RenderBasicShapesScene(const Test2DConfig& config)
	{
		 //ZG_CORE_TRACE("RenderBasicShapesScene called - showQuads: {}, showLines: {}, showCircles: {}", 
			//config.showQuads, config.showLines, config.showCircles);
		
		// Apply render mode settings
		if (config.renderMode == 1) // Wireframe mode
		{
			// Enable wireframe rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (config.renderMode == 2) // Points mode
		{
			// Enable point rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else // Normal mode
		{
			// Ensure normal rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		// Render quads
		if (config.showQuads)
		{
			// ZG_CORE_TRACE("Rendering quads");
			// Basic colored quads
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 1.0f;
				
				glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
				if (i == 1) color = { 0.0f, 1.0f, 0.0f, 1.0f };
				if (i == 2) color = { 0.0f, 0.0f, 1.0f, 1.0f };
				
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.5f, 0.5f }, color);
				IncrementObjectCount();
				
				// Draw bounding box if enabled
				if (config.showBoundingBoxes)
				{
					DrawBoundingBox({ x, y, 0.0f }, { 0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f, 0.5f });
				}
			}
			
			// Animated quads (if animation is enabled)
			if (config.animateQuads || config.animateAll)
			{
				for (int i = 0; i < 2; i++)
				{
					float x = -0.5f + i * 1.0f;
					float y = 0.5f + sin(m_Time * config.animationSpeed + i) * 0.2f;
					float rotation = m_Time * config.animationSpeed * 45.0f + i * 90.0f;
					float scale = 0.3f + sin(m_Time * config.animationSpeed * 2.0f + i) * 0.1f;
					
					Zgine::BatchRenderer2D::DrawRotatedQuad({ x, y, 0.0f }, { scale, scale }, rotation,
						{ 1.0f, 1.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
			else
			{
				// Static rotated quads
				for (int i = 0; i < 2; i++)
				{
					float x = -0.5f + i * 1.0f;
					float y = 0.5f;
					float rotation = m_Time * config.animationSpeed * (i + 1);
					
					Zgine::BatchRenderer2D::DrawRotatedQuad({ x, y, 0.0f }, { 0.3f, 0.3f }, rotation,
						{ 1.0f, 1.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
		}

		// Render lines
		if (config.showLines)
		{
			// Triangle outlines
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 0.0f;
				
				// Triangle using three lines
				Zgine::BatchRenderer2D::DrawLine({ x, y + 0.3f, 0.0f }, { x - 0.2f, y - 0.3f, 0.0f }, 
					{ 0.0f, 1.0f, 0.0f, 1.0f }, config.lineThickness);
				Zgine::BatchRenderer2D::DrawLine({ x - 0.2f, y - 0.3f, 0.0f }, { x + 0.2f, y - 0.3f, 0.0f }, 
					{ 0.0f, 1.0f, 0.0f, 1.0f }, config.lineThickness);
				Zgine::BatchRenderer2D::DrawLine({ x + 0.2f, y - 0.3f, 0.0f }, { x, y + 0.3f, 0.0f }, 
					{ 0.0f, 1.0f, 0.0f, 1.0f }, config.lineThickness);
				IncrementObjectCount(3);
			}
			
			// Grid lines
			for (int i = 0; i < 5; i++)
			{
				float x = -1.0f + i * 0.5f;
				Zgine::BatchRenderer2D::DrawLine({ x, -1.5f, 0.0f }, { x, 1.5f, 0.0f }, 
					{ 0.5f, 0.5f, 0.5f, 1.0f }, config.lineThickness * 0.5f);
				IncrementObjectCount();
			}
		}

		// Render circles
		if (config.showCircles)
		{
			// Animated circles (if animation is enabled)
			if (config.animateCircles || config.animateAll)
			{
				for (int i = 0; i < 3; i++)
				{
					float x = -1.0f + i * 1.0f;
					float y = -1.0f + sin(m_Time * config.animationSpeed + i) * 0.3f;
					float radius = config.circleRadius * (0.8f + 0.4f * sin(m_Time * config.animationSpeed * 1.5f + i));
					
					glm::vec4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
					if (i == 1) color = { 1.0f, 0.0f, 1.0f, 1.0f };
					if (i == 2) color = { 0.0f, 1.0f, 1.0f, 1.0f };
					
					Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, radius, 
						color, config.circleSegments);
					IncrementObjectCount();
					
					// Draw bounding box if enabled
					if (config.showBoundingBoxes)
					{
						DrawBoundingBox({ x, y, 0.0f }, { radius * 2.0f, radius * 2.0f }, { 1.0f, 1.0f, 0.0f, 0.5f });
					}
				}
			}
			else
			{
				// Static filled circles
				for (int i = 0; i < 2; i++)
				{
					float x = -0.5f + i * 1.0f;
					float y = -1.0f;
					
					glm::vec4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
					if (i == 1) color = { 1.0f, 0.0f, 1.0f, 1.0f };
					
					Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, config.circleRadius, 
						color, config.circleSegments);
					IncrementObjectCount();
					
					// Draw bounding box if enabled
					if (config.showBoundingBoxes)
					{
						DrawBoundingBox({ x, y, 0.0f }, { config.circleRadius * 2.0f, config.circleRadius * 2.0f }, { 1.0f, 1.0f, 0.0f, 0.5f });
					}
				}
			}
			
			// Circle outlines
			for (int i = 0; i < 2; i++)
			{
				float x = -0.5f + i * 1.0f;
				float y = -0.5f;
				
				Zgine::BatchRenderer2D::DrawCircleOutline({ x, y, 0.0f }, config.circleRadius * 0.7f, 
					{ 0.0f, 1.0f, 1.0f, 1.0f }, config.lineThickness, config.circleSegments);
				IncrementObjectCount();
			}
		}
	}

	void Test2DModule::RenderAdvancedShapesScene(const Test2DConfig& config)
	{
		// Apply render mode settings
		if (config.renderMode == 1) // Wireframe mode
		{
			// Enable wireframe rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (config.renderMode == 2) // Points mode
		{
			// Enable point rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else // Normal mode
		{
			// Ensure normal rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Render basic shapes if enabled (for Mixed preset)
		if (config.showQuads)
		{
			ZG_CORE_TRACE("Advanced Shapes Scene: Rendering quads (showQuads=true)");
			// Basic colored quads
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 1.5f; // Position higher to avoid overlap with advanced shapes
				
				glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
				if (i == 1) color = { 0.0f, 1.0f, 0.0f, 1.0f };
				if (i == 2) color = { 0.0f, 0.0f, 1.0f, 1.0f };
				
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.5f, 0.5f }, color);
				IncrementObjectCount();
				
				// Draw bounding box if enabled
				if (config.showBoundingBoxes)
				{
					DrawBoundingBox({ x, y, 0.0f }, { 0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f, 0.5f });
				}
			}
		}
		else
		{
		// ZG_CORE_TRACE("Advanced Shapes Scene: Skipping quads (showQuads=false)");
		}

		if (config.showLines)
		{
			ZG_CORE_TRACE("Advanced Shapes Scene: Rendering lines (showLines=true)");
			// Grid lines
			for (int i = 0; i < 5; i++)
			{
				float x = -1.0f + i * 0.5f;
				Zgine::BatchRenderer2D::DrawLine({ x, -1.5f, 0.0f }, { x, 1.5f, 0.0f }, 
					{ 0.5f, 0.5f, 0.5f, 1.0f }, config.lineThickness * 0.5f);
				IncrementObjectCount();
			}
		}
		else
		{
		// ZG_CORE_TRACE("Advanced Shapes Scene: Skipping lines (showLines=false)");
		}

		if (config.showCircles)
		{
			ZG_CORE_TRACE("Advanced Shapes Scene: Rendering circles (showCircles=true)");
			// Static filled circles
			for (int i = 0; i < 2; i++)
			{
				float x = -0.5f + i * 1.0f;
				float y = -1.5f; // Position lower to avoid overlap
				
				glm::vec4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
				if (i == 1) color = { 1.0f, 0.0f, 1.0f, 1.0f };
				
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, config.circleRadius, 
					color, config.circleSegments);
				IncrementObjectCount();
				
				// Draw bounding box if enabled
				if (config.showBoundingBoxes)
				{
					DrawBoundingBox({ x, y, 0.0f }, { config.circleRadius * 2.0f, config.circleRadius * 2.0f }, { 1.0f, 1.0f, 0.0f, 0.5f });
				}
			}
		}
		else
		{
		// ZG_CORE_TRACE("Advanced Shapes Scene: Skipping circles (showCircles=false)");
		}

		// Render triangles
		if (config.showTriangles)
		{
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 1.0f;
				
				// Triangle using DrawTriangle
				Zgine::BatchRenderer2D::DrawTriangle(
					{ x, y + 0.3f, 0.0f },
					{ x - 0.3f, y - 0.3f, 0.0f },
					{ x + 0.3f, y - 0.3f, 0.0f },
					{ 1.0f, 0.5f, 0.0f, 1.0f }
				);
				IncrementObjectCount();
			}
		}

		// Render ellipses
		if (config.showEllipses)
		{
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 0.0f;
				
				// Filled ellipse
				Zgine::BatchRenderer2D::DrawEllipse({ x, y, 0.0f }, 
					config.circleRadius * 1.5f, config.circleRadius * 0.8f, 
					{ 0.0f, 1.0f, 0.0f, 1.0f }, config.ellipseSegments);
				IncrementObjectCount();
				
				// Ellipse outline
				Zgine::BatchRenderer2D::DrawEllipseOutline({ x, y - 0.8f, 0.0f }, 
					config.circleRadius * 1.2f, config.circleRadius * 0.6f, 
					{ 0.0f, 1.0f, 1.0f, 1.0f }, config.lineThickness, config.ellipseSegments);
				IncrementObjectCount();
			}
		}

		// Render arcs
		if (config.showArcs)
		{
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = -1.0f;
				
				// Different arc angles
				float startAngle = i * 1.0f;
				float endAngle = startAngle + 2.0f;
				
				Zgine::BatchRenderer2D::DrawArc({ x, y, 0.0f }, config.circleRadius, 
					startAngle, endAngle, { 1.0f, 0.0f, 1.0f, 1.0f }, 
					config.lineThickness, config.circleSegments);
				IncrementObjectCount();
			}
		}

		// Render gradients
		if (config.showGradients)
		{
			for (int i = 0; i < 2; i++)
			{
				float x = -0.5f + i * 1.0f;
				float y = 0.5f;
				
				// Gradient quad
				Zgine::BatchRenderer2D::DrawQuadGradient({ x, y, 0.0f }, { 0.6f, 0.6f },
					{ 1.0f, 0.0f, 0.0f, 1.0f },  // Top Left - Red
					{ 0.0f, 1.0f, 0.0f, 1.0f },  // Top Right - Green
					{ 0.0f, 0.0f, 1.0f, 1.0f },  // Bottom Left - Blue
					{ 1.0f, 1.0f, 0.0f, 1.0f }   // Bottom Right - Yellow
				);
				IncrementObjectCount();
				
				// Rotated gradient quad
				float rotation = m_Time * config.animationSpeed;
				Zgine::BatchRenderer2D::DrawRotatedQuadGradient({ x, y - 0.8f, 0.0f }, { 0.4f, 0.4f }, rotation,
					{ 1.0f, 1.0f, 1.0f, 1.0f },  // Top Left - White
					{ 0.0f, 0.0f, 0.0f, 1.0f },  // Top Right - Black
					{ 0.5f, 0.5f, 0.5f, 1.0f },  // Bottom Left - Gray
					{ 0.8f, 0.8f, 0.8f, 1.0f }   // Bottom Right - Light Gray
				);
				IncrementObjectCount();
			}
		}

		// Advanced Effects - Special visual effects when enabled
		if (config.showAdvanced)
		{
			// Pulsing circles with rainbow colors
			for (int i = 0; i < 5; i++)
			{
				float x = -2.0f + i * 1.0f;
				float y = -0.5f;
				float pulse = 0.3f + 0.2f * sin(m_Time * config.animationSpeed * 2.0f + i);
				
				// Rainbow color cycling
				float hue = (m_Time * config.animationSpeed + i) * 0.5f;
				glm::vec4 color = {
					0.5f + 0.5f * sin(hue),
					0.5f + 0.5f * sin(hue + 2.094f), // 120 degrees
					0.5f + 0.5f * sin(hue + 4.188f), // 240 degrees
					1.0f
				};
				
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, pulse, color, config.circleSegments);
				IncrementObjectCount();
			}
			
			// Spinning star pattern
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = 1.5f;
				float rotation = m_Time * config.animationSpeed * 90.0f + i * 120.0f;
				
				// Create a star using multiple triangles
				for (int j = 0; j < 5; j++)
				{
					float angle = rotation + j * 72.0f; // 72 degrees between star points
					float radius = 0.2f;
					float px = x + radius * cos(glm::radians(angle));
					float py = y + radius * sin(glm::radians(angle));
					
					Zgine::BatchRenderer2D::DrawTriangle(
						{ x, y, 0.0f },
						{ px, py, 0.0f },
						{ x + radius * 0.3f * cos(glm::radians(angle + 36.0f)), 
						  y + radius * 0.3f * sin(glm::radians(angle + 36.0f)), 0.0f },
						{ 1.0f, 1.0f, 0.0f, 0.8f }
					);
					IncrementObjectCount();
				}
			}
		}
	}

	void Test2DModule::RenderAnimatedShapesScene(const Test2DConfig& config)
	{
		// Animated circles
		if (config.showCircles && (config.animateCircles || config.animateAll))
		{
			for (int i = 0; i < 5; i++)
			{
				float x = -2.0f + i * 1.0f;
				float y = sin(m_Time * config.animationSpeed + i) * 0.5f;
				float radius = config.circleRadius * (0.5f + 0.3f * sin(m_Time * config.animationSpeed * 1.5f + i));
				
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, radius, 
					{ 0.0f, 1.0f, 1.0f, 1.0f }, config.circleSegments);
				IncrementObjectCount();
			}
		}

		// Animated quads
		if (config.showQuads && (config.animateQuads || config.animateAll))
		{
			for (int i = 0; i < 4; i++)
			{
				float x = -1.5f + i * 1.0f;
				float y = cos(m_Time * config.animationSpeed + i) * 0.3f;
				float scale = 0.3f + sin(m_Time * config.animationSpeed * 2.0f + i) * 0.2f;
				float rotation = m_Time * config.animationSpeed * 45.0f + i * 90.0f;
				
				Zgine::BatchRenderer2D::DrawRotatedQuad({ x, y, 0.0f }, { scale, scale }, rotation,
					{ 1.0f, 0.5f, 0.0f, 1.0f });
				IncrementObjectCount();
			}
		}

		// Animated triangles
		if (config.showTriangles)
		{
			for (int i = 0; i < 3; i++)
			{
				float x = -1.0f + i * 1.0f;
				float y = -1.0f + sin(m_Time * config.animationSpeed * 0.8f + i) * 0.4f;
				float offset = sin(m_Time * config.animationSpeed + i) * 0.1f;
				
				Zgine::BatchRenderer2D::DrawTriangle(
					{ x, y + 0.3f + offset, 0.0f },
					{ x - 0.3f, y - 0.3f - offset, 0.0f },
					{ x + 0.3f, y - 0.3f - offset, 0.0f },
					{ 1.0f, 0.0f, 1.0f, 1.0f }
				);
				IncrementObjectCount();
			}
		}

		// Animated ellipses
		if (config.showEllipses)
		{
			for (int i = 0; i < 2; i++)
			{
				float x = -0.5f + i * 1.0f;
				float y = 0.5f + cos(m_Time * config.animationSpeed * 1.2f + i) * 0.2f;
				float radiusX = config.circleRadius * (1.0f + 0.5f * sin(m_Time * config.animationSpeed + i));
				float radiusY = config.circleRadius * (0.8f + 0.3f * cos(m_Time * config.animationSpeed * 1.5f + i));
				
				Zgine::BatchRenderer2D::DrawEllipse({ x, y, 0.0f }, radiusX, radiusY, 
					{ 0.0f, 1.0f, 0.0f, 1.0f }, config.circleSegments);
				IncrementObjectCount();
			}
		}

		// Animated gradients
		if (config.showGradients)
		{
			float x = 0.0f;
			float y = -0.5f;
			float rotation = m_Time * config.animationSpeed * 30.0f;
			
			// Color cycling gradient
			float colorCycle = sin(m_Time * config.animationSpeed * 0.5f) * 0.5f + 0.5f;
			glm::vec4 color1 = { colorCycle, 1.0f - colorCycle, 0.5f, 1.0f };
			glm::vec4 color2 = { 1.0f - colorCycle, colorCycle, 0.5f, 1.0f };
			glm::vec4 color3 = { 0.5f, colorCycle, 1.0f - colorCycle, 1.0f };
			glm::vec4 color4 = { 1.0f - colorCycle, 0.5f, colorCycle, 1.0f };
			
			Zgine::BatchRenderer2D::DrawRotatedQuadGradient({ x, y, 0.0f }, { 0.8f, 0.8f }, rotation,
				color1, color2, color3, color4);
			IncrementObjectCount();
		}
	}

	void Test2DModule::RenderPerformanceTestScene(const Test2DConfig& config)
	{
		// Performance test with different object counts based on configuration
		int objectCount = 100;
		if (config.showAdvanced) objectCount = 500;
		if (config.showGradients) objectCount = 1000;
		
		// Random number generation for performance testing
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> disPos(-4.0f, 4.0f);
		std::uniform_real_distribution<float> disColor(0.0f, 1.0f);
		std::uniform_real_distribution<float> disSize(0.05f, 0.3f);
		std::uniform_real_distribution<float> disRotation(0.0f, 360.0f);

		// Render quads for performance testing
		if (config.showQuads)
		{
			for (int i = 0; i < objectCount / 4; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float size = disSize(gen);
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { size, size }, color);
				IncrementObjectCount();
			}
		}

		// Render rotated quads for performance testing
		if (config.showAdvanced)
		{
			for (int i = 0; i < objectCount / 4; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float size = disSize(gen);
				float rotation = disRotation(gen);
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawRotatedQuad({ x, y, 0.0f }, { size, size }, rotation, color);
				IncrementObjectCount();
			}
		}

		// Render circles for performance testing
		if (config.showCircles)
		{
			for (int i = 0; i < objectCount / 4; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float radius = disSize(gen) * 0.5f;
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, radius, color, config.circleSegments);
				IncrementObjectCount();
			}
		}

		// Render triangles for performance testing
		if (config.showTriangles)
		{
			for (int i = 0; i < objectCount / 8; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float size = disSize(gen);
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawTriangle(
					{ x, y + size, 0.0f },
					{ x - size, y - size, 0.0f },
					{ x + size, y - size, 0.0f },
					color
				);
				IncrementObjectCount();
			}
		}

		// Render ellipses for performance testing
		if (config.showEllipses)
		{
			for (int i = 0; i < objectCount / 6; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float radiusX = disSize(gen);
				float radiusY = disSize(gen) * 0.7f;
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawEllipse({ x, y, 0.0f }, radiusX, radiusY, color, config.circleSegments);
				IncrementObjectCount();
			}
		}

		// Render gradients for performance testing (most expensive)
		if (config.showGradients)
		{
			for (int i = 0; i < objectCount / 10; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float size = disSize(gen);
				float rotation = disRotation(gen);
				
				glm::vec4 color1 = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				glm::vec4 color2 = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				glm::vec4 color3 = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				glm::vec4 color4 = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawRotatedQuadGradient({ x, y, 0.0f }, { size, size }, rotation,
					color1, color2, color3, color4);
				IncrementObjectCount();
			}
		}

		// Render lines for performance testing
		if (config.showLines)
		{
			for (int i = 0; i < objectCount / 2; i++)
			{
				float x1 = disPos(gen);
				float y1 = disPos(gen);
				float x2 = disPos(gen);
				float y2 = disPos(gen);
				float thickness = disSize(gen) * 0.1f;
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawLine({ x1, y1, 0.0f }, { x2, y2, 0.0f }, color, thickness);
				IncrementObjectCount();
			}
		}

		// Render arcs for performance testing
		if (config.showArcs)
		{
			for (int i = 0; i < objectCount / 8; i++)
			{
				float x = disPos(gen);
				float y = disPos(gen);
				float radius = disSize(gen) * 0.5f;
				float startAngle = disRotation(gen) * 3.14159f / 180.0f;
				float endAngle = startAngle + (disRotation(gen) * 3.14159f / 180.0f);
				float thickness = disSize(gen) * 0.05f;
				glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
				
				Zgine::BatchRenderer2D::DrawArc({ x, y, 0.0f }, radius, startAngle, endAngle, 
					color, thickness, config.circleSegments);
				IncrementObjectCount();
			}
		}
	}

	void Test2DModule::RenderDefaultTestPattern()
	{
		// Render a minimal test pattern to show that the renderer is working
		// This helps users understand that they need to enable rendering options
		
		// Draw a simple crosshair in the center
		Zgine::BatchRenderer2D::DrawLine({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 
			{ 0.5f, 0.5f, 0.5f, 1.0f }, 0.02f);
		Zgine::BatchRenderer2D::DrawLine({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 
			{ 0.5f, 0.5f, 0.5f, 1.0f }, 0.02f);
		IncrementObjectCount(2);
		
		// Draw a simple circle in the center to indicate the renderer is working
		Zgine::BatchRenderer2D::DrawCircle({ 0.0f, 0.0f, 0.0f }, 0.3f, { 0.3f, 0.3f, 0.3f, 0.3f }, 16);
		IncrementObjectCount();
		
		// Note: No quads are rendered by default to avoid confusion with checkboxes
	}

	void Test2DModule::DrawBoundingBox(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		// Draw bounding box outline using lines
		float halfWidth = size.x * 0.5f;
		float halfHeight = size.y * 0.5f;
		
		// Top edge
		Zgine::BatchRenderer2D::DrawLine(
			{ position.x - halfWidth, position.y + halfHeight, position.z },
			{ position.x + halfWidth, position.y + halfHeight, position.z },
			color, 0.01f
		);
		
		// Right edge
		Zgine::BatchRenderer2D::DrawLine(
			{ position.x + halfWidth, position.y + halfHeight, position.z },
			{ position.x + halfWidth, position.y - halfHeight, position.z },
			color, 0.01f
		);
		
		// Bottom edge
		Zgine::BatchRenderer2D::DrawLine(
			{ position.x + halfWidth, position.y - halfHeight, position.z },
			{ position.x - halfWidth, position.y - halfHeight, position.z },
			color, 0.01f
		);
		
		// Left edge
		Zgine::BatchRenderer2D::DrawLine(
			{ position.x - halfWidth, position.y - halfHeight, position.z },
			{ position.x - halfWidth, position.y + halfHeight, position.z },
			color, 0.01f
		);
		
		IncrementObjectCount(4);
	}

}
