#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Zgine {

	class Texture2D;

	// Material properties
	struct MaterialProperties
	{
		glm::vec3 Albedo = {1.0f, 1.0f, 1.0f};        // Base color
		float Metallic = 0.0f;                         // Metallic factor (0-1)
		float Roughness = 0.5f;                       // Roughness factor (0-1)
		float Emissive = 0.0f;                        // Emissive strength
		glm::vec3 EmissiveColor = {1.0f, 1.0f, 1.0f}; // Emissive color
		float Transparency = 1.0f;                    // Transparency (0-1)
		float RefractionIndex = 1.0f;                 // Refraction index
	};

	// Material class
	class Material
	{
	public:
		Material(const std::string& name = "DefaultMaterial")
			: m_Name(name), m_Properties() {}

		virtual ~Material() = default;

		// Getters
		const std::string& GetName() const { return m_Name; }
		const MaterialProperties& GetProperties() const { return m_Properties; }
		
		// Texture getters
		const std::shared_ptr<Texture2D>& GetAlbedoTexture() const { return m_AlbedoTexture; }
		const std::shared_ptr<Texture2D>& GetNormalTexture() const { return m_NormalTexture; }
		const std::shared_ptr<Texture2D>& GetMetallicTexture() const { return m_MetallicTexture; }
		const std::shared_ptr<Texture2D>& GetRoughnessTexture() const { return m_RoughnessTexture; }
		const std::shared_ptr<Texture2D>& GetEmissiveTexture() const { return m_EmissiveTexture; }

		// Setters
		void SetName(const std::string& name) { m_Name = name; }
		void SetProperties(const MaterialProperties& properties) { m_Properties = properties; }
		
		// Property setters
		void SetAlbedo(const glm::vec3& albedo) { m_Properties.Albedo = albedo; }
		void SetMetallic(float metallic) { m_Properties.Metallic = glm::clamp(metallic, 0.0f, 1.0f); }
		void SetRoughness(float roughness) { m_Properties.Roughness = glm::clamp(roughness, 0.0f, 1.0f); }
		void SetEmissive(float emissive) { m_Properties.Emissive = emissive; }
		void SetEmissiveColor(const glm::vec3& color) { m_Properties.EmissiveColor = color; }
		void SetTransparency(float transparency) { m_Properties.Transparency = glm::clamp(transparency, 0.0f, 1.0f); }
		void SetRefractionIndex(float index) { m_Properties.RefractionIndex = index; }

		// Texture setters
		void SetAlbedoTexture(const std::shared_ptr<Texture2D>& texture) { m_AlbedoTexture = texture; }
		void SetNormalTexture(const std::shared_ptr<Texture2D>& texture) { m_NormalTexture = texture; }
		void SetMetallicTexture(const std::shared_ptr<Texture2D>& texture) { m_MetallicTexture = texture; }
		void SetRoughnessTexture(const std::shared_ptr<Texture2D>& texture) { m_RoughnessTexture = texture; }
		void SetEmissiveTexture(const std::shared_ptr<Texture2D>& texture) { m_EmissiveTexture = texture; }

		// Upload material data to shader
		void UploadToShader(class Shader* shader);

	private:
		std::string m_Name;
		MaterialProperties m_Properties;
		
		// Textures
		std::shared_ptr<Texture2D> m_AlbedoTexture;
		std::shared_ptr<Texture2D> m_NormalTexture;
		std::shared_ptr<Texture2D> m_MetallicTexture;
		std::shared_ptr<Texture2D> m_RoughnessTexture;
		std::shared_ptr<Texture2D> m_EmissiveTexture;
	};

	// Predefined materials
	class MaterialLibrary
	{
	public:
		static MaterialLibrary& GetInstance()
		{
			static MaterialLibrary instance;
			return instance;
		}

		// Material management
		void AddMaterial(const std::string& name, std::shared_ptr<Material> material);
		std::shared_ptr<Material> GetMaterial(const std::string& name);
		void RemoveMaterial(const std::string& name);
		void ClearMaterials();
		size_t GetMaterialCount() const { return m_Materials.size(); }

		// Predefined materials
		std::shared_ptr<Material> CreateDefaultMaterial();
		std::shared_ptr<Material> CreateMetallicMaterial();
		std::shared_ptr<Material> CreateGlassMaterial();
		std::shared_ptr<Material> CreateEmissiveMaterial();
		std::shared_ptr<Material> CreatePBRMaterial(const glm::vec3& albedo, float metallic, float roughness);

	private:
		MaterialLibrary() = default;
		~MaterialLibrary() = default;
		MaterialLibrary(const MaterialLibrary&) = delete;
		MaterialLibrary& operator=(const MaterialLibrary&) = delete;

		std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
	};

}