#include "zgpch.h"
#include "Lighting.h"
#include "Shader.h"

namespace Zgine {

	void LightingSystem::AddLight(std::shared_ptr<Light> light)
	{
		if (light)
		{
			m_Lights.push_back(light);
		}
	}

	void LightingSystem::RemoveLight(std::shared_ptr<Light> light)
	{
		auto it = std::find(m_Lights.begin(), m_Lights.end(), light);
		if (it != m_Lights.end())
		{
			m_Lights.erase(it);
		}
	}

	void LightingSystem::ClearLights()
	{
		m_Lights.clear();
	}

	void LightingSystem::SetAmbientLighting(const glm::vec3& color, float intensity)
	{
		m_AmbientColor = color;
		m_AmbientIntensity = intensity;
	}

	void LightingSystem::UpdateShaderUniforms(Shader* shader)
	{
		if (!shader)
			return;

		// Update ambient lighting
		shader->UploadUniformFloat3("u_AmbientColor", m_AmbientColor);
		shader->UploadUniformFloat("u_AmbientIntensity", m_AmbientIntensity);

		// Update directional lights (limit to 4 for performance)
		int directionalLightCount = 0;
		for (const auto& light : m_Lights)
		{
			if (light->GetType() == LightType::Directional && directionalLightCount < 4)
			{
				if (light->IsEnabled())
				{
					auto dirLight = std::static_pointer_cast<DirectionalLight>(light);
					std::string prefix = "u_DirectionalLights[" + std::to_string(directionalLightCount) + "]";
					
					shader->UploadUniformFloat3(prefix + ".direction", dirLight->GetDirection());
					shader->UploadUniformFloat3(prefix + ".color", dirLight->GetColor());
					shader->UploadUniformFloat(prefix + ".intensity", dirLight->GetIntensity());
					directionalLightCount++;
				}
			}
		}
		shader->UploadUniformInt("u_DirectionalLightCount", directionalLightCount);

		// Update point lights (limit to 8 for performance)
		int pointLightCount = 0;
		for (const auto& light : m_Lights)
		{
			if (light->GetType() == LightType::Point && pointLightCount < 8)
			{
				if (light->IsEnabled())
				{
					auto pointLight = std::static_pointer_cast<PointLight>(light);
					std::string prefix = "u_PointLights[" + std::to_string(pointLightCount) + "]";
					
					shader->UploadUniformFloat3(prefix + ".position", pointLight->GetPosition());
					shader->UploadUniformFloat3(prefix + ".color", pointLight->GetColor());
					shader->UploadUniformFloat(prefix + ".intensity", pointLight->GetIntensity());
					shader->UploadUniformFloat(prefix + ".range", pointLight->GetRange());
					pointLightCount++;
				}
			}
		}
		shader->UploadUniformInt("u_PointLightCount", pointLightCount);

		// Update spot lights (limit to 4 for performance)
		int spotLightCount = 0;
		for (const auto& light : m_Lights)
		{
			if (light->GetType() == LightType::Spot && spotLightCount < 4)
			{
				if (light->IsEnabled())
				{
					auto spotLight = std::static_pointer_cast<SpotLight>(light);
					std::string prefix = "u_SpotLights[" + std::to_string(spotLightCount) + "]";
					
					shader->UploadUniformFloat3(prefix + ".position", spotLight->GetPosition());
					shader->UploadUniformFloat3(prefix + ".direction", spotLight->GetDirection());
					shader->UploadUniformFloat3(prefix + ".color", spotLight->GetColor());
					shader->UploadUniformFloat(prefix + ".intensity", spotLight->GetIntensity());
					shader->UploadUniformFloat(prefix + ".range", spotLight->GetRange());
					shader->UploadUniformFloat(prefix + ".innerConeAngle", spotLight->GetInnerConeAngle());
					shader->UploadUniformFloat(prefix + ".outerConeAngle", spotLight->GetOuterConeAngle());
					spotLightCount++;
				}
			}
		}
		shader->UploadUniformInt("u_SpotLightCount", spotLightCount);
	}

}
