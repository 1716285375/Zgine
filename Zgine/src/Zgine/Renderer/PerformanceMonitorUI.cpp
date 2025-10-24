#include "zgpch.h"
#include "PerformanceMonitorUI.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace Zgine {

	// Static member definitions
	bool PerformanceMonitorUI::s_Initialized = false;
	bool PerformanceMonitorUI::s_ShowMainWindow = true;
	bool PerformanceMonitorUI::s_ShowBenchmarkWindow = false;
	bool PerformanceMonitorUI::s_ShowMemoryAnalysisWindow = false;
	bool PerformanceMonitorUI::s_ShowRealTimeMonitoringWindow = false;
	bool PerformanceMonitorUI::s_ShowPerformanceHistoryWindow = false;
	bool PerformanceMonitorUI::s_IsRecording = false;
	bool PerformanceMonitorUI::s_AutoBenchmark = false;
	float PerformanceMonitorUI::s_UpdateFrequency = 60.0f;
	std::string PerformanceMonitorUI::s_CurrentTheme = "dark";

	PerformanceMonitorUI::PerformanceData PerformanceMonitorUI::s_CurrentData = {};
	std::vector<PerformanceMonitorUI::PerformanceData> PerformanceMonitorUI::s_PerformanceHistory;
	std::vector<float> PerformanceMonitorUI::s_FPSHistory;
	std::vector<float> PerformanceMonitorUI::s_MemoryHistory;
	std::vector<float> PerformanceMonitorUI::s_FrameTimeHistory;

	float PerformanceMonitorUI::s_LastUpdateTime = 0.0f;
	float PerformanceMonitorUI::s_UpdateTimer = 0.0f;
	size_t PerformanceMonitorUI::s_MaxHistorySize = 1000;

	// PerformanceOverlay static members
	bool PerformanceOverlay::s_Initialized = false;
	bool PerformanceOverlay::s_ShowOverlay = true;
	int PerformanceOverlay::s_Position = 0; // Top-left
	float PerformanceOverlay::s_Transparency = 0.8f;
	bool PerformanceOverlay::s_ShowFPS = true;
	bool PerformanceOverlay::s_ShowFrameTime = true;
	bool PerformanceOverlay::s_ShowMemory = true;
	bool PerformanceOverlay::s_ShowDrawCalls = true;

	PerformanceMonitorUI::PerformanceData PerformanceOverlay::s_CurrentData = {};

	void PerformanceMonitorUI::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("PerformanceMonitorUI::Init() called multiple times");
			return;
		}

		// Initialize performance benchmark system
		PerformanceBenchmark::Init();

		// Initialize overlay
		PerformanceOverlay::Init();

		// Set default theme
		ApplyTheme(s_CurrentTheme);

		s_Initialized = true;
		ZG_CORE_INFO("PerformanceMonitorUI::Init() completed");
	}

	void PerformanceMonitorUI::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("PerformanceMonitorUI::Shutdown() called without initialization");
			return;
		}

		StopRecording();
		PerformanceOverlay::Shutdown();
		PerformanceBenchmark::Shutdown();

		s_Initialized = false;
		ZG_CORE_INFO("PerformanceMonitorUI::Shutdown() completed");
	}

	void PerformanceMonitorUI::Render(bool showWindow)
	{
		if (!s_Initialized) return;

		s_ShowMainWindow = showWindow;

		// Render overlay first (always on top)
		PerformanceOverlay::Render();

		// Render main window
		if (s_ShowMainWindow)
		{
			RenderMainWindow();
		}

		// Render other windows
		if (s_ShowBenchmarkWindow)
		{
			RenderBenchmarkWindow();
		}

		if (s_ShowMemoryAnalysisWindow)
		{
			RenderMemoryAnalysisWindow();
		}

		if (s_ShowRealTimeMonitoringWindow)
		{
			RenderRealTimeMonitoringWindow();
		}

		if (s_ShowPerformanceHistoryWindow)
		{
			RenderPerformanceHistoryWindow();
		}
	}

	void PerformanceMonitorUI::Update(float deltaTime)
	{
		if (!s_Initialized) return;

		s_UpdateTimer += deltaTime;
		if (s_UpdateTimer >= 1.0f / s_UpdateFrequency)
		{
			UpdatePerformanceData(deltaTime);
			s_UpdateTimer = 0.0f;
		}

		// Update overlay data
		PerformanceOverlay::UpdateData(s_CurrentData);
	}

	void PerformanceMonitorUI::SetPerformanceData(const PerformanceData& data)
	{
		s_CurrentData = data;
	}

	void PerformanceMonitorUI::ShowBenchmarkWindow(bool show)
	{
		s_ShowBenchmarkWindow = show;
	}

	void PerformanceMonitorUI::ShowMemoryAnalysisWindow(bool show)
	{
		s_ShowMemoryAnalysisWindow = show;
	}

	void PerformanceMonitorUI::ShowRealTimeMonitoringWindow(bool show)
	{
		s_ShowRealTimeMonitoringWindow = show;
	}

	void PerformanceMonitorUI::ShowPerformanceHistoryWindow(bool show)
	{
		s_ShowPerformanceHistoryWindow = show;
	}

	void PerformanceMonitorUI::StartRecording()
	{
		if (s_IsRecording)
		{
			ZG_CORE_WARN("PerformanceMonitorUI::StartRecording() - Already recording");
			return;
		}

		s_IsRecording = true;
		s_PerformanceHistory.clear();
		s_FPSHistory.clear();
		s_MemoryHistory.clear();
		s_FrameTimeHistory.clear();

		ZG_CORE_INFO("PerformanceMonitorUI::StartRecording() - Started recording performance data");
	}

	void PerformanceMonitorUI::StopRecording()
	{
		if (!s_IsRecording)
		{
			ZG_CORE_WARN("PerformanceMonitorUI::StopRecording() - Not recording");
			return;
		}

		s_IsRecording = false;
		ZG_CORE_INFO("PerformanceMonitorUI::StopRecording() - Stopped recording performance data");
	}

	void PerformanceMonitorUI::ClearHistory()
	{
		s_PerformanceHistory.clear();
		s_FPSHistory.clear();
		s_MemoryHistory.clear();
		s_FrameTimeHistory.clear();
		ZG_CORE_INFO("PerformanceMonitorUI::ClearHistory() - Cleared performance history");
	}

	void PerformanceMonitorUI::ExportToCSV(const std::string& filename)
	{
		std::ofstream file(filename);
		if (!file.is_open())
		{
			ZG_CORE_ERROR("PerformanceMonitorUI::ExportToCSV() - Failed to open file: {}", filename);
			return;
		}

		// Write CSV header
		file << "Timestamp,FPS,FrameTime,MemoryUsage,CPUUsage,GPUUsage,DrawCalls,Vertices,Indices,RenderTime,UpdateTime\n";

		// Write data
		for (size_t i = 0; i < s_PerformanceHistory.size(); ++i)
		{
			const auto& data = s_PerformanceHistory[i];
			file << i << ",";
			file << data.currentFPS << ",";
			file << data.frameTime << ",";
			file << data.memoryUsage << ",";
			file << data.cpuUsage << ",";
			file << data.gpuUsage << ",";
			file << data.drawCalls << ",";
			file << data.vertices << ",";
			file << data.indices << ",";
			file << data.renderTime << ",";
			file << data.updateTime << "\n";
		}

		file.close();
		ZG_CORE_INFO("PerformanceMonitorUI::ExportToCSV() - Exported {} records to: {}", s_PerformanceHistory.size(), filename);
	}

	PerformanceMonitorUI::PerformanceData PerformanceMonitorUI::GetCurrentData()
	{
		return s_CurrentData;
	}

	std::vector<PerformanceMonitorUI::PerformanceData> PerformanceMonitorUI::GetHistory()
	{
		return s_PerformanceHistory;
	}

	void PerformanceMonitorUI::SetTheme(const std::string& theme)
	{
		s_CurrentTheme = theme;
		ApplyTheme(theme);
	}

	void PerformanceMonitorUI::SetUpdateFrequency(float frequency)
	{
		s_UpdateFrequency = std::max(1.0f, std::min(120.0f, frequency));
	}

	void PerformanceMonitorUI::SetAutoBenchmark(bool enable)
	{
		s_AutoBenchmark = enable;
	}

	PerformanceMonitorUI::UIStats PerformanceMonitorUI::GetUIStats()
	{
		return {
			s_IsRecording,
			s_PerformanceHistory.size(),
			s_UpdateFrequency,
			s_AutoBenchmark,
			s_CurrentTheme
		};
	}

	void PerformanceMonitorUI::RenderMainWindow()
	{
		ImGui::Begin("Performance Monitor", &s_ShowMainWindow, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Benchmark", nullptr, &s_ShowBenchmarkWindow);
				ImGui::MenuItem("Memory Analysis", nullptr, &s_ShowMemoryAnalysisWindow);
				ImGui::MenuItem("Real-time Monitoring", nullptr, &s_ShowRealTimeMonitoringWindow);
				ImGui::MenuItem("Performance History", nullptr, &s_ShowPerformanceHistoryWindow);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Recording"))
			{
				if (ImGui::MenuItem("Start Recording", nullptr, false, !s_IsRecording))
				{
					StartRecording();
				}
				if (ImGui::MenuItem("Stop Recording", nullptr, false, s_IsRecording))
				{
					StopRecording();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Clear History"))
				{
					ClearHistory();
				}
				if (ImGui::MenuItem("Export to CSV"))
				{
					ExportToCSV("performance_data.csv");
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::BeginMenu("Theme"))
				{
					if (ImGui::MenuItem("Dark", nullptr, s_CurrentTheme == "dark"))
					{
						SetTheme("dark");
					}
					if (ImGui::MenuItem("Light", nullptr, s_CurrentTheme == "light"))
					{
						SetTheme("light");
					}
					if (ImGui::MenuItem("Classic", nullptr, s_CurrentTheme == "classic"))
					{
						SetTheme("classic");
					}
					ImGui::EndMenu();
				}

				ImGui::MenuItem("Auto Benchmark", nullptr, &s_AutoBenchmark);
				
				float freq = s_UpdateFrequency;
				if (ImGui::SliderFloat("Update Frequency", &freq, 1.0f, 120.0f, "%.1f Hz"))
				{
					SetUpdateFrequency(freq);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// Main performance display
		ImGui::Text("Current Performance");
		ImGui::Separator();

		// FPS display
		ImGui::Text("FPS: %.1f", s_CurrentData.currentFPS);
		ImGui::SameLine();
		ImGui::Text("(%.1f - %.1f)", s_CurrentData.minFPS, s_CurrentData.maxFPS);

		// Frame time display
		ImGui::Text("Frame Time: %.2f ms", s_CurrentData.frameTime);

		// Memory usage
		ImGui::Text("Memory: %s", FormatBytes(s_CurrentData.memoryUsage).c_str());

		// CPU/GPU usage
		ImGui::Text("CPU: %s", FormatPercentage(s_CurrentData.cpuUsage).c_str());
		ImGui::Text("GPU: %s", FormatPercentage(s_CurrentData.gpuUsage).c_str());

		ImGui::Separator();

		// Rendering stats
		ImGui::Text("Rendering Stats");
		ImGui::Text("Draw Calls: %u", s_CurrentData.drawCalls);
		ImGui::Text("Vertices: %u", s_CurrentData.vertices);
		ImGui::Text("Indices: %u", s_CurrentData.indices);
		ImGui::Text("Render Time: %.2f ms", s_CurrentData.renderTime);
		ImGui::Text("Update Time: %.2f ms", s_CurrentData.updateTime);

		ImGui::Separator();

		// Recording status
		if (s_IsRecording)
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Recording: %zu samples", s_PerformanceHistory.size());
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Not Recording");
		}

		ImGui::End();
	}

	void PerformanceMonitorUI::RenderBenchmarkWindow()
	{
		ImGui::Begin("Benchmark Suite", &s_ShowBenchmarkWindow);

		ImGui::Text("Performance Benchmark Tests");
		ImGui::Separator();

		if (ImGui::Button("Run All Tests"))
		{
			auto scenarios = PerformanceBenchmark::GetPredefinedScenarios();
			auto results = PerformanceBenchmark::RunBenchmarkSuite(scenarios);
			ProcessBenchmarkResults(results);
		}

		ImGui::SameLine();
		// TODO: Fix BenchmarkScenarios namespace access
		// if (ImGui::Button("Run Stress Test"))
		// {
		// 	auto baseScenario = PerformanceBenchmark::BenchmarkScenarios::Basic2DRendering();
		// 	auto results = PerformanceBenchmark::RunStressTest(baseScenario, 5.0f, 1.5f);
		// 	ProcessBenchmarkResults(results);
		// }

		ImGui::Separator();

		// Individual test buttons
		// TODO: Fix BenchmarkScenarios namespace access
		// if (ImGui::Button("Basic 2D Test"))
		// {
		// 	auto scenario = PerformanceBenchmark::BenchmarkScenarios::Basic2DRendering();
		// 	auto result = PerformanceBenchmark::RunBenchmark(scenario);
		// 	ProcessBenchmarkResults({result});
		// }

		ImGui::SameLine();
		// TODO: Fix BenchmarkScenarios namespace access
		// if (ImGui::Button("Basic 3D Test"))
		// {
		// 	auto scenario = PerformanceBenchmark::BenchmarkScenarios::Basic3DRendering();
		// 	auto result = PerformanceBenchmark::RunBenchmark(scenario);
		// 	ProcessBenchmarkResults({result});
		// }

		// TODO: Fix BenchmarkScenarios namespace access
		// if (ImGui::Button("High Quad Count"))
		// {
		// 	auto scenario = PerformanceBenchmark::BenchmarkScenarios::HighQuadCount2D(10000);
		// 	auto result = PerformanceBenchmark::RunBenchmark(scenario);
		// 	ProcessBenchmarkResults({result});
		// }

		ImGui::SameLine();
		// TODO: Fix BenchmarkScenarios namespace access
		// if (ImGui::Button("High Object Count"))
		// {
		// 	auto scenario = PerformanceBenchmark::BenchmarkScenarios::HighObjectCount3D(5000);
		// 	auto result = PerformanceBenchmark::RunBenchmark(scenario);
		// 	ProcessBenchmarkResults({result});
		// }

		ImGui::End();
	}

	void PerformanceMonitorUI::RenderMemoryAnalysisWindow()
	{
		ImGui::Begin("Memory Analysis", &s_ShowMemoryAnalysisWindow);

		ImGui::Text("Memory Usage Analysis");
		ImGui::Separator();

		// Current memory usage
		ImGui::Text("Current Memory: %s", FormatBytes(s_CurrentData.memoryUsage).c_str());

		// Memory history chart
		if (!s_MemoryHistory.empty())
		{
			RenderPerformanceChart(s_MemoryHistory, "Memory Usage Over Time", 0.0f, 0.0f);
		}

		// Memory statistics
		if (!s_MemoryHistory.empty())
		{
			auto minMem = *std::min_element(s_MemoryHistory.begin(), s_MemoryHistory.end());
			auto maxMem = *std::max_element(s_MemoryHistory.begin(), s_MemoryHistory.end());
			auto avgMem = std::accumulate(s_MemoryHistory.begin(), s_MemoryHistory.end(), 0.0f) / s_MemoryHistory.size();

			ImGui::Text("Memory Statistics:");
			ImGui::Text("  Min: %s", FormatBytes(static_cast<size_t>(minMem)).c_str());
			ImGui::Text("  Max: %s", FormatBytes(static_cast<size_t>(maxMem)).c_str());
			ImGui::Text("  Avg: %s", FormatBytes(static_cast<size_t>(avgMem)).c_str());
		}

		ImGui::End();
	}

	void PerformanceMonitorUI::RenderRealTimeMonitoringWindow()
	{
		ImGui::Begin("Real-time Monitoring", &s_ShowRealTimeMonitoringWindow);

		ImGui::Text("Real-time Performance Monitoring");
		ImGui::Separator();

		if (ImGui::Button("Start Monitoring"))
		{
			PerformanceBenchmark::StartRealTimeMonitoring(0); // Infinite monitoring
		}

		ImGui::SameLine();
		if (ImGui::Button("Stop Monitoring"))
		{
			PerformanceBenchmark::StopRealTimeMonitoring();
		}

		ImGui::Separator();

		// Real-time stats
		if (PerformanceBenchmark::IsMonitoringActive())
		{
			auto stats = PerformanceBenchmark::GetCurrentStats();
			ImGui::Text("Monitoring Active");
			ImGui::Text("Average FPS: %.1f", stats.averageFPS);
			ImGui::Text("Frame Time: %.2f ms", stats.averageFrameTime);
			ImGui::Text("Total Frames: %zu", stats.totalFrames);
		}
		else
		{
			ImGui::Text("Monitoring Inactive");
		}

		ImGui::End();
	}

	void PerformanceMonitorUI::RenderPerformanceHistoryWindow()
	{
		ImGui::Begin("Performance History", &s_ShowPerformanceHistoryWindow);

		ImGui::Text("Performance History");
		ImGui::Separator();

		// FPS chart
		if (!s_FPSHistory.empty())
		{
			RenderPerformanceChart(s_FPSHistory, "FPS Over Time", 0.0f, 120.0f);
		}

		// Frame time chart
		if (!s_FrameTimeHistory.empty())
		{
			RenderPerformanceChart(s_FrameTimeHistory, "Frame Time Over Time", 0.0f, 50.0f);
		}

		// Performance table
		RenderPerformanceTable();

		ImGui::End();
	}

	void PerformanceMonitorUI::RenderPerformanceChart(const std::vector<float>& data, const std::string& title, float minValue, float maxValue)
	{
		ImGui::Text("%s", title.c_str());
		
		if (data.empty())
		{
			ImGui::Text("No data available");
			return;
		}

		// Calculate chart bounds
		float chartMin = minValue;
		float chartMax = maxValue;
		if (chartMin == chartMax)
		{
			chartMin = *std::min_element(data.begin(), data.end());
			chartMax = *std::max_element(data.begin(), data.end());
			float range = chartMax - chartMin;
			chartMin -= range * 0.1f;
			chartMax += range * 0.1f;
		}

		// Render chart
		ImGui::PlotLines("", data.data(), static_cast<int>(data.size()), 0, nullptr, chartMin, chartMax, ImVec2(0, 100));
	}

	void PerformanceMonitorUI::RenderPerformanceTable()
	{
		if (s_PerformanceHistory.empty())
		{
			ImGui::Text("No performance data available");
			return;
		}

		ImGui::Text("Performance Data Table");
		ImGui::Separator();

		// Table headers
		ImGui::Columns(6, "PerformanceTable");
		ImGui::SetColumnWidth(0, 80);
		ImGui::SetColumnWidth(1, 60);
		ImGui::SetColumnWidth(2, 80);
		ImGui::SetColumnWidth(3, 100);
		ImGui::SetColumnWidth(4, 80);
		ImGui::SetColumnWidth(5, 80);

		ImGui::Text("Time");
		ImGui::NextColumn();
		ImGui::Text("FPS");
		ImGui::NextColumn();
		ImGui::Text("Frame Time");
		ImGui::NextColumn();
		ImGui::Text("Memory");
		ImGui::NextColumn();
		ImGui::Text("Draw Calls");
		ImGui::NextColumn();
		ImGui::Text("Vertices");
		ImGui::NextColumn();

		ImGui::Separator();

		// Show last 20 entries
		size_t start = s_PerformanceHistory.size() > 20 ? s_PerformanceHistory.size() - 20 : 0;
		for (size_t i = start; i < s_PerformanceHistory.size(); ++i)
		{
			const auto& data = s_PerformanceHistory[i];
			
			ImGui::Text("%zu", i);
			ImGui::NextColumn();
			ImGui::Text("%.1f", data.currentFPS);
			ImGui::NextColumn();
			ImGui::Text("%.2f", data.frameTime);
			ImGui::NextColumn();
			ImGui::Text("%s", FormatBytes(data.memoryUsage).c_str());
			ImGui::NextColumn();
			ImGui::Text("%u", data.drawCalls);
			ImGui::NextColumn();
			ImGui::Text("%u", data.vertices);
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

	void PerformanceMonitorUI::UpdatePerformanceData(float deltaTime)
	{
		// Update current data from renderer stats
		auto stats2D = BatchRenderer2D::GetStats();
		auto stats3D = BatchRenderer3D::GetStats();

		s_CurrentData.drawCalls = stats2D.DrawCalls + stats3D.DrawCalls;
		s_CurrentData.vertices = stats2D.QuadCount * 4 + stats3D.CubeCount * 8 + stats3D.SphereCount * 8 + stats3D.CylinderCount * 8;
		s_CurrentData.indices = stats2D.QuadCount * 6 + stats3D.CubeCount * 36 + stats3D.SphereCount * 36 + stats3D.CylinderCount * 36;

		// Calculate FPS
		s_CurrentData.currentFPS = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
		s_CurrentData.frameTime = deltaTime * 1000.0f; // Convert to milliseconds

		// Update min/max FPS
		if (s_CurrentData.minFPS == 0.0f || s_CurrentData.currentFPS < s_CurrentData.minFPS)
		{
			s_CurrentData.minFPS = s_CurrentData.currentFPS;
		}
		if (s_CurrentData.currentFPS > s_CurrentData.maxFPS)
		{
			s_CurrentData.maxFPS = s_CurrentData.currentFPS;
		}

		// Add to history if recording
		if (s_IsRecording)
		{
			AddToHistory(s_CurrentData);
		}
	}

	void PerformanceMonitorUI::AddToHistory(const PerformanceData& data)
	{
		s_PerformanceHistory.push_back(data);
		s_FPSHistory.push_back(data.currentFPS);
		s_MemoryHistory.push_back(static_cast<float>(data.memoryUsage));
		s_FrameTimeHistory.push_back(data.frameTime);

		// Limit history size
		if (s_PerformanceHistory.size() > s_MaxHistorySize)
		{
			s_PerformanceHistory.erase(s_PerformanceHistory.begin());
			s_FPSHistory.erase(s_FPSHistory.begin());
			s_MemoryHistory.erase(s_MemoryHistory.begin());
			s_FrameTimeHistory.erase(s_FrameTimeHistory.begin());
		}
	}

	void PerformanceMonitorUI::ProcessBenchmarkResults(const std::vector<PerformanceBenchmark::BenchmarkResult>& results)
	{
		ZG_CORE_INFO("PerformanceMonitorUI::ProcessBenchmarkResults() - Processing {} benchmark results", results.size());
		
		// Generate and display report
		std::string report = PerformanceBenchmark::GenerateReport(results);
		ZG_CORE_INFO("Benchmark Report:\n{}", report);

		// Save results
		PerformanceBenchmark::SaveResults(results, "benchmark_results.csv");
	}

	std::string PerformanceMonitorUI::FormatBytes(size_t bytes)
	{
		const char* units[] = {"B", "KB", "MB", "GB", "TB"};
		int unit = 0;
		double size = static_cast<double>(bytes);

		while (size >= 1024.0 && unit < 4)
		{
			size /= 1024.0;
			unit++;
		}

		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
		return ss.str();
	}

	std::string PerformanceMonitorUI::FormatPercentage(double percentage)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(1) << percentage << "%";
		return ss.str();
	}

	std::string PerformanceMonitorUI::FormatTime(float time)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << time << " ms";
		return ss.str();
	}

	ImVec4 PerformanceMonitorUI::GetPerformanceColor(float value, float minValue, float maxValue)
	{
		float normalized = (value - minValue) / (maxValue - minValue);
		normalized = std::max(0.0f, std::min(1.0f, normalized));

		if (normalized < 0.5f)
		{
			// Green to Yellow
			return ImVec4(normalized * 2.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			// Yellow to Red
			return ImVec4(1.0f, 2.0f - normalized * 2.0f, 0.0f, 1.0f);
		}
	}

	void PerformanceMonitorUI::ApplyTheme(const std::string& theme)
	{
		if (theme == "dark")
		{
			ImGui::StyleColorsDark();
		}
		else if (theme == "light")
		{
			ImGui::StyleColorsLight();
		}
		else if (theme == "classic")
		{
			ImGui::StyleColorsClassic();
		}
	}

	// PerformanceOverlay implementation
	void PerformanceOverlay::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("PerformanceOverlay::Init() called multiple times");
			return;
		}

		s_Initialized = true;
		ZG_CORE_INFO("PerformanceOverlay::Init() completed");
	}

	void PerformanceOverlay::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("PerformanceOverlay::Shutdown() called without initialization");
			return;
		}

		s_Initialized = false;
		ZG_CORE_INFO("PerformanceOverlay::Shutdown() completed");
	}

	void PerformanceOverlay::Render(bool show)
	{
		if (!s_Initialized || !show) return;

		s_ShowOverlay = show;
		RenderOverlay();
	}

	void PerformanceOverlay::SetPosition(int position)
	{
		s_Position = std::max(0, std::min(3, position));
	}

	void PerformanceOverlay::SetTransparency(float alpha)
	{
		s_Transparency = std::max(0.0f, std::min(1.0f, alpha));
	}

	void PerformanceOverlay::SetMetrics(bool showFPS, bool showFrameTime, bool showMemory, bool showDrawCalls)
	{
		s_ShowFPS = showFPS;
		s_ShowFrameTime = showFrameTime;
		s_ShowMemory = showMemory;
		s_ShowDrawCalls = showDrawCalls;
	}

	void PerformanceOverlay::UpdateData(const PerformanceMonitorUI::PerformanceData& data)
	{
		s_CurrentData = data;
	}

	void PerformanceOverlay::RenderOverlay()
	{
		ImGui::SetNextWindowPos(GetOverlayPosition(), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(s_Transparency);
		
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

		ImGui::Begin("Performance Overlay", nullptr, flags);

		ImVec4 color = GetOverlayColor();
		ImGui::PushStyleColor(ImGuiCol_Text, color);

		if (s_ShowFPS)
		{
			ImGui::Text("FPS: %.1f", s_CurrentData.currentFPS);
		}

		if (s_ShowFrameTime)
		{
			ImGui::Text("Frame: %.2f ms", s_CurrentData.frameTime);
		}

		if (s_ShowMemory)
		{
			ImGui::Text("Memory: %s", PerformanceOverlay::FormatBytes(s_CurrentData.memoryUsage).c_str());
		}

		if (s_ShowDrawCalls)
		{
			ImGui::Text("Draw Calls: %u", s_CurrentData.drawCalls);
		}

		ImGui::PopStyleColor();
		ImGui::End();
	}

	ImVec2 PerformanceOverlay::GetOverlayPosition()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 pos;

		switch (s_Position)
		{
		case 0: // Top-left
			pos = ImVec2(10, 10);
			break;
		case 1: // Top-right
			pos = ImVec2(io.DisplaySize.x - 200, 10);
			break;
		case 2: // Bottom-left
			pos = ImVec2(10, io.DisplaySize.y - 100);
			break;
		case 3: // Bottom-right
			pos = ImVec2(io.DisplaySize.x - 200, io.DisplaySize.y - 100);
			break;
		default:
			pos = ImVec2(10, 10);
			break;
		}

		return pos;
	}

	ImVec4 PerformanceOverlay::GetOverlayColor()
	{
		// Color based on FPS performance
		if (s_CurrentData.currentFPS >= 60.0f)
		{
			return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
		}
		else if (s_CurrentData.currentFPS >= 30.0f)
		{
			return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		}
		else
		{
			return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		}
	}

	std::string PerformanceOverlay::FormatBytes(size_t bytes)
	{
		const char* units[] = { "B", "KB", "MB", "GB", "TB" };
		int unitIndex = 0;
		double size = static_cast<double>(bytes);

		while (size >= 1024.0 && unitIndex < 4)
		{
			size /= 1024.0;
			unitIndex++;
		}

		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
		return ss.str();
	}

}
