#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <memory>

namespace Zgine {

	/**
	 * @brief High-precision timer for performance measurement
	 * @details Provides microsecond precision timing for profiling
	 */
	class PerformanceTimer
	{
	public:
		PerformanceTimer() : m_StartTime(std::chrono::high_resolution_clock::now()) {}
		
		/**
		 * @brief Get elapsed time in milliseconds
		 */
		double GetElapsedMs() const
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime);
			return duration.count() / 1000.0;
		}
		
		/**
		 * @brief Get elapsed time in microseconds
		 */
		double GetElapsedUs() const
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime);
			return static_cast<double>(duration.count());
		}
		
		/**
		 * @brief Reset the timer
		 */
		void Reset()
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

	private:
		std::chrono::high_resolution_clock::time_point m_StartTime;
	};

	/**
	 * @brief RAII timer for automatic profiling
	 * @details Automatically measures time in a scope
	 */
	class ScopedTimer
	{
	public:
		explicit ScopedTimer(const std::string& name) : m_Name(name), m_Timer() {}
		
	~ScopedTimer()
	{
		// TODO: Implement performance profiling backend
		// For now, this is a placeholder
		//double elapsed = m_Timer.GetElapsedMs();
		//ZG_CORE_TRACE("{} took {} ms", m_Name, elapsed);
	}

	private:
		std::string m_Name;
		PerformanceTimer m_Timer;
	};

	/**
	 * @brief Performance profiling data structure
	 */
	struct ProfileData
	{
		std::string name;
		double totalTime = 0.0;
		double minTime = std::numeric_limits<double>::max();
		double maxTime = 0.0;
		uint32_t callCount = 0;
		double averageTime = 0.0;
		
		void Update(double time)
		{
			totalTime += time;
			minTime = std::min(minTime, time);
			maxTime = std::max(maxTime, time);
			callCount++;
			averageTime = totalTime / callCount;
		}
	};

	/**
	 * @brief Comprehensive performance profiler
	 * @details Tracks and analyzes performance metrics across the application
	 */
	class PerformanceProfiler
	{
	public:
		/**
		 * @brief Initialize the profiler
		 */
		static void Init();
		
		/**
		 * @brief Shutdown the profiler
		 */
		static void Shutdown();
		
		/**
		 * @brief Start profiling a scope
		 * @param name Scope name
		 * @return Timer for the scope
		 */
		static std::unique_ptr<PerformanceTimer> StartScope(const std::string& name);
		
		/**
		 * @brief Record scope timing
		 * @param name Scope name
		 * @param time Time in milliseconds
		 */
		static void RecordScope(const std::string& name, double time);
		
		/**
		 * @brief Record frame timing
		 * @param frameTime Frame time in milliseconds
		 */
		static void RecordFrame(double frameTime);
		
		/**
		 * @brief Record draw call
		 * @param vertexCount Number of vertices drawn
		 * @param indexCount Number of indices drawn
		 */
		static void RecordDrawCall(uint32_t vertexCount, uint32_t indexCount);
		
		/**
		 * @brief Record memory allocation
		 * @param size Allocation size in bytes
		 */
		static void RecordMemoryAllocation(size_t size);
		
		/**
		 * @brief Record memory deallocation
		 * @param size Deallocation size in bytes
		 */
		static void RecordMemoryDeallocation(size_t size);
		
		/**
		 * @brief Get profile data for a scope
		 * @param name Scope name
		 * @return Profile data or nullptr if not found
		 */
		static const ProfileData* GetProfileData(const std::string& name);
		
		/**
		 * @brief Get all profile data
		 * @return Map of all profile data
		 */
		static std::unordered_map<std::string, ProfileData> GetAllProfileData();
		
		/**
		 * @brief Get frame statistics
		 */
		struct FrameStats
		{
			double AverageFrameTime;
			double MinFrameTime;
			double MaxFrameTime;
			double FPS;
			uint32_t FrameCount;
			uint32_t TotalDrawCalls;
			uint32_t TotalVertices;
			uint32_t TotalIndices;
			size_t TotalMemoryAllocated;
			size_t PeakMemoryUsage;
		};
		
		static FrameStats GetFrameStats();
		
		/**
		 * @brief Clear all profiling data
		 */
		static void Clear();
		
		/**
		 * @brief Enable/disable profiling
		 * @param enabled Whether profiling is enabled
		 */
		static void SetEnabled(bool enabled);
		
		/**
		 * @brief Check if profiling is enabled
		 */
		static bool IsEnabled();

	private:
		static std::unordered_map<std::string, ProfileData> s_ProfileData;
		static std::vector<double> s_FrameTimes;
		static std::mutex s_Mutex;
		static bool s_Enabled;
		static bool s_Initialized;
		
		// Frame statistics
		static std::atomic<uint32_t> s_TotalDrawCalls;
		static std::atomic<uint32_t> s_TotalVertices;
		static std::atomic<uint32_t> s_TotalIndices;
		static std::atomic<size_t> s_TotalMemoryAllocated;
		static std::atomic<size_t> s_PeakMemoryUsage;
		static std::atomic<size_t> s_CurrentMemoryUsage;
	};

	/**
	 * @brief GPU performance profiler
	 * @details Tracks GPU-specific performance metrics
	 */
	class GPUProfiler
	{
	public:
		/**
		 * @brief Initialize GPU profiler
		 */
		static void Init();
		
		/**
		 * @brief Shutdown GPU profiler
		 */
		static void Shutdown();
		
		/**
		 * @brief Start GPU timing query
		 * @param name Query name
		 * @return Query ID
		 */
		static uint32_t StartQuery(const std::string& name);
		
		/**
		 * @brief End GPU timing query
		 * @param queryId Query ID
		 */
		static void EndQuery(uint32_t queryId);
		
		/**
		 * @brief Get GPU timing results
		 * @param name Query name
		 * @return Time in milliseconds, or -1 if not ready
		 */
		static double GetQueryResult(const std::string& name);
		
		/**
		 * @brief Get GPU statistics
		 */
		struct GPUStats
		{
			double AverageGPUTime;
			double MinGPUTime;
			double MaxGPUTime;
			uint32_t QueryCount;
			bool IsSupported;
		};
		
		static GPUStats GetStats();
		
		/**
		 * @brief Check if GPU profiling is supported
		 */
		static bool IsSupported();

	private:
		struct QueryData
		{
			uint32_t queryId;
			std::string name;
			bool isActive;
			std::chrono::high_resolution_clock::time_point startTime;
		};
		
		static std::unordered_map<std::string, QueryData> s_Queries;
		static std::unordered_map<std::string, double> s_Results;
		static std::mutex s_Mutex;
		static bool s_Initialized;
		static bool s_Supported;
		static uint32_t s_NextQueryId;
	};

	/**
	 * @brief Performance monitoring dashboard
	 * @details Provides real-time performance monitoring and visualization
	 */
	class PerformanceDashboard
	{
	public:
		/**
		 * @brief Initialize the dashboard
		 */
		static void Init();
		
		/**
		 * @brief Shutdown the dashboard
		 */
		static void Shutdown();
		
		/**
		 * @brief Render the dashboard
		 */
		static void Render();
		
		/**
		 * @brief Set dashboard visibility
		 * @param visible Whether dashboard is visible
		 */
		static void SetVisible(bool visible);
		
		/**
		 * @brief Check if dashboard is visible
		 */
		static bool IsVisible();
		
		/**
		 * @brief Set dashboard position
		 * @param x X position
		 * @param y Y position
		 */
		static void SetPosition(float x, float y);
		
		/**
		 * @brief Set dashboard size
		 * @param width Width
		 * @param height Height
		 */
		static void SetSize(float width, float height);

	private:
		static bool s_Visible;
		static bool s_Initialized;
		static float s_PositionX;
		static float s_PositionY;
		static float s_Width;
		static float s_Height;
		
		static void RenderFrameStats();
		static void RenderProfileData();
		static void RenderMemoryStats();
		static void RenderGPUStats();
	};

	// Convenience macros for profiling
	#define ZG_PROFILE_SCOPE(name) Zgine::ScopedTimer _timer(name)
	#define ZG_PROFILE_FUNCTION() Zgine::ScopedTimer _timer(__FUNCTION__)

}
