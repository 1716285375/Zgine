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

	/**
	 * @brief Resource manager singleton for efficient resource loading and caching
	 * @details This class provides centralized management of all engine resources
	 *          including textures, shaders, and materials with thread-safe operations
	 */
	class ResourceManager
	{
	public:
		/**
		 * @brief Get the singleton instance
		 * @return ResourceManager& Reference to the singleton instance
		 */
		static ResourceManager& GetInstance()
		{
			static ResourceManager instance;
			return instance;
		}

		/**
		 * @brief Load a texture from file path
		 * @param path The file path to the texture
		 * @return std::shared_ptr<Texture2D> The loaded texture, or nullptr if failed
		 */
		std::shared_ptr<Texture2D> LoadTexture(const std::string& path);
		
		/**
		 * @brief Create a new texture with specified dimensions
		 * @param width The width of the texture
		 * @param height The height of the texture
		 * @return std::shared_ptr<Texture2D> The created texture
		 */
		std::shared_ptr<Texture2D> CreateTexture(uint32_t width, uint32_t height);
		
		/**
		 * @brief Unload a texture from memory
		 * @param path The file path of the texture to unload
		 */
		void UnloadTexture(const std::string& path);
		
		/**
		 * @brief Clear all textures from memory
		 */
		void ClearTextures();

		/**
		 * @brief Load a shader with vertex and fragment source code
		 * @param name The name identifier for the shader
		 * @param vertexSrc The vertex shader source code
		 * @param fragmentSrc The fragment shader source code
		 * @return std::shared_ptr<Shader> The loaded shader, or nullptr if failed
		 */
		std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		
		/**
		 * @brief Get a shader by name
		 * @param name The name of the shader to retrieve
		 * @return std::shared_ptr<Shader> The shader, or nullptr if not found
		 */
		std::shared_ptr<Shader> GetShader(const std::string& name);
		
		/**
		 * @brief Unload a shader from memory
		 * @param name The name of the shader to unload
		 */
		void UnloadShader(const std::string& name);
		
		/**
		 * @brief Clear all shaders from memory
		 */
		void ClearShaders();

		/**
		 * @brief Create a new material with specified name
		 * @param name The name identifier for the material
		 * @return std::shared_ptr<Material> The created material
		 */
		std::shared_ptr<Material> CreateMaterial(const std::string& name);
		
		/**
		 * @brief Get a material by name
		 * @param name The name of the material to retrieve
		 * @return std::shared_ptr<Material> The material, or nullptr if not found
		 */
		std::shared_ptr<Material> GetMaterial(const std::string& name);
		
		/**
		 * @brief Unload a material from memory
		 * @param name The name of the material to unload
		 */
		void UnloadMaterial(const std::string& name);
		
		/**
		 * @brief Clear all materials from memory
		 */
		void ClearMaterials();

		/**
		 * @brief Get the number of loaded textures
		 * @return size_t The number of textures
		 */
		size_t GetTextureCount() const { return m_Textures.size(); }
		
		/**
		 * @brief Get the number of loaded shaders
		 * @return size_t The number of shaders
		 */
		size_t GetShaderCount() const { return m_Shaders.size(); }
		
		/**
		 * @brief Get the number of loaded materials
		 * @return size_t The number of materials
		 */
		size_t GetMaterialCount() const { return m_Materials.size(); }
		
		/**
		 * @brief Get the total memory usage of all resources
		 * @return size_t The total memory usage in bytes
		 */
		size_t GetTotalMemoryUsage() const;

		/**
		 * @brief Clear all resources from memory
		 */
		void ClearAll();

	private:
		ResourceManager() = default;
		~ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		// Resource caches
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;  ///< Texture cache map
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;     ///< Shader cache map
		std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials; ///< Material cache map

		// Thread safety
		mutable std::mutex m_TexturesMutex;   ///< Mutex for texture operations
		mutable std::mutex m_ShadersMutex;    ///< Mutex for shader operations
		mutable std::mutex m_MaterialsMutex;   ///< Mutex for material operations
	};

	/**
	 * @brief Resource pool template for efficient object reuse
	 * @details This template class provides object pooling functionality to reduce
	 *          memory allocation overhead by reusing objects instead of creating new ones
	 * @tparam T The type of objects to pool
	 */
	template<typename T>
	class ResourcePool
	{
	public:
		/**
		 * @brief Construct a new ResourcePool object
		 * @param initialSize The initial size of the pool
		 */
		ResourcePool(size_t initialSize = 100) : m_PoolSize(initialSize)
		{
			Reserve(initialSize);
		}

		/**
		 * @brief Destroy the ResourcePool object
		 */
		~ResourcePool() = default;

		/**
		 * @brief Acquire an object from the pool
		 * @return std::shared_ptr<T> A shared pointer to an object from the pool
		 */
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

		/**
		 * @brief Return an object to the pool for reuse
		 * @param obj The object to return to the pool
		 */
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

		/**
		 * @brief Reserve space in the pool
		 * @param count The number of objects to reserve
		 */
		void Reserve(size_t count)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			for (size_t i = 0; i < count; ++i)
			{
				m_Pool.push_back(std::make_shared<T>());
			}
		}

		/**
		 * @brief Get the current pool size
		 * @return size_t The number of objects currently in the pool
		 */
		size_t GetPoolSize() const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_Pool.size();
		}

		/**
		 * @brief Get the total number of objects created
		 * @return size_t The total number of objects created
		 */
		size_t GetTotalCreated() const { return m_TotalCreated; }

	private:
		std::vector<std::shared_ptr<T>> m_Pool;  ///< Pool of reusable objects
		mutable std::mutex m_Mutex;              ///< Mutex for thread safety
		size_t m_PoolSize;                       ///< Size of the pool
		size_t m_TotalCreated = 0;               ///< Total number of objects created
	};

}
