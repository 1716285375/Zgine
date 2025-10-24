#include "zgpch.h"
#include "HDRSystem.h"
#include "RenderCommand.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace Zgine {

	// HDRFramebuffer Implementation
	HDRFramebuffer::HDRFramebuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		CreateTextures();
	}

	HDRFramebuffer::~HDRFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
	}

	void HDRFramebuffer::CreateTextures()
	{
		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		// Create HDR color texture
		uint32_t colorTextureID;
		glGenTextures(1, &colorTextureID);
		glBindTexture(GL_TEXTURE_2D, colorTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureID, 0);

		// Create depth texture
		uint32_t depthTextureID;
		glGenTextures(1, &depthTextureID);
		glBindTexture(GL_TEXTURE_2D, depthTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ZG_CORE_ERROR("HDR framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// TODO: Create texture wrappers for existing OpenGL textures
		// m_ColorTexture = Texture2D::Create(colorTextureID, m_Width, m_Height);
		// m_DepthTexture = Texture2D::Create(depthTextureID, m_Width, m_Height);
	}

	void HDRFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Width, m_Height);
	}

	void HDRFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void HDRFramebuffer::BindColorTexture(uint32_t slot)
	{
		m_ColorTexture->Bind(slot);
	}

	void HDRFramebuffer::BindDepthTexture(uint32_t slot)
	{
		m_DepthTexture->Bind(slot);
	}

	void HDRFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		CreateTextures();
	}

	// HDRToneMappingRenderer Implementation
	HDRToneMappingRenderer::HDRToneMappingRenderer()
		: m_Initialized(false), m_CurrentExposure(1.0f), m_TargetExposure(1.0f)
	{
	}

	HDRToneMappingRenderer::~HDRToneMappingRenderer()
	{
		Shutdown();
	}

	void HDRToneMappingRenderer::Init()
	{
		if (m_Initialized)
			return;

		CreateShaders();
		CreateQuad();
		CreateBloomFramebuffers();

		m_Initialized = true;
		ZG_CORE_INFO("HDR tone mapping renderer initialized");
	}

	void HDRToneMappingRenderer::Shutdown()
	{
		if (!m_Initialized)
			return;

		m_ToneMappingShader.reset();
		m_BloomShader.reset();
		m_BrightnessShader.reset();
		m_BlurShader.reset();
		m_QuadVA.reset();
		m_QuadVB.reset();
		m_QuadIB.reset();
		m_BloomFramebuffers.clear();
		m_BrightnessFramebuffer.reset();

		m_Initialized = false;
		ZG_CORE_INFO("HDR tone mapping renderer shutdown");
	}

	void HDRToneMappingRenderer::Render(Ref<Texture2D> hdrTexture, const HDRConfig& config)
	{
		if (!m_Initialized)
			return;

		// Update auto exposure
		if (config.autoExposure)
		{
			UpdateAutoExposure(hdrTexture, 0.016f); // Assume 60 FPS
		}

		// Render brightness pass for bloom
		RenderBrightnessPass(hdrTexture);

		// Render bloom pass
		RenderBloomPass();

		// Render final tone mapping pass
		RenderToneMappingPass(hdrTexture, config);
	}

	void HDRToneMappingRenderer::RenderBloom(Ref<Texture2D> hdrTexture, const HDRConfig& config)
	{
		if (!m_Initialized)
			return;

		RenderBrightnessPass(hdrTexture);
		RenderBloomPass();
	}

	void HDRToneMappingRenderer::UpdateAutoExposure(Ref<Texture2D> hdrTexture, float deltaTime)
	{
		// Calculate average luminance
		float averageLuminance = HDRUtils::CalculateAverageLuminance(hdrTexture);
		
		// Calculate target exposure
		float targetLuminance = 0.18f; // Middle gray
		m_TargetExposure = targetLuminance / (averageLuminance + 0.001f);
		
		// Smooth exposure transition
		float adaptationRate = 0.1f;
		m_CurrentExposure = glm::mix(m_CurrentExposure, m_TargetExposure, adaptationRate * deltaTime);
	}

	void HDRToneMappingRenderer::CreateShaders()
	{
		// Tone mapping shader
		std::string toneMappingVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec2 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = vec4(a_Position, 0.0, 1.0);
			}
		)";

		std::string toneMappingFragmentSrc = R"(
			#version 330 core
			out vec4 FragColor;

			in vec2 v_TexCoord;

			uniform sampler2D u_HDRTexture;
			uniform sampler2D u_BloomTexture;
			uniform float u_Exposure;
			uniform float u_Gamma;
			uniform int u_ToneMappingOperator;

			vec3 LinearToneMapping(vec3 color, float exposure)
			{
				return color * exposure;
			}

			vec3 ReinhardToneMapping(vec3 color, float exposure)
			{
				color *= exposure;
				return color / (1.0 + color);
			}

			vec3 ReinhardJodieToneMapping(vec3 color, float exposure)
			{
				color *= exposure;
				vec3 tc = color / (1.0 + color);
				return mix(color / (1.0 + luminance(color)), tc, tc);
			}

			vec3 ACESToneMapping(vec3 color, float exposure)
			{
				color *= exposure;
				float a = 2.51;
				float b = 0.03;
				float c = 2.43;
				float d = 0.59;
				float e = 0.14;
				return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
			}

			float luminance(vec3 color)
			{
				return dot(color, vec3(0.2126, 0.7152, 0.0722));
			}

			void main()
			{
				vec3 hdrColor = texture(u_HDRTexture, v_TexCoord).rgb;
				vec3 bloomColor = texture(u_BloomTexture, v_TexCoord).rgb;
				
				// Combine HDR and bloom
				vec3 color = hdrColor + bloomColor;
				
				// Apply tone mapping
				if (u_ToneMappingOperator == 0) // Linear
					color = LinearToneMapping(color, u_Exposure);
				else if (u_ToneMappingOperator == 1) // Reinhard
					color = ReinhardToneMapping(color, u_Exposure);
				else if (u_ToneMappingOperator == 2) // Reinhard Jodie
					color = ReinhardJodieToneMapping(color, u_Exposure);
				else if (u_ToneMappingOperator == 3) // ACES
					color = ACESToneMapping(color, u_Exposure);
				
				// Apply gamma correction
				color = pow(color, vec3(1.0 / u_Gamma));
				
				FragColor = vec4(color, 1.0);
			}
		)";

		m_ToneMappingShader = std::make_shared<Shader>(toneMappingVertexSrc, toneMappingFragmentSrc);

		// Brightness extraction shader
		std::string brightnessFragmentSrc = R"(
			#version 330 core
			out vec4 FragColor;

			in vec2 v_TexCoord;

			uniform sampler2D u_HDRTexture;
			uniform float u_Threshold;

			void main()
			{
				vec3 color = texture(u_HDRTexture, v_TexCoord).rgb;
				float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
				
				if (brightness > u_Threshold)
				{
					FragColor = vec4(color, 1.0);
				}
				else
				{
					FragColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}
		)";

		m_BrightnessShader = std::make_shared<Shader>(toneMappingVertexSrc, brightnessFragmentSrc);

		// Bloom shader
		std::string bloomFragmentSrc = R"(
			#version 330 core
			out vec4 FragColor;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;
			uniform bool u_Horizontal;
			uniform float u_Weight[5];

			void main()
			{
				vec2 tex_offset = 1.0 / textureSize(u_Texture, 0);
				vec3 result = texture(u_Texture, v_TexCoord).rgb * u_Weight[0];
				
				if (u_Horizontal)
				{
					for (int i = 1; i < 5; ++i)
					{
						result += texture(u_Texture, v_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
						result += texture(u_Texture, v_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
					}
				}
				else
				{
					for (int i = 1; i < 5; ++i)
					{
						result += texture(u_Texture, v_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
						result += texture(u_Texture, v_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
					}
				}
				
				FragColor = vec4(result, 1.0);
			}
		)";

		m_BlurShader = std::make_shared<Shader>(toneMappingVertexSrc, bloomFragmentSrc);
	}

	void HDRToneMappingRenderer::CreateQuad()
	{
		float vertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		uint32_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_QuadVB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_QuadIB.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		BufferLayout layout = {
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		m_QuadVB->SetLayout(layout);
		m_QuadVA.reset(VertexArray::Create());
		m_QuadVA->AddVertexBuffer(m_QuadVB);
		m_QuadVA->SetIndexBuffer(m_QuadIB);
	}

	void HDRToneMappingRenderer::CreateBloomFramebuffers()
	{
		// Create brightness framebuffer
		m_BrightnessFramebuffer = std::make_shared<HDRFramebuffer>(1920, 1080); // Default resolution

		// Create bloom framebuffers (downsampled)
		for (int i = 0; i < 5; ++i)
		{
			int width = 1920 >> (i + 1);
			int height = 1080 >> (i + 1);
			auto framebuffer = std::make_shared<HDRFramebuffer>(width, height);
			m_BloomFramebuffers.push_back(framebuffer);
		}
	}

	void HDRToneMappingRenderer::RenderBrightnessPass(Ref<Texture2D> hdrTexture)
	{
		m_BrightnessFramebuffer->Bind();
		m_BrightnessShader->Bind();
		
		hdrTexture->Bind(0);
		m_BrightnessShader->UploadUniformInt("u_HDRTexture", 0);
		m_BrightnessShader->UploadUniformFloat("u_Threshold", 1.0f);
		
		m_QuadVA->Bind();
		RenderCommand::DrawIndexed(m_QuadVA);
		m_QuadVA->Unbind();
		
		m_BrightnessShader->Unbind();
		m_BrightnessFramebuffer->Unbind();
	}

	void HDRToneMappingRenderer::RenderBloomPass()
	{
		// Horizontal blur
		m_BlurShader->Bind();
		m_BlurShader->UploadUniformInt("u_Horizontal", 1);
		
		float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
		for (int i = 0; i < 5; ++i)
		{
			m_BlurShader->UploadUniformFloat("u_Weight[" + std::to_string(i) + "]", weights[i]);
		}
		
		m_BrightnessFramebuffer->BindColorTexture(0);
		m_BlurShader->UploadUniformInt("u_Texture", 0);
		
		m_QuadVA->Bind();
		RenderCommand::DrawIndexed(m_QuadVA);
		m_QuadVA->Unbind();
		
		// Vertical blur
		m_BlurShader->UploadUniformInt("u_Horizontal", 0);
		
		m_QuadVA->Bind();
		RenderCommand::DrawIndexed(m_QuadVA);
		m_QuadVA->Unbind();
		
		m_BlurShader->Unbind();
	}

	void HDRToneMappingRenderer::RenderToneMappingPass(Ref<Texture2D> hdrTexture, const HDRConfig& config)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		m_ToneMappingShader->Bind();
		
		hdrTexture->Bind(0);
		m_BrightnessFramebuffer->BindColorTexture(1);
		
		m_ToneMappingShader->UploadUniformInt("u_HDRTexture", 0);
		m_ToneMappingShader->UploadUniformInt("u_BloomTexture", 1);
		m_ToneMappingShader->UploadUniformFloat("u_Exposure", m_CurrentExposure);
		m_ToneMappingShader->UploadUniformFloat("u_Gamma", config.gamma);
		m_ToneMappingShader->UploadUniformInt("u_ToneMappingOperator", static_cast<int>(config.toneMapping));
		
		m_QuadVA->Bind();
		RenderCommand::DrawIndexed(m_QuadVA);
		m_QuadVA->Unbind();
		
		m_ToneMappingShader->Unbind();
	}

	// HDRSystem Implementation
	bool HDRSystem::s_Initialized = false;
	HDRConfig HDRSystem::s_Config;
	std::unique_ptr<HDRFramebuffer> HDRSystem::s_Framebuffer = nullptr;
	std::unique_ptr<HDRToneMappingRenderer> HDRSystem::s_Renderer = nullptr;

	void HDRSystem::Init()
	{
		if (s_Initialized)
			return;

		s_Framebuffer = std::make_unique<HDRFramebuffer>(1920, 1080);
		s_Renderer = std::make_unique<HDRToneMappingRenderer>();
		s_Renderer->Init();
		
		s_Initialized = true;
		ZG_CORE_INFO("HDR system initialized");
	}

	void HDRSystem::Shutdown()
	{
		if (!s_Initialized)
			return;

		s_Renderer.reset();
		s_Framebuffer.reset();
		
		s_Initialized = false;
		ZG_CORE_INFO("HDR system shutdown");
	}

	void HDRSystem::BeginScene()
	{
		if (s_Framebuffer)
			s_Framebuffer->Bind();
	}

	void HDRSystem::EndScene()
	{
		if (s_Framebuffer)
			s_Framebuffer->Unbind();
	}

	void HDRSystem::Render()
	{
		if (s_Renderer && s_Framebuffer)
		{
			s_Renderer->Render(s_Framebuffer->GetColorTexture(), s_Config);
		}
	}

	void HDRSystem::Resize(uint32_t width, uint32_t height)
	{
		if (s_Framebuffer)
			s_Framebuffer->Resize(width, height);
	}

	void HDRSystem::SetExposure(float exposure)
	{
		s_Config.exposure = exposure;
		if (s_Renderer)
			s_Renderer->SetExposure(exposure);
	}

	void HDRSystem::SetGamma(float gamma)
	{
		s_Config.gamma = gamma;
	}

	void HDRSystem::SetToneMapping(ToneMappingOperator op)
	{
		s_Config.toneMapping = op;
	}

	void HDRSystem::SetAutoExposure(bool enabled)
	{
		s_Config.autoExposure = enabled;
	}

	Ref<Texture2D> HDRSystem::GetHDRTexture()
	{
		return s_Framebuffer ? s_Framebuffer->GetColorTexture() : nullptr;
	}

	Ref<Texture2D> HDRSystem::GetFinalTexture()
	{
		return s_Framebuffer ? s_Framebuffer->GetColorTexture() : nullptr;
	}

	// HDRUtils Implementation
	glm::vec3 HDRUtils::LinearToneMapping(const glm::vec3& color, float exposure)
	{
		return color * exposure;
	}

	glm::vec3 HDRUtils::ReinhardToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		return mapped / (1.0f + mapped);
	}

	glm::vec3 HDRUtils::ReinhardJodieToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		glm::vec3 tc = mapped / (1.0f + mapped);
		return glm::mix(mapped / (1.0f + CalculateLuminance(mapped)), tc, tc);
	}

	glm::vec3 HDRUtils::ACESToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		float a = 2.51f;
		float b = 0.03f;
		float c = 2.43f;
		float d = 0.59f;
		float e = 0.14f;
		return glm::clamp((mapped * (a * mapped + b)) / (mapped * (c * mapped + d) + e), 0.0f, 1.0f);
	}

	glm::vec3 HDRUtils::Uncharted2ToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		
		float A = 0.15f;
		float B = 0.50f;
		float C = 0.10f;
		float D = 0.20f;
		float E = 0.02f;
		float F = 0.30f;
		
		glm::vec3 result = ((mapped * (A * mapped + C * B) + D * E) / (mapped * (A * mapped + B) + D * F)) - E / F;
		return result;
	}

	glm::vec3 HDRUtils::FilmicToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		
		glm::vec3 x = glm::max(glm::vec3(0.0f), mapped - 0.004f);
		return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
	}

	glm::vec3 HDRUtils::LottesToneMapping(const glm::vec3& color, float exposure)
	{
		glm::vec3 mapped = color * exposure;
		
		glm::vec3 a = glm::vec3(1.6f);
		glm::vec3 d = glm::vec3(0.977f);
		glm::vec3 hdrMax = glm::vec3(8.0f);
		glm::vec3 midIn = glm::vec3(0.18f);
		glm::vec3 midOut = glm::vec3(0.267f);
		
		glm::vec3 b = (-glm::pow(midIn, a) + glm::pow(hdrMax, a) * midOut) / 
		              ((glm::pow(hdrMax, a * d) - glm::pow(midIn, a * d)) * midOut);
		glm::vec3 c = (glm::pow(hdrMax, a * d) * glm::pow(midIn, a) - glm::pow(hdrMax, a) * glm::pow(midIn, a * d) * midOut) / 
		              ((glm::pow(hdrMax, a * d) - glm::pow(midIn, a * d)) * midOut);
		
		return glm::pow(mapped, a) / (glm::pow(mapped, a * d) * b + c);
	}

	glm::vec3 HDRUtils::LinearToSRGB(const glm::vec3& linear)
	{
		return glm::vec3(
			LinearToSRGB(linear.r),
			LinearToSRGB(linear.g),
			LinearToSRGB(linear.b)
		);
	}

	glm::vec3 HDRUtils::SRGBToLinear(const glm::vec3& srgb)
	{
		return glm::vec3(
			SRGBToLinear(srgb.r),
			SRGBToLinear(srgb.g),
			SRGBToLinear(srgb.b)
		);
	}

	float HDRUtils::LinearToSRGB(float linear)
	{
		if (linear <= 0.0031308f)
			return 12.92f * linear;
		else
			return 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;
	}

	float HDRUtils::SRGBToLinear(float srgb)
	{
		if (srgb <= 0.04045f)
			return srgb / 12.92f;
		else
			return std::pow((srgb + 0.055f) / 1.055f, 2.4f);
	}

	float HDRUtils::CalculateLuminance(const glm::vec3& color)
	{
		return glm::dot(color, glm::vec3(0.2126f, 0.7152f, 0.0722f));
	}

	float HDRUtils::CalculateAverageLuminance(Ref<Texture2D> texture)
	{
		// This is a simplified implementation
		// In a real implementation, you would sample multiple pixels and calculate the average
		return 0.5f; // Placeholder
	}

	float HDRUtils::CalculateAutoExposure(Ref<Texture2D> texture, float targetLuminance)
	{
		float averageLuminance = CalculateAverageLuminance(texture);
		return targetLuminance / (averageLuminance + 0.001f);
	}

	glm::vec3 HDRUtils::ApplyGammaCorrection(const glm::vec3& color, float gamma)
	{
		return glm::pow(color, glm::vec3(1.0f / gamma));
	}

	float HDRUtils::ApplyGammaCorrection(float value, float gamma)
	{
		return std::pow(value, 1.0f / gamma);
	}

	glm::vec3 HDRUtils::ApplyColorGrading(const glm::vec3& color, const glm::vec3& lift, const glm::vec3& gamma, const glm::vec3& gain)
	{
		glm::vec3 result = color;
		result = result * gain;
		result = glm::pow(result, gamma);
		result = result + lift;
		return glm::clamp(result, 0.0f, 1.0f);
	}

	glm::vec3 HDRUtils::ApplyWhiteBalance(const glm::vec3& color, float temperature, float tint)
	{
		// Simplified white balance implementation
		glm::vec3 result = color;
		result.r *= (1.0f + temperature * 0.1f);
		result.b *= (1.0f - temperature * 0.1f);
		result.g *= (1.0f + tint * 0.1f);
		return glm::clamp(result, 0.0f, 1.0f);
	}

	glm::vec3 HDRUtils::ApplyContrast(const glm::vec3& color, float contrast)
	{
		return glm::clamp((color - 0.5f) * contrast + 0.5f, 0.0f, 1.0f);
	}

	glm::vec3 HDRUtils::ApplySaturation(const glm::vec3& color, float saturation)
	{
		float luminance = CalculateLuminance(color);
		return glm::mix(glm::vec3(luminance), color, saturation);
	}

	glm::vec3 HDRUtils::ApplyBrightness(const glm::vec3& color, float brightness)
	{
		return glm::clamp(color + brightness, 0.0f, 1.0f);
	}

}
