#include "PerformanceProfiler.h"
#include "../Core.h"
#include <algorithm>
#include <limits>

namespace Zgine {

	// PerformanceProfiler static members
	std::unordered_map<std::string, ProfileData> PerformanceProfiler::s_ProfileData;
	std::vector<double> PerformanceProfiler::s_FrameTimes;
	std::mutex PerformanceProfiler::s_Mutex;
	bool PerformanceProfiler::s_Enabled = true;
	bool PerformanceProfiler::s_Initialized = false;

	// Frame statistics
	std::atomic<uint32_t> PerformanceProfiler::s_TotalDrawCalls{0};
	std::atomic<uint32_t> PerformanceProfiler::s_TotalVertices{0};
	std::atomic<uint32_t> PerformanceProfiler::s_TotalIndices{0};
	std::atomic<size_t> PerformanceProfiler::s_TotalMemoryAllocated{0};
	std::atomic<size_t> PerformanceProfiler::s_PeakMemoryUsage{0};
	std::atomic<size_t> PerformanceProfiler::s_CurrentMemoryUsage{0};

	// GPUProfiler static members
	std::unordered_map<std::string, GPUProfiler::QueryData> GPUProfiler::s_Queries;
	std::unordered_map<std::string, double> GPUProfiler::s_Results;
	std::mutex GPUProfiler::s_Mutex;
	bool GPUProfiler::s_Initialized = false;
	bool GPUProfiler::s_Supported = false;
	uint32_t GPUProfiler::s_NextQueryId = 1;

	// PerformanceDashboard static members
	bool PerformanceDashboard::s_Visible = false;
	bool PerformanceDashboard::s_Initialized = false;
	float PerformanceDashboard::s_PositionX = 10.0f;
	float PerformanceDashboard::s_PositionY = 10.0f;
	float PerformanceDashboard::s_Width = 300.0f;
	float PerformanceDashboard::s_Height = 400.0f;

	// PerformanceProfiler implementation
	void PerformanceProfiler::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("PerformanceProfiler::Init() called multiple times");
			return;
		}
		
		s_FrameTimes.reserve(60); // Reserve space for 1 second at 60 FPS
		s_Initialized = true;
		
		ZG_CORE_INFO("PerformanceProfiler::Init() completed");
	}

	void PerformanceProfiler::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("PerformanceProfiler::Shutdown() called without initialization");
			return;
		}
		
		s_ProfileData.clear();
		s_FrameTimes.clear();
		s_Initialized = false;
		
		ZG_CORE_INFO("PerformanceProfiler::Shutdown() completed");
	}

	std::unique_ptr<PerformanceTimer> PerformanceProfiler::StartScope(const std::string& name)
	{
		if (!s_Enabled) return nullptr;
		
		return std::make_unique<PerformanceTimer>();
	}

	void PerformanceProfiler::RecordScope(const std::string& name, double time)
	{
		if (!s_Enabled) return;
		
		std::lock_guard<std::mutex> lock(s_Mutex);
		s_ProfileData[name].Update(time);
	}

	void PerformanceProfiler::RecordFrame(double frameTime)
	{
		if (!s_Enabled) return;
		
		std::lock_guard<std::mutex> lock(s_Mutex);
		
		s_FrameTimes.push_back(frameTime);
		
		// Keep only the last 60 frames (1 second at 60 FPS)
		if (s_FrameTimes.size() > 60)
		{
			s_FrameTimes.erase(s_FrameTimes.begin());
		}
	}

	void PerformanceProfiler::RecordDrawCall(uint32_t vertexCount, uint32_t indexCount)
	{
		if (!s_Enabled) return;
		
		s_TotalDrawCalls++;
		s_TotalVertices += vertexCount;
		s_TotalIndices += indexCount;
	}

	void PerformanceProfiler::RecordMemoryAllocation(size_t size)
	{
		if (!s_Enabled) return;
		
		s_TotalMemoryAllocated += size;
		s_CurrentMemoryUsage += size;
		
		size_t current = s_CurrentMemoryUsage.load();
		size_t peak = s_PeakMemoryUsage.load();
		
		while (current > peak && !s_PeakMemoryUsage.compare_exchange_weak(peak, current))
		{
			peak = s_PeakMemoryUsage.load();
		}
	}

	void PerformanceProfiler::RecordMemoryDeallocation(size_t size)
	{
		if (!s_Enabled) return;
		
		s_CurrentMemoryUsage -= size;
	}

	const ProfileData* PerformanceProfiler::GetProfileData(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(s_Mutex);
		auto it = s_ProfileData.find(name);
		return (it != s_ProfileData.end()) ? &it->second : nullptr;
	}

	std::unordered_map<std::string, ProfileData> PerformanceProfiler::GetAllProfileData()
	{
		std::lock_guard<std::mutex> lock(s_Mutex);
		return s_ProfileData;
	}

	PerformanceProfiler::FrameStats PerformanceProfiler::GetFrameStats()
	{
		std::lock_guard<std::mutex> lock(s_Mutex);
		
		FrameStats stats{};
		
		if (!s_FrameTimes.empty())
		{
			double totalTime = 0.0;
			stats.MinFrameTime = std::numeric_limits<double>::max();
			stats.MaxFrameTime = 0.0;
			
			for (double frameTime : s_FrameTimes)
			{
				totalTime += frameTime;
				stats.MinFrameTime = std::min(stats.MinFrameTime, frameTime);
				stats.MaxFrameTime = std::max(stats.MaxFrameTime, frameTime);
			}
			
			stats.AverageFrameTime = totalTime / s_FrameTimes.size();
			stats.FPS = 1000.0 / stats.AverageFrameTime; // Convert ms to FPS
		}
		
		stats.FrameCount = static_cast<uint32_t>(s_FrameTimes.size());
		stats.TotalDrawCalls = s_TotalDrawCalls.load();
		stats.TotalVertices = s_TotalVertices.load();
		stats.TotalIndices = s_TotalIndices.load();
		stats.TotalMemoryAllocated = s_TotalMemoryAllocated.load();
		stats.PeakMemoryUsage = s_PeakMemoryUsage.load();
		
		return stats;
	}

	void PerformanceProfiler::Clear()
	{
		std::lock_guard<std::mutex> lock(s_Mutex);
		s_ProfileData.clear();
		s_FrameTimes.clear();
		s_TotalDrawCalls = 0;
		s_TotalVertices = 0;
		s_TotalIndices = 0;
		s_TotalMemoryAllocated = 0;
		s_PeakMemoryUsage = 0;
		s_CurrentMemoryUsage = 0;
	}

	void PerformanceProfiler::SetEnabled(bool enabled)
	{
		s_Enabled = enabled;
		ZG_CORE_INFO("PerformanceProfiler::SetEnabled() - {}", enabled ? "enabled" : "disabled");
	}

	bool PerformanceProfiler::IsEnabled()
	{
		return s_Enabled;
	}

	// GPUProfiler implementation
	void GPUProfiler::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("GPUProfiler::Init() called multiple times");
			return;
		}
		
		// Check if GPU profiling is supported
		// This is a simplified check - in practice, you'd check for specific OpenGL extensions
		s_Supported = true; // Assume supported for now
		s_Initialized = true;
		
		ZG_CORE_INFO("GPUProfiler::Init() completed - GPU profiling {}", s_Supported ? "supported" : "not supported");
	}

	void GPUProfiler::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("GPUProfiler::Shutdown() called without initialization");
			return;
		}
		
		s_Queries.clear();
		s_Results.clear();
		s_Initialized = false;
		
		ZG_CORE_INFO("GPUProfiler::Shutdown() completed");
	}

	uint32_t GPUProfiler::StartQuery(const std::string& name)
	{
		if (!s_Supported || !s_Initialized) return 0;
		
		std::lock_guard<std::mutex> lock(s_Mutex);
		
		uint32_t queryId = s_NextQueryId++;
		s_Queries[name] = {
			queryId,
			name,
			true,
			std::chrono::high_resolution_clock::now()
		};
		
		return queryId;
	}

	void GPUProfiler::EndQuery(uint32_t queryId)
	{
		if (!s_Supported || !s_Initialized) return;
		
		std::lock_guard<std::mutex> lock(s_Mutex);
		
		// Find the query by ID
		for (auto& pair : s_Queries)
		{
			if (pair.second.queryId == queryId && pair.second.isActive)
			{
				pair.second.isActive = false;
				
				// Calculate elapsed time (simplified - in practice, you'd use OpenGL queries)
				auto endTime = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - pair.second.startTime);
				double timeMs = duration.count() / 1000.0;
				
				s_Results[pair.first] = timeMs;
				break;
			}
		}
	}

	double GPUProfiler::GetQueryResult(const std::string& name)
	{
		if (!s_Supported || !s_Initialized) return -1.0;
		
		std::lock_guard<std::mutex> lock(s_Mutex);
		auto it = s_Results.find(name);
		return (it != s_Results.end()) ? it->second : -1.0;
	}

	GPUProfiler::GPUStats GPUProfiler::GetStats()
	{
		std::lock_guard<std::mutex> lock(s_Mutex);
		
		GPUStats stats{};
		stats.IsSupported = s_Supported;
		stats.QueryCount = static_cast<uint32_t>(s_Results.size());
		
		if (!s_Results.empty())
		{
			double totalTime = 0.0;
			stats.MinGPUTime = std::numeric_limits<double>::max();
			stats.MaxGPUTime = 0.0;
			
			for (const auto& pair : s_Results)
			{
				double time = pair.second;
				totalTime += time;
				stats.MinGPUTime = std::min(stats.MinGPUTime, time);
				stats.MaxGPUTime = std::max(stats.MaxGPUTime, time);
			}
			
			stats.AverageGPUTime = totalTime / s_Results.size();
		}
		
		return stats;
	}

	bool GPUProfiler::IsSupported()
	{
		return s_Supported;
	}

	// PerformanceDashboard implementation
	void PerformanceDashboard::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("PerformanceDashboard::Init() called multiple times");
			return;
		}
		
		s_Initialized = true;
		ZG_CORE_INFO("PerformanceDashboard::Init() completed");
	}

	void PerformanceDashboard::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("PerformanceDashboard::Shutdown() called without initialization");
			return;
		}
		
		s_Initialized = false;
		ZG_CORE_INFO("PerformanceDashboard::Shutdown() completed");
	}

	void PerformanceDashboard::Render()
	{
		if (!s_Visible || !s_Initialized) return;
		
		// This would typically use ImGui or similar for rendering
		// For now, we'll just log the performance data
		auto frameStats = PerformanceProfiler::GetFrameStats();
		auto profileData = PerformanceProfiler::GetAllProfileData();
		
		ZG_CORE_TRACE("Performance Dashboard - FPS: {:.1f}, Frame Time: {:.2f}ms, Draw Calls: {}, Vertices: {}", 
			frameStats.FPS, frameStats.AverageFrameTime, frameStats.TotalDrawCalls, frameStats.TotalVertices);
		
		for (const auto& pair : profileData)
		{
			ZG_CORE_TRACE("  {} - Avg: {:.2f}ms, Min: {:.2f}ms, Max: {:.2f}ms, Calls: {}", 
				pair.first, pair.second.averageTime, pair.second.minTime, pair.second.maxTime, pair.second.callCount);
		}
	}

	void PerformanceDashboard::SetVisible(bool visible)
	{
		s_Visible = visible;
	}

	bool PerformanceDashboard::IsVisible()
	{
		return s_Visible;
	}

	void PerformanceDashboard::SetPosition(float x, float y)
	{
		s_PositionX = x;
		s_PositionY = y;
	}

	void PerformanceDashboard::SetSize(float width, float height)
	{
		s_Width = width;
		s_Height = height;
	}

	void PerformanceDashboard::RenderFrameStats()
	{
		// Implementation would render frame statistics
	}

	void PerformanceDashboard::RenderProfileData()
	{
		// Implementation would render profile data
	}

	void PerformanceDashboard::RenderMemoryStats()
	{
		// Implementation would render memory statistics
	}

	void PerformanceDashboard::RenderGPUStats()
	{
		// Implementation would render GPU statistics
	}

}
