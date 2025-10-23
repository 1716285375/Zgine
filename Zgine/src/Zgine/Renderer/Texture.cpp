#include "zgpch.h"
#include "Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Zgine/Renderer/RendererAPI.h"

namespace Zgine {

	/**
	 * @brief Create a new texture with specified dimensions
	 * @param width The texture width
	 * @param height The texture height
	 * @return Ref<Texture> A reference-counted pointer to the created texture
	 * @details Creates an empty texture with the specified dimensions
	 */
	Ref<Texture> Texture::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/**
	 * @brief Create a new texture from file
	 * @param path The path to the texture file
	 * @return Ref<Texture> A reference-counted pointer to the created texture
	 * @details Loads a texture from the specified file path
	 */
	Ref<Texture> Texture::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/**
	 * @brief Create a new 2D texture with specified dimensions
	 * @param width The texture width
	 * @param height The texture height
	 * @return Ref<Texture2D> A reference-counted pointer to the created texture
	 * @details Creates an empty 2D texture with the specified dimensions
	 */
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/**
	 * @brief Create a new 2D texture from file
	 * @param path The path to the texture file
	 * @return Ref<Texture2D> A reference-counted pointer to the created texture
	 * @details Loads a 2D texture from the specified file path
	 */
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/**
	 * @brief Construct a new Texture2D object with specified dimensions
	 * @param width The texture width
	 * @param height The texture height
	 * @details Creates an empty 2D texture with the specified dimensions
	 */
	Texture2D::Texture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_RendererID(0), m_IsLoaded(false)
	{
		// Texture2D constructor implementation
	}

	/**
	 * @brief Construct a new Texture2D object from file
	 * @param path The path to the texture file
	 * @details Loads a 2D texture from the specified file path
	 */
	Texture2D::Texture2D(const std::string& path)
		: m_Path(path), m_Width(0), m_Height(0), m_RendererID(0), m_IsLoaded(false)
	{
		// Texture2D constructor implementation
	}

	/**
	 * @brief Destroy the Texture2D object
	 * @details Properly cleans up the texture resources
	 */
	Texture2D::~Texture2D()
	{
		// Texture2D destructor implementation
	}

}
