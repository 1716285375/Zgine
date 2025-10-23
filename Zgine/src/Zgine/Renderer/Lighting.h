#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>

namespace Zgine {

	// Light types
	enum class LightType
	{
		Directional = 0,
		Point = 1,
		Spot = 2
	};

	// Base light class
	class Light
	{
	public:
		Light(LightType type, const glm::vec3& color = {1.0f, 1.0f, 1.0f}, float intensity = 1.0f)
			: m_Type(type), m_Color(color), m_Intensity(intensity), m_Enabled(true) {}

		virtual ~Light() = default;

		LightType GetType() const { return m_Type; }
		const glm::vec3& GetColor() const { return m_Color; }
		float GetIntensity() const { return m_Intensity; }
		bool IsEnabled() const { return m_Enabled; }

		void SetColor(const glm::vec3& color) { m_Color = color; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		void SetEnabled(bool enabled) { m_Enabled = enabled; }

		// Virtual methods for light-specific properties
		virtual const glm::vec3& GetPosition() const { static const glm::vec3 zero(0.0f); return zero; }
		virtual const glm::vec3& GetDirection() const { static const glm::vec3 zero(0.0f); return zero; }
		virtual float GetRange() const { return 0.0f; }
		virtual float GetInnerConeAngle() const { return 0.0f; }
		virtual float GetOuterConeAngle() const { return 0.0f; }

	protected:
		LightType m_Type;
		glm::vec3 m_Color;
		float m_Intensity;
		bool m_Enabled;
	};

	// Directional light (like sun)
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(const glm::vec3& direction = {0.0f, -1.0f, 0.0f}, 
						const glm::vec3& color = {1.0f, 1.0f, 1.0f}, 
						float intensity = 1.0f)
			: Light(LightType::Directional, color, intensity), m_Direction(glm::normalize(direction)) {}

		const glm::vec3& GetDirection() const override { return m_Direction; }
		void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }

	private:
		glm::vec3 m_Direction;
	};

	// Point light (like light bulb)
	class PointLight : public Light
	{
	public:
		PointLight(const glm::vec3& position = {0.0f, 0.0f, 0.0f},
				  const glm::vec3& color = {1.0f, 1.0f, 1.0f},
				  float intensity = 1.0f,
				  float range = 10.0f)
			: Light(LightType::Point, color, intensity), m_Position(position), m_Range(range) {}

		const glm::vec3& GetPosition() const override { return m_Position; }
		float GetRange() const override { return m_Range; }

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetRange(float range) { m_Range = range; }

	private:
		glm::vec3 m_Position;
		float m_Range;
	};

	// Spot light (like flashlight)
	class SpotLight : public Light
	{
	public:
		SpotLight(const glm::vec3& position = {0.0f, 0.0f, 0.0f},
				 const glm::vec3& direction = {0.0f, -1.0f, 0.0f},
				 const glm::vec3& color = {1.0f, 1.0f, 1.0f},
				 float intensity = 1.0f,
				 float range = 10.0f,
				 float innerConeAngle = 12.5f,
				 float outerConeAngle = 17.5f)
			: Light(LightType::Spot, color, intensity), 
			  m_Position(position), 
			  m_Direction(glm::normalize(direction)),
			  m_Range(range),
			  m_InnerConeAngle(innerConeAngle),
			  m_OuterConeAngle(outerConeAngle) {}

		const glm::vec3& GetPosition() const override { return m_Position; }
		const glm::vec3& GetDirection() const override { return m_Direction; }
		float GetRange() const override { return m_Range; }
		float GetInnerConeAngle() const override { return m_InnerConeAngle; }
		float GetOuterConeAngle() const override { return m_OuterConeAngle; }

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }
		void SetRange(float range) { m_Range = range; }
		void SetConeAngles(float inner, float outer) { m_InnerConeAngle = inner; m_OuterConeAngle = outer; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		float m_Range;
		float m_InnerConeAngle;
		float m_OuterConeAngle;
	};

	// Lighting system manager
	class LightingSystem
	{
	public:
		static LightingSystem& GetInstance()
		{
			static LightingSystem instance;
			return instance;
		}

		// Light management
		void AddLight(std::shared_ptr<Light> light);
		void RemoveLight(std::shared_ptr<Light> light);
		void ClearLights();

		// Getters
		const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }
		size_t GetLightCount() const { return m_Lights.size(); }

		// Ambient lighting
		const glm::vec3& GetAmbientColor() const { return m_AmbientColor; }
		float GetAmbientIntensity() const { return m_AmbientIntensity; }
		void SetAmbientLighting(const glm::vec3& color, float intensity);

		// Update lighting uniforms in shaders
		void UpdateShaderUniforms(class Shader* shader);

	private:
		LightingSystem() = default;
		~LightingSystem() = default;
		LightingSystem(const LightingSystem&) = delete;
		LightingSystem& operator=(const LightingSystem&) = delete;

		std::vector<std::shared_ptr<Light>> m_Lights;
		glm::vec3 m_AmbientColor = {0.1f, 0.1f, 0.1f};
		float m_AmbientIntensity = 0.3f;
	};

}
