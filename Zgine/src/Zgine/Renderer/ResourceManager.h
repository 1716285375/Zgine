#pragma once

#include "Zgine/Core/SmartPointers.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>

#include "Zgine/Renderer/Texture.h"
#include "Zgine/Renderer/Shader.h"

namespace Zgine {

	/**
	 * @brief High-performance resource cache with LRU eviction
	 * @details Manages resources with automatic cleanup and memory optimization
	 */
	template<typename T>
	class ResourceCache
	{
	public:
		struct CacheEntry
		{
			Ref<T> resource;
			size_t lastAccess;
			size_t accessCount;
			size_t memorySize;
			
			CacheEntry() : resource(nullptr), lastAccess(0), accessCount(0), memorySize(0) {}
			CacheEntry(Ref<T> res, size_t size) 
				: resource(res), lastAccess(0), accessCount(0), memorySize(size) {}
		};

		explicit ResourceCache(size_t maxMemoryBytes = 100 * 1024 * 1024) // 100MB default
			: m_MaxMemoryBytes(maxMemoryBytes), m_CurrentMemoryBytes(0), m_AccessCounter(0)
		{
		}

		/**
		 * @brief Get a resource from cache
		 * @param key Resource key
		 * @return Shared pointer to resource, or nullptr if not found
		 */
		Ref<T> Get(const std::string& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			auto it = m_Cache.find(key);
			if (it != m_Cache.end())
			{
				it->second.lastAccess = ++m_AccessCounter;
				it->second.accessCount++;
				return it->second.resource;
			}
			
			return nullptr;
		}

		/**
		 * @brief Store a resource in cache
		 * @param key Resource key
		 * @param resource Resource to store
		 * @param memorySize Memory size of the resource
		 */
		void Store(const std::string& key, Ref<T> resource, size_t memorySize)
		{
			if (!resource) return;
			
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			// Remove existing entry if it exists
			RemoveEntry(key);
			
			// Check if we need to evict resources
			while (m_CurrentMemoryBytes + memorySize > m_MaxMemoryBytes && !m_Cache.empty())
			{
				EvictLeastRecentlyUsed();
			}
			
			// Add new entry
			m_Cache[key] = CacheEntry(resource, memorySize);
			m_CurrentMemoryBytes += memorySize;
		}

		/**
		 * @brief Remove a resource from cache
		 * @param key Resource key
		 */
		void Remove(const std::string& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			RemoveEntry(key);
		}

		/**
		 * @brief Clear all resources from cache
		 */
		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_Cache.clear();
			m_CurrentMemoryBytes = 0;
		}

		/**
		 * @brief Get cache statistics
		 */
		struct CacheStats
		{
			size_t EntryCount;
			size_t TotalMemoryBytes;
			size_t MaxMemoryBytes;
			double HitRate;
			size_t TotalAccesses;
		};

		CacheStats GetStats() const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			size_t totalAccesses = 0;
			for (const auto& entry : m_Cache)
			{
				totalAccesses += entry.second.accessCount;
			}
			
			return {
				m_Cache.size(),
				m_CurrentMemoryBytes,
				m_MaxMemoryBytes,
				m_TotalAccesses > 0 ? static_cast<double>(m_CacheHits) / m_TotalAccesses : 0.0,
				totalAccesses
			};
		}

		/**
		 * @brief Set maximum memory limit
		 */
		void SetMaxMemory(size_t maxMemoryBytes)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_MaxMemoryBytes = maxMemoryBytes;
			
			// Evict resources if we're over the new limit
			while (m_CurrentMemoryBytes > m_MaxMemoryBytes && !m_Cache.empty())
			{
				EvictLeastRecentlyUsed();
			}
		}

	private:
		void RemoveEntry(const std::string& key)
		{
			auto it = m_Cache.find(key);
			if (it != m_Cache.end())
			{
				m_CurrentMemoryBytes -= it->second.memorySize;
				m_Cache.erase(it);
			}
		}

		void EvictLeastRecentlyUsed()
		{
			if (m_Cache.empty()) return;
			
			auto oldest = m_Cache.begin();
			for (auto it = m_Cache.begin(); it != m_Cache.end(); ++it)
			{
				if (it->second.lastAccess < oldest->second.lastAccess)
				{
					oldest = it;
				}
			}
			
			m_CurrentMemoryBytes -= oldest->second.memorySize;
			m_Cache.erase(oldest);
		}

		std::unordered_map<std::string, CacheEntry> m_Cache;
		mutable std::mutex m_Mutex;
		size_t m_MaxMemoryBytes;
		size_t m_CurrentMemoryBytes;
		std::atomic<size_t> m_AccessCounter;
		std::atomic<size_t> m_TotalAccesses{0};
		std::atomic<size_t> m_CacheHits{0};
	};

	/**
	 * @brief Optimized texture manager with caching and compression
	 * @details Manages texture loading, caching, and memory optimization
	 */
	class TextureManager
	{
	public:
		static void Init();
		static void Shutdown();
		
		/**
		 * @brief Load texture with caching
		 * @param path Texture file path
		 * @return Shared pointer to texture
		 */
		static Ref<Texture2D> LoadTexture(const std::string& path);
		
		/**
		 * @brief Create texture from data
		 * @param data Texture data
		 * @param width Texture width
		 * @param height Texture height
		 * @param format Texture format
		 * @return Shared pointer to texture
		 */
		static Ref<Texture2D> CreateTexture(const void* data, uint32_t width, uint32_t height);
		
		/**
		 * @brief Get texture by ID
		 * @param textureID OpenGL texture ID
		 * @return Shared pointer to texture
		 */
		static Ref<Texture2D> GetTexture(uint32_t textureID);
		
		/**
		 * @brief Preload textures for better performance
		 * @param paths List of texture paths to preload
		 */
		static void PreloadTextures(const std::vector<std::string>& paths);
		
		/**
		 * @brief Clear texture cache
		 */
		static void ClearCache();
		
		/**
		 * @brief Get texture manager statistics
		 */
		struct TextureStats
		{
			size_t LoadedTextures;
			size_t CacheHits;
			size_t CacheMisses;
			size_t TotalMemoryBytes;
			double HitRate;
		};
		
		static TextureStats GetStats();

	private:
		static std::unique_ptr<ResourceCache<Texture2D>> s_TextureCache;
		static std::unordered_map<uint32_t, Ref<Texture2D>> s_TextureRegistry;
		static std::mutex s_RegistryMutex;
		static bool s_Initialized;
	};

	/**
	 * @brief Optimized shader manager with compilation caching
	 * @details Manages shader compilation, caching, and optimization
	 */
	class ShaderManager
	{
	public:
		static void Init();
		static void Shutdown();
		
		/**
		 * @brief Load shader with caching
		 * @param name Shader name
		 * @param vertexSrc Vertex shader source
		 * @param fragmentSrc Fragment shader source
		 * @return Shared pointer to shader
		 */
		static Ref<Shader> LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		
		/**
		 * @brief Load shader from file
		 * @param filepath Shader file path
		 * @return Shared pointer to shader
		 */
		static Ref<Shader> LoadShaderFromFile(const std::string& filepath);
		
		/**
		 * @brief Get shader by name
		 * @param name Shader name
		 * @return Shared pointer to shader
		 */
		static Ref<Shader> GetShader(const std::string& name);
		
		/**
		 * @brief Precompile shaders for better performance
		 * @param shaderDefinitions List of shader definitions
		 */
		static void PrecompileShaders(const std::vector<std::pair<std::string, std::pair<std::string, std::string>>>& shaderDefinitions);
		
		/**
		 * @brief Clear shader cache
		 */
		static void ClearCache();
		
		/**
		 * @brief Get shader manager statistics
		 */
		struct ShaderStats
		{
			size_t LoadedShaders;
			size_t CacheHits;
			size_t CacheMisses;
			size_t CompilationFailures;
			double HitRate;
		};
		
		static ShaderStats GetStats();

	private:
		static std::unique_ptr<ResourceCache<Shader>> s_ShaderCache;
		static std::unordered_map<std::string, Ref<Shader>> s_ShaderRegistry;
		static std::mutex s_RegistryMutex;
		static bool s_Initialized;
	};

	/**
	 * @brief Global resource manager
	 * @details Coordinates all resource management systems
	 */
	class ResourceManager
	{
	public:
		static void Init();
		static void Shutdown();
		
		/**
		 * @brief Update resource management systems
		 * @param deltaTime Time since last update
		 */
		static void Update(float deltaTime);
		
		/**
		 * @brief Get total memory usage
		 */
		static size_t GetTotalMemoryUsage();
		
		/**
		 * @brief Set memory limits
		 * @param textureMemoryLimit Texture memory limit in bytes
		 * @param shaderMemoryLimit Shader memory limit in bytes
		 */
		static void SetMemoryLimits(size_t textureMemoryLimit, size_t shaderMemoryLimit);
		
		/**
		 * @brief Force garbage collection
		 */
		static void ForceGarbageCollection();
		
		/**
		 * @brief Get comprehensive resource statistics
		 */
		struct ResourceStats
		{
			TextureManager::TextureStats TextureStats;
			ShaderManager::ShaderStats ShaderStats;
			size_t TotalMemoryUsage;
			size_t TextureMemoryLimit;
			size_t ShaderMemoryLimit;
		};
		
		static ResourceStats GetStats();

	private:
		static bool s_Initialized;
		static float s_LastGCTime;
		static const float s_GCTimeInterval; // 5 seconds
	};

}