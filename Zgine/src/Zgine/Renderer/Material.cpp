#include "zgpch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace Zgine {

	void Material::UploadToShader(Shader* shader)
	{
		if (!shader)
			return;

		// Upload material properties
		shader->UploadUniformFloat3("u_Material.albedo", m_Properties.Albedo);
		shader->UploadUniformFloat("u_Material.metallic", m_Properties.Metallic);
		shader->UploadUniformFloat("u_Material.roughness", m_Properties.Roughness);
		shader->UploadUniformFloat("u_Material.emissive", m_Properties.Emissive);
		shader->UploadUniformFloat3("u_Material.emissiveColor", m_Properties.EmissiveColor);
		shader->UploadUniformFloat("u_Material.transparency", m_Properties.Transparency);
		shader->UploadUniformFloat("u_Material.refractionIndex", m_Properties.RefractionIndex);

		// Upload texture indices
		shader->UploadUniformInt("u_Material.hasAlbedoTexture", m_AlbedoTexture ? 1 : 0);
		shader->UploadUniformInt("u_Material.hasNormalTexture", m_NormalTexture ? 1 : 0);
		shader->UploadUniformInt("u_Material.hasMetallicTexture", m_MetallicTexture ? 1 : 0);
		shader->UploadUniformInt("u_Material.hasRoughnessTexture", m_RoughnessTexture ? 1 : 0);
		shader->UploadUniformInt("u_Material.hasEmissiveTexture", m_EmissiveTexture ? 1 : 0);
	}

	// MaterialLibrary implementation
	void MaterialLibrary::AddMaterial(const std::string& name, std::shared_ptr<Material> material)
	{
		m_Materials[name] = material;
	}

	std::shared_ptr<Material> MaterialLibrary::GetMaterial(const std::string& name)
	{
		auto it = m_Materials.find(name);
		return (it != m_Materials.end()) ? it->second : nullptr;
	}

	void MaterialLibrary::RemoveMaterial(const std::string& name)
	{
		m_Materials.erase(name);
	}

	void MaterialLibrary::ClearMaterials()
	{
		m_Materials.clear();
	}

	std::shared_ptr<Material> MaterialLibrary::CreateDefaultMaterial()
	{
		auto material = std::make_shared<Material>("DefaultMaterial");
		material->SetAlbedo({0.8f, 0.8f, 0.8f});
		material->SetMetallic(0.0f);
		material->SetRoughness(0.5f);
		material->SetEmissive(0.0f);
		return material;
	}

	std::shared_ptr<Material> MaterialLibrary::CreateMetallicMaterial()
	{
		auto material = std::make_shared<Material>("MetallicMaterial");
		material->SetAlbedo({0.7f, 0.7f, 0.7f});
		material->SetMetallic(1.0f);
		material->SetRoughness(0.1f);
		material->SetEmissive(0.0f);
		return material;
	}

	std::shared_ptr<Material> MaterialLibrary::CreateGlassMaterial()
	{
		auto material = std::make_shared<Material>("GlassMaterial");
		material->SetAlbedo({0.9f, 0.9f, 0.9f});
		material->SetMetallic(0.0f);
		material->SetRoughness(0.0f);
		material->SetTransparency(0.3f);
		material->SetRefractionIndex(1.5f);
		return material;
	}

	std::shared_ptr<Material> MaterialLibrary::CreateEmissiveMaterial()
	{
		auto material = std::make_shared<Material>("EmissiveMaterial");
		material->SetAlbedo({0.1f, 0.1f, 0.1f});
		material->SetMetallic(0.0f);
		material->SetRoughness(0.8f);
		material->SetEmissive(2.0f);
		material->SetEmissiveColor({1.0f, 0.5f, 0.2f});
		return material;
	}

	std::shared_ptr<Material> MaterialLibrary::CreatePBRMaterial(const glm::vec3& albedo, float metallic, float roughness)
	{
		auto material = std::make_shared<Material>("PBRMaterial");
		material->SetAlbedo(albedo);
		material->SetMetallic(metallic);
		material->SetRoughness(roughness);
		material->SetEmissive(0.0f);
		return material;
	}

}