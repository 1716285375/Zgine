#pragma once

#include "zgpch.h"
#include "../Core.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "PerspectiveCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace Zgine {

	/**
	 * @brief Shadow map resolution
	 */
	enum class ShadowMapResolution
	{
		Low = 512,
		Medium = 1024,
		High = 2048,
		Ultra = 4096
	};

	/**
	 * @brief Shadow map type
	 */
	enum class ShadowMapType
	{
		Directional = 0,
		Point,
		Spot,
		Cascaded
	};

	/**
	 * @brief Shadow map configuration
	 */
	struct ShadowMapConfig
	{
		ShadowMapResolution resolution = ShadowMapResolution::High;
		ShadowMapType type = ShadowMapType::Directional;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		float bias = 0.005f;
		float normalBias = 0.01f;
		bool softShadows = true;
		bool enableSoftShadows = true;
		bool enableCascadedShadows = true;
		float softness = 1.0f;
		int cascadeCount = 4;
		float cascadeSplitLambda = 0.95f;
	};

	/**
	 * @brief Light source for shadow casting
	 */
	struct ShadowLight
	{
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
		float range;
		float spotAngle;
		bool castShadows = true;
	};

	/**
	 * @brief Shadow map framebuffer
	 */
	class ShadowMapFramebuffer
	{
	public:
		ShadowMapFramebuffer(uint32_t width, uint32_t height, ShadowMapType type);
		~ShadowMapFramebuffer();

		void Bind();
		void Unbind();
		void BindTexture(uint32_t slot = 0);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		ShadowMapType GetType() const { return m_Type; }
		Ref<Texture2D> GetTexture() const { return m_ShadowTexture; }

		void Resize(uint32_t width, uint32_t height);

	private:
		uint32_t m_FramebufferID;
		Ref<Texture2D> m_ShadowTexture;
		uint32_t m_Width, m_Height;
		ShadowMapType m_Type;

		void CreateTexture();
	};

	/**
	 * @brief Shadow map renderer
	 */
	class ShadowMapRenderer
	{
	public:
		ShadowMapRenderer();
		~ShadowMapRenderer();

		void Init(const ShadowMapConfig& config);
		void Shutdown();

		void BeginShadowPass(const ShadowLight& light, const glm::vec3& sceneCenter);
		void EndShadowPass();

		void RenderShadowMap();
		void RenderDebugQuad();

		Ref<Texture2D> GetShadowTexture() const;
		glm::mat4 GetLightSpaceMatrix() const { return m_LightSpaceMatrix; }
		const ShadowMapConfig& GetConfig() const { return m_Config; }

		void SetConfig(const ShadowMapConfig& config);
		void SetLight(const ShadowLight& light);

		// Cascaded shadow maps
		void BeginCascadedShadowPass(const ShadowLight& light, const PerspectiveCamera& camera);
		void EndCascadedShadowPass();
		std::vector<glm::mat4> GetCascadeMatrices() const { return m_CascadeMatrices; }
		std::vector<float> GetCascadeDistances() const { return m_CascadeDistances; }

	private:
		bool m_Initialized;
		ShadowMapConfig m_Config;
		ShadowLight m_CurrentLight;
		
		Ref<ShadowMapFramebuffer> m_ShadowFramebuffer;
		Ref<Shader> m_ShadowShader;
		Ref<Shader> m_DebugShader;
		
		glm::mat4 m_LightSpaceMatrix;
		glm::mat4 m_LightProjectionMatrix;
		glm::mat4 m_LightViewMatrix;

		// Cascaded shadow maps
		std::vector<glm::mat4> m_CascadeMatrices;
		std::vector<float> m_CascadeDistances;
		std::vector<Ref<ShadowMapFramebuffer>> m_CascadeFramebuffers;

		void CreateShaders();
		void CreateCascadeFramebuffers();
		void CalculateCascadeSplits(const PerspectiveCamera& camera);
		void CalculateCascadeMatrices(const ShadowLight& light, const PerspectiveCamera& camera);
		glm::mat4 CalculateLightSpaceMatrix(const ShadowLight& light, const glm::vec3& sceneCenter);
	};

	/**
	 * @brief Shadow mapping system
	 */
	class ShadowMappingSystem
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene();
		static void EndScene();

		static void AddLight(const ShadowLight& light);
		static void RemoveLight(uint32_t index);
		static void ClearLights();

		static void RenderShadows();
		static void RenderDebug();

		static ShadowMapRenderer* GetRenderer() { return s_Renderer.get(); }
		static bool IsInitialized() { return s_Initialized; }

		// Configuration
		static void SetConfig(const ShadowMapConfig& config);
		static const ShadowMapConfig& GetConfig() { return s_Config; }

		// Light management
		static uint32_t GetLightCount() { return static_cast<uint32_t>(s_Lights.size()); }
		static const ShadowLight& GetLight(uint32_t index) { return s_Lights[index]; }
		static void SetLight(uint32_t index, const ShadowLight& light);

		// Debug
		static void SetDebugEnabled(bool enabled) { s_DebugEnabled = enabled; }
		static bool IsDebugEnabled() { return s_DebugEnabled; }

	private:
		static bool s_Initialized;
		static bool s_DebugEnabled;
		static ShadowMapConfig s_Config;
		static std::unique_ptr<ShadowMapRenderer> s_Renderer;
		static std::vector<ShadowLight> s_Lights;
	};

	/**
	 * @brief Shadow mapping utilities
	 */
	class ShadowUtils
	{
	public:
		// Matrix calculations
		static glm::mat4 CalculateDirectionalLightMatrix(const glm::vec3& direction, const glm::vec3& center, float radius);
		static glm::mat4 CalculatePointLightMatrix(const glm::vec3& position, float nearPlane, float farPlane);
		static glm::mat4 CalculateSpotLightMatrix(const glm::vec3& position, const glm::vec3& direction, float fov, float nearPlane, float farPlane);

		// Sampling
		static float SampleShadowMap(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare);
		static float SampleShadowMapPCF(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare, float texelSize);
		static float SampleShadowMapPCSS(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare, float texelSize, float lightRadius);

		// Bias calculations
		static float CalculateShadowBias(const glm::vec3& lightDir, const glm::vec3& normal, float bias, float normalBias);
		static glm::vec3 CalculateNormalBias(const glm::vec3& normal, float normalBias);

		// Debug
		static void RenderShadowMapDebug(Ref<Texture2D> shadowMap, const glm::vec2& position, const glm::vec2& size);
	};

}
