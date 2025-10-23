#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Zgine {

	class Texture2D;

	/**
	 * @brief Material properties structure containing PBR material parameters
	 * @details This structure defines all the physical properties of a material
	 *          used in Physically Based Rendering (PBR) pipeline
	 */
	struct MaterialProperties
	{
		glm::vec3 Albedo = {1.0f, 1.0f, 1.0f};        ///< Base color of the material
		float Metallic = 0.0f;                         ///< Metallic factor (0-1), 0 = dielectric, 1 = metallic
		float Roughness = 0.5f;                       ///< Roughness factor (0-1), 0 = smooth, 1 = rough
		float Emissive = 0.0f;                        ///< Emissive strength multiplier
		glm::vec3 EmissiveColor = {1.0f, 1.0f, 1.0f}; ///< Color of emitted light
		float Transparency = 1.0f;                    ///< Transparency factor (0-1), 0 = opaque, 1 = transparent
		float RefractionIndex = 1.0f;                 ///< Index of refraction for transparent materials
	};

	/**
	 * @brief Material class representing a PBR material with textures and properties
	 * @details This class encapsulates all material data including PBR properties
	 *          and associated textures for rendering
	 */
	class Material
	{
	public:
		/**
		 * @brief Construct a new Material object
		 * @param name The name identifier for this material
		 */
		Material(const std::string& name = "DefaultMaterial")
			: m_Name(name), m_Properties() {}

		/**
		 * @brief Destroy the Material object
		 */
		virtual ~Material() = default;

		/**
		 * @brief Get the material name
		 * @return const std::string& The material name
		 */
		const std::string& GetName() const { return m_Name; }
		
		/**
		 * @brief Get the material properties
		 * @return const MaterialProperties& The material properties structure
		 */
		const MaterialProperties& GetProperties() const { return m_Properties; }
		
		/**
		 * @brief Get the albedo texture
		 * @return const std::shared_ptr<Texture2D>& The albedo texture
		 */
		const std::shared_ptr<Texture2D>& GetAlbedoTexture() const { return m_AlbedoTexture; }
		
		/**
		 * @brief Get the normal texture
		 * @return const std::shared_ptr<Texture2D>& The normal texture
		 */
		const std::shared_ptr<Texture2D>& GetNormalTexture() const { return m_NormalTexture; }
		
		/**
		 * @brief Get the metallic texture
		 * @return const std::shared_ptr<Texture2D>& The metallic texture
		 */
		const std::shared_ptr<Texture2D>& GetMetallicTexture() const { return m_MetallicTexture; }
		
		/**
		 * @brief Get the roughness texture
		 * @return const std::shared_ptr<Texture2D>& The roughness texture
		 */
		const std::shared_ptr<Texture2D>& GetRoughnessTexture() const { return m_RoughnessTexture; }
		
		/**
		 * @brief Get the emissive texture
		 * @return const std::shared_ptr<Texture2D>& The emissive texture
		 */
		const std::shared_ptr<Texture2D>& GetEmissiveTexture() const { return m_EmissiveTexture; }

		/**
		 * @brief Set the material name
		 * @param name The new material name
		 */
		void SetName(const std::string& name) { m_Name = name; }
		
		/**
		 * @brief Set the material properties
		 * @param properties The new material properties
		 */
		void SetProperties(const MaterialProperties& properties) { m_Properties = properties; }
		
		/**
		 * @brief Set the albedo color
		 * @param albedo The albedo color vector
		 */
		void SetAlbedo(const glm::vec3& albedo) { m_Properties.Albedo = albedo; }
		
		/**
		 * @brief Set the metallic factor
		 * @param metallic The metallic factor (0-1)
		 */
		void SetMetallic(float metallic) { m_Properties.Metallic = glm::clamp(metallic, 0.0f, 1.0f); }
		
		/**
		 * @brief Set the roughness factor
		 * @param roughness The roughness factor (0-1)
		 */
		void SetRoughness(float roughness) { m_Properties.Roughness = glm::clamp(roughness, 0.0f, 1.0f); }
		
		/**
		 * @brief Set the emissive strength
		 * @param emissive The emissive strength multiplier
		 */
		void SetEmissive(float emissive) { m_Properties.Emissive = emissive; }
		
		/**
		 * @brief Set the emissive color
		 * @param color The emissive color vector
		 */
		void SetEmissiveColor(const glm::vec3& color) { m_Properties.EmissiveColor = color; }
		
		/**
		 * @brief Set the transparency factor
		 * @param transparency The transparency factor (0-1)
		 */
		void SetTransparency(float transparency) { m_Properties.Transparency = glm::clamp(transparency, 0.0f, 1.0f); }
		
		/**
		 * @brief Set the refraction index
		 * @param index The refraction index value
		 */
		void SetRefractionIndex(float index) { m_Properties.RefractionIndex = index; }

		/**
		 * @brief Set the albedo texture
		 * @param texture The albedo texture
		 */
		void SetAlbedoTexture(const std::shared_ptr<Texture2D>& texture) { m_AlbedoTexture = texture; }
		
		/**
		 * @brief Set the normal texture
		 * @param texture The normal texture
		 */
		void SetNormalTexture(const std::shared_ptr<Texture2D>& texture) { m_NormalTexture = texture; }
		
		/**
		 * @brief Set the metallic texture
		 * @param texture The metallic texture
		 */
		void SetMetallicTexture(const std::shared_ptr<Texture2D>& texture) { m_MetallicTexture = texture; }
		
		/**
		 * @brief Set the roughness texture
		 * @param texture The roughness texture
		 */
		void SetRoughnessTexture(const std::shared_ptr<Texture2D>& texture) { m_RoughnessTexture = texture; }
		
		/**
		 * @brief Set the emissive texture
		 * @param texture The emissive texture
		 */
		void SetEmissiveTexture(const std::shared_ptr<Texture2D>& texture) { m_EmissiveTexture = texture; }

		/**
		 * @brief Upload material data to shader uniforms
		 * @param shader Pointer to the shader to upload data to
		 */
		void UploadToShader(class Shader* shader);

	private:
		std::string m_Name;                    ///< Material name identifier
		MaterialProperties m_Properties;       ///< Material PBR properties
		
		// Texture maps
		std::shared_ptr<Texture2D> m_AlbedoTexture;     ///< Base color texture
		std::shared_ptr<Texture2D> m_NormalTexture;      ///< Normal map texture
		std::shared_ptr<Texture2D> m_MetallicTexture;   ///< Metallic map texture
		std::shared_ptr<Texture2D> m_RoughnessTexture;  ///< Roughness map texture
		std::shared_ptr<Texture2D> m_EmissiveTexture;   ///< Emissive map texture
	};

	/**
	 * @brief Material library singleton for managing predefined materials
	 * @details This class provides a centralized way to create and manage
	 *          common material types used throughout the engine
	 */
	class MaterialLibrary
	{
	public:
		/**
		 * @brief Get the singleton instance
		 * @return MaterialLibrary& Reference to the singleton instance
		 */
		static MaterialLibrary& GetInstance()
		{
			static MaterialLibrary instance;
			return instance;
		}

		/**
		 * @brief Add a material to the library
		 * @param name The name identifier for the material
		 * @param material The material to add
		 */
		void AddMaterial(const std::string& name, std::shared_ptr<Material> material);
		
		/**
		 * @brief Get a material from the library
		 * @param name The name of the material to retrieve
		 * @return std::shared_ptr<Material> The material, or nullptr if not found
		 */
		std::shared_ptr<Material> GetMaterial(const std::string& name);
		
		/**
		 * @brief Remove a material from the library
		 * @param name The name of the material to remove
		 */
		void RemoveMaterial(const std::string& name);
		
		/**
		 * @brief Clear all materials from the library
		 */
		void ClearMaterials();
		
		/**
		 * @brief Get the number of materials in the library
		 * @return size_t The number of materials
		 */
		size_t GetMaterialCount() const { return m_Materials.size(); }

		/**
		 * @brief Create a default material
		 * @return std::shared_ptr<Material> The created default material
		 */
		std::shared_ptr<Material> CreateDefaultMaterial();
		
		/**
		 * @brief Create a metallic material
		 * @return std::shared_ptr<Material> The created metallic material
		 */
		std::shared_ptr<Material> CreateMetallicMaterial();
		
		/**
		 * @brief Create a glass material
		 * @return std::shared_ptr<Material> The created glass material
		 */
		std::shared_ptr<Material> CreateGlassMaterial();
		
		/**
		 * @brief Create an emissive material
		 * @return std::shared_ptr<Material> The created emissive material
		 */
		std::shared_ptr<Material> CreateEmissiveMaterial();
		
		/**
		 * @brief Create a custom PBR material
		 * @param albedo The albedo color
		 * @param metallic The metallic factor
		 * @param roughness The roughness factor
		 * @return std::shared_ptr<Material> The created PBR material
		 */
		std::shared_ptr<Material> CreatePBRMaterial(const glm::vec3& albedo, float metallic, float roughness);

	private:
		MaterialLibrary() = default;
		~MaterialLibrary() = default;
		MaterialLibrary(const MaterialLibrary&) = delete;
		MaterialLibrary& operator=(const MaterialLibrary&) = delete;

		std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials; ///< Material storage map
	};

}