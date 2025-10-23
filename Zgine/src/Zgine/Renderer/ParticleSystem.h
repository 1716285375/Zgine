#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

#include "Zgine/Core/SmartPointers.h"
#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Texture.h"
#include "Zgine/Renderer/VertexArray.h"

namespace Zgine {

	/**
	 * @brief Particle structure containing all particle data
	 */
	struct Particle
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Acceleration = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 Size = { 1.0f, 1.0f };
		float Rotation = 0.0f;
		float RotationSpeed = 0.0f;
		float Life = 1.0f;
		float MaxLife = 1.0f;
		float Age = 0.0f;
		bool Active = false;
	};

	/**
	 * @brief Particle emitter configuration
	 */
	struct ParticleEmitterConfig
	{
		// Emission properties
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 VelocityMin = { -1.0f, -1.0f, -1.0f };
		glm::vec3 VelocityMax = { 1.0f, 1.0f, 1.0f };
		glm::vec3 AccelerationMin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 AccelerationMax = { 0.0f, 0.0f, 0.0f };
		
		// Visual properties
		glm::vec4 ColorStart = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 ColorEnd = { 1.0f, 1.0f, 1.0f, 0.0f };
		glm::vec2 SizeStart = { 1.0f, 1.0f };
		glm::vec2 SizeEnd = { 0.1f, 0.1f };
		
		// Life properties
		float LifeMin = 1.0f;
		float LifeMax = 2.0f;
		
		// Rotation properties
		float RotationMin = 0.0f;
		float RotationMax = 0.0f;
		float RotationSpeedMin = 0.0f;
		float RotationSpeedMax = 0.0f;
		
		// Emission rate
		float EmissionRate = 10.0f; // particles per second
		int MaxParticles = 1000;
		
		// Texture
		Ref<Texture2D> Texture = nullptr;
		
		// Custom update function
		std::function<void(Particle&, float)> CustomUpdate = nullptr;
	};

	/**
	 * @brief Particle system for managing and rendering particles
	 */
	class ParticleSystem
	{
	public:
		/**
		 * @brief Constructor
		 * @param config The particle emitter configuration
		 */
		explicit ParticleSystem(const ParticleEmitterConfig& config);
		
		/**
		 * @brief Destructor
		 */
		~ParticleSystem();

		/**
		 * @brief Update the particle system
		 * @param deltaTime Time elapsed since last update
		 */
		void OnUpdate(float deltaTime);

		/**
		 * @brief Render all active particles
		 * @param viewProjection The view-projection matrix
		 */
		void OnRender(const glm::mat4& viewProjection);

		/**
		 * @brief Emit a burst of particles
		 * @param count Number of particles to emit
		 */
		void EmitBurst(int count);

		/**
		 * @brief Start continuous emission
		 */
		void StartEmission();

		/**
		 * @brief Stop continuous emission
		 */
		void StopEmission();

		/**
		 * @brief Check if emission is active
		 * @return True if emission is active
		 */
		bool IsEmitting() const { return m_Emitting; }

		/**
		 * @brief Get the number of active particles
		 * @return Number of active particles
		 */
		int GetActiveParticleCount() const;

		/**
		 * @brief Get the emitter configuration
		 * @return Reference to the emitter configuration
		 */
		ParticleEmitterConfig& GetConfig() { return m_Config; }

		/**
		 * @brief Get the emitter configuration (const)
		 * @return Const reference to the emitter configuration
		 */
		const ParticleEmitterConfig& GetConfig() const { return m_Config; }

		/**
		 * @brief Set the emitter position
		 * @param position New position
		 */
		void SetPosition(const glm::vec3& position) { m_Config.Position = position; }

		/**
		 * @brief Get the emitter position
		 * @return Current position
		 */
		const glm::vec3& GetPosition() const { return m_Config.Position; }

	private:
		/**
		 * @brief Initialize the particle system
		 */
		void Init();

		/**
		 * @brief Emit a single particle
		 */
		void EmitParticle();

		/**
		 * @brief Update a single particle
		 * @param particle The particle to update
		 * @param deltaTime Time elapsed since last update
		 */
		void UpdateParticle(Particle& particle, float deltaTime);

		/**
		 * @brief Reset a particle to inactive state
		 * @param particle The particle to reset
		 */
		void ResetParticle(Particle& particle);

		/**
		 * @brief Generate random float between min and max
		 * @param min Minimum value
		 * @param max Maximum value
		 * @return Random float
		 */
		float RandomFloat(float min, float max);

		/**
		 * @brief Generate random vector between min and max
		 * @param min Minimum vector
		 * @param max Maximum vector
		 * @return Random vector
		 */
		glm::vec3 RandomVector(const glm::vec3& min, const glm::vec3& max);

		/**
		 * @brief Linear interpolation between two values
		 * @param a Start value
		 * @param b End value
		 * @param t Interpolation factor (0-1)
		 * @return Interpolated value
		 */
		template<typename T>
		T Lerp(const T& a, const T& b, float t)
		{
			return a + (b - a) * t;
		}

	private:
		ParticleEmitterConfig m_Config;
		std::vector<Particle> m_Particles;
		
		// Rendering
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<Shader> m_Shader;
		
		// Emission
		bool m_Emitting = false;
		float m_EmissionTimer = 0.0f;
		int m_NextParticleIndex = 0;
		
		// Statistics
		int m_ActiveParticleCount = 0;
		int m_TotalEmittedParticles = 0;
	};

	/**
	 * @brief Particle system manager for handling multiple particle systems
	 */
	class ParticleSystemManager
	{
	public:
		/**
		 * @brief Initialize the particle system manager
		 */
		static void Init();

		/**
		 * @brief Shutdown the particle system manager
		 */
		static void Shutdown();

		/**
		 * @brief Update all particle systems
		 * @param deltaTime Time elapsed since last update
		 */
		static void OnUpdate(float deltaTime);

		/**
		 * @brief Render all particle systems
		 * @param viewProjection The view-projection matrix
		 */
		static void OnRender(const glm::mat4& viewProjection);

		/**
		 * @brief Add a particle system
		 * @param system The particle system to add
		 */
		static void AddParticleSystem(Ref<ParticleSystem> system);

		/**
		 * @brief Remove a particle system
		 * @param system The particle system to remove
		 */
		static void RemoveParticleSystem(Ref<ParticleSystem> system);

		/**
		 * @brief Get the total number of active particles across all systems
		 * @return Total active particle count
		 */
		static int GetTotalActiveParticleCount();

		/**
		 * @brief Clear all particle systems
		 */
		static void ClearAll();

	private:
		static std::vector<Ref<ParticleSystem>> s_ParticleSystems;
		static bool s_Initialized;
	};

} // namespace Zgine
