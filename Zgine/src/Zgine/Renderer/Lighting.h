#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>

namespace Zgine {

	/**
	 * @brief Enumeration of light types supported by the lighting system
	 */
	enum class LightType
	{
		Directional = 0,  ///< Directional light (like sun)
		Point = 1,        ///< Point light (like light bulb)
		Spot = 2          ///< Spot light (like flashlight)
	};

	/**
	 * @brief Base light class providing common lighting functionality
	 * @details This abstract base class defines the interface for all light types
	 *          in the engine's lighting system
	 */
	class Light
	{
	public:
		/**
		 * @brief Construct a new Light object
		 * @param type The type of light
		 * @param color The color of the light
		 * @param intensity The intensity of the light
		 */
		Light(LightType type, const glm::vec3& color = {1.0f, 1.0f, 1.0f}, float intensity = 1.0f)
			: m_Type(type), m_Color(color), m_Intensity(intensity), m_Enabled(true) {}

		/**
		 * @brief Destroy the Light object
		 */
		virtual ~Light() = default;

		/**
		 * @brief Get the light type
		 * @return LightType The type of this light
		 */
		LightType GetType() const { return m_Type; }
		
		/**
		 * @brief Get the light color
		 * @return const glm::vec3& The color of this light
		 */
		const glm::vec3& GetColor() const { return m_Color; }
		
		/**
		 * @brief Get the light intensity
		 * @return float The intensity of this light
		 */
		float GetIntensity() const { return m_Intensity; }
		
		/**
		 * @brief Check if the light is enabled
		 * @return bool True if the light is enabled
		 */
		bool IsEnabled() const { return m_Enabled; }

		/**
		 * @brief Set the light color
		 * @param color The new color for this light
		 */
		void SetColor(const glm::vec3& color) { m_Color = color; }
		
		/**
		 * @brief Set the light intensity
		 * @param intensity The new intensity for this light
		 */
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		
		/**
		 * @brief Set whether the light is enabled
		 * @param enabled True to enable the light, false to disable
		 */
		void SetEnabled(bool enabled) { m_Enabled = enabled; }

		/**
		 * @brief Get the light position (virtual method)
		 * @return const glm::vec3& The position of the light
		 */
		virtual const glm::vec3& GetPosition() const { static const glm::vec3 zero(0.0f); return zero; }
		
		/**
		 * @brief Get the light direction (virtual method)
		 * @return const glm::vec3& The direction of the light
		 */
		virtual const glm::vec3& GetDirection() const { static const glm::vec3 zero(0.0f); return zero; }
		
		/**
		 * @brief Get the light range (virtual method)
		 * @return float The range of the light
		 */
		virtual float GetRange() const { return 0.0f; }
		
		/**
		 * @brief Get the inner cone angle (virtual method)
		 * @return float The inner cone angle in degrees
		 */
		virtual float GetInnerConeAngle() const { return 0.0f; }
		
		/**
		 * @brief Get the outer cone angle (virtual method)
		 * @return float The outer cone angle in degrees
		 */
		virtual float GetOuterConeAngle() const { return 0.0f; }

	protected:
		LightType m_Type;     ///< Type of this light
		glm::vec3 m_Color;    ///< Color of this light
		float m_Intensity;    ///< Intensity of this light
		bool m_Enabled;       ///< Whether this light is enabled
	};

	/**
	 * @brief Directional light class representing infinite distance light sources
	 * @details This light type simulates light sources at infinite distance,
	 *          such as the sun, providing uniform directional lighting
	 */
	class DirectionalLight : public Light
	{
	public:
		/**
		 * @brief Construct a new DirectionalLight object
		 * @param direction The direction vector of the light
		 * @param color The color of the light
		 * @param intensity The intensity of the light
		 */
		DirectionalLight(const glm::vec3& direction = {0.0f, -1.0f, 0.0f}, 
						const glm::vec3& color = {1.0f, 1.0f, 1.0f}, 
						float intensity = 1.0f)
			: Light(LightType::Directional, color, intensity), m_Direction(glm::normalize(direction)) {}

		/**
		 * @brief Get the light direction
		 * @return const glm::vec3& The normalized direction vector
		 */
		const glm::vec3& GetDirection() const override { return m_Direction; }
		
		/**
		 * @brief Set the light direction
		 * @param direction The new direction vector (will be normalized)
		 */
		void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }

	private:
		glm::vec3 m_Direction;  ///< Normalized direction vector of the light
	};

	/**
	 * @brief Point light class representing omnidirectional light sources
	 * @details This light type simulates light sources that emit light in all
	 *          directions from a specific point, such as light bulbs
	 */
	class PointLight : public Light
	{
	public:
		/**
		 * @brief Construct a new PointLight object
		 * @param position The world position of the light
		 * @param color The color of the light
		 * @param intensity The intensity of the light
		 * @param range The maximum range of the light
		 */
		PointLight(const glm::vec3& position = {0.0f, 0.0f, 0.0f},
				  const glm::vec3& color = {1.0f, 1.0f, 1.0f},
				  float intensity = 1.0f,
				  float range = 10.0f)
			: Light(LightType::Point, color, intensity), m_Position(position), m_Range(range) {}

		/**
		 * @brief Get the light position
		 * @return const glm::vec3& The world position of the light
		 */
		const glm::vec3& GetPosition() const override { return m_Position; }
		
		/**
		 * @brief Get the light range
		 * @return float The maximum range of the light
		 */
		float GetRange() const override { return m_Range; }

		/**
		 * @brief Set the light position
		 * @param position The new world position of the light
		 */
		void SetPosition(const glm::vec3& position) { m_Position = position; }
		
		/**
		 * @brief Set the light range
		 * @param range The new maximum range of the light
		 */
		void SetRange(float range) { m_Range = range; }

	private:
		glm::vec3 m_Position;  ///< World position of the light
		float m_Range;        ///< Maximum range of the light
	};

	/**
	 * @brief Spot light class representing directional light sources with cone falloff
	 * @details This light type simulates light sources that emit light in a cone
	 *          shape, such as flashlights or stage lights
	 */
	class SpotLight : public Light
	{
	public:
		/**
		 * @brief Construct a new SpotLight object
		 * @param position The world position of the light
		 * @param direction The direction vector of the light cone
		 * @param color The color of the light
		 * @param intensity The intensity of the light
		 * @param range The maximum range of the light
		 * @param innerConeAngle The inner cone angle in degrees
		 * @param outerConeAngle The outer cone angle in degrees
		 */
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

		/**
		 * @brief Get the light position
		 * @return const glm::vec3& The world position of the light
		 */
		const glm::vec3& GetPosition() const override { return m_Position; }
		
		/**
		 * @brief Get the light direction
		 * @return const glm::vec3& The normalized direction vector
		 */
		const glm::vec3& GetDirection() const override { return m_Direction; }
		
		/**
		 * @brief Get the light range
		 * @return float The maximum range of the light
		 */
		float GetRange() const override { return m_Range; }
		
		/**
		 * @brief Get the inner cone angle
		 * @return float The inner cone angle in degrees
		 */
		float GetInnerConeAngle() const override { return m_InnerConeAngle; }
		
		/**
		 * @brief Get the outer cone angle
		 * @return float The outer cone angle in degrees
		 */
		float GetOuterConeAngle() const override { return m_OuterConeAngle; }

		/**
		 * @brief Set the light position
		 * @param position The new world position of the light
		 */
		void SetPosition(const glm::vec3& position) { m_Position = position; }
		
		/**
		 * @brief Set the light direction
		 * @param direction The new direction vector (will be normalized)
		 */
		void SetDirection(const glm::vec3& direction) { m_Direction = glm::normalize(direction); }
		
		/**
		 * @brief Set the light range
		 * @param range The new maximum range of the light
		 */
		void SetRange(float range) { m_Range = range; }
		
		/**
		 * @brief Set the cone angles
		 * @param inner The inner cone angle in degrees
		 * @param outer The outer cone angle in degrees
		 */
		void SetConeAngles(float inner, float outer) { m_InnerConeAngle = inner; m_OuterConeAngle = outer; }

	private:
		glm::vec3 m_Position;        ///< World position of the light
		glm::vec3 m_Direction;      ///< Normalized direction vector of the light cone
		float m_Range;              ///< Maximum range of the light
		float m_InnerConeAngle;     ///< Inner cone angle in degrees
		float m_OuterConeAngle;     ///< Outer cone angle in degrees
	};

	/**
	 * @brief Lighting system singleton for managing all lights in the scene
	 * @details This class provides centralized management of all light sources,
	 *          ambient lighting, and shader uniform updates
	 */
	class LightingSystem
	{
	public:
		/**
		 * @brief Get the singleton instance
		 * @return LightingSystem& Reference to the singleton instance
		 */
		static LightingSystem& GetInstance()
		{
			static LightingSystem instance;
			return instance;
		}

		/**
		 * @brief Add a light to the lighting system
		 * @param light The light to add
		 */
		void AddLight(std::shared_ptr<Light> light);
		
		/**
		 * @brief Remove a light from the lighting system
		 * @param light The light to remove
		 */
		void RemoveLight(std::shared_ptr<Light> light);
		
		/**
		 * @brief Clear all lights from the lighting system
		 */
		void ClearLights();

		/**
		 * @brief Get all lights in the system
		 * @return const std::vector<std::shared_ptr<Light>>& Vector of all lights
		 */
		const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }
		
		/**
		 * @brief Get the number of lights in the system
		 * @return size_t The number of lights
		 */
		size_t GetLightCount() const { return m_Lights.size(); }

		/**
		 * @brief Get the ambient light color
		 * @return const glm::vec3& The ambient light color
		 */
		const glm::vec3& GetAmbientColor() const { return m_AmbientColor; }
		
		/**
		 * @brief Get the ambient light intensity
		 * @return float The ambient light intensity
		 */
		float GetAmbientIntensity() const { return m_AmbientIntensity; }
		
		/**
		 * @brief Set the ambient lighting parameters
		 * @param color The ambient light color
		 * @param intensity The ambient light intensity
		 */
		void SetAmbientLighting(const glm::vec3& color, float intensity);

		/**
		 * @brief Update lighting uniforms in a shader
		 * @param shader Pointer to the shader to update
		 */
		void UpdateShaderUniforms(class Shader* shader);

	private:
		LightingSystem() = default;
		~LightingSystem() = default;
		LightingSystem(const LightingSystem&) = delete;
		LightingSystem& operator=(const LightingSystem&) = delete;

		std::vector<std::shared_ptr<Light>> m_Lights;  ///< Vector of all lights in the system
		glm::vec3 m_AmbientColor = {0.1f, 0.1f, 0.1f}; ///< Ambient light color
		float m_AmbientIntensity = 0.3f;               ///< Ambient light intensity
	};

}
