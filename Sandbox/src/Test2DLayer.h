#pragma once

#include "Zgine/Layer.h"
#include "Testing/Test2DModule.h"
#include "imgui.h"

namespace Sandbox {

	class Test2DLayer : public Zgine::Layer
	{
	public:
		Test2DLayer();
		virtual ~Test2DLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Zgine::Event& e) override;

		// Test module access
		Test2DModule& GetTest2DModule() { return m_Test2DModule; }
		const Test2DModule& GetTest2DModule() const { return m_Test2DModule; }

	private:
		Test2DModule m_Test2DModule;
	};

}