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

namespace Zgine {

	/**
	 * @brief Tone mapping operators
	 */
	enum class ToneMappingOperator
	{
		None = 0,
		Linear,
		Reinhard,
		ReinhardJodie,
		ACES,
		Uncharted2,
		Filmic,
		Lottes
	};

	/**
	 * @brief HDR configuration
	 */
	struct HDRConfig
	{
		bool enabled = true;
		float exposure = 1.0f;
		float gamma = 2.2f;
		ToneMappingOperator toneMapping = ToneMappingOperator::ACES;
		bool autoExposure = false;
		float autoExposureSpeed = 1.0f;
		float minExposure = 0.1f;
		float maxExposure = 10.0f;
		float whitePoint = 1.0f;
		float adaptationRate = 0.1f;
	};

	/**
	 * @brief HDR framebuffer
	 */
	class HDRFramebuffer
	{
	public:
		HDRFramebuffer(uint32_t width, uint32_t height);
		~HDRFramebuffer();

		void Bind();
		void Unbind();
		void BindColorTexture(uint32_t slot = 0);
		void BindDepthTexture(uint32_t slot = 0);

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		Ref<Texture2D> GetColorTexture() const { return m_ColorTexture; }
		Ref<Texture2D> GetDepthTexture() const { return m_DepthTexture; }

	private:
		uint32_t m_FramebufferID;
		Ref<Texture2D> m_ColorTexture;
		Ref<Texture2D> m_DepthTexture;
		uint32_t m_Width, m_Height;

		void CreateTextures();
	};

	/**
	 * @brief HDR tone mapping renderer
	 */
	class HDRToneMappingRenderer
	{
	public:
		HDRToneMappingRenderer();
		~HDRToneMappingRenderer();

		void Init();
		void Shutdown();

		void Render(Ref<Texture2D> hdrTexture, const HDRConfig& config);
		void RenderBloom(Ref<Texture2D> hdrTexture, const HDRConfig& config);

		void SetExposure(float exposure) { m_CurrentExposure = exposure; }
		float GetExposure() const { return m_CurrentExposure; }

		void UpdateAutoExposure(Ref<Texture2D> hdrTexture, float deltaTime);

	private:
		bool m_Initialized;
		float m_CurrentExposure;
		float m_TargetExposure;
		
		Ref<Shader> m_ToneMappingShader;
		Ref<Shader> m_BloomShader;
		Ref<Shader> m_BrightnessShader;
		Ref<Shader> m_BlurShader;
		
		Ref<VertexArray> m_QuadVA;
		Ref<VertexBuffer> m_QuadVB;
		Ref<IndexBuffer> m_QuadIB;

		// Bloom framebuffers
		std::vector<Ref<HDRFramebuffer>> m_BloomFramebuffers;
		Ref<HDRFramebuffer> m_BrightnessFramebuffer;

		void CreateShaders();
		void CreateQuad();
		void CreateBloomFramebuffers();
		void RenderBrightnessPass(Ref<Texture2D> hdrTexture);
		void RenderBloomPass();
		void RenderToneMappingPass(Ref<Texture2D> hdrTexture, const HDRConfig& config);
		std::string GetToneMappingShaderSource(ToneMappingOperator op);
	};

	/**
	 * @brief HDR system
	 */
	class HDRSystem
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene();
		static void EndScene();
		static void Render();

		static void Resize(uint32_t width, uint32_t height);

		static HDRFramebuffer* GetFramebuffer() { return s_Framebuffer.get(); }
		static HDRToneMappingRenderer* GetRenderer() { return s_Renderer.get(); }
		static bool IsInitialized() { return s_Initialized; }

		// Configuration
		static void SetConfig(const HDRConfig& config) { s_Config = config; }
		static const HDRConfig& GetConfig() { return s_Config; }

		// Quick access methods
		static void SetExposure(float exposure);
		static void SetGamma(float gamma);
		static void SetToneMapping(ToneMappingOperator op);
		static void SetAutoExposure(bool enabled);

		// Texture access
		static Ref<Texture2D> GetHDRTexture();
		static Ref<Texture2D> GetFinalTexture();

	private:
		static bool s_Initialized;
		static HDRConfig s_Config;
		static std::unique_ptr<HDRFramebuffer> s_Framebuffer;
		static std::unique_ptr<HDRToneMappingRenderer> s_Renderer;
	};

	/**
	 * @brief HDR utilities
	 */
	class HDRUtils
	{
	public:
		// Tone mapping functions
		static glm::vec3 LinearToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 ReinhardToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 ReinhardJodieToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 ACESToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 Uncharted2ToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 FilmicToneMapping(const glm::vec3& color, float exposure);
		static glm::vec3 LottesToneMapping(const glm::vec3& color, float exposure);

		// Color space conversions
		static glm::vec3 LinearToSRGB(const glm::vec3& linear);
		static glm::vec3 SRGBToLinear(const glm::vec3& srgb);
		static float LinearToSRGB(float linear);
		static float SRGBToLinear(float srgb);

		// Exposure calculations
		static float CalculateLuminance(const glm::vec3& color);
		static float CalculateAverageLuminance(Ref<Texture2D> texture);
		static float CalculateAutoExposure(Ref<Texture2D> texture, float targetLuminance = 0.18f);

		// Gamma correction
		static glm::vec3 ApplyGammaCorrection(const glm::vec3& color, float gamma);
		static float ApplyGammaCorrection(float value, float gamma);

		// Color grading
		static glm::vec3 ApplyColorGrading(const glm::vec3& color, const glm::vec3& lift, const glm::vec3& gamma, const glm::vec3& gain);
		static glm::vec3 ApplyWhiteBalance(const glm::vec3& color, float temperature, float tint);
		static glm::vec3 ApplyContrast(const glm::vec3& color, float contrast);
		static glm::vec3 ApplySaturation(const glm::vec3& color, float saturation);
		static glm::vec3 ApplyBrightness(const glm::vec3& color, float brightness);
	};

}
