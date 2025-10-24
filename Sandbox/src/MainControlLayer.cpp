#include "MainControlLayer.h"
#include "Zgine/Log.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Application.h"

namespace Sandbox {

	MainControlLayer::MainControlLayer()
	{
		ZG_CORE_INFO("MainControlLayer created!");
	}

	MainControlLayer::~MainControlLayer()
	{
		ZG_CORE_INFO("MainControlLayer destroyed!");
	}

	void MainControlLayer::OnAttach()
	{
		ZG_CORE_INFO("MainControlLayer attached!");
		
		// Initialize all managers
		m_UIManager.OnAttach();
		m_RenderManager.OnAttach();
		m_SceneManager.OnAttach();
		m_SettingsManager.OnAttach();
		
		// Set manager references
		m_UIManager.SetRenderManager(&m_RenderManager);
		m_UIManager.SetSceneManager(&m_SceneManager);
		m_UIManager.SetSettingsManager(&m_SettingsManager);
		m_UIManager.SetTest2DModule(&m_RenderManager.GetTest2DModule());
		m_UIManager.SetTest3DModule(&m_RenderManager.GetTest3DModule());
		
		// Set scene manager references
		m_SceneManager.SetTest2DModule(&m_RenderManager.GetTest2DModule());
		m_SceneManager.SetTest3DModule(&m_RenderManager.GetTest3DModule());
		
		// Initialize performance monitoring
		if (m_PerformanceMonitoringEnabled)
		{
			Zgine::PerformanceBenchmark::Init();
			ZG_CORE_INFO("Performance monitoring initialized");
		}
	}

	void MainControlLayer::OnUpdate(Zgine::Timestep ts)
	{
		// Update all managers
		m_UIManager.OnUpdate(ts);
		m_RenderManager.OnUpdate(ts);
		m_SceneManager.OnUpdate(ts);
		m_SettingsManager.OnUpdate(ts);
		
		// Render test modules
		m_RenderManager.Render2D();
		m_RenderManager.Render3D();
		
		// Update performance monitoring
		if (m_PerformanceMonitoringEnabled)
		{
			// Performance monitoring is handled by PerformanceBenchmark internally
		}
	}

	void MainControlLayer::OnImGuiRender()
	{
		// Render all UI components
		m_UIManager.OnImGuiRender();
		m_SceneManager.OnImGuiRender();
		m_SettingsManager.OnImGuiRender();

		// Render performance monitoring
		if (m_PerformanceMonitoringEnabled)
		{
			// Performance monitoring UI is handled by PerformanceMonitorUI
		}
	}

	void MainControlLayer::OnEvent(Zgine::Event& e)
	{
		// Forward events to managers
		m_UIManager.OnEvent(e);
		m_RenderManager.OnEvent(e);

		// Handle specific events
		Zgine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Zgine::WindowResizeEvent>([this](Zgine::WindowResizeEvent& e) { 
			return OnWindowResize(e); 
		});
	}

	bool MainControlLayer::OnWindowResize(Zgine::WindowResizeEvent& e)
	{
		// Handle window resize
		ZG_CORE_INFO("Window resized to: {}x{}", e.GetWidth(), e.GetHeight());
		return false;
	}

}