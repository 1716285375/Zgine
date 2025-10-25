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
	// Initialize logging system with development configuration by default
	// You can change this to use different configurations based on build type
	#ifdef ZG_DEBUG
		// Development configuration for debug builds
		Zgine::LogConfig devConfig;
		devConfig.enableConsoleOutput = true;
		devConfig.enableFileOutput = true;
		devConfig.enableRotatingFiles = true;
		devConfig.logDirectory = "logs/dev";
		devConfig.consoleLevel = Zgine::LogLevel::Trace;
		devConfig.fileLevel = Zgine::LogLevel::Trace;
		devConfig.enableColors = true;
		devConfig.enableTimestamps = true;
		devConfig.enableThreadIds = false;
		devConfig.enableSourceLocation = false;
		Zgine::Log::Init(devConfig);
	#else
		// Production configuration for release builds
		Zgine::LogConfig prodConfig;
		prodConfig.enableConsoleOutput = false;
		prodConfig.enableFileOutput = true;
		prodConfig.enableRotatingFiles = true;
		prodConfig.logDirectory = "logs/prod";
		prodConfig.consoleLevel = Zgine::LogLevel::Off;
		prodConfig.fileLevel = Zgine::LogLevel::Warn;
		prodConfig.enableColors = false;
		prodConfig.enableTimestamps = true;
		prodConfig.enableThreadIds = false;
		prodConfig.enableSourceLocation = false;
		Zgine::Log::Init(prodConfig);
	#endif

	ZG_CORE_INFO("Zgine Engine Starting...");
	ZG_CORE_INFO("Log system initialized successfully");

	auto app = Zgine::CreateApplication();
	app->Run();
	
	ZG_CORE_INFO("Application shutting down...");
	delete app;
	
	// Shutdown logging system
	Zgine::Log::Shutdown();
	// 注意：不要在Log::Shutdown()之后使用任何日志宏
	// 因为此时日志系统已经被关闭
	
	return 0;
}

#endif