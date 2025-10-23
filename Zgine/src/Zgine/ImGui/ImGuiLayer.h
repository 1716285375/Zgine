#pragma once

#include "Zgine/Layer.h"

#include "Zgine/Events/KeyEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/Events/ApplicationEvent.h"

namespace Zgine {
	class ZG_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};

}


