#include "zgpch.h"
#include "PerformanceBenchmark.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"
#include "PerformanceProfiler.h"
#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace Zgine {

	// Static member definitions
	bool PerformanceBenchmark::s_Initialized = false;
	bool PerformanceBenchmark::s_MonitoringActive = false;
	std::chrono::high_resolution_clock::time_point PerformanceBenchmark::s_MonitoringStartTime;
	std::vector<double> PerformanceBenchmark::s_CurrentFrameTimes;
	std::mutex PerformanceBenchmark::s_MonitoringMutex;

	void PerformanceBenchmark::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("PerformanceBenchmark::Init() called multiple times");
			return;
		}

		// Initialize performance profiler
		PerformanceProfiler::Init();

		s_Initialized = true;
		ZG_CORE_INFO("PerformanceBenchmark::Init() completed");
	}

	void PerformanceBenchmark::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("PerformanceBenchmark::Shutdown() called without initialization");
			return;
		}

		StopRealTimeMonitoring();
		PerformanceProfiler::Shutdown();

		s_Initialized = false;
		ZG_CORE_INFO("PerformanceBenchmark::Shutdown() completed");
	}

	PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::RunBenchmark(const TestScenario& scenario)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("PerformanceBenchmark::RunBenchmark() called without initialization");
			return {};
		}

		ZG_CORE_INFO("PerformanceBenchmark::RunBenchmark() - Starting test: {}", scenario.name);

		// Setup
		if (scenario.setup)
		{
			scenario.setup();
		}

		// Run benchmark
		std::vector<double> frameTimes;
		auto startTime = std::chrono::high_resolution_clock::now();
		auto endTime = startTime + std::chrono::seconds(scenario.duration);

		while (std::chrono::high_resolution_clock::now() < endTime)
		{
			auto frameStart = std::chrono::high_resolution_clock::now();

			// Render frame
			if (scenario.render)
			{
				scenario.render();
			}

			auto frameEnd = std::chrono::high_resolution_clock::now();
			auto frameDuration = std::chrono::duration<double, std::milli>(frameEnd - frameStart).count();
			frameTimes.push_back(frameDuration);

			// Target FPS limiting
			if (scenario.targetFPS > 0)
			{
				double targetFrameTime = 1000.0 / scenario.targetFPS;
				if (frameDuration < targetFrameTime)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetFrameTime - frameDuration)));
				}
			}
		}

		// Cleanup
		if (scenario.cleanup)
		{
			scenario.cleanup();
		}

		// Calculate results
		auto result = CalculateStats(frameTimes, scenario.name);
		result.memoryUsage = GetCurrentMemoryUsage();
		result.cpuUsage = GetCurrentCPUUsage();
		result.gpuUsage = GetCurrentGPUUsage();

		ZG_CORE_INFO("PerformanceBenchmark::RunBenchmark() - Completed test: {} - Avg FPS: {:.2f}", 
			scenario.name, result.averageFPS);

		return result;
	}

	std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceBenchmark::RunBenchmarkSuite(const std::vector<TestScenario>& scenarios)
	{
		std::vector<BenchmarkResult> results;
		results.reserve(scenarios.size());

		ZG_CORE_INFO("PerformanceBenchmark::RunBenchmarkSuite() - Running {} tests", scenarios.size());

		for (const auto& scenario : scenarios)
		{
			auto result = RunBenchmark(scenario);
			results.push_back(result);
		}

		ZG_CORE_INFO("PerformanceBenchmark::RunBenchmarkSuite() - Completed all tests");
		return results;
	}

	std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceBenchmark::RunStressTest(const TestScenario& baseScenario, float maxMultiplier, float step)
	{
		std::vector<BenchmarkResult> results;
		std::vector<TestScenario> stressScenarios;

		ZG_CORE_INFO("PerformanceBenchmark::RunStressTest() - Testing up to {}x load", maxMultiplier);

		for (float multiplier = 1.0f; multiplier <= maxMultiplier; multiplier *= step)
		{
			TestScenario stressScenario = baseScenario;
			stressScenario.name = baseScenario.name + " (Load: " + std::to_string(multiplier) + "x)";
			
			// Modify render function to apply multiplier
			auto originalRender = stressScenario.render;
			stressScenario.render = [originalRender, multiplier]() {
				for (int i = 0; i < static_cast<int>(multiplier); ++i)
				{
					if (originalRender)
					{
						originalRender();
					}
				}
			};

			stressScenarios.push_back(stressScenario);
		}

		return RunBenchmarkSuite(stressScenarios);
	}

	std::string PerformanceBenchmark::GenerateReport(const std::vector<BenchmarkResult>& results)
	{
		if (results.empty())
		{
			return "No benchmark results to report.";
		}

		std::stringstream report;
		report << "=== Performance Benchmark Report ===\n\n";

		// Summary table
		report << "Test Name                    | Avg FPS | Min FPS | Max FPS | Avg Frame Time | Memory Usage\n";
		report << "----------------------------|---------|---------|---------|----------------|------------\n";

		for (const auto& result : results)
		{
			report << std::left << std::setw(28) << result.testName.substr(0, 27) << " | ";
			report << std::setw(7) << std::fixed << std::setprecision(1) << result.averageFPS << " | ";
			report << std::setw(7) << std::fixed << std::setprecision(1) << result.minFPS << " | ";
			report << std::setw(7) << std::fixed << std::setprecision(1) << result.maxFPS << " | ";
			report << std::setw(14) << std::fixed << std::setprecision(2) << result.averageFrameTime << " | ";
			report << std::setw(11) << (result.memoryUsage / 1024 / 1024) << " MB\n";
		}

		report << "\n";

		// Detailed analysis
		report << "=== Detailed Analysis ===\n\n";

		// Find best and worst performers
		auto bestFPS = std::max_element(results.begin(), results.end(), 
			[](const BenchmarkResult& a, const BenchmarkResult& b) { return a.averageFPS < b.averageFPS; });
		auto worstFPS = std::min_element(results.begin(), results.end(), 
			[](const BenchmarkResult& a, const BenchmarkResult& b) { return a.averageFPS < b.averageFPS; });

		report << "Best Performance: " << bestFPS->testName << " (" << bestFPS->averageFPS << " FPS)\n";
		report << "Worst Performance: " << worstFPS->testName << " (" << worstFPS->averageFPS << " FPS)\n";
		report << "Performance Range: " << (bestFPS->averageFPS - worstFPS->averageFPS) << " FPS\n\n";

		// Memory analysis
		auto maxMemory = std::max_element(results.begin(), results.end(),
			[](const BenchmarkResult& a, const BenchmarkResult& b) { return a.memoryUsage < b.memoryUsage; });
		auto minMemory = std::min_element(results.begin(), results.end(),
			[](const BenchmarkResult& a, const BenchmarkResult& b) { return a.memoryUsage < b.memoryUsage; });

		report << "Memory Usage:\n";
		report << "  Highest: " << (maxMemory->memoryUsage / 1024 / 1024) << " MB (" << maxMemory->testName << ")\n";
		report << "  Lowest: " << (minMemory->memoryUsage / 1024 / 1024) << " MB (" << minMemory->testName << ")\n";
		report << "  Range: " << ((maxMemory->memoryUsage - minMemory->memoryUsage) / 1024 / 1024) << " MB\n\n";

		// Frame time stability analysis
		report << "Frame Time Stability:\n";
		for (const auto& result : results)
		{
			double stability = (result.maxFrameTime - result.minFrameTime) / result.averageFrameTime * 100.0;
			report << "  " << result.testName << ": " << std::fixed << std::setprecision(1) << stability << "% variation\n";
		}

		return report.str();
	}

	void PerformanceBenchmark::SaveResults(const std::vector<BenchmarkResult>& results, const std::string& filename)
	{
		std::ofstream file(filename);
		if (!file.is_open())
		{
			ZG_CORE_ERROR("PerformanceBenchmark::SaveResults() - Failed to open file: {}", filename);
			return;
		}

		// Write CSV header
		file << "TestName,AvgFPS,MinFPS,MaxFPS,AvgFrameTime,MinFrameTime,MaxFrameTime,TotalFrames,MemoryUsage,CPUUsage,GPUUsage\n";

		// Write data
		for (const auto& result : results)
		{
			file << result.testName << ",";
			file << result.averageFPS << ",";
			file << result.minFPS << ",";
			file << result.maxFPS << ",";
			file << result.averageFrameTime << ",";
			file << result.minFrameTime << ",";
			file << result.maxFrameTime << ",";
			file << result.totalFrames << ",";
			file << result.memoryUsage << ",";
			file << result.cpuUsage << ",";
			file << result.gpuUsage << "\n";
		}

		file.close();
		ZG_CORE_INFO("PerformanceBenchmark::SaveResults() - Results saved to: {}", filename);
	}

	std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceBenchmark::LoadResults(const std::string& filename)
	{
		std::vector<BenchmarkResult> results;
		std::ifstream file(filename);
		if (!file.is_open())
		{
			ZG_CORE_ERROR("PerformanceBenchmark::LoadResults() - Failed to open file: {}", filename);
			return results;
		}

		std::string line;
		std::getline(file, line); // Skip header

		while (std::getline(file, line))
		{
			std::stringstream ss(line);
			std::string token;
			BenchmarkResult result;

			std::getline(ss, token, ',');
			result.testName = token;

			std::getline(ss, token, ',');
			result.averageFPS = std::stod(token);

			std::getline(ss, token, ',');
			result.minFPS = std::stod(token);

			std::getline(ss, token, ',');
			result.maxFPS = std::stod(token);

			std::getline(ss, token, ',');
			result.averageFrameTime = std::stod(token);

			std::getline(ss, token, ',');
			result.minFrameTime = std::stod(token);

			std::getline(ss, token, ',');
			result.maxFrameTime = std::stod(token);

			std::getline(ss, token, ',');
			result.totalFrames = std::stoull(token);

			std::getline(ss, token, ',');
			result.memoryUsage = std::stoull(token);

			std::getline(ss, token, ',');
			result.cpuUsage = std::stod(token);

			std::getline(ss, token, ',');
			result.gpuUsage = std::stod(token);

			results.push_back(result);
		}

		file.close();
		ZG_CORE_INFO("PerformanceBenchmark::LoadResults() - Loaded {} results from: {}", results.size(), filename);
		return results;
	}

	std::vector<PerformanceBenchmark::TestScenario> PerformanceBenchmark::GetPredefinedScenarios()
	{
		return {
			BenchmarkScenarios::Basic2DRendering(),
			BenchmarkScenarios::Basic3DRendering(),
			BenchmarkScenarios::HighQuadCount2D(5000),
			BenchmarkScenarios::HighObjectCount3D(2000),
			BenchmarkScenarios::TextureSwitching(50),
			BenchmarkScenarios::ShaderSwitching(25),
			BenchmarkScenarios::MemoryStressTest(),
			BenchmarkScenarios::ParticleSystemStress(50000),
			BenchmarkScenarios::MixedRenderingTest()
		};
	}

	void PerformanceBenchmark::StartRealTimeMonitoring(uint32_t duration)
	{
		std::lock_guard<std::mutex> lock(s_MonitoringMutex);
		
		if (s_MonitoringActive)
		{
			ZG_CORE_WARN("PerformanceBenchmark::StartRealTimeMonitoring() - Already monitoring");
			return;
		}

		s_MonitoringActive = true;
		s_MonitoringStartTime = std::chrono::high_resolution_clock::now();
		s_CurrentFrameTimes.clear();

		ZG_CORE_INFO("PerformanceBenchmark::StartRealTimeMonitoring() - Started monitoring for {} seconds", duration);
	}

	void PerformanceBenchmark::StopRealTimeMonitoring()
	{
		std::lock_guard<std::mutex> lock(s_MonitoringMutex);
		
		if (!s_MonitoringActive)
		{
			ZG_CORE_WARN("PerformanceBenchmark::StopRealTimeMonitoring() - Not monitoring");
			return;
		}

		s_MonitoringActive = false;
		ZG_CORE_INFO("PerformanceBenchmark::StopRealTimeMonitoring() - Stopped monitoring");
	}

	PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::GetCurrentStats()
	{
		std::lock_guard<std::mutex> lock(s_MonitoringMutex);
		
		if (!s_MonitoringActive || s_CurrentFrameTimes.empty())
		{
			return {};
		}

		auto result = CalculateStats(s_CurrentFrameTimes, "Real-time Monitoring");
		result.memoryUsage = GetCurrentMemoryUsage();
		result.cpuUsage = GetCurrentCPUUsage();
		result.gpuUsage = GetCurrentGPUUsage();

		return result;
	}

	bool PerformanceBenchmark::IsMonitoringActive()
	{
		std::lock_guard<std::mutex> lock(s_MonitoringMutex);
		return s_MonitoringActive;
	}

	double PerformanceBenchmark::MeasureFrameTime(const std::function<void()>& renderFunction)
	{
		auto start = std::chrono::high_resolution_clock::now();
		renderFunction();
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::milli>(end - start).count();
	}

	size_t PerformanceBenchmark::GetCurrentMemoryUsage()
	{
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
		{
			return pmc.WorkingSetSize;
		}
#else
		// Linux implementation
		std::ifstream statusFile("/proc/self/status");
		std::string line;
		while (std::getline(statusFile, line))
		{
			if (line.substr(0, 6) == "VmRSS:")
			{
				std::istringstream iss(line);
				std::string key, value, unit;
				iss >> key >> value >> unit;
				return std::stoull(value) * 1024; // Convert KB to bytes
			}
		}
#endif
		return 0;
	}

	double PerformanceBenchmark::GetCurrentCPUUsage()
	{
		// Simplified CPU usage calculation
		// In a real implementation, you'd use more sophisticated methods
		static auto lastTime = std::chrono::high_resolution_clock::now();
		static auto lastIdleTime = std::chrono::high_resolution_clock::now();
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto idleTime = std::chrono::high_resolution_clock::now();
		
		double cpuUsage = 0.0;
		// This is a placeholder - real CPU usage calculation would be more complex
		
		lastTime = currentTime;
		lastIdleTime = idleTime;
		
		return cpuUsage;
	}

	double PerformanceBenchmark::GetCurrentGPUUsage()
	{
		// GPU usage monitoring would require platform-specific implementations
		// This is a placeholder
		return 0.0;
	}

	PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::CalculateStats(const std::vector<double>& frameTimes, const std::string& testName)
	{
		if (frameTimes.empty())
		{
			return {};
		}

		BenchmarkResult result;
		result.testName = testName;
		result.totalFrames = frameTimes.size();

		// Calculate frame time statistics
		result.averageFrameTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
		result.minFrameTime = *std::min_element(frameTimes.begin(), frameTimes.end());
		result.maxFrameTime = *std::max_element(frameTimes.begin(), frameTimes.end());

		// Calculate FPS statistics
		result.averageFPS = 1000.0 / result.averageFrameTime;
		result.minFPS = 1000.0 / result.maxFrameTime;
		result.maxFPS = 1000.0 / result.minFrameTime;

		// Store individual frame times
		result.frameTimes = frameTimes;

		return result;
	}

	// Predefined scenarios implementation
	namespace BenchmarkScenarios {

		PerformanceBenchmark::TestScenario Basic2DRendering()
		{
			return {
				"Basic 2D Rendering",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[]() {
					// Render some basic quads
					for (int i = 0; i < 100; ++i)
					{
						float x = (i % 10) * 0.2f - 1.0f;
						float y = (i / 10) * 0.2f - 1.0f;
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.1f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				5, // 5 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario Basic3DRendering()
		{
			return {
				"Basic 3D Rendering",
				[]() {
					BatchRenderer3D::Init();
					BatchRenderer3D::BeginScene({});
				},
				[]() {
					// Render some basic cubes
					for (int i = 0; i < 50; ++i)
					{
						float x = (i % 5) * 0.5f - 1.0f;
						float y = (i / 5) * 0.5f - 1.0f;
						BatchRenderer3D::DrawCube({x, y, 0}, {0.2f, 0.2f, 0.2f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer3D::EndScene();
					BatchRenderer3D::Flush();
				},
				[]() {
					BatchRenderer3D::Shutdown();
				},
				5, // 5 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario HighQuadCount2D(uint32_t quadCount)
		{
			return {
				"High Quad Count 2D (" + std::to_string(quadCount) + " quads)",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[quadCount]() {
					// Render many quads
					for (uint32_t i = 0; i < quadCount; ++i)
					{
						float x = (i % 100) * 0.02f - 1.0f;
						float y = (i / 100) * 0.02f - 1.0f;
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.01f, 0.01f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				10, // 10 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario HighObjectCount3D(uint32_t objectCount)
		{
			return {
				"High Object Count 3D (" + std::to_string(objectCount) + " objects)",
				[]() {
					BatchRenderer3D::Init();
					BatchRenderer3D::BeginScene({});
				},
				[objectCount]() {
					// Render many 3D objects
					for (uint32_t i = 0; i < objectCount; ++i)
					{
						float x = (i % 50) * 0.1f - 2.5f;
						float y = (i / 50) * 0.1f - 2.5f;
						BatchRenderer3D::DrawCube({x, y, 0}, {0.05f, 0.05f, 0.05f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer3D::EndScene();
					BatchRenderer3D::Flush();
				},
				[]() {
					BatchRenderer3D::Shutdown();
				},
				10, // 10 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario TextureSwitching(uint32_t textureCount)
		{
			return {
				"Texture Switching (" + std::to_string(textureCount) + " textures)",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[textureCount]() {
					// Simulate texture switching
					for (uint32_t i = 0; i < textureCount; ++i)
					{
						float x = (i % 20) * 0.1f - 1.0f;
						float y = (i / 20) * 0.1f - 1.0f;
						// Note: This would require actual texture objects in a real implementation
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.05f, 0.05f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				5, // 5 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario ShaderSwitching(uint32_t shaderCount)
		{
			return {
				"Shader Switching (" + std::to_string(shaderCount) + " shaders)",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[shaderCount]() {
					// Simulate shader switching
					for (uint32_t i = 0; i < shaderCount; ++i)
					{
						float x = (i % 10) * 0.2f - 1.0f;
						float y = (i / 10) * 0.2f - 1.0f;
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.1f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				5, // 5 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario MemoryStressTest()
		{
			return {
				"Memory Stress Test",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[]() {
					// Create and destroy many objects to stress memory management
					for (int i = 0; i < 1000; ++i)
					{
						float x = (i % 50) * 0.04f - 1.0f;
						float y = (i / 50) * 0.04f - 1.0f;
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.02f, 0.02f}, {1.0f, 0.0f, 0.0f, 1.0f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				15, // 15 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario ParticleSystemStress(uint32_t particleCount)
		{
			return {
				"Particle System Stress (" + std::to_string(particleCount) + " particles)",
				[]() {
					BatchRenderer2D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
				},
				[particleCount]() {
					// Simulate particle system rendering
					for (uint32_t i = 0; i < particleCount; ++i)
					{
						float x = ((i % 1000) % 100) * 0.02f - 1.0f;
						float y = ((i % 1000) / 100) * 0.02f - 1.0f;
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.01f, 0.01f}, {1.0f, 1.0f, 0.0f, 0.5f});
					}
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
				},
				10, // 10 seconds
				60  // Target 60 FPS
			};
		}

		PerformanceBenchmark::TestScenario MixedRenderingTest()
		{
			return {
				"Mixed Rendering Test (2D + 3D)",
				[]() {
					BatchRenderer2D::Init();
					BatchRenderer3D::Init();
					OrthographicCamera camera(-1.0f, 1.0f, -1.0f, 1.0f);
					BatchRenderer2D::BeginScene(camera);
					BatchRenderer3D::BeginScene({});
				},
				[]() {
					// Render both 2D and 3D objects
					for (int i = 0; i < 50; ++i)
					{
						float x = (i % 10) * 0.2f - 1.0f;
						float y = (i / 10) * 0.2f - 1.0f;
						
						// 2D quads
						BatchRenderer2D::DrawQuad({x, y, 0}, {0.1f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f});
						
						// 3D cubes
						BatchRenderer3D::DrawCube({x, y, 0.5f}, {0.05f, 0.05f, 0.05f}, {0.0f, 1.0f, 0.0f, 1.0f});
					}
					
					BatchRenderer2D::EndScene();
					BatchRenderer2D::Flush();
					BatchRenderer3D::EndScene();
					BatchRenderer3D::Flush();
				},
				[]() {
					BatchRenderer2D::Shutdown();
					BatchRenderer3D::Shutdown();
				},
				8, // 8 seconds
				60  // Target 60 FPS
			};
		}

	}

}
