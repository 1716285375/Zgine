#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Zgine/Log.h"


namespace Zgine {
	Application::Application()
	{

	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		ZG_TRACE(e.ToString());  // ÏÔÊ½×ª»»Îª×Ö·û´®

		while (true);
	}
}
