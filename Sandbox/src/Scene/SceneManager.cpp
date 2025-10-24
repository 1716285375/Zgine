#include "SceneManager.h"
#include "Zgine/Renderer/BatchRenderer2D.h"
#include "Zgine/Renderer/BatchRenderer3D.h"
#include "Zgine/Log.h"
#include "../Testing/Test2DModule.h"
#include "../Testing/Test3DModule.h"
#include "imgui.h"
#include <random>

namespace Sandbox {

	SceneManager::SceneManager()
	{
		ZG_CORE_INFO("SceneManager created");
	}

	SceneManager::~SceneManager()
	{
		ZG_CORE_INFO("SceneManager destroyed");
	}

	void SceneManager::OnAttach()
	{
		InitializeExampleScenes();
		ZG_CORE_INFO("SceneManager attached");
	}

	void SceneManager::OnUpdate(float ts)
	{
		UpdatePerformance(ts);
		
		if (m_StressTest.isRunning)
		{
			UpdateStressTest(ts);
		}

		if (m_CurrentSceneIndex >= 0 && m_CurrentSceneIndex < m_ExampleScenes.size())
		{
			if (m_ExampleScenes[m_CurrentSceneIndex].isRunning)
			{
				m_ExampleScenes[m_CurrentSceneIndex].renderFunction();
			}
		}
	}

	void SceneManager::OnImGuiRender()
	{
		// Stress Test Window
		if (ImGui::Begin("Stress Test"))
		{
			ImGui::Text("Stress Test Configuration");
			ImGui::Separator();

			ImGui::SliderInt("Object Count", &m_StressTest.objectCount, 100, 10000);
			ImGui::SliderFloat("Duration (seconds)", &m_StressTest.duration, 1.0f, 60.0f);

			if (!m_StressTest.isRunning)
			{
				if (ImGui::Button("Start Stress Test"))
				{
					StartStressTest();
				}
			}
			else
			{
				if (ImGui::Button("Stop Stress Test"))
				{
					StopStressTest();
				}

				float elapsed = m_FPS - m_StressTest.startTime;
				ImGui::Text("Elapsed: %.2f / %.2f seconds", elapsed, m_StressTest.duration);
				ImGui::ProgressBar(elapsed / m_StressTest.duration);
			}

			ImGui::Separator();
			ImGui::Text("Performance Metrics");
			ImGui::Text("FPS: %.1f", m_FPS);
			ImGui::Text("Frame Count: %d", m_FrameCount);
		}
		ImGui::End();

		// Example Scenes Window
		if (ImGui::Begin("Example Scenes"))
		{
			ImGui::Text("Available Example Scenes");
			ImGui::Separator();

			for (int i = 0; i < m_ExampleScenes.size(); i++)
			{
				const auto& scene = m_ExampleScenes[i];
				
				ImGui::PushID(i);
				if (ImGui::Button(scene.name.c_str()))
				{
					StartExampleScene(i);
				}
				ImGui::PopID();
				
				ImGui::SameLine();
				ImGui::Text("- %s", scene.description.c_str());
			}

			if (m_CurrentSceneIndex >= 0 && m_CurrentSceneIndex < m_ExampleScenes.size())
			{
				const auto& currentScene = m_ExampleScenes[m_CurrentSceneIndex];
				ImGui::Separator();
				ImGui::Text("Current Scene: %s", currentScene.name.c_str());
				
				if (currentScene.isRunning)
				{
					if (ImGui::Button("Stop Scene"))
					{
						StopExampleScene();
					}
				}
				else
				{
					if (ImGui::Button("Start Scene"))
					{
						StartExampleScene(m_CurrentSceneIndex);
					}
				}
			}
		}
		ImGui::End();
	}

	void SceneManager::StartStressTest()
	{
		m_StressTest.isRunning = true;
		m_StressTest.startTime = m_FPS;
		GenerateStressTestData();
		ZG_CORE_INFO("Stress test started with {} objects", m_StressTest.objectCount);
	}

	void SceneManager::StopStressTest()
	{
		m_StressTest.isRunning = false;
		ZG_CORE_INFO("Stress test stopped");
	}

	void SceneManager::UpdateStressTest(float ts)
	{
		float elapsed = m_FPS - m_StressTest.startTime;
		if (elapsed >= m_StressTest.duration)
		{
			StopStressTest();
		}
	}

	void SceneManager::RenderStressTest()
	{
		if (!m_StressTest.isRunning) return;

		for (int i = 0; i < m_StressTest.objectCount; i++)
		{
			const auto& pos = m_StressTest.positions[i];
			const auto& color = m_StressTest.colors[i];
			
			Zgine::BatchRenderer2D::DrawQuad({ pos.x, pos.y, 0.0f }, { 0.1f, 0.1f }, color);
		}
	}

	void SceneManager::RegisterExampleScene(const std::string& name, const std::string& description, 
		std::function<void()> renderFunction)
	{
		m_ExampleScenes.push_back({ name, description, renderFunction, false });
		ZG_CORE_INFO("Registered example scene: {}", name);
	}

	void SceneManager::StartExampleScene(int index)
	{
		if (index >= 0 && index < m_ExampleScenes.size())
		{
			StopExampleScene(); // Stop current scene
			m_CurrentSceneIndex = index;
			m_ExampleScenes[index].isRunning = true;
			ZG_CORE_INFO("Started example scene: {}", m_ExampleScenes[index].name);
		}
	}

	void SceneManager::StopExampleScene()
	{
		if (m_CurrentSceneIndex >= 0 && m_CurrentSceneIndex < m_ExampleScenes.size())
		{
			m_ExampleScenes[m_CurrentSceneIndex].isRunning = false;
		}
	}

	void SceneManager::RenderExampleScenes()
	{
		if (m_CurrentSceneIndex >= 0 && m_CurrentSceneIndex < m_ExampleScenes.size())
		{
			if (m_ExampleScenes[m_CurrentSceneIndex].isRunning)
			{
				m_ExampleScenes[m_CurrentSceneIndex].renderFunction();
			}
		}
	}

	void SceneManager::UpdatePerformance(float ts)
	{
		m_FrameCount++;
		m_FPSTimer += ts;
		
		if (m_FPSTimer >= 1.0f)
		{
			m_FPS = m_FrameCount / m_FPSTimer;
			m_FrameCount = 0;
			m_FPSTimer = 0.0f;
		}
	}

	void SceneManager::SwitchTo2DScene(const std::string& sceneName)
	{
		if (m_Test2DModule)
		{
			m_Test2DModule->SetActiveScene(sceneName);
			ZG_CORE_INFO("Switched to 2D scene: {}", sceneName);
		}
	}

	void SceneManager::SwitchTo3DScene(const std::string& sceneName)
	{
		if (m_Test3DModule)
		{
			m_Test3DModule->SetActiveScene(sceneName);
			ZG_CORE_INFO("Switched to 3D scene: {}", sceneName);
		}
	}

	std::vector<std::string> SceneManager::GetAvailable2DScenes() const
	{
		std::vector<std::string> scenes;
		if (m_Test2DModule)
		{
			for (const auto& scene : m_Test2DModule->GetScenes())
			{
				scenes.push_back(scene.GetName());
			}
		}
		return scenes;
	}

	std::vector<std::string> SceneManager::GetAvailable3DScenes() const
	{
		std::vector<std::string> scenes;
		if (m_Test3DModule)
		{
			for (const auto& scene : m_Test3DModule->GetScenes())
			{
				scenes.push_back(scene.GetName());
			}
		}
		return scenes;
	}

	void SceneManager::InitializeExampleScenes()
	{
		// Scene 1: Colorful Grid
		RegisterExampleScene("Colorful Grid", "A grid of colorful squares", [this]() {
			for (int x = 0; x < 10; x++)
			{
				for (int y = 0; y < 10; y++)
				{
					float posX = -2.0f + x * 0.4f;
					float posY = -2.0f + y * 0.4f;
					glm::vec4 color = GetRandomColor();
					Zgine::BatchRenderer2D::DrawQuad({ posX, posY, 0.0f }, { 0.3f, 0.3f }, color);
				}
			}
		});

		// Scene 2: Rotating Circles
		RegisterExampleScene("Rotating Circles", "Circles that rotate around the center", [this]() {
			static float time = 0.0f;
			time += 0.016f; // Assuming 60 FPS
			
			for (int i = 0; i < 8; i++)
			{
				float angle = time + i * 0.785f; // 45 degrees apart
				float radius = 1.0f;
				float x = cos(angle) * radius;
				float y = sin(angle) * radius;
				
				Zgine::BatchRenderer2D::DrawCircle({ x, y, 0.0f }, 0.2f, 
					{ 0.8f, 0.2f, 0.8f, 1.0f }, 16);
			}
		});

		// Scene 3: Particle Explosion
		RegisterExampleScene("Particle Explosion", "Simulated particle explosion", [this]() {
			static float time = 0.0f;
			time += 0.016f;
			
			for (int i = 0; i < 50; i++)
			{
				float angle = i * 0.125f; // 45 degrees apart
				float speed = 2.0f;
				float x = cos(angle) * speed * time;
				float y = sin(angle) * speed * time;
				
				float alpha = 1.0f - (time / 3.0f); // Fade out over 3 seconds
				if (alpha < 0.0f) alpha = 0.0f;
				
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.1f, 0.1f }, 
					{ 1.0f, 0.5f, 0.0f, alpha });
			}
		});
	}

	void SceneManager::GenerateStressTestData()
	{
		m_StressTest.positions.clear();
		m_StressTest.colors.clear();
		
		m_StressTest.positions.reserve(m_StressTest.objectCount);
		m_StressTest.colors.reserve(m_StressTest.objectCount);
		
		for (int i = 0; i < m_StressTest.objectCount; i++)
		{
			m_StressTest.positions.push_back(GetRandomPosition());
			m_StressTest.colors.push_back(GetRandomColor());
		}
	}

	glm::vec4 SceneManager::GetRandomColor() const
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		
		return { dis(gen), dis(gen), dis(gen), 1.0f };
	}

	glm::vec3 SceneManager::GetRandomPosition() const
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> disX(-3.0f, 3.0f);
		static std::uniform_real_distribution<float> disY(-2.0f, 2.0f);
		
		return { disX(gen), disY(gen), 0.0f };
	}

}
