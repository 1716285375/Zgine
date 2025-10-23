#pragma once

#include "zgpch.h"
#include "../Core.h"
#include "PerformanceProfiler.h"
#include <chrono>
#include <vector>
#include <string>
#include <functional>

namespace Zgine {

	/**
	 * @brief Performance benchmark system for testing rendering performance
	 * @details Provides comprehensive benchmarking tools for measuring and comparing
	 *          rendering performance across different scenarios and optimizations
	 */
	class PerformanceBenchmark
	{
	public:
		struct BenchmarkResult
		{
			std::string testName;
			double averageFrameTime;      // Average frame time in milliseconds
			double minFrameTime;         // Minimum frame time in milliseconds
			double maxFrameTime;         // Maximum frame time in milliseconds
			double averageFPS;           // Average FPS
			double minFPS;              // Minimum FPS
			double maxFPS;              // Maximum FPS
			size_t totalFrames;         // Total frames rendered
			size_t memoryUsage;         // Memory usage in bytes
			double cpuUsage;            // CPU usage percentage
			double gpuUsage;            // GPU usage percentage
			std::vector<double> frameTimes; // Individual frame times
		};

		struct TestScenario
		{
			std::string name;
			std::function<void()> setup;
			std::function<void()> render;
			std::function<void()> cleanup;
			uint32_t duration;          // Test duration in seconds
			uint32_t targetFPS;        // Target FPS for the test
		};

		/**
		 * @brief Initialize the benchmark system
		 */
		static void Init();

		/**
		 * @brief Shutdown the benchmark system
		 */
		static void Shutdown();

		/**
		 * @brief Run a single benchmark test
		 * @param scenario The test scenario to run
		 * @return Benchmark result
		 */
		static BenchmarkResult RunBenchmark(const TestScenario& scenario);

		/**
		 * @brief Run multiple benchmark tests and compare results
		 * @param scenarios Vector of test scenarios
		 * @return Vector of benchmark results
		 */
		static std::vector<BenchmarkResult> RunBenchmarkSuite(const std::vector<TestScenario>& scenarios);

		/**
		 * @brief Run stress test with increasing load
		 * @param baseScenario Base scenario to stress test
		 * @param maxMultiplier Maximum load multiplier
		 * @param step Step size for load increase
		 * @return Vector of results for different load levels
		 */
		static std::vector<BenchmarkResult> RunStressTest(const TestScenario& baseScenario, 
			float maxMultiplier = 10.0f, float step = 1.5f);

		/**
		 * @brief Generate performance report
		 * @param results Vector of benchmark results
		 * @return Formatted performance report string
		 */
		static std::string GenerateReport(const std::vector<BenchmarkResult>& results);

		/**
		 * @brief Save benchmark results to file
		 * @param results Vector of benchmark results
		 * @param filename Output filename
		 */
		static void SaveResults(const std::vector<BenchmarkResult>& results, const std::string& filename);

		/**
		 * @brief Load benchmark results from file
		 * @param filename Input filename
		 * @return Vector of benchmark results
		 */
		static std::vector<BenchmarkResult> LoadResults(const std::string& filename);

		/**
		 * @brief Get predefined test scenarios
		 * @return Vector of common test scenarios
		 */
		static std::vector<TestScenario> GetPredefinedScenarios();

		/**
		 * @brief Start real-time performance monitoring
		 * @param duration Monitoring duration in seconds (0 = infinite)
		 */
		static void StartRealTimeMonitoring(uint32_t duration = 0);

		/**
		 * @brief Stop real-time performance monitoring
		 */
		static void StopRealTimeMonitoring();

		/**
		 * @brief Get current real-time performance stats
		 * @return Current performance statistics
		 */
		static BenchmarkResult GetCurrentStats();

		/**
		 * @brief Check if real-time monitoring is active
		 * @return True if monitoring is active
		 */
		static bool IsMonitoringActive();

	private:
		static bool s_Initialized;
		static bool s_MonitoringActive;
		static std::chrono::high_resolution_clock::time_point s_MonitoringStartTime;
		static std::vector<double> s_CurrentFrameTimes;
		static std::mutex s_MonitoringMutex;

		/**
		 * @brief Measure frame time for a single frame
		 * @param renderFunction Function to render a frame
		 * @return Frame time in milliseconds
		 */
		static double MeasureFrameTime(const std::function<void()>& renderFunction);

		/**
		 * @brief Get current memory usage
		 * @return Memory usage in bytes
		 */
		static size_t GetCurrentMemoryUsage();

		/**
		 * @brief Get current CPU usage
		 * @return CPU usage percentage
		 */
		static double GetCurrentCPUUsage();

		/**
		 * @brief Get current GPU usage
		 * @return GPU usage percentage
		 */
		static double GetCurrentGPUUsage();

		/**
		 * @brief Calculate statistics from frame times
		 * @param frameTimes Vector of frame times
		 * @return Calculated statistics
		 */
		static BenchmarkResult CalculateStats(const std::vector<double>& frameTimes, const std::string& testName);
	};

	/**
	 * @brief Predefined benchmark scenarios for common use cases
	 */
	namespace BenchmarkScenarios {

		/**
		 * @brief Basic 2D rendering test
		 */
		Zgine::PerformanceBenchmark::TestScenario Basic2DRendering();

		/**
		 * @brief Basic 3D rendering test
		 */
		Zgine::PerformanceBenchmark::TestScenario Basic3DRendering();

		/**
		 * @brief High quad count 2D test
		 */
		Zgine::PerformanceBenchmark::TestScenario HighQuadCount2D(uint32_t quadCount = 10000);

		/**
		 * @brief High object count 3D test
		 */
		Zgine::PerformanceBenchmark::TestScenario HighObjectCount3D(uint32_t objectCount = 5000);

		/**
		 * @brief Texture switching test
		 */
		Zgine::PerformanceBenchmark::TestScenario TextureSwitching(uint32_t textureCount = 100);

		/**
		 * @brief Shader switching test
		 */
		Zgine::PerformanceBenchmark::TestScenario ShaderSwitching(uint32_t shaderCount = 50);

		/**
		 * @brief Memory stress test
		 */
		Zgine::PerformanceBenchmark::TestScenario MemoryStressTest();

		/**
		 * @brief Particle system stress test
		 */
		Zgine::PerformanceBenchmark::TestScenario ParticleSystemStress(uint32_t particleCount = 100000);

		/**
		 * @brief Mixed rendering test (2D + 3D)
		 */
		Zgine::PerformanceBenchmark::TestScenario MixedRenderingTest();
	}

}
