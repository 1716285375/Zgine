#pragma once

#include "Zgine/Layer.h"
#include "Zgine/Core/SmartPointers.h"
#include "UI/UIManager.h"
#include "Rendering/RenderManager.h"
#include "Scene/SceneManager.h"
#include "Settings/SettingsManager.h"
#include "Zgine/Renderer/PerformanceMonitorUI.h"
#include "Zgine/Renderer/PerformanceBenchmark.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Testing/ECSTestLayer.h"

namespace Sandbox {

	class MainControlLayer : public Zgine::Layer
	{
	public:
		MainControlLayer();
		virtual ~MainControlLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Zgine::Event& e) override;

	private:
		// Core managers
		UIManager m_UIManager;
		RenderManager m_RenderManager;
		SceneManager m_SceneManager;
		SettingsManager m_SettingsManager;

		// Performance monitoring
		bool m_PerformanceMonitoringEnabled = true;
		
		// ECS testing
		Zgine::Scope<ECSTestLayer> m_ECSTestLayer;

		// Event handling
		bool OnWindowResize(Zgine::WindowResizeEvent& e);
	};

}