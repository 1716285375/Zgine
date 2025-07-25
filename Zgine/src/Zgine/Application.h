#pragma once

#include "Core.h"


namespace Zgine {
	class ZG_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}

