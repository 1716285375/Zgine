#pragma once

#include "zgpch.h"
#include "../Core.h"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <mutex>

namespace Zgine {

	/**
	 * @brief Memory analysis and profiling system
	 * @details Provides comprehensive memory usage analysis, leak detection,
	 *          and memory optimization recommendations
	 */
	class MemoryAnalyzer
	{
	public:
		struct MemorySnapshot
		{
			std::chrono::high_resolution_clock::time_point timestamp;
			size_t totalMemory;
			size_t heapMemory;
			size_t stackMemory;
			size_t textureMemory;
			size_t bufferMemory;
			size_t shaderMemory;
			uint32_t allocationCount;
			uint32_t deallocationCount;
			std::string description;
		};

		struct MemoryLeak
		{
			void* address;
			size_t size;
			std::string type;
			std::string file;
			uint32_t line;
			std::chrono::high_resolution_clock::time_point allocationTime;
		};

		struct MemoryStats
		{
			size_t peakMemory;
			size_t currentMemory;
			size_t totalAllocations;
			size_t totalDeallocations;
			size_t activeAllocations;
			double averageAllocationSize;
			double fragmentationRatio;
			std::vector<MemoryLeak> leaks;
		};

		/**
		 * @brief Initialize the memory analyzer
		 */
		static void Init();

		/**
		 * @brief Shutdown the memory analyzer
		 */
		static void Shutdown();

		/**
		 * @brief Take a memory snapshot
		 * @param description Description of the snapshot
		 * @return Memory snapshot
		 */
		static MemorySnapshot TakeSnapshot(const std::string& description = "");

		/**
		 * @brief Compare two memory snapshots
		 * @param before Snapshot taken before
		 * @param after Snapshot taken after
		 * @return Difference analysis
		 */
		static std::string CompareSnapshots(const MemorySnapshot& before, const MemorySnapshot& after);

		/**
		 * @brief Detect memory leaks
		 * @return Vector of detected leaks
		 */
		static std::vector<MemoryLeak> DetectLeaks();

		/**
		 * @brief Get current memory statistics
		 * @return Current memory statistics
		 */
		static MemoryStats GetMemoryStats();

		/**
		 * @brief Start memory tracking
		 */
		static void StartTracking();

		/**
		 * @brief Stop memory tracking
		 */
		static void StopTracking();

		/**
		 * @brief Check if memory tracking is active
		 * @return True if tracking is active
		 */
		static bool IsTrackingActive();

		/**
		 * @brief Generate memory report
		 * @return Formatted memory report
		 */
		static std::string GenerateReport();

		/**
		 * @brief Save memory report to file
		 * @param filename Output filename
		 */
		static void SaveReport(const std::string& filename);

		/**
		 * @brief Get memory usage by category
		 * @return Map of category to memory usage
		 */
		static std::unordered_map<std::string, size_t> GetMemoryUsageByCategory();

		/**
		 * @brief Get allocation history
		 * @return Vector of allocation events
		 */
		static std::vector<MemorySnapshot> GetAllocationHistory();

		/**
		 * @brief Clear allocation history
		 */
		static void ClearHistory();

		/**
		 * @brief Set memory tracking options
		 * @param trackAllocations Whether to track allocations
		 * @param trackDeallocations Whether to track deallocations
		 * @param trackStack Whether to track stack usage
		 * @param trackHeap Whether to track heap usage
		 */
		static void SetTrackingOptions(bool trackAllocations, bool trackDeallocations, 
			bool trackStack = true, bool trackHeap = true);

		/**
		 * @brief Get memory optimization suggestions
		 * @return Vector of optimization suggestions
		 */
		static std::vector<std::string> GetOptimizationSuggestions();

		/**
		 * @brief Force garbage collection
		 */
		static void ForceGarbageCollection();

		/**
		 * @brief Get memory fragmentation info
		 * @return Fragmentation information
		 */
		static struct FragmentationInfo {
			double ratio;
			size_t largestFreeBlock;
			size_t totalFreeMemory;
			size_t totalAllocatedMemory;
		} GetFragmentationInfo();

	private:
		static bool s_Initialized;
		static bool s_TrackingActive;
		static bool s_TrackAllocations;
		static bool s_TrackDeallocations;
		static bool s_TrackStack;
		static bool s_TrackHeap;

		static std::vector<MemorySnapshot> s_Snapshots;
		static std::vector<MemoryLeak> s_DetectedLeaks;
		static std::mutex s_Mutex;

		/**
		 * @brief Get current memory usage
		 * @return Current memory usage in bytes
		 */
		static size_t GetCurrentMemoryUsage();

		/**
		 * @brief Get heap memory usage
		 * @return Heap memory usage in bytes
		 */
		static size_t GetHeapMemoryUsage();

		/**
		 * @brief Get stack memory usage
		 * @return Stack memory usage in bytes
		 */
		static size_t GetStackMemoryUsage();

		/**
		 * @brief Get texture memory usage
		 * @return Texture memory usage in bytes
		 */
		static size_t GetTextureMemoryUsage();

		/**
		 * @brief Get buffer memory usage
		 * @return Buffer memory usage in bytes
		 */
		static size_t GetBufferMemoryUsage();

		/**
		 * @brief Get shader memory usage
		 * @return Shader memory usage in bytes
		 */
		static size_t GetShaderMemoryUsage();

		/**
		 * @brief Format memory size
		 * @param bytes Memory size in bytes
		 * @return Formatted string
		 */
		static std::string FormatMemorySize(size_t bytes);

		/**
		 * @brief Format timestamp
		 * @param timestamp Timestamp to format
		 * @return Formatted string
		 */
		static std::string FormatTimestamp(const std::chrono::high_resolution_clock::time_point& timestamp);
	};

	/**
	 * @brief Memory allocation tracker for debugging
	 * @details Tracks individual memory allocations for debugging purposes
	 */
	class MemoryTracker
	{
	public:
		struct AllocationInfo
		{
			void* address;
			size_t size;
			std::string type;
			std::string file;
			uint32_t line;
			std::chrono::high_resolution_clock::time_point timestamp;
		};

		/**
		 * @brief Track a memory allocation
		 * @param address Memory address
		 * @param size Allocation size
		 * @param type Allocation type
		 * @param file Source file
		 * @param line Source line
		 */
		static void TrackAllocation(void* address, size_t size, const std::string& type, 
			const std::string& file, uint32_t line);

		/**
		 * @brief Track a memory deallocation
		 * @param address Memory address
		 */
		static void TrackDeallocation(void* address);

		/**
		 * @brief Get allocation info
		 * @param address Memory address
		 * @return Allocation info or nullptr if not found
		 */
		static AllocationInfo* GetAllocationInfo(void* address);

		/**
		 * @brief Get all active allocations
		 * @return Vector of active allocations
		 */
		static std::vector<AllocationInfo> GetActiveAllocations();

		/**
		 * @brief Clear all tracking data
		 */
		static void Clear();

		/**
		 * @brief Enable/disable tracking
		 * @param enable Whether to enable tracking
		 */
		static void SetEnabled(bool enable);

		/**
		 * @brief Check if tracking is enabled
		 * @return True if tracking is enabled
		 */
		static bool IsEnabled();

	private:
		static bool s_Enabled;
		static std::unordered_map<void*, AllocationInfo> s_Allocations;
		static std::mutex s_Mutex;
	};

}
