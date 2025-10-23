#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>
#include <mutex>
#include <vector>

namespace Zgine {

	class Texture2D;
	class Shader;
	class Material;

	// Resource manager for efficient resource loading and caching
	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance()
		{
			static ResourceManager instance;
			return instance;
		}

		// Texture management
		std::shared_ptr<Texture2D> LoadTexture(const std::string& path);
		std::shared_ptr<Texture2D> CreateTexture(uint32_t width, uint32_t height);
		void UnloadTexture(const std::string& path);
		void ClearTextures();

		// Shader management
		std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		std::shared_ptr<Shader> GetShader(const std::string& name);
		void UnloadShader(const std::string& name);
		void ClearShaders();

		// Material management
		std::shared_ptr<Material> CreateMaterial(const std::string& name);
		std::shared_ptr<Material> GetMaterial(const std::string& name);
		void UnloadMaterial(const std::string& name);
		void ClearMaterials();

		// Resource statistics
		size_t GetTextureCount() const { return m_Textures.size(); }
		size_t GetShaderCount() const { return m_Shaders.size(); }
		size_t GetMaterialCount() const { return m_Materials.size(); }
		size_t GetTotalMemoryUsage() const;

		// Cleanup
		void ClearAll();

	private:
		ResourceManager() = default;
		~ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		// Resource caches
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
		std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;

		// Thread safety
		mutable std::mutex m_TexturesMutex;
		mutable std::mutex m_ShadersMutex;
		mutable std::mutex m_MaterialsMutex;
	};

	// Resource pool for efficient object reuse
	template<typename T>
	class ResourcePool
	{
	public:
		ResourcePool(size_t initialSize = 100) : m_PoolSize(initialSize)
		{
			Reserve(initialSize);
		}

		~ResourcePool() = default;

		// Get an object from the pool
		std::shared_ptr<T> Acquire()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			if (m_Pool.empty())
			{
				Reserve(m_PoolSize);
			}

			auto obj = m_Pool.back();
			m_Pool.pop_back();
			return obj;
		}

		// Return an object to the pool
		void Release(std::shared_ptr<T> obj)
		{
			if (!obj)
				return;

			std::lock_guard<std::mutex> lock(m_Mutex);
			
			// Reset object state if it has a reset method
			if constexpr (requires { obj->Reset(); })
			{
				obj->Reset();
			}

			m_Pool.push_back(obj);
		}

		// Reserve space in the pool
		void Reserve(size_t count)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			for (size_t i = 0; i < count; ++i)
			{
				m_Pool.push_back(std::make_shared<T>());
			}
		}

		// Get pool statistics
		size_t GetPoolSize() const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_Pool.size();
		}

		size_t GetTotalCreated() const { return m_TotalCreated; }

	private:
		std::vector<std::shared_ptr<T>> m_Pool;
		mutable std::mutex m_Mutex;
		size_t m_PoolSize;
		size_t m_TotalCreated = 0;
	};

}
