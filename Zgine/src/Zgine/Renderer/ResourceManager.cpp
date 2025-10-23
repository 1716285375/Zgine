#include "zgpch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"

namespace Zgine {

	std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_TexturesMutex);
		
		auto it = m_Textures.find(path);
		if (it != m_Textures.end())
		{
			return it->second;
		}

		auto texture = Texture2D::Create(path);
		if (texture)
		{
			m_Textures[path] = texture;
		}
		return texture;
	}

	std::shared_ptr<Texture2D> ResourceManager::CreateTexture(uint32_t width, uint32_t height)
	{
		auto texture = Texture2D::Create(width, height);
		return texture;
	}

	void ResourceManager::UnloadTexture(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_TexturesMutex);
		m_Textures.erase(path);
	}

	void ResourceManager::ClearTextures()
	{
		std::lock_guard<std::mutex> lock(m_TexturesMutex);
		m_Textures.clear();
	}

	std::shared_ptr<Shader> ResourceManager::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		std::lock_guard<std::mutex> lock(m_ShadersMutex);
		
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
		{
			return it->second;
		}

		auto shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
		m_Shaders[name] = shader;
		return shader;
	}

	std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_ShadersMutex);
		
		auto it = m_Shaders.find(name);
		return (it != m_Shaders.end()) ? it->second : nullptr;
	}

	void ResourceManager::UnloadShader(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_ShadersMutex);
		m_Shaders.erase(name);
	}

	void ResourceManager::ClearShaders()
	{
		std::lock_guard<std::mutex> lock(m_ShadersMutex);
		m_Shaders.clear();
	}

	std::shared_ptr<Material> ResourceManager::CreateMaterial(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_MaterialsMutex);
		
		auto it = m_Materials.find(name);
		if (it != m_Materials.end())
		{
			return it->second;
		}

		auto material = std::make_shared<Material>(name);
		m_Materials[name] = material;
		return material;
	}

	std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_MaterialsMutex);
		
		auto it = m_Materials.find(name);
		return (it != m_Materials.end()) ? it->second : nullptr;
	}

	void ResourceManager::UnloadMaterial(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(m_MaterialsMutex);
		m_Materials.erase(name);
	}

	void ResourceManager::ClearMaterials()
	{
		std::lock_guard<std::mutex> lock(m_MaterialsMutex);
		m_Materials.clear();
	}

	size_t ResourceManager::GetTotalMemoryUsage() const
	{
		size_t total = 0;
		
		// Estimate texture memory usage
		{
			std::lock_guard<std::mutex> lock(m_TexturesMutex);
			for (const auto& [name, texture] : m_Textures)
			{
				total += texture->GetWidth() * texture->GetHeight() * 4; // Assume RGBA
			}
		}
		
		// Shader memory usage is minimal (just the source code)
		{
			std::lock_guard<std::mutex> lock(m_ShadersMutex);
			total += m_Shaders.size() * 1024; // Rough estimate
		}
		
		// Material memory usage is minimal
		{
			std::lock_guard<std::mutex> lock(m_MaterialsMutex);
			total += m_Materials.size() * 256; // Rough estimate
		}
		
		return total;
	}

	void ResourceManager::ClearAll()
	{
		ClearTextures();
		ClearShaders();
		ClearMaterials();
	}

}
