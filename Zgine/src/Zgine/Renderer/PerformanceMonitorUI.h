#pragma once

#include "zgpch.h"
#include "../Core.h"
#include "PerformanceBenchmark.h"
#include "PerformanceProfiler.h"
#include "ResourceManager.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <memory>

namespace Zgine {

	/**
	 * @brief Performance monitoring UI for real-time performance visualization
	 * @details Provides comprehensive UI for monitoring rendering performance,
	 *          running benchmarks, and analyzing performance data
	 */
	class PerformanceMonitorUI
	{
	public:
		struct PerformanceData
		{
			float currentFPS;
			float averageFPS;
			float minFPS;
			float maxFPS;
			float frameTime;
			size_t memoryUsage;
			double cpuUsage;
			double gpuUsage;
			uint32_t drawCalls;
			uint32_t vertices;
			uint32_t indices;
			float renderTime;
			float updateTime;
		};

		/**
		 * @brief Initialize the performance monitor UI
		 */
		static void Init();

		/**
		 * @brief Shutdown the performance monitor UI
		 */
		static void Shutdown();

		/**
		 * @brief Render the performance monitor UI
		 * @param showWindow Whether to show the main window
		 */
		static void Render(bool showWindow = true);

		/**
		 * @brief Update performance data
		 * @param deltaTime Time since last update
		 */
		static void Update(float deltaTime);

		/**
		 * @brief Set current performance data
		 * @param data Performance data to display
		 */
		static void SetPerformanceData(const PerformanceData& data);

		/**
		 * @brief Show benchmark window
		 * @param show Whether to show the window
		 */
		static void ShowBenchmarkWindow(bool show = true);

		/**
		 * @brief Show memory analysis window
		 * @param show Whether to show the window
		 */
		static void ShowMemoryAnalysisWindow(bool show = true);

		/**
		 * @brief Show real-time monitoring window
		 * @param show Whether to show the window
		 */
		static void ShowRealTimeMonitoringWindow(bool show = true);

		/**
		 * @brief Show performance history window
		 * @param show Whether to show the window
		 */
		static void ShowPerformanceHistoryWindow(bool show = true);

		/**
		 * @brief Start recording performance data
		 */
		static void StartRecording();

		/**
		 * @brief Stop recording performance data
		 */
		static void StopRecording();

		/**
		 * @brief Clear recorded performance data
		 */
		static void ClearHistory();

		/**
		 * @brief Export performance data to CSV
		 * @param filename Output filename
		 */
		static void ExportToCSV(const std::string& filename);

		/**
		 * @brief Get current performance data
		 * @return Current performance data
		 */
		static PerformanceData GetCurrentData();

		/**
		 * @brief Get performance history
		 * @return Vector of historical performance data
		 */
		static std::vector<PerformanceData> GetHistory();

		/**
		 * @brief Set UI theme
		 * @param theme Theme name ("dark", "light", "classic")
		 */
		static void SetTheme(const std::string& theme);

		/**
		 * @brief Set update frequency
		 * @param frequency Updates per second
		 */
		static void SetUpdateFrequency(float frequency);

		/**
		 * @brief Enable/disable automatic benchmarking
		 * @param enable Whether to enable automatic benchmarking
		 */
		static void SetAutoBenchmark(bool enable);

		/**
		 * @brief Get UI statistics
		 * @return UI statistics
		 */
		static struct UIStats {
			bool isRecording;
			size_t historySize;
			float updateFrequency;
			bool autoBenchmark;
			std::string currentTheme;
		} GetUIStats();

	private:
		static bool s_Initialized;
		static bool s_ShowMainWindow;
		static bool s_ShowBenchmarkWindow;
		static bool s_ShowMemoryAnalysisWindow;
		static bool s_ShowRealTimeMonitoringWindow;
		static bool s_ShowPerformanceHistoryWindow;
		static bool s_IsRecording;
		static bool s_AutoBenchmark;
		static float s_UpdateFrequency;
		static std::string s_CurrentTheme;

		static PerformanceData s_CurrentData;
		static std::vector<PerformanceData> s_PerformanceHistory;
		static std::vector<float> s_FPSHistory;
		static std::vector<float> s_MemoryHistory;
		static std::vector<float> s_FrameTimeHistory;

		static float s_LastUpdateTime;
		static float s_UpdateTimer;
		static size_t s_MaxHistorySize;

		// UI rendering functions
		static void RenderMainWindow();
		static void RenderBenchmarkWindow();
		static void RenderMemoryAnalysisWindow();
		static void RenderRealTimeMonitoringWindow();
		static void RenderPerformanceHistoryWindow();
		static void RenderPerformanceChart(const std::vector<float>& data, const std::string& title, 
			float minValue = 0.0f, float maxValue = 0.0f);
		static void RenderPerformanceTable();
		static void RenderSystemInfo();
		static void RenderOptimizationSuggestions();

		// Data processing functions
		static void UpdatePerformanceData(float deltaTime);
		static void AddToHistory(const PerformanceData& data);
		static void ProcessBenchmarkResults(const std::vector<PerformanceBenchmark::BenchmarkResult>& results);
		static void GenerateOptimizationSuggestions();

		// Utility functions
		static std::string FormatBytes(size_t bytes);
		static std::string FormatPercentage(double percentage);
		static std::string FormatTime(float time);
		static ImVec4 GetPerformanceColor(float value, float minValue, float maxValue);
		static void ApplyTheme(const std::string& theme);
	};

	/**
	 * @brief Performance overlay for minimal performance display
	 * @details Provides a minimal overlay for displaying key performance metrics
	 */
	class PerformanceOverlay
	{
	public:
		/**
		 * @brief Initialize the performance overlay
		 */
		static void Init();

		/**
		 * @brief Shutdown the performance overlay
		 */
		static void Shutdown();

		/**
		 * @brief Render the performance overlay
		 * @param show Whether to show the overlay
		 */
		static void Render(bool show = true);

		/**
		 * @brief Set overlay position
		 * @param position Overlay position (0=top-left, 1=top-right, 2=bottom-left, 3=bottom-right)
		 */
		static void SetPosition(int position);

		/**
		 * @brief Set overlay transparency
		 * @param alpha Transparency value (0.0-1.0)
		 */
		static void SetTransparency(float alpha);

		/**
		 * @brief Enable/disable specific metrics
		 * @param showFPS Whether to show FPS
		 * @param showFrameTime Whether to show frame time
		 * @param showMemory Whether to show memory usage
		 * @param showDrawCalls Whether to show draw calls
		 */
		static void SetMetrics(bool showFPS, bool showFrameTime, bool showMemory, bool showDrawCalls);

		/**
		 * @brief Update overlay data
		 * @param data Performance data
		 */
		static void UpdateData(const PerformanceMonitorUI::PerformanceData& data);

		/**
		 * @brief Format bytes into human-readable string
		 */
		static std::string FormatBytes(size_t bytes);

	private:
		static bool s_Initialized;
		static bool s_ShowOverlay;
		static int s_Position;
		static float s_Transparency;
		static bool s_ShowFPS;
		static bool s_ShowFrameTime;
		static bool s_ShowMemory;
		static bool s_ShowDrawCalls;

		static PerformanceMonitorUI::PerformanceData s_CurrentData;

		static void RenderOverlay();
		static ImVec2 GetOverlayPosition();
		static ImVec4 GetOverlayColor();
	};

}
