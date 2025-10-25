#include "sandbox_pch.h"
#include "Test2DLayer.h"
#include "Zgine/Logging/Log.h"

namespace Sandbox {

	Test2DLayer::Test2DLayer()
		: ImGuiLayer()
	{
		ZG_CORE_INFO("Test2DLayer created");
	}

	Test2DLayer::~Test2DLayer()
	{
		ZG_CORE_INFO("Test2DLayer destroyed");
	}

	void Test2DLayer::OnAttach()
	{
		ZG_CORE_INFO("Test2DLayer attached");
		m_Test2DModule.OnAttach();
	}

	void Test2DLayer::OnUpdate(Zgine::Timestep ts)
	{
		m_Test2DModule.OnUpdate(ts);
		
		// Begin 2D rendering
		m_Test2DModule.BeginScene();
		
		// Render 2D test scene
		m_Test2DModule.RenderActiveScene();
		
		// End 2D rendering
		m_Test2DModule.EndScene();
	}

	void Test2DLayer::OnImGuiRender()
	{
		m_Test2DModule.OnImGuiRender();
	}

	void Test2DLayer::OnEvent(Zgine::Event& e)
	{
		m_Test2DModule.OnEvent(e);
	}

}