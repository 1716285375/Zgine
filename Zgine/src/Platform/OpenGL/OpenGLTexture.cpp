#include "zgpch.h"
#include "OpenGLTexture.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <GLFW/glfw3.h>

namespace Zgine {

	/**
	 * @brief Construct a new OpenGLTexture2D object with specified dimensions
	 * @param width The texture width
	 * @param height The texture height
	 * @details Creates an empty OpenGL 2D texture with the specified dimensions
	 */
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: Texture2D(width, height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// Try to use DSA if available, fallback to traditional methods
		typedef void (*PFNGLCREATETEXTURESPROC)(GLenum target, GLsizei n, GLuint* textures);
		typedef void (*PFNGLTEXTURESTORAGE2DPROC)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
		typedef void (*PFNGLTEXTUREPARAMETERIPROC)(GLuint texture, GLenum pname, GLint param);
		
		PFNGLCREATETEXTURESPROC glCreateTexturesPtr = (PFNGLCREATETEXTURESPROC)glfwGetProcAddress("glCreateTextures");
		PFNGLTEXTURESTORAGE2DPROC glTextureStorage2DPtr = (PFNGLTEXTURESTORAGE2DPROC)glfwGetProcAddress("glTextureStorage2D");
		PFNGLTEXTUREPARAMETERIPROC glTextureParameteriPtr = (PFNGLTEXTUREPARAMETERIPROC)glfwGetProcAddress("glTextureParameteri");
		
		if (glCreateTexturesPtr && glTextureStorage2DPtr && glTextureParameteriPtr)
		{
			// Use DSA (OpenGL 4.5+)
			glCreateTexturesPtr(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2DPtr(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
			glTextureParameteriPtr(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteriPtr(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteriPtr(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteriPtr(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			// Fallback to traditional methods
			ZG_CORE_WARN("DSA not available, using traditional texture functions");
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	/**
	 * @brief Construct a new OpenGLTexture2D object from file
	 * @param path The path to the texture file
	 * @details Loads an OpenGL 2D texture from the specified file path
	 */
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: Texture2D(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		ZG_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		ZG_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		// Use traditional OpenGL calls for compatibility
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		
		// Upload texture data
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
		m_IsLoaded = true;
	}

	/**
	 * @brief Destroy the OpenGLTexture2D object
	 * @details Properly cleans up OpenGL texture resources
	 */
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	/**
	 * @brief Set texture data
	 * @param data Pointer to the texture data
	 * @param size Size of the data in bytes
	 * @details Uploads pixel data to the OpenGL texture
	 */
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		ZG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	/**
	 * @brief Bind the texture to a texture slot
	 * @param slot The texture slot to bind to (default: 0)
	 * @details Makes this OpenGL texture active for rendering
	 */
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		// Use traditional OpenGL binding
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

}
