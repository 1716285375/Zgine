#pragma once

#include "Zgine/Core.h"
#include "Zgine/Renderer/OrthographicCamera.h"
#include "Zgine/Renderer/BatchRenderer2D.h"
#include "Zgine/Input.h"
#include "Zgine/KeyCodes.h"
#include "Zgine/Events/Event.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <functional>
#include <string>
#include <vector>

namespace Sandbox {

	// 2D Test Configuration
	struct Test2DConfig
	{
		// Render options - Default to false (no rendering by default)
		bool showQuads = false;
		bool showLines = false;
		bool showCircles = false;
		bool showAdvanced = false;
		bool showTriangles = false;
		bool showEllipses = false;
		bool showArcs = false;
		bool showGradients = false;

		// Animation options
		bool animateCircles = false;
		bool animateQuads = false;
		bool animateAll = false;  // Master animation toggle
		bool animationPaused = false;  // Animation pause state
		float animationSpeed = 1.0f;

		// Settings
		float lineThickness = 0.05f;
		float circleRadius = 0.3f;
		int circleSegments = 32;
		int ellipseSegments = 16;

		// Render options
		bool wireframeMode = false;
		bool showBoundingBoxes = false;
		int renderMode = 0;  // 0=Normal, 1=Wireframe, 2=Points

		// Camera settings
		float cameraSpeed = 2.0f;
		glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	};

	// 2D Test Scene
	class Test2DScene
	{
	public:
		Test2DScene(const std::string& name, std::function<void(const Test2DConfig&)> renderFunc)
			: m_Name(name), m_RenderFunction(renderFunc) {}

		const std::string& GetName() const { return m_Name; }
		void Render(const Test2DConfig& config) const { m_RenderFunction(config); }

	private:
		std::string m_Name;
		std::function<void(const Test2DConfig&)> m_RenderFunction;
	};

	// 2D Test Module Manager
	class Test2DModule
	{
	public:
		Test2DModule();
		~Test2DModule();

		void OnAttach();
		void OnUpdate(float ts);
		void OnImGuiRender();
		void OnEvent(Zgine::Event& e);

		// Configuration
		Test2DConfig& GetConfig() { return m_Config; }
		const Test2DConfig& GetConfig() const { return m_Config; }

		// Scene management
		void RegisterScene(const std::string& name, std::function<void(const Test2DConfig&)> renderFunc);
		void SetActiveScene(const std::string& name);
		const std::string& GetActiveScene() const { return m_ActiveScene; }
		const std::vector<Test2DScene>& GetScenes() const { return m_Scenes; }

		// Camera access
		Zgine::OrthographicCamera& GetCamera() { return m_Camera; }
		const Zgine::OrthographicCamera& GetCamera() const { return m_Camera; }

		// Performance
		float GetFPS() const { return m_FPS; }
		int GetObjectCount() const { return m_ObjectCount; }
		void ResetObjectCount() { m_ObjectCount = 0; }
		void IncrementObjectCount(int count = 1) { m_ObjectCount += count; }

		// Rendering
		void BeginScene();
		void EndScene();
		void RenderActiveScene();

		// UI state
		void SetShowConfigWindow(bool show) { m_ShowConfigWindow = show; }
		bool IsConfigWindowVisible() const { return m_ShowConfigWindow; }
		void SetShowPerformanceWindow(bool show) { m_ShowPerformanceWindow = show; }
		bool IsPerformanceWindowVisible() const { return m_ShowPerformanceWindow; }
		void SetShowSceneSelector(bool show) { m_ShowSceneSelector = show; }
		bool IsSceneSelectorVisible() const { return m_ShowSceneSelector; }

	private:
		void UpdateCamera(float ts);
		void UpdateAnimations(float ts);
		void RenderUI();

		// Built-in scenes
		void RenderBasicShapesScene(const Test2DConfig& config);
		void RenderAdvancedShapesScene(const Test2DConfig& config);
		void RenderAnimatedShapesScene(const Test2DConfig& config);
		void RenderPerformanceTestScene(const Test2DConfig& config);
		void RenderDefaultTestPattern();
		
		// Utility methods
		void DrawBoundingBox(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		// UI rendering
		void RenderConfigWindow();
		void RenderPerformanceWindow();
		void RenderSceneSelector();

	private:
		// Core components
		Zgine::OrthographicCamera m_Camera;
		Test2DConfig m_Config;
		float m_Time;

		// Scene management
		std::vector<Test2DScene> m_Scenes;
		std::string m_ActiveScene;

		// Performance tracking
		float m_FPS = 0.0f;
		int m_FrameCount = 0;
		float m_FPSTimer = 0.0f;
		int m_ObjectCount = 0;

		// UI state
		bool m_ShowConfigWindow = true;
		bool m_ShowPerformanceWindow = false;
		bool m_ShowSceneSelector = true;
	};

}
