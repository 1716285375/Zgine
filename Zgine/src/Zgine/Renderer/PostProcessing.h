#pragma once

#include "zgpch.h"
#include "../Core.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

namespace Zgine {

	/**
	 * @brief Post-processing effect types
	 */
	enum class PostEffectType
	{
		None = 0,
		Bloom,
		Blur,
		Sharpen,
		EdgeDetection,
		Emboss,
		Sepia,
		GrayScale,
		Invert,
		Vignette,
		ChromaticAberration,
		FilmGrain,
		MotionBlur,
		DepthOfField,
		SSAO,
		FXAA,
		SMAA,
		TAA
	};

	/**
	 * @brief Post-processing effect parameters
	 */
	struct PostEffectParams
	{
		bool enabled = true;
		float intensity = 1.0f;
		float threshold = 0.5f;
		float radius = 1.0f;
		glm::vec3 colorTint = glm::vec3(1.0f);
		float vignetteStrength = 0.5f;
		float vignetteRadius = 0.8f;
		float chromaticAberrationStrength = 0.02f;
		float filmGrainStrength = 0.1f;
		float motionBlurStrength = 0.5f;
		float depthOfFieldFocus = 10.0f;
		float depthOfFieldRange = 5.0f;
		float ssaoRadius = 0.5f;
		float ssaoBias = 0.025f;
		int ssaoSamples = 16;
	};

	/**
	 * @brief Frame buffer for post-processing
	 */
	class PostProcessingFramebuffer
	{
	public:
		PostProcessingFramebuffer(uint32_t width, uint32_t height);
		~PostProcessingFramebuffer();

		void Resize(uint32_t width, uint32_t height);
		void Bind();
		void Unbind();
		void BindColorTexture(uint32_t slot = 0);
		void BindDepthTexture(uint32_t slot = 0);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		Ref<Texture2D> GetColorTexture() const { return m_ColorTexture; }
		Ref<Texture2D> GetDepthTexture() const { return m_DepthTexture; }

	private:
		uint32_t m_FramebufferID;
		uint32_t m_RenderbufferID;
		Ref<Texture2D> m_ColorTexture;
		Ref<Texture2D> m_DepthTexture;
		uint32_t m_Width, m_Height;
	};

	/**
	 * @brief Post-processing effect shader
	 */
	class PostEffectShader
	{
	public:
		PostEffectShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~PostEffectShader();

		void Bind() const;
		void Unbind() const;
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, int value);
		void SetUniform(const std::string& name, const glm::vec2& value);
		void SetUniform(const std::string& name, const glm::vec3& value);
		void SetUniform(const std::string& name, const glm::vec4& value);
		void SetUniform(const std::string& name, const glm::mat4& value);

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
		Ref<Shader> m_Shader;
	};

	/**
	 * @brief Post-processing effect
	 */
	class PostEffect
	{
	public:
		PostEffect(PostEffectType type, const std::string& name);
		~PostEffect();

		void Init();
		void Shutdown();
		void Render(Ref<Texture2D> inputTexture, Ref<Texture2D> outputTexture, const PostEffectParams& params);

		PostEffectType GetType() const { return m_Type; }
		const std::string& GetName() const { return m_Name; }
		bool IsInitialized() const { return m_Initialized; }

		void SetEnabled(bool enabled) { m_Params.enabled = enabled; }
		bool IsEnabled() const { return m_Params.enabled; }

		PostEffectParams& GetParams() { return m_Params; }
		const PostEffectParams& GetParams() const { return m_Params; }

	private:
		PostEffectType m_Type;
		std::string m_Name;
		bool m_Initialized;
		PostEffectParams m_Params;
		
		Ref<PostEffectShader> m_Shader;
		Ref<VertexArray> m_QuadVA;
		Ref<VertexBuffer> m_QuadVB;
		Ref<IndexBuffer> m_QuadIB;

		void CreateQuad();
		std::string GetVertexShaderSource() const;
		std::string GetFragmentShaderSource() const;
	};

	/**
	 * @brief Post-processing pipeline
	 */
	class PostProcessingPipeline
	{
	public:
		PostProcessingPipeline();
		~PostProcessingPipeline();

		void Init(uint32_t width, uint32_t height);
		void Shutdown();
		void Resize(uint32_t width, uint32_t height);

		void BeginScene();
		void EndScene();
		void Render();

		void AddEffect(PostEffectType type, const std::string& name);
		void RemoveEffect(PostEffectType type);
		void ClearEffects();

		PostEffect* GetEffect(PostEffectType type);
		const std::vector<Ref<PostEffect>>& GetEffects() const { return m_Effects; }

		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		bool IsEnabled() const { return m_Enabled; }

		Ref<Texture2D> GetFinalTexture() const;
		Ref<Texture2D> GetColorTexture() const;
		Ref<Texture2D> GetDepthTexture() const;

		// Preset configurations
		void LoadPreset(const std::string& presetName);
		void SavePreset(const std::string& presetName);
		std::vector<std::string> GetAvailablePresets() const;

	private:
		bool m_Initialized;
		bool m_Enabled;
		uint32_t m_Width, m_Height;

		std::vector<Ref<PostEffect>> m_Effects;
		std::vector<Ref<PostProcessingFramebuffer>> m_Framebuffers;
		
		Ref<VertexArray> m_FullscreenQuad;
		Ref<VertexBuffer> m_FullscreenQuadVB;
		Ref<IndexBuffer> m_FullscreenQuadIB;

		void CreateFullscreenQuad();
		void CreateFramebuffers();
		void RenderEffect(Ref<PostEffect> effect, Ref<Texture2D> input, Ref<Texture2D> output);
	};

	/**
	 * @brief Post-processing manager
	 */
	class PostProcessingManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene();
		static void EndScene();
		static void Render();

		static void Resize(uint32_t width, uint32_t height);

		static PostProcessingPipeline* GetPipeline() { return s_Pipeline.get(); }
		static bool IsInitialized() { return s_Initialized; }

		// Quick access methods
		static void EnableEffect(PostEffectType type);
		static void DisableEffect(PostEffectType type);
		static void ToggleEffect(PostEffectType type);
		static bool IsEffectEnabled(PostEffectType type);

		static void SetEffectIntensity(PostEffectType type, float intensity);
		static void SetEffectThreshold(PostEffectType type, float threshold);
		static void SetEffectRadius(PostEffectType type, float radius);

		// Preset management
		static void LoadPreset(const std::string& presetName);
		static void SavePreset(const std::string& presetName);
		static std::vector<std::string> GetAvailablePresets();

	private:
		static bool s_Initialized;
		static std::unique_ptr<PostProcessingPipeline> s_Pipeline;
	};

}
