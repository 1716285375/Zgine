#pragma once

/**
 * @file EntryPoint.h
 * @brief Application entry point for the Zgine engine
 * @details This file contains the main function and application startup logic
 */

#ifdef ZG_PLATFORM_WINDOWS

extern Zgine::Application* Zgine::CreateApplication();

/**
 * @brief Main application entry point
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return int Application exit code
 * @details Initializes the logging system, creates the application,
 *          runs it, and cleans up resources
 */
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