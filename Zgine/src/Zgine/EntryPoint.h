#pragma once


#ifdef ZG_PLATFORM_WINDOWS

extern Zgine::Application* Zgine::CreateApplication();

int main(int argc, char** argv) {
	Zgine::Log::Init();
	ZG_CORE_WARN("Initialized Log!");
	ZG_INFO("Hello Zgine! {}", "nihao");

	auto app = Zgine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif