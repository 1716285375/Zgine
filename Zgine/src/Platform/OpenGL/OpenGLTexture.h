#pragma once

#include "Zgine/Renderer/Texture.h"

namespace Zgine {

	/**
	 * @brief OpenGL implementation of 2D textures
	 * @details This class provides OpenGL-specific implementation of 2D textures,
	 *          handling texture creation, loading, and binding operations
	 */
	class OpenGLTexture2D : public Texture2D
	{
	public:
		/**
		 * @brief Construct a new OpenGLTexture2D object with specified dimensions
		 * @param width The texture width
		 * @param height The texture height
		 * @details Creates an empty OpenGL 2D texture with the specified dimensions
		 */
		OpenGLTexture2D(uint32_t width, uint32_t height);
		
		/**
		 * @brief Construct a new OpenGLTexture2D object from file
		 * @param path The path to the texture file
		 * @details Loads an OpenGL 2D texture from the specified file path
		 */
		OpenGLTexture2D(const std::string& path);
		
		/**
		 * @brief Destroy the OpenGLTexture2D object
		 * @details Properly cleans up OpenGL texture resources
		 */
		virtual ~OpenGLTexture2D();

		/**
		 * @brief Set texture data
		 * @param data Pointer to the texture data
		 * @param size Size of the data in bytes
		 * @details Uploads pixel data to the OpenGL texture
		 */
		virtual void SetData(void* data, uint32_t size) override;

		/**
		 * @brief Bind the texture to a texture slot
		 * @param slot The texture slot to bind to (default: 0)
		 * @details Makes this OpenGL texture active for rendering
		 */
		virtual void Bind(uint32_t slot = 0) const override;

	private:
		uint32_t m_InternalFormat;  ///< OpenGL internal texture format
		uint32_t m_DataFormat;      ///< OpenGL data format for texture upload
	};

}
