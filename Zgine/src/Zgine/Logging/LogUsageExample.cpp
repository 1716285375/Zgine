/**
 * @file LogUsageExample.cpp
 * @brief Examples of how to use the enhanced logging system
 * @details This file demonstrates various features of the optimized logging system
 */

#include "zgpch.h"
#include "Log.h"
#include "LogConfigExample.h"
#include <thread>
#include <chrono>

namespace Zgine {
namespace Examples {

/**
 * @brief Demonstrate basic logging functionality
 */
void BasicLoggingExample() {
    ZG_CORE_INFO("=== Basic Logging Example ===");
    
    // Basic logging with different levels
    ZG_CORE_TRACE("This is a trace message");
    ZG_CORE_DEBUG("This is a debug message");
    ZG_CORE_INFO("This is an info message");
    ZG_CORE_WARN("This is a warning message");
    ZG_CORE_ERROR("This is an error message");
    ZG_CORE_FATAL("This is a fatal message");
    
    // Client logging
    ZG_INFO("This is a client info message");
    ZG_WARN("This is a client warning message");
    ZG_ERROR("This is a client error message");
}

/**
 * @brief Demonstrate category-specific logging
 */
void CategoryLoggingExample() {
    ZG_CORE_INFO("=== Category Logging Example ===");
    
    // Renderer logging
    ZG_RENDERER_TRACE("Initializing renderer...");
    ZG_RENDERER_INFO("Renderer initialized successfully");
    ZG_RENDERER_WARN("Low memory warning in renderer");
    ZG_RENDERER_ERROR("Failed to create shader");
    
    // Audio logging
    ZG_AUDIO_TRACE("Loading audio file...");
    ZG_AUDIO_INFO("Audio file loaded successfully");
    ZG_AUDIO_WARN("Audio buffer underrun detected");
    ZG_AUDIO_ERROR("Failed to initialize audio device");
    
    // ECS logging
    ZG_ECS_TRACE("Creating entity...");
    ZG_ECS_INFO("Entity created with ID: {}", 12345);
    ZG_ECS_WARN("Entity limit approaching");
    ZG_ECS_ERROR("Failed to create entity");
}

/**
 * @brief Demonstrate performance logging
 */
void PerformanceLoggingExample() {
    ZG_CORE_INFO("=== Performance Logging Example ===");
    
    // Performance timing
    ZG_PERF_START(render_loop);
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    ZG_PERF_END(render_loop, Zgine::Log::GetCoreLogger());
    
    // Multiple performance measurements
    ZG_PERF_START(physics_update);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    ZG_PERF_END(physics_update, Zgine::Log::GetCoreLogger());
    
    ZG_PERF_START(audio_processing);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    ZG_PERF_END(audio_processing, Zgine::Log::GetCoreLogger());
}

/**
 * @brief Demonstrate conditional logging
 */
void ConditionalLoggingExample() {
    ZG_CORE_INFO("=== Conditional Logging Example ===");
    
    bool debugMode = true;
    bool verboseMode = false;
    int errorCount = 0;
    
    // Conditional logging based on flags
    ZG_CORE_TRACE_IF(debugMode, "Debug mode is enabled");
    ZG_CORE_DEBUG_IF(verboseMode, "Verbose mode is enabled");
    ZG_CORE_INFO_IF(errorCount > 0, "Error count: {}", errorCount);
    
    // Conditional logging based on conditions
    ZG_CORE_TRACE_IF(errorCount == 0, "No errors detected");
    ZG_CORE_WARN_IF(errorCount > 5, "High error count: {}", errorCount);
}

/**
 * @brief Demonstrate custom logger creation
 */
void CustomLoggerExample() {
    ZG_CORE_INFO("=== Custom Logger Example ===");
    
    // Create custom loggers
    auto networkLogger = Zgine::Log::CreateLogger("Network", LogCategory::Network);
    auto physicsLogger = Zgine::Log::CreateLogger("Physics", LogCategory::Physics);
    auto scriptingLogger = Zgine::Log::CreateLogger("Scripting", LogCategory::Scripting);
    
    // Use custom loggers
    networkLogger->info("Network connection established");
    networkLogger->warn("Network latency high: {}ms", 150);
    networkLogger->error("Network connection lost");
    
    physicsLogger->trace("Physics simulation started");
    physicsLogger->info("Physics objects: {}", 1000);
    physicsLogger->warn("Physics simulation slow");
    
    scriptingLogger->debug("Script loaded: {}", "player_controller.lua");
    scriptingLogger->info("Script execution completed");
    scriptingLogger->error("Script error: {}", "undefined variable");
}

/**
 * @brief Demonstrate runtime configuration changes
 */
void RuntimeConfigurationExample() {
    ZG_CORE_INFO("=== Runtime Configuration Example ===");
    
    // Change log levels at runtime
    ZG_CORE_INFO("Changing console level to WARN");
    Zgine::Log::SetConsoleLevel(LogLevel::Warn);
    
    ZG_CORE_TRACE("This trace message should not appear in console");
    ZG_CORE_INFO("This info message should not appear in console");
    ZG_CORE_WARN("This warning message should appear in console");
    ZG_CORE_ERROR("This error message should appear in console");
    
    // Disable console output
    ZG_CORE_INFO("Disabling console output");
    Zgine::Log::EnableConsoleOutput(false);
    
    ZG_CORE_INFO("This message should not appear in console");
    
    // Re-enable console output
    ZG_CORE_INFO("Re-enabling console output");
    Zgine::Log::EnableConsoleOutput(true);
    
    // Reset to default level
    Zgine::Log::SetConsoleLevel(LogLevel::Info);
    ZG_CORE_INFO("Console level reset to INFO");
}

/**
 * @brief Demonstrate different configuration scenarios
 */
void ConfigurationExample() {
    ZG_CORE_INFO("=== Configuration Example ===");
    
    // Note: In a real application, you would call Log::Init() with different configs
    // Here we just show the available configurations
    
    ZG_CORE_INFO("Available configurations:");
    ZG_CORE_INFO("- DevelopmentConfig: Verbose logging for debugging");
    ZG_CORE_INFO("- ProductionConfig: Minimal logging for performance");
    ZG_CORE_INFO("- DebugConfig: Maximum verbosity for troubleshooting");
    ZG_CORE_INFO("- PerformanceConfig: Minimal overhead for performance testing");
    ZG_CORE_INFO("- ConsoleOnlyConfig: Simple console-only logging");
    
    // Example of how to use different configurations:
    /*
    // For development
    Zgine::Log::Init(Zgine::Examples::LogConfigExamples::DevelopmentConfig());
    
    // For production
    Zgine::Log::Init(Zgine::Examples::LogConfigExamples::ProductionConfig());
    
    // For debugging
    Zgine::Log::Init(Zgine::Examples::LogConfigExamples::DebugConfig());
    */
}

/**
 * @brief Demonstrate multi-threaded logging
 */
void MultiThreadedLoggingExample() {
    ZG_CORE_INFO("=== Multi-threaded Logging Example ===");
    
    // Create multiple threads that log simultaneously
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 3; ++j) {
                ZG_CORE_INFO("Thread {} - Message {}", i, j);
                ZG_RENDERER_INFO("Thread {} - Renderer message {}", i, j);
                ZG_AUDIO_INFO("Thread {} - Audio message {}", i, j);
                
                // Small delay to simulate work
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    ZG_CORE_INFO("Multi-threaded logging example completed");
}

/**
 * @brief Run all logging examples
 */
void RunAllExamples() {
    ZG_CORE_INFO("Starting logging system examples...");
    
    BasicLoggingExample();
    CategoryLoggingExample();
    PerformanceLoggingExample();
    ConditionalLoggingExample();
    CustomLoggerExample();
    RuntimeConfigurationExample();
    ConfigurationExample();
    MultiThreadedLoggingExample();
    
    ZG_CORE_INFO("All logging examples completed!");
}

} // namespace Examples
} // namespace Zgine
