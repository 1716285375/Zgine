#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>

namespace Zgine {

	/**
	 * @brief High-performance object pool for rendering primitives
	 * @details Provides efficient allocation and reuse of rendering objects
	 */
	template<typename T>
	class ObjectPool
	{
	public:
		explicit ObjectPool(size_t initialSize = 100, size_t maxSize = 10000)
			: m_MaxSize(maxSize), m_TotalAllocated(0), m_TotalReused(0)
		{
			ExpandPool(initialSize);
		}

		~ObjectPool()
		{
			Clear();
		}

		/**
		 * @brief Get an object from the pool
		 * @param constructor Function to construct the object
		 * @return Shared pointer to the object
		 */
		template<typename... Args>
		std::shared_ptr<T> Acquire(Args&&... args)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			if (m_FreeList.empty())
			{
				if (m_TotalAllocated >= m_MaxSize)
				{
					// Pool is full, create a temporary object
					return std::make_shared<T>(std::forward<Args>(args)...);
				}
				ExpandPool(std::min(m_TotalAllocated, m_MaxSize - m_TotalAllocated));
			}

			T* obj = m_FreeList.back();
			m_FreeList.pop_back();
			m_TotalAllocated++;
			
			// Construct the object in place
			new(obj) T(std::forward<Args>(args)...);
			
			return std::shared_ptr<T>(obj, [this](T* ptr) {
				this->Release(ptr);
			});
		}

		/**
		 * @brief Release an object back to the pool
		 * @param obj Object to release
		 */
		void Release(T* obj)
		{
			if (!obj) return;
			
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			// Call destructor
			obj->~T();
			
			m_FreeList.push_back(obj);
			m_TotalReused++;
		}

		/**
		 * @brief Get pool statistics
		 */
		struct PoolStats
		{
			size_t TotalAllocated;
			size_t FreeCount;
			size_t TotalReused;
			size_t MaxSize;
			double ReuseRate;
		};

		PoolStats GetStats() const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return {
				m_TotalAllocated,
				m_FreeList.size(),
				m_TotalReused,
				m_MaxSize,
				m_TotalAllocated > 0 ? static_cast<double>(m_TotalReused) / m_TotalAllocated : 0.0
			};
		}

		/**
		 * @brief Clear all objects from the pool
		 */
		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			for (auto& block : m_MemoryBlocks)
			{
				delete[] block;
			}
			m_MemoryBlocks.clear();
			m_FreeList.clear();
			m_TotalAllocated = 0;
			m_TotalReused = 0;
		}

		/**
		 * @brief Set maximum pool size
		 */
		void SetMaxSize(size_t maxSize)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_MaxSize = maxSize;
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
		}

		std::vector<T*> m_MemoryBlocks;
		std::vector<T*> m_FreeList;
		mutable std::mutex m_Mutex;
		size_t m_MaxSize;
		std::atomic<size_t> m_TotalAllocated;
		std::atomic<size_t> m_TotalReused;
	};

	/**
	 * @brief Render command pool for batching operations
	 * @details Efficiently manages render commands for batch processing
	 */
	struct RenderCommand
	{
		enum class Type
		{
			DrawQuad,
			DrawLine,
			DrawCircle,
			DrawTriangle,
			DrawEllipse,
			DrawArc,
			DrawQuadGradient,
			DrawRotatedQuad,
			DrawRotatedQuadGradient,
			DrawQuadTransform
		};

		Type type;
		glm::vec3 position;
		glm::vec2 size;
		glm::vec4 color;
		float rotation;
		float thickness;
		int segments;
		int entityID;
		uint32_t textureID;
		
		// Additional data for complex shapes
		glm::vec3 p1, p2, p3; // For triangles and lines
		glm::vec4 colorTopLeft, colorTopRight, colorBottomLeft, colorBottomRight; // For gradients
		glm::mat4 transform; // For transformed quads
		
		RenderCommand() = default;
	};

	/**
	 * @brief Render command batch processor
	 * @details Processes render commands in batches for optimal performance
	 */
	class RenderCommandBatch
	{
	public:
		explicit RenderCommandBatch(size_t initialCapacity = 1000)
			: m_Capacity(initialCapacity), m_Size(0)
		{
			m_Commands.reserve(initialCapacity);
		}

		/**
		 * @brief Add a render command to the batch
		 */
		void AddCommand(const RenderCommand& command)
		{
			if (m_Size >= m_Capacity)
			{
				Flush();
			}
			
			m_Commands.push_back(command);
			m_Size++;
		}

		/**
		 * @brief Process all commands in the batch
		 */
		void Flush()
		{
			if (m_Size == 0) return;
			
			// Sort commands by type for better batching
			std::sort(m_Commands.begin(), m_Commands.end(), [](const RenderCommand& a, const RenderCommand& b) {
				return static_cast<int>(a.type) < static_cast<int>(b.type);
			});
			
			// Process commands in batches by type
			ProcessCommandsByType();
			
			// Clear the batch
			m_Commands.clear();
			m_Size = 0;
		}

		/**
		 * @brief Get current batch size
		 */
		size_t Size() const { return m_Size; }

		/**
		 * @brief Get batch capacity
		 */
		size_t Capacity() const { return m_Capacity; }

		/**
		 * @brief Set batch capacity
		 */
		void SetCapacity(size_t capacity)
		{
			m_Capacity = capacity;
			m_Commands.reserve(capacity);
		}

		/**
		 * @brief Clear the batch
		 */
		void Clear()
		{
			m_Commands.clear();
			m_Size = 0;
		}

	private:
		void ProcessCommandsByType()
		{
			if (m_Commands.empty()) return;
			
			RenderCommand::Type currentType = m_Commands[0].type;
			size_t startIndex = 0;
			
			for (size_t i = 1; i < m_Commands.size(); ++i)
			{
				if (m_Commands[i].type != currentType)
				{
					ProcessCommandBatch(currentType, startIndex, i - startIndex);
					currentType = m_Commands[i].type;
					startIndex = i;
				}
			}
			
			// Process the last batch
			ProcessCommandBatch(currentType, startIndex, m_Commands.size() - startIndex);
		}

		void ProcessCommandBatch(RenderCommand::Type type, size_t startIndex, size_t count)
		{
			switch (type)
			{
			case RenderCommand::Type::DrawQuad:
				ProcessQuadBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawLine:
				ProcessLineBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawCircle:
				ProcessCircleBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawTriangle:
				ProcessTriangleBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawEllipse:
				ProcessEllipseBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawArc:
				ProcessArcBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawQuadGradient:
				ProcessQuadGradientBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawRotatedQuad:
				ProcessRotatedQuadBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawRotatedQuadGradient:
				ProcessRotatedQuadGradientBatch(startIndex, count);
				break;
			case RenderCommand::Type::DrawQuadTransform:
				ProcessQuadTransformBatch(startIndex, count);
				break;
			}
		}

		// Batch processing methods for each command type
		void ProcessQuadBatch(size_t startIndex, size_t count);
		void ProcessLineBatch(size_t startIndex, size_t count);
		void ProcessCircleBatch(size_t startIndex, size_t count);
		void ProcessTriangleBatch(size_t startIndex, size_t count);
		void ProcessEllipseBatch(size_t startIndex, size_t count);
		void ProcessArcBatch(size_t startIndex, size_t count);
		void ProcessQuadGradientBatch(size_t startIndex, size_t count);
		void ProcessRotatedQuadBatch(size_t startIndex, size_t count);
		void ProcessRotatedQuadGradientBatch(size_t startIndex, size_t count);
		void ProcessQuadTransformBatch(size_t startIndex, size_t count);

		std::vector<RenderCommand> m_Commands;
		size_t m_Capacity;
		size_t m_Size;
	};

	/**
	 * @brief Global render command manager
	 * @details Manages render commands across the entire application
	 */
	class RenderCommandManager
	{
	public:
		static void Init();
		static void Shutdown();
		
		static RenderCommandBatch& GetCurrentBatch();
		static void FlushCurrentBatch();
		static void ClearAllBatches();
		
		static void SetBatchCapacity(size_t capacity);
		static size_t GetTotalCommandCount();
		
		static void AddCommand(const RenderCommand& command);
		
		// Statistics
		struct ManagerStats
		{
			size_t TotalCommands;
			size_t TotalBatches;
			size_t AverageBatchSize;
			double AverageFlushTime;
		};
		
		static ManagerStats GetStats();

	private:
		static std::unique_ptr<RenderCommandBatch> s_CurrentBatch;
		static std::vector<std::unique_ptr<RenderCommandBatch>> s_BatchPool;
		static size_t s_BatchCapacity;
		static size_t s_TotalCommands;
		static size_t s_TotalBatches;
		static bool s_Initialized;
	};

}
