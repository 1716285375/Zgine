#pragma once

#include <string>
#include <memory>
#include <cstdint>

namespace Zgine {

	/**
	 * @brief Abstract base class for texture implementations
	 * @details This class defines the interface for all texture types,
	 *          providing platform-independent texture operations
	 */
	class Texture
	{
	public:
		/**
		 * @brief Destroy the Texture object
		 */
		virtual ~Texture() = default;

		/**
		 * @brief Get the texture width
		 * @return uint32_t The texture width in pixels
		 */
		virtual uint32_t GetWidth() const = 0;
		
		/**
		 * @brief Get the texture height
		 * @return uint32_t The texture height in pixels
		 */
		virtual uint32_t GetHeight() const = 0;
		
		/**
		 * @brief Get the renderer ID of the texture
		 * @return uint32_t The OpenGL texture ID
		 */
		virtual uint32_t GetRendererID() const = 0;

		/**
		 * @brief Set texture data
		 * @param data Pointer to the texture data
		 * @param size Size of the data in bytes
		 * @details Uploads pixel data to the texture
		 */
		virtual void SetData(void* data, uint32_t size) = 0;

		/**
		 * @brief Bind the texture to a texture slot
		 * @param slot The texture slot to bind to (default: 0)
		 * @details Makes this texture active for rendering
		 */
		virtual void Bind(uint32_t slot = 0) const = 0;

		/**
		 * @brief Check if the texture is loaded
		 * @return bool True if the texture is successfully loaded
		 */
		virtual bool IsLoaded() const = 0;

		/**
		 * @brief Get the texture file path
		 * @return const std::string& Reference to the texture file path
		 */
		virtual const std::string& GetPath() const = 0;

		/**
		 * @brief Create a new texture with specified dimensions
		 * @param width The texture width
		 * @param height The texture height
		 * @return Ref<Texture> A reference-counted pointer to the created texture
		 * @details Creates an empty texture with the specified dimensions
		 */
		static Ref<Texture> Create(uint32_t width, uint32_t height);
		
		/**
		 * @brief Create a new texture from file
		 * @param path The path to the texture file
		 * @return Ref<Texture> A reference-counted pointer to the created texture
		 * @details Loads a texture from the specified file path
		 */
		static Ref<Texture> Create(const std::string& path);
	};

	/**
	 * @brief 2D texture implementation
	 * @details This class provides a concrete implementation of 2D textures
	 *          with platform-specific rendering support
	 */
	class Texture2D : public Texture
	{
	public:
		/**
		 * @brief Construct a new Texture2D object with specified dimensions
		 * @param width The texture width
		 * @param height The texture height
		 * @details Creates an empty 2D texture with the specified dimensions
		 */
		Texture2D(uint32_t width, uint32_t height);
		
		/**
		 * @brief Construct a new Texture2D object from file
		 * @param path The path to the texture file
		 * @details Loads a 2D texture from the specified file path
		 */
		Texture2D(const std::string& path);
		
		/**
		 * @brief Destroy the Texture2D object
		 * @details Properly cleans up the texture resources
		 */
		virtual ~Texture2D();

		/**
		 * @brief Get the texture width
		 * @return uint32_t The texture width in pixels
		 */
		virtual uint32_t GetWidth() const override { return m_Width; }
		
		/**
		 * @brief Get the texture height
		 * @return uint32_t The texture height in pixels
		 */
		virtual uint32_t GetHeight() const override { return m_Height; }
		
		/**
		 * @brief Get the renderer ID of the texture
		 * @return uint32_t The OpenGL texture ID
		 */
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		/**
		 * @brief Set texture data
		 * @param data Pointer to the texture data
		 * @param size Size of the data in bytes
		 * @details Uploads pixel data to the texture
		 */
		virtual void SetData(void* data, uint32_t size) = 0;

		/**
		 * @brief Bind the texture to a texture slot
		 * @param slot The texture slot to bind to (default: 0)
		 * @details Makes this texture active for rendering
		 */
		virtual void Bind(uint32_t slot = 0) const = 0;

		/**
		 * @brief Check if the texture is loaded
		 * @return bool True if the texture is successfully loaded
		 */
		virtual bool IsLoaded() const override { return m_IsLoaded; }

		/**
		 * @brief Get the texture file path
		 * @return const std::string& Reference to the texture file path
		 */
		virtual const std::string& GetPath() const override { return m_Path; }

		/**
		 * @brief Create a new 2D texture with specified dimensions
		 * @param width The texture width
		 * @param height The texture height
		 * @return Ref<Texture2D> A reference-counted pointer to the created texture
		 * @details Creates an empty 2D texture with the specified dimensions
		 */
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		
		/**
		 * @brief Create a new 2D texture from file
		 * @param path The path to the texture file
		 * @return Ref<Texture2D> A reference-counted pointer to the created texture
		 * @details Loads a 2D texture from the specified file path
		 */
		static Ref<Texture2D> Create(const std::string& path);

		/**
		 * @brief Compare two textures for equality
		 * @param other The other texture to compare with
		 * @return bool True if the textures have the same renderer ID
		 * @details Compares textures based on their renderer ID
		 */
		bool operator==(const Texture2D& other) const
		{
			return m_RendererID == other.m_RendererID;
		}

	protected:
		std::string m_Path;        ///< Texture file path
		uint32_t m_Width, m_Height; ///< Texture dimensions
		uint32_t m_RendererID;     ///< OpenGL texture ID
		bool m_IsLoaded = false;   ///< Whether the texture is loaded
	};

}
