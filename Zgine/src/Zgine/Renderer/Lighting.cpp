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

		// Update ambient lighting (only what the shader actually supports)
		shader->UploadUniformFloat3("u_AmbientColor", m_AmbientColor);
		shader->UploadUniformFloat("u_AmbientIntensity", m_AmbientIntensity);

		// Note: The current BatchRenderer3D shader only supports basic ambient lighting
		// Complex lighting (directional, point, spot) is not implemented in the shader yet
		// This prevents the "Uniform not found" errors
	}

}
