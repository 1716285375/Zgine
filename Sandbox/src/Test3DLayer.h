#pragma once

#include "Zgine/Layer.h"
#include "Testing/Test3DModule.h"
#include "imgui.h"

namespace Sandbox {

	class Test3DLayer : public Zgine::Layer
	{
	public:
		Test3DLayer();
		virtual ~Test3DLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Zgine::Event& e) override;

		// Test module access
		Test3DModule& GetTest3DModule() { return m_Test3DModule; }
		const Test3DModule& GetTest3DModule() const { return m_Test3DModule; }

	private:
		Test3DModule m_Test3DModule;
	};

}