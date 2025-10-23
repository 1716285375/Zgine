#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace Zgine {

	/**
	 * @brief High-performance memory pool for rendering objects
	 * @details Provides efficient memory allocation and deallocation for rendering primitives
	 */
	template<typename T>
	class MemoryPool
	{
	public:
		explicit MemoryPool(size_t initialSize = 1000, size_t growthFactor = 2)
			: m_GrowthFactor(growthFactor), m_TotalAllocated(0)
		{
			ExpandPool(initialSize);
		}

		~MemoryPool()
		{
			for (auto& block : m_MemoryBlocks)
			{
				delete[] block;
			}
		}

		/**
		 * @brief Allocate memory for an object
		 * @return Pointer to allocated memory
		 */
		T* Allocate()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			if (m_FreeList.empty())
			{
				ExpandPool(m_CurrentPoolSize * m_GrowthFactor);
			}

			T* ptr = m_FreeList.back();
			m_FreeList.pop_back();
			m_TotalAllocated++;
			
			return ptr;
		}

		/**
		 * @brief Deallocate memory back to the pool
		 * @param ptr Pointer to deallocate
		 */
		void Deallocate(T* ptr)
		{
			if (!ptr) return;
			
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_FreeList.push_back(ptr);
			m_TotalAllocated--;
		}

		/**
		 * @brief Get current pool statistics
		 */
		struct PoolStats
		{
			size_t TotalBlocks;
			size_t FreeCount;
			size_t AllocatedCount;
			size_t TotalMemoryBytes;
		};

		PoolStats GetStats() const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return {
				m_MemoryBlocks.size(),
				m_FreeList.size(),
				m_TotalAllocated,
				m_CurrentPoolSize * sizeof(T) * m_MemoryBlocks.size()
			};
		}

		/**
		 * @brief Clear all allocated memory
		 */
		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_FreeList.clear();
			m_TotalAllocated = 0;
			
			for (auto& block : m_MemoryBlocks)
			{
				delete[] block;
			}
			m_MemoryBlocks.clear();
			m_CurrentPoolSize = 0;
		}

	private:
		void ExpandPool(size_t newSize)
		{
			T* newBlock = new T[newSize];
			m_MemoryBlocks.push_back(newBlock);
			
			// Add all new memory to free list
			for (size_t i = 0; i < newSize; ++i)
			{
				m_FreeList.push_back(&newBlock[i]);
			}
			
			m_CurrentPoolSize = newSize;
		}

		std::vector<T*> m_MemoryBlocks;
		std::vector<T*> m_FreeList;
		mutable std::mutex m_Mutex;
		size_t m_GrowthFactor;
		size_t m_CurrentPoolSize = 0;
		std::atomic<size_t> m_TotalAllocated;
	};

	/**
	 * @brief High-performance ring buffer for vertex data
	 * @details Provides efficient circular buffer for vertex streaming
	 */
	template<typename T>
	class RingBuffer
	{
	public:
		explicit RingBuffer(size_t capacity)
			: m_Capacity(capacity), m_Head(0), m_Tail(0), m_Size(0)
		{
			m_Buffer = std::make_unique<T[]>(capacity);
		}

		/**
		 * @brief Push data to the buffer
		 * @param data Data to push
		 * @return true if successful, false if buffer is full
		 */
		bool Push(const T& data)
		{
			if (m_Size >= m_Capacity)
				return false;

			m_Buffer[m_Tail] = data;
			m_Tail = (m_Tail + 1) % m_Capacity;
			m_Size++;
			return true;
		}

		/**
		 * @brief Push multiple data items
		 * @param data Array of data
		 * @param count Number of items to push
		 * @return Number of items actually pushed
		 */
		size_t Push(const T* data, size_t count)
		{
			size_t pushed = 0;
			for (size_t i = 0; i < count && m_Size < m_Capacity; ++i)
			{
				if (Push(data[i]))
					pushed++;
			}
			return pushed;
		}

		/**
		 * @brief Pop data from the buffer
		 * @param data Reference to store popped data
		 * @return true if successful, false if buffer is empty
		 */
		bool Pop(T& data)
		{
			if (m_Size == 0)
				return false;

			data = m_Buffer[m_Head];
			m_Head = (m_Head + 1) % m_Capacity;
			m_Size--;
			return true;
		}

		/**
		 * @brief Get current buffer size
		 */
		size_t Size() const { return m_Size; }

		/**
		 * @brief Get buffer capacity
		 */
		size_t Capacity() const { return m_Capacity; }

		/**
		 * @brief Check if buffer is empty
		 */
		bool Empty() const { return m_Size == 0; }

		/**
		 * @brief Check if buffer is full
		 */
		bool Full() const { return m_Size >= m_Capacity; }

		/**
		 * @brief Clear the buffer
		 */
		void Clear()
		{
			m_Head = 0;
			m_Tail = 0;
			m_Size = 0;
		}

		/**
		 * @brief Get pointer to buffer data (for direct access)
		 */
		T* Data() { return m_Buffer.get(); }

		/**
		 * @brief Get const pointer to buffer data
		 */
		const T* Data() const { return m_Buffer.get(); }

	private:
		std::unique_ptr<T[]> m_Buffer;
		size_t m_Capacity;
		size_t m_Head;
		size_t m_Tail;
		size_t m_Size;
	};

	/**
	 * @brief Performance statistics tracker
	 * @details Tracks rendering performance metrics
	 */
	class PerformanceTracker
	{
	public:
		struct FrameStats
		{
			uint32_t DrawCalls = 0;
			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;
			uint32_t TextureBinds = 0;
			uint32_t ShaderSwitches = 0;
			float FrameTime = 0.0f;
			float GPUTime = 0.0f;
		};

		static void RecordDrawCall() { s_CurrentFrame.DrawCalls++; }
		static void RecordVertices(uint32_t count) { s_CurrentFrame.VertexCount += count; }
		static void RecordIndices(uint32_t count) { s_CurrentFrame.IndexCount += count; }
		static void RecordTextureBind() { s_CurrentFrame.TextureBinds++; }
		static void RecordShaderSwitch() { s_CurrentFrame.ShaderSwitches++; }
		static void RecordFrameTime(float time) { s_CurrentFrame.FrameTime = time; }
		static void RecordGPUTime(float time) { s_CurrentFrame.GPUTime = time; }

		static void EndFrame()
		{
			s_FrameHistory.push_back(s_CurrentFrame);
			if (s_FrameHistory.size() > s_MaxHistorySize)
			{
				s_FrameHistory.erase(s_FrameHistory.begin());
			}
			s_CurrentFrame = FrameStats{};
		}

		static FrameStats GetCurrentFrame() { return s_CurrentFrame; }
		static std::vector<FrameStats> GetFrameHistory() { return s_FrameHistory; }

		static FrameStats GetAverageStats()
		{
			if (s_FrameHistory.empty())
				return FrameStats{};

			FrameStats avg{};
			for (const auto& frame : s_FrameHistory)
			{
				avg.DrawCalls += frame.DrawCalls;
				avg.VertexCount += frame.VertexCount;
				avg.IndexCount += frame.IndexCount;
				avg.TextureBinds += frame.TextureBinds;
				avg.ShaderSwitches += frame.ShaderSwitches;
				avg.FrameTime += frame.FrameTime;
				avg.GPUTime += frame.GPUTime;
			}

			size_t count = s_FrameHistory.size();
			avg.DrawCalls /= count;
			avg.VertexCount /= count;
			avg.IndexCount /= count;
			avg.TextureBinds /= count;
			avg.ShaderSwitches /= count;
			avg.FrameTime /= count;
			avg.GPUTime /= count;

			return avg;
		}

	private:
		static FrameStats s_CurrentFrame;
		static std::vector<FrameStats> s_FrameHistory;
		static const size_t s_MaxHistorySize = 60; // 1 second at 60 FPS
	};

}
