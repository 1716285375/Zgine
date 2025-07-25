#pragma once

#ifdef ZG_PLATFORM_WINDOWS

extern Zgine::Application* Zgine::CreateApplication();

int main(int argc, char** argv) {
	printf("Zgine Start!\n");
	auto app = Zgine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif