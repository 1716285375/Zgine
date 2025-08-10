#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Zgine {
	class ZG_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}

