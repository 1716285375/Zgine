#pragma once

#include "Zgine/Renderer/PerformanceBenchmark.h"
#include "../Testing/Test2DModule.h"
#include "../Testing/Test3DModule.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

namespace Sandbox {

	struct StressTestConfig
	{
		int objectCount = 1000;
		float duration = 10.0f;
		bool isRunning = false;
		float startTime = 0.0f;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec4> colors;
	};

	struct ExampleScene
	{
		std::string name;
		std::string description;
		std::function<void()> renderFunction;
		bool isRunning = false;
	};

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void OnAttach();
		void OnUpdate(float ts);
		void OnImGuiRender();

		// Test module access
		void SetTest2DModule(Test2DModule* test2DModule) { m_Test2DModule = test2DModule; }
		void SetTest3DModule(Test3DModule* test3DModule) { m_Test3DModule = test3DModule; }

		// Stress Testing
		void StartStressTest();
		void StopStressTest();
		void UpdateStressTest(float ts);
		void RenderStressTest();
		bool IsStressTestRunning() const { return m_StressTest.isRunning; }

		// Example Scenes
		void RegisterExampleScene(const std::string& name, const std::string& description, 
			std::function<void()> renderFunction);
		void StartExampleScene(int index);
		void StopExampleScene();
		void RenderExampleScenes();
		int GetCurrentSceneIndex() const { return m_CurrentSceneIndex; }
		const std::vector<ExampleScene>& GetExampleScenes() const { return m_ExampleScenes; }

		// Performance
		void UpdatePerformance(float ts);
		float GetFPS() const { return m_FPS; }
		int GetFrameCount() const { return m_FrameCount; }

		// Scene switching
		void SwitchTo2DScene(const std::string& sceneName);
		void SwitchTo3DScene(const std::string& sceneName);
		std::vector<std::string> GetAvailable2DScenes() const;
		std::vector<std::string> GetAvailable3DScenes() const;

	private:
		StressTestConfig m_StressTest;
		std::vector<ExampleScene> m_ExampleScenes;
		int m_CurrentSceneIndex = 0;

		// Test module references
		Test2DModule* m_Test2DModule = nullptr;
		Test3DModule* m_Test3DModule = nullptr;

		// Performance tracking
		float m_FPS = 0.0f;
		int m_FrameCount = 0;
		float m_FPSTimer = 0.0f;

		void InitializeExampleScenes();
		void GenerateStressTestData();
		glm::vec4 GetRandomColor() const;
		glm::vec3 GetRandomPosition() const;
	};

}
