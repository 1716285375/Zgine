#include "zgpch.h"
#include "PostProcessing.h"
#include "RenderCommand.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace Zgine {

	// PostProcessingFramebuffer Implementation
	PostProcessingFramebuffer::PostProcessingFramebuffer(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		// Create framebuffer
		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		// Create color texture
		m_ColorTexture = Texture2D::Create(width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture->GetRendererID(), 0);

		// Create depth texture
		// TODO: Create depth texture with specific format
		// m_DepthTexture = Texture2D::Create(width, height, GL_DEPTH_COMPONENT24);
		m_DepthTexture = Texture2D::Create(width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->GetRendererID(), 0);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ZG_CORE_ERROR("Post-processing framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	PostProcessingFramebuffer::~PostProcessingFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
		glDeleteRenderbuffers(1, &m_RenderbufferID);
	}

	void PostProcessingFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		// Recreate textures with new size
		m_ColorTexture = Texture2D::Create(width, height);
		// TODO: Create depth texture with specific format
		// m_DepthTexture = Texture2D::Create(width, height, GL_DEPTH_COMPONENT24);
		m_DepthTexture = Texture2D::Create(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture->GetRendererID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->GetRendererID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PostProcessingFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Width, m_Height);
	}

	void PostProcessingFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PostProcessingFramebuffer::BindColorTexture(uint32_t slot)
	{
		m_ColorTexture->Bind(slot);
	}

	void PostProcessingFramebuffer::BindDepthTexture(uint32_t slot)
	{
		m_DepthTexture->Bind(slot);
	}

	// PostEffectShader Implementation
	PostEffectShader::PostEffectShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		m_Shader = std::make_shared<Shader>(vertexSrc, fragmentSrc);
	}

	PostEffectShader::~PostEffectShader()
	{
	}

	void PostEffectShader::Bind() const
	{
		m_Shader->Bind();
	}

	void PostEffectShader::Unbind() const
	{
		m_Shader->Unbind();
	}

	void PostEffectShader::SetUniform(const std::string& name, float value)
	{
		m_Shader->UploadUniformFloat(name, value);
	}

	void PostEffectShader::SetUniform(const std::string& name, int value)
	{
		m_Shader->UploadUniformInt(name, value);
	}

	void PostEffectShader::SetUniform(const std::string& name, const glm::vec2& value)
	{
		m_Shader->UploadUniformFloat2(name, value);
	}

	void PostEffectShader::SetUniform(const std::string& name, const glm::vec3& value)
	{
		m_Shader->UploadUniformFloat3(name, value);
	}

	void PostEffectShader::SetUniform(const std::string& name, const glm::vec4& value)
	{
		m_Shader->UploadUniformFloat4(name, value);
	}

	void PostEffectShader::SetUniform(const std::string& name, const glm::mat4& value)
	{
		m_Shader->UploadUniformMat4(name, value);
	}

	// PostEffect Implementation
	PostEffect::PostEffect(PostEffectType type, const std::string& name)
		: m_Type(type), m_Name(name), m_Initialized(false)
	{
	}

	PostEffect::~PostEffect()
	{
		Shutdown();
	}

	void PostEffect::Init()
	{
		if (m_Initialized)
			return;

		CreateQuad();
		
		std::string vertexSrc = GetVertexShaderSource();
		std::string fragmentSrc = GetFragmentShaderSource();
		
		m_Shader = std::make_shared<PostEffectShader>(m_Name, vertexSrc, fragmentSrc);
		
		m_Initialized = true;
		ZG_CORE_INFO("Post-effect '{0}' initialized", m_Name);
	}

	void PostEffect::Shutdown()
	{
		if (!m_Initialized)
			return;

		m_Shader.reset();
		m_QuadVA.reset();
		m_QuadVB.reset();
		m_QuadIB.reset();
		
		m_Initialized = false;
		ZG_CORE_INFO("Post-effect '{0}' shutdown", m_Name);
	}

	void PostEffect::Render(Ref<Texture2D> inputTexture, Ref<Texture2D> outputTexture, const PostEffectParams& params)
	{
		if (!m_Initialized || !params.enabled)
			return;

		m_Shader->Bind();
		
		// Set uniforms based on effect type
		switch (m_Type)
		{
		case PostEffectType::Bloom:
			m_Shader->SetUniform("u_Threshold", params.threshold);
			m_Shader->SetUniform("u_Intensity", params.intensity);
			break;
		case PostEffectType::Blur:
			m_Shader->SetUniform("u_Radius", params.radius);
			m_Shader->SetUniform("u_Intensity", params.intensity);
			break;
		case PostEffectType::Vignette:
			m_Shader->SetUniform("u_VignetteStrength", params.vignetteStrength);
			m_Shader->SetUniform("u_VignetteRadius", params.vignetteRadius);
			break;
		case PostEffectType::ChromaticAberration:
			m_Shader->SetUniform("u_ChromaticAberrationStrength", params.chromaticAberrationStrength);
			break;
		case PostEffectType::FilmGrain:
			m_Shader->SetUniform("u_FilmGrainStrength", params.filmGrainStrength);
			break;
		case PostEffectType::MotionBlur:
			m_Shader->SetUniform("u_MotionBlurStrength", params.motionBlurStrength);
			break;
		case PostEffectType::DepthOfField:
			m_Shader->SetUniform("u_Focus", params.depthOfFieldFocus);
			m_Shader->SetUniform("u_Range", params.depthOfFieldRange);
			break;
		case PostEffectType::SSAO:
			m_Shader->SetUniform("u_Radius", params.ssaoRadius);
			m_Shader->SetUniform("u_Bias", params.ssaoBias);
			m_Shader->SetUniform("u_Samples", params.ssaoSamples);
			break;
		default:
			m_Shader->SetUniform("u_Intensity", params.intensity);
			break;
		}

		// Bind input texture
		inputTexture->Bind(0);
		m_Shader->SetUniform("u_Texture", 0);

		// Render fullscreen quad
		m_QuadVA->Bind();
		RenderCommand::DrawIndexed(m_QuadVA);
		m_QuadVA->Unbind();

		m_Shader->Unbind();
	}

	void PostEffect::CreateQuad()
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

	std::string PostEffect::GetVertexShaderSource() const
	{
		return R"(
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
	}

	std::string PostEffect::GetFragmentShaderSource() const
	{
		switch (m_Type)
		{
		case PostEffectType::Bloom:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Threshold;
				uniform float u_Intensity;

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					
					// Extract bright areas
					float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
					if (brightness > u_Threshold)
					{
						FragColor = vec4(color * u_Intensity, 1.0);
					}
					else
					{
						FragColor = vec4(0.0, 0.0, 0.0, 1.0);
					}
				}
			)";

		case PostEffectType::Blur:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Radius;
				uniform float u_Intensity;
				uniform vec2 u_Resolution;

				void main()
				{
					vec2 texelSize = 1.0 / u_Resolution;
					vec3 color = vec3(0.0);
					float total = 0.0;

					for (int x = -2; x <= 2; x++)
					{
						for (int y = -2; y <= 2; y++)
						{
							vec2 offset = vec2(x, y) * texelSize * u_Radius;
							float weight = 1.0 - length(vec2(x, y)) / 3.0;
							color += texture(u_Texture, v_TexCoord + offset).rgb * weight;
							total += weight;
						}
					}

					FragColor = vec4(color / total * u_Intensity, 1.0);
				}
			)";

		case PostEffectType::Vignette:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_VignetteStrength;
				uniform float u_VignetteRadius;

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					
					vec2 center = vec2(0.5, 0.5);
					float dist = distance(v_TexCoord, center);
					float vignette = 1.0 - smoothstep(u_VignetteRadius, 1.0, dist);
					
					FragColor = vec4(color * vignette, 1.0);
				}
			)";

		case PostEffectType::ChromaticAberration:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_ChromaticAberrationStrength;

				void main()
				{
					vec2 center = vec2(0.5, 0.5);
					vec2 offset = (v_TexCoord - center) * u_ChromaticAberrationStrength;
					
					float r = texture(u_Texture, v_TexCoord + offset).r;
					float g = texture(u_Texture, v_TexCoord).g;
					float b = texture(u_Texture, v_TexCoord - offset).b;
					
					FragColor = vec4(r, g, b, 1.0);
				}
			)";

		case PostEffectType::FilmGrain:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_FilmGrainStrength;
				uniform float u_Time;

				float random(vec2 st)
				{
					return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
				}

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					
					float noise = random(v_TexCoord + u_Time) * u_FilmGrainStrength;
					color += noise - u_FilmGrainStrength * 0.5;
					
					FragColor = vec4(color, 1.0);
				}
			)";

		case PostEffectType::GrayScale:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Intensity;

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					float gray = dot(color, vec3(0.299, 0.587, 0.114));
					vec3 grayColor = vec3(gray);
					
					FragColor = vec4(mix(color, grayColor, u_Intensity), 1.0);
				}
			)";

		case PostEffectType::Sepia:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Intensity;

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					
					vec3 sepia = vec3(
						dot(color, vec3(0.393, 0.769, 0.189)),
						dot(color, vec3(0.349, 0.686, 0.168)),
						dot(color, vec3(0.272, 0.534, 0.131))
					);
					
					FragColor = vec4(mix(color, sepia, u_Intensity), 1.0);
				}
			)";

		case PostEffectType::Invert:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Intensity;

				void main()
				{
					vec3 color = texture(u_Texture, v_TexCoord).rgb;
					vec3 inverted = 1.0 - color;
					
					FragColor = vec4(mix(color, inverted, u_Intensity), 1.0);
				}
			)";

		default:
			return R"(
				#version 330 core
				out vec4 FragColor;

				in vec2 v_TexCoord;

				uniform sampler2D u_Texture;
				uniform float u_Intensity;

				void main()
				{
					FragColor = texture(u_Texture, v_TexCoord);
				}
			)";
		}
	}

	// PostProcessingPipeline Implementation
	PostProcessingPipeline::PostProcessingPipeline()
		: m_Initialized(false), m_Enabled(true), m_Width(0), m_Height(0)
	{
	}

	PostProcessingPipeline::~PostProcessingPipeline()
	{
		Shutdown();
	}

	void PostProcessingPipeline::Init(uint32_t width, uint32_t height)
	{
		if (m_Initialized)
			return;

		m_Width = width;
		m_Height = height;

		CreateFullscreenQuad();
		CreateFramebuffers();

		m_Initialized = true;
		ZG_CORE_INFO("Post-processing pipeline initialized ({0}x{1})", width, height);
	}

	void PostProcessingPipeline::Shutdown()
	{
		if (!m_Initialized)
			return;

		for (auto& effect : m_Effects)
		{
			effect->Shutdown();
		}
		m_Effects.clear();

		m_Framebuffers.clear();
		m_FullscreenQuad.reset();
		m_FullscreenQuadVB.reset();
		m_FullscreenQuadIB.reset();

		m_Initialized = false;
		ZG_CORE_INFO("Post-processing pipeline shutdown");
	}

	void PostProcessingPipeline::Resize(uint32_t width, uint32_t height)
	{
		if (!m_Initialized)
			return;

		m_Width = width;
		m_Height = height;

		for (auto& framebuffer : m_Framebuffers)
		{
			framebuffer->Resize(width, height);
		}

		ZG_CORE_INFO("Post-processing pipeline resized to {0}x{1}", width, height);
	}

	void PostProcessingPipeline::BeginScene()
	{
		if (!m_Initialized || !m_Enabled)
			return;

		// Bind first framebuffer for scene rendering
		if (!m_Framebuffers.empty())
		{
			m_Framebuffers[0]->Bind();
		}
	}

	void PostProcessingPipeline::EndScene()
	{
		if (!m_Initialized || !m_Enabled)
			return;

		// Unbind framebuffer
		if (!m_Framebuffers.empty())
		{
			m_Framebuffers[0]->Unbind();
		}
	}

	void PostProcessingPipeline::Render()
	{
		if (!m_Initialized || !m_Enabled || m_Effects.empty())
			return;

		// Render effects in sequence
		Ref<Texture2D> currentTexture = m_Framebuffers[0]->GetColorTexture();
		
		for (size_t i = 0; i < m_Effects.size(); ++i)
		{
			Ref<PostEffect> effect = m_Effects[i];
			if (!effect->IsEnabled())
				continue;

			Ref<Texture2D> outputTexture;
			if (i + 1 < m_Framebuffers.size())
			{
				outputTexture = m_Framebuffers[i + 1]->GetColorTexture();
				m_Framebuffers[i + 1]->Bind();
			}
			else
			{
				// Last effect, render to screen
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			RenderEffect(effect, currentTexture, outputTexture);
			currentTexture = outputTexture;
		}
	}

	void PostProcessingPipeline::AddEffect(PostEffectType type, const std::string& name)
	{
		auto effect = std::make_shared<PostEffect>(type, name);
		effect->Init();
		m_Effects.push_back(effect);
		
		// Create additional framebuffer if needed
		if (m_Framebuffers.size() < m_Effects.size() + 1)
		{
			auto framebuffer = std::make_shared<PostProcessingFramebuffer>(m_Width, m_Height);
			m_Framebuffers.push_back(framebuffer);
		}
	}

	void PostProcessingPipeline::RemoveEffect(PostEffectType type)
	{
		auto it = std::find_if(m_Effects.begin(), m_Effects.end(),
			[type](const Ref<PostEffect>& effect) { return effect->GetType() == type; });
		
		if (it != m_Effects.end())
		{
			(*it)->Shutdown();
			m_Effects.erase(it);
		}
	}

	void PostProcessingPipeline::ClearEffects()
	{
		for (auto& effect : m_Effects)
		{
			effect->Shutdown();
		}
		m_Effects.clear();
	}

	PostEffect* PostProcessingPipeline::GetEffect(PostEffectType type)
	{
		auto it = std::find_if(m_Effects.begin(), m_Effects.end(),
			[type](const Ref<PostEffect>& effect) { return effect->GetType() == type; });
		
		return (it != m_Effects.end()) ? it->get() : nullptr;
	}

	Ref<Texture2D> PostProcessingPipeline::GetFinalTexture() const
	{
		if (m_Framebuffers.empty())
			return nullptr;
		return m_Framebuffers.back()->GetColorTexture();
	}

	Ref<Texture2D> PostProcessingPipeline::GetColorTexture() const
	{
		if (m_Framebuffers.empty())
			return nullptr;
		return m_Framebuffers[0]->GetColorTexture();
	}

	Ref<Texture2D> PostProcessingPipeline::GetDepthTexture() const
	{
		if (m_Framebuffers.empty())
			return nullptr;
		return m_Framebuffers[0]->GetDepthTexture();
	}

	void PostProcessingPipeline::CreateFullscreenQuad()
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

		m_FullscreenQuadVB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_FullscreenQuadIB.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		BufferLayout layout = {
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		m_FullscreenQuadVB->SetLayout(layout);
		m_FullscreenQuad.reset(VertexArray::Create());
		m_FullscreenQuad->AddVertexBuffer(m_FullscreenQuadVB);
		m_FullscreenQuad->SetIndexBuffer(m_FullscreenQuadIB);
	}

	void PostProcessingPipeline::CreateFramebuffers()
	{
		// Create initial framebuffer for scene rendering
		auto framebuffer = std::make_shared<PostProcessingFramebuffer>(m_Width, m_Height);
		m_Framebuffers.push_back(framebuffer);
	}

	void PostProcessingPipeline::RenderEffect(Ref<PostEffect> effect, Ref<Texture2D> input, Ref<Texture2D> output)
	{
		effect->Render(input, output, effect->GetParams());
	}

	// PostProcessingManager Implementation
	bool PostProcessingManager::s_Initialized = false;
	std::unique_ptr<PostProcessingPipeline> PostProcessingManager::s_Pipeline = nullptr;

	void PostProcessingManager::Init()
	{
		if (s_Initialized)
			return;

		s_Pipeline = std::make_unique<PostProcessingPipeline>();
		s_Initialized = true;
		
		ZG_CORE_INFO("Post-processing manager initialized");
	}

	void PostProcessingManager::Shutdown()
	{
		if (!s_Initialized)
			return;

		s_Pipeline.reset();
		s_Initialized = false;
		
		ZG_CORE_INFO("Post-processing manager shutdown");
	}

	void PostProcessingManager::BeginScene()
	{
		if (s_Pipeline)
			s_Pipeline->BeginScene();
	}

	void PostProcessingManager::EndScene()
	{
		if (s_Pipeline)
			s_Pipeline->EndScene();
	}

	void PostProcessingManager::Render()
	{
		if (s_Pipeline)
			s_Pipeline->Render();
	}

	void PostProcessingManager::Resize(uint32_t width, uint32_t height)
	{
		if (s_Pipeline)
			s_Pipeline->Resize(width, height);
	}

	void PostProcessingManager::EnableEffect(PostEffectType type)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->SetEnabled(true);
		}
	}

	void PostProcessingManager::DisableEffect(PostEffectType type)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->SetEnabled(false);
		}
	}

	void PostProcessingManager::ToggleEffect(PostEffectType type)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->SetEnabled(!effect->IsEnabled());
		}
	}

	bool PostProcessingManager::IsEffectEnabled(PostEffectType type)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			return effect ? effect->IsEnabled() : false;
		}
		return false;
	}

	void PostProcessingManager::SetEffectIntensity(PostEffectType type, float intensity)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->GetParams().intensity = intensity;
		}
	}

	void PostProcessingManager::SetEffectThreshold(PostEffectType type, float threshold)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->GetParams().threshold = threshold;
		}
	}

	void PostProcessingManager::SetEffectRadius(PostEffectType type, float radius)
	{
		if (s_Pipeline)
		{
			PostEffect* effect = s_Pipeline->GetEffect(type);
			if (effect)
				effect->GetParams().radius = radius;
		}
	}

}
