#include "zgpch.h"
#include "ResourceManager.h"
#include "../Core.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace Zgine {

	// TextureManager static members
	std::unique_ptr<ResourceCache<Texture2D>> TextureManager::s_TextureCache;
	std::unordered_map<uint32_t, Ref<Texture2D>> TextureManager::s_TextureRegistry;
	std::mutex TextureManager::s_RegistryMutex;
	bool TextureManager::s_Initialized = false;

	// ShaderManager static members
	std::unique_ptr<ResourceCache<Shader>> ShaderManager::s_ShaderCache;
	std::unordered_map<std::string, Ref<Shader>> ShaderManager::s_ShaderRegistry;
	std::mutex ShaderManager::s_RegistryMutex;
	bool ShaderManager::s_Initialized = false;

	// ResourceManager static members
	bool ResourceManager::s_Initialized = false;
	float ResourceManager::s_LastGCTime = 0.0f;
	const float ResourceManager::s_GCTimeInterval = 5.0f;

	// TextureManager implementation
	void TextureManager::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("TextureManager::Init() called multiple times");
			return;
		}
		
		s_TextureCache = std::make_unique<ResourceCache<Texture2D>>(50 * 1024 * 1024); // 50MB
		s_Initialized = true;
		
		ZG_CORE_INFO("TextureManager::Init() completed");
	}

	void TextureManager::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("TextureManager::Shutdown() called without initialization");
			return;
		}
		
		s_TextureCache.reset();
		s_TextureRegistry.clear();
		s_Initialized = false;
		
		ZG_CORE_INFO("TextureManager::Shutdown() completed");
	}

	Ref<Texture2D> TextureManager::LoadTexture(const std::string& path)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("TextureManager::LoadTexture() called without initialization");
			return nullptr;
		}
		
		// Check cache first
		auto cached = s_TextureCache->Get(path);
		if (cached)
		{
			return cached;
		}
		
		// Load texture from file
		if (!std::filesystem::exists(path))
		{
			ZG_CORE_ERROR("TextureManager::LoadTexture() - File not found: {}", path);
			return nullptr;
		}
		
		auto texture = Texture2D::Create(path);
		if (!texture)
		{
			ZG_CORE_ERROR("TextureManager::LoadTexture() - Failed to load texture: {}", path);
			return nullptr;
		}
		
		// Estimate memory size (width * height * channels * bytes per channel)
		size_t memorySize = texture->GetWidth() * texture->GetHeight() * 4; // Assume RGBA
		
		// Cache the texture
		s_TextureCache->Store(path, texture, memorySize);
		
		// Register texture
		{
			std::lock_guard<std::mutex> lock(s_RegistryMutex);
			s_TextureRegistry[texture->GetRendererID()] = texture;
		}
		
		ZG_CORE_TRACE("TextureManager::LoadTexture() - Loaded texture: {} ({}x{}, {} bytes)", 
			path, texture->GetWidth(), texture->GetHeight(), memorySize);
		
		return texture;
	}

	Ref<Texture2D> TextureManager::CreateTexture(const void* data, uint32_t width, uint32_t height)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("TextureManager::CreateTexture() called without initialization");
			return nullptr;
		}
		
		auto texture = Texture2D::Create(width, height);
		if (!texture)
		{
			ZG_CORE_ERROR("TextureManager::CreateTexture() - Failed to create texture");
			return nullptr;
		}
		
		texture->SetData(const_cast<void*>(data), width * height * 4); // Assume RGBA
		
		// Register texture
		{
			std::lock_guard<std::mutex> lock(s_RegistryMutex);
			s_TextureRegistry[texture->GetRendererID()] = texture;
		}
		
		return texture;
	}

	Ref<Texture2D> TextureManager::GetTexture(uint32_t textureID)
	{
		std::lock_guard<std::mutex> lock(s_RegistryMutex);
		auto it = s_TextureRegistry.find(textureID);
		return (it != s_TextureRegistry.end()) ? it->second : nullptr;
	}

	void TextureManager::PreloadTextures(const std::vector<std::string>& paths)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("TextureManager::PreloadTextures() called without initialization");
			return;
		}
		
		ZG_CORE_INFO("TextureManager::PreloadTextures() - Preloading {} textures", paths.size());
		
		for (const auto& path : paths)
		{
			LoadTexture(path);
		}
		
		ZG_CORE_INFO("TextureManager::PreloadTextures() - Completed");
	}

	void TextureManager::ClearCache()
	{
		if (s_TextureCache)
		{
			s_TextureCache->Clear();
		}
	}

	TextureManager::TextureStats TextureManager::GetStats()
	{
		if (!s_TextureCache)
		{
			return {};
		}
		
		auto cacheStats = s_TextureCache->GetStats();
		return {
			cacheStats.EntryCount,
			cacheStats.TotalAccesses,
			0, // Cache misses not tracked in current implementation
			cacheStats.TotalMemoryBytes,
			cacheStats.HitRate
		};
	}

	// ShaderManager implementation
	void ShaderManager::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("ShaderManager::Init() called multiple times");
			return;
		}
		
		s_ShaderCache = std::make_unique<ResourceCache<Shader>>(10 * 1024 * 1024); // 10MB
		s_Initialized = true;
		
		ZG_CORE_INFO("ShaderManager::Init() completed");
	}

	void ShaderManager::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("ShaderManager::Shutdown() called without initialization");
			return;
		}
		
		s_ShaderCache.reset();
		s_ShaderRegistry.clear();
		s_Initialized = false;
		
		ZG_CORE_INFO("ShaderManager::Shutdown() completed");
	}

	Ref<Shader> ShaderManager::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("ShaderManager::LoadShader() called without initialization");
			return nullptr;
		}
		
		// Check cache first
		auto cached = s_ShaderCache->Get(name);
		if (cached)
		{
			return cached;
		}
		
		// Create shader
		auto shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
		if (!shader)
		{
			ZG_CORE_ERROR("ShaderManager::LoadShader() - Failed to create shader: {}", name);
			return nullptr;
		}
		
		// Estimate memory size (source code size)
		size_t memorySize = vertexSrc.size() + fragmentSrc.size();
		
		// Cache the shader
		s_ShaderCache->Store(name, shader, memorySize);
		
		// Register shader
		{
			std::lock_guard<std::mutex> lock(s_RegistryMutex);
			s_ShaderRegistry[name] = shader;
		}
		
		ZG_CORE_TRACE("ShaderManager::LoadShader() - Loaded shader: {} ({} bytes)", name, memorySize);
		
		return shader;
	}

	Ref<Shader> ShaderManager::LoadShaderFromFile(const std::string& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			ZG_CORE_ERROR("ShaderManager::LoadShaderFromFile() - File not found: {}", filepath);
			return nullptr;
		}
		
		std::ifstream file(filepath);
		if (!file.is_open())
		{
			ZG_CORE_ERROR("ShaderManager::LoadShaderFromFile() - Failed to open file: {}", filepath);
			return nullptr;
		}
		
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string source = buffer.str();
		
		// Simple shader parsing (assumes single file contains both vertex and fragment shaders)
		// This is a simplified implementation - in practice, you'd want more sophisticated parsing
		std::string name = std::filesystem::path(filepath).stem().string();
		
		return LoadShader(name, source, source); // Simplified - assumes same source for both
	}

	Ref<Shader> ShaderManager::GetShader(const std::string& name)
	{
		std::lock_guard<std::mutex> lock(s_RegistryMutex);
		auto it = s_ShaderRegistry.find(name);
		return (it != s_ShaderRegistry.end()) ? it->second : nullptr;
	}

	void ShaderManager::PrecompileShaders(const std::vector<std::pair<std::string, std::pair<std::string, std::string>>>& shaderDefinitions)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("ShaderManager::PrecompileShaders() called without initialization");
			return;
		}
		
		ZG_CORE_INFO("ShaderManager::PrecompileShaders() - Precompiling {} shaders", shaderDefinitions.size());
		
		for (const auto& definition : shaderDefinitions)
		{
			LoadShader(definition.first, definition.second.first, definition.second.second);
		}
		
		ZG_CORE_INFO("ShaderManager::PrecompileShaders() - Completed");
	}

	void ShaderManager::ClearCache()
	{
		if (s_ShaderCache)
		{
			s_ShaderCache->Clear();
		}
	}

	ShaderManager::ShaderStats ShaderManager::GetStats()
	{
		if (!s_ShaderCache)
		{
			return {};
		}
		
		auto cacheStats = s_ShaderCache->GetStats();
		return {
			cacheStats.EntryCount,
			cacheStats.TotalAccesses,
			0, // Cache misses not tracked in current implementation
			0, // Compilation failures not tracked in current implementation
			cacheStats.HitRate
		};
	}

	// ResourceManager implementation
	void ResourceManager::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("ResourceManager::Init() called multiple times");
			return;
		}
		
		TextureManager::Init();
		ShaderManager::Init();
		s_Initialized = true;
		
		ZG_CORE_INFO("ResourceManager::Init() completed");
	}

	void ResourceManager::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("ResourceManager::Shutdown() called without initialization");
			return;
		}
		
		ShaderManager::Shutdown();
		TextureManager::Shutdown();
		s_Initialized = false;
		
		ZG_CORE_INFO("ResourceManager::Shutdown() completed");
	}

	void ResourceManager::Update(float deltaTime)
	{
		if (!s_Initialized) return;
		
		s_LastGCTime += deltaTime;
		
		// Perform garbage collection periodically
		if (s_LastGCTime >= s_GCTimeInterval)
		{
			ForceGarbageCollection();
			s_LastGCTime = 0.0f;
		}
	}

	size_t ResourceManager::GetTotalMemoryUsage()
	{
		if (!s_Initialized) return 0;
		
		auto textureStats = TextureManager::GetStats();
		// ShaderStats doesn't have TotalMemoryBytes, so we only count texture memory
		
		return textureStats.TotalMemoryBytes;
	}

	void ResourceManager::SetMemoryLimits(size_t textureMemoryLimit, size_t shaderMemoryLimit)
	{
		if (!s_Initialized) return;
		
		// Note: Direct access to private members not available
		// This would need to be implemented through public methods in TextureManager and ShaderManager
		ZG_CORE_INFO("ResourceManager::SetMemoryLimits() - Texture: {}MB, Shader: {}MB", 
			textureMemoryLimit / (1024 * 1024), shaderMemoryLimit / (1024 * 1024));
	}

	void ResourceManager::ForceGarbageCollection()
	{
		if (!s_Initialized) return;
		
		ZG_CORE_TRACE("ResourceManager::ForceGarbageCollection() - Starting garbage collection");
		
		// Force cache cleanup
		TextureManager::ClearCache();
		ShaderManager::ClearCache();
		
		ZG_CORE_TRACE("ResourceManager::ForceGarbageCollection() - Completed");
	}

	ResourceManager::ResourceStats ResourceManager::GetStats()
	{
		if (!s_Initialized)
		{
			return {};
		}
		
		return {
			TextureManager::GetStats(),
			ShaderManager::GetStats(),
			GetTotalMemoryUsage(),
			50 * 1024 * 1024, // Default texture limit
			10 * 1024 * 1024  // Default shader limit
		};
	}

}