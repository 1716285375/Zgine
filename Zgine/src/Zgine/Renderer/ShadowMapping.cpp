#include "zgpch.h"
#include "ShadowMapping.h"
#include "RenderCommand.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace Zgine {

	// ShadowMapFramebuffer Implementation
	ShadowMapFramebuffer::ShadowMapFramebuffer(uint32_t width, uint32_t height, ShadowMapType type)
		: m_Width(width), m_Height(height), m_Type(type)
	{
		CreateTexture();
	}

	ShadowMapFramebuffer::~ShadowMapFramebuffer()
	{
		glDeleteFramebuffers(1, &m_FramebufferID);
	}

	void ShadowMapFramebuffer::CreateTexture()
	{
		glGenFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		// Create shadow texture
		uint32_t textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ZG_CORE_ERROR("Shadow map framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// TODO: Create texture wrapper for existing OpenGL texture
		 //m_ShadowTexture = Texture2D::Create(textureID, m_Width, m_Height);
	}

	void ShadowMapFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glViewport(0, 0, m_Width, m_Height);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void ShadowMapFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ShadowMapFramebuffer::BindTexture(uint32_t slot)
	{
		m_ShadowTexture->Bind(slot);
	}

	void ShadowMapFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		CreateTexture();
	}

	// ShadowMapRenderer Implementation
	ShadowMapRenderer::ShadowMapRenderer()
		: m_Initialized(false)
	{
	}

	ShadowMapRenderer::~ShadowMapRenderer()
	{
		Shutdown();
	}

	void ShadowMapRenderer::Init(const ShadowMapConfig& config)
	{
		if (m_Initialized)
			return;

		m_Config = config;
		
		uint32_t resolution = static_cast<uint32_t>(config.resolution);
		m_ShadowFramebuffer = std::make_shared<ShadowMapFramebuffer>(resolution, resolution, config.type);
		
		CreateShaders();
		
		if (config.enableCascadedShadows)
		{
			CreateCascadeFramebuffers();
		}

		m_Initialized = true;
		ZG_CORE_INFO("Shadow map renderer initialized");
	}

	void ShadowMapRenderer::Shutdown()
	{
		if (!m_Initialized)
			return;

		m_ShadowFramebuffer.reset();
		m_ShadowShader.reset();
		m_DebugShader.reset();
		m_CascadeFramebuffers.clear();

		m_Initialized = false;
		ZG_CORE_INFO("Shadow map renderer shutdown");
	}

	void ShadowMapRenderer::BeginShadowPass(const ShadowLight& light, const glm::vec3& sceneCenter)
	{
		if (!m_Initialized)
			return;

		m_CurrentLight = light;
		m_LightSpaceMatrix = CalculateLightSpaceMatrix(light, sceneCenter);
		
		m_ShadowFramebuffer->Bind();
		m_ShadowShader->Bind();
		m_ShadowShader->UploadUniformMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
	}

	void ShadowMapRenderer::EndShadowPass()
	{
		if (!m_Initialized)
			return;

		m_ShadowShader->Unbind();
		m_ShadowFramebuffer->Unbind();
	}

	void ShadowMapRenderer::BeginCascadedShadowPass(const ShadowLight& light, const PerspectiveCamera& camera)
	{
		if (!m_Initialized || !m_Config.enableCascadedShadows)
			return;

		m_CurrentLight = light;
		CalculateCascadeSplits(camera);
		CalculateCascadeMatrices(light, camera);
		
		m_ShadowShader->Bind();
	}

	void ShadowMapRenderer::EndCascadedShadowPass()
	{
		if (!m_Initialized)
			return;

		m_ShadowShader->Unbind();
	}

	void ShadowMapRenderer::CreateShaders()
	{
		// Shadow mapping shader
		std::string shadowVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec2 a_TexCoord;

			uniform mat4 u_LightSpaceMatrix;
			uniform mat4 u_Model;

			void main()
			{
				gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
			}
		)";

		std::string shadowFragmentSrc = R"(
			#version 330 core
			void main()
			{
				// Empty fragment shader for depth-only rendering
			}
		)";

		m_ShadowShader = std::make_shared<Shader>(shadowVertexSrc, shadowFragmentSrc);

		// Debug shader
		std::string debugVertexSrc = R"(
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

		std::string debugFragmentSrc = R"(
			#version 330 core
			out vec4 FragColor;

			in vec2 v_TexCoord;

			uniform sampler2D u_ShadowMap;

			void main()
			{
				float depth = texture(u_ShadowMap, v_TexCoord).r;
				FragColor = vec4(vec3(depth), 1.0);
			}
		)";

		m_DebugShader = std::make_shared<Shader>(debugVertexSrc, debugFragmentSrc);
	}

	void ShadowMapRenderer::CreateCascadeFramebuffers()
	{
		uint32_t resolution = static_cast<uint32_t>(m_Config.resolution);
		
		for (int i = 0; i < m_Config.cascadeCount; ++i)
		{
			auto framebuffer = std::make_shared<ShadowMapFramebuffer>(resolution, resolution, ShadowMapType::Directional);
			m_CascadeFramebuffers.push_back(framebuffer);
		}
	}

	void ShadowMapRenderer::CalculateCascadeSplits(const PerspectiveCamera& camera)
	{
		m_CascadeDistances.clear();
		m_CascadeDistances.resize(m_Config.cascadeCount + 1);

		float nearPlane = camera.GetNearPlane();
		float farPlane = camera.GetFarPlane();
		float lambda = m_Config.cascadeSplitLambda;

		for (int i = 0; i <= m_Config.cascadeCount; ++i)
		{
			float p = static_cast<float>(i) / static_cast<float>(m_Config.cascadeCount);
			float logSplit = nearPlane * std::pow(farPlane / nearPlane, p);
			float uniformSplit = nearPlane + (farPlane - nearPlane) * p;
			
			m_CascadeDistances[i] = glm::mix(uniformSplit, logSplit, lambda);
		}
	}

	void ShadowMapRenderer::CalculateCascadeMatrices(const ShadowLight& light, const PerspectiveCamera& camera)
	{
		m_CascadeMatrices.clear();
		m_CascadeMatrices.resize(m_Config.cascadeCount);

		for (int i = 0; i < m_Config.cascadeCount; ++i)
		{
			float nearPlane = m_CascadeDistances[i];
			float farPlane = m_CascadeDistances[i + 1];

			// Calculate camera frustum corners for this cascade
			glm::mat4 invViewProj = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix());
			
			std::vector<glm::vec4> frustumCorners = {
				{-1.0f,  1.0f, -1.0f, 1.0f},
				{ 1.0f,  1.0f, -1.0f, 1.0f},
				{ 1.0f, -1.0f, -1.0f, 1.0f},
				{-1.0f, -1.0f, -1.0f, 1.0f},
				{-1.0f,  1.0f,  1.0f, 1.0f},
				{ 1.0f,  1.0f,  1.0f, 1.0f},
				{ 1.0f, -1.0f,  1.0f, 1.0f},
				{-1.0f, -1.0f,  1.0f, 1.0f}
			};

			glm::vec3 center = glm::vec3(0.0f);
			for (auto& corner : frustumCorners)
			{
				glm::vec4 invCorner = invViewProj * corner;
				invCorner /= invCorner.w;
				center += glm::vec3(invCorner);
			}
			center /= 8.0f;

			m_CascadeMatrices[i] = CalculateLightSpaceMatrix(light, center);
		}
	}

	glm::mat4 ShadowMapRenderer::CalculateLightSpaceMatrix(const ShadowLight& light, const glm::vec3& sceneCenter)
	{
		glm::vec3 lightPos = light.position;
		glm::vec3 lightDir = glm::normalize(light.direction);
		
		// Calculate light view matrix
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
		
		// Calculate light projection matrix (orthographic for directional lights)
		float orthoSize = 50.0f; // Adjust based on scene size
		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, m_Config.nearPlane, m_Config.farPlane);
		
		m_LightProjectionMatrix = lightProjection;
		m_LightViewMatrix = lightView;
		
		return lightProjection * lightView;
	}

	void ShadowMapRenderer::SetConfig(const ShadowMapConfig& config)
	{
		m_Config = config;
		if (m_Initialized)
		{
			Shutdown();
			Init(config);
		}
	}

	void ShadowMapRenderer::SetLight(const ShadowLight& light)
	{
		m_CurrentLight = light;
	}

	// ShadowMappingSystem Implementation
	bool ShadowMappingSystem::s_Initialized = false;
	bool ShadowMappingSystem::s_DebugEnabled = false;
	ShadowMapConfig ShadowMappingSystem::s_Config;
	std::unique_ptr<ShadowMapRenderer> ShadowMappingSystem::s_Renderer = nullptr;
	std::vector<ShadowLight> ShadowMappingSystem::s_Lights;

	void ShadowMappingSystem::Init()
	{
		if (s_Initialized)
			return;

		s_Renderer = std::make_unique<ShadowMapRenderer>();
		s_Renderer->Init(s_Config);
		
		s_Initialized = true;
		ZG_CORE_INFO("Shadow mapping system initialized");
	}

	void ShadowMappingSystem::Shutdown()
	{
		if (!s_Initialized)
			return;

		s_Renderer.reset();
		s_Lights.clear();
		
		s_Initialized = false;
		ZG_CORE_INFO("Shadow mapping system shutdown");
	}

	void ShadowMappingSystem::BeginScene()
	{
		if (!s_Initialized)
			return;

		// Begin shadow pass for each light
		for (const auto& light : s_Lights)
		{
			if (light.castShadows)
			{
				s_Renderer->BeginShadowPass(light, glm::vec3(0.0f));
			}
		}
	}

	void ShadowMappingSystem::EndScene()
	{
		if (!s_Initialized)
			return;

		s_Renderer->EndShadowPass();
	}

	void ShadowMappingSystem::AddLight(const ShadowLight& light)
	{
		s_Lights.push_back(light);
	}

	void ShadowMappingSystem::RemoveLight(uint32_t index)
	{
		if (index < s_Lights.size())
		{
			s_Lights.erase(s_Lights.begin() + index);
		}
	}

	void ShadowMappingSystem::ClearLights()
	{
		s_Lights.clear();
	}

	void ShadowMappingSystem::RenderShadows()
	{
		if (!s_Initialized)
			return;

		// Shadow rendering is handled in BeginScene/EndScene
	}

	void ShadowMappingSystem::RenderDebug()
	{
		if (!s_Initialized || !s_DebugEnabled)
			return;

		// Render shadow map debug quad
		// Implementation would go here
	}

	void ShadowMappingSystem::SetConfig(const ShadowMapConfig& config)
	{
		s_Config = config;
		if (s_Renderer)
		{
			s_Renderer->SetConfig(config);
		}
	}

	void ShadowMappingSystem::SetLight(uint32_t index, const ShadowLight& light)
	{
		if (index < s_Lights.size())
		{
			s_Lights[index] = light;
		}
	}

	// ShadowUtils Implementation
	glm::mat4 ShadowUtils::CalculateDirectionalLightMatrix(const glm::vec3& direction, const glm::vec3& center, float radius)
	{
		glm::vec3 lightPos = center - direction * radius;
		glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-radius, radius, -radius, radius, 0.1f, radius * 2.0f);
		
		return lightProjection * lightView;
	}

	glm::mat4 ShadowUtils::CalculatePointLightMatrix(const glm::vec3& position, float nearPlane, float farPlane)
	{
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
		return lightProjection;
	}

	glm::mat4 ShadowUtils::CalculateSpotLightMatrix(const glm::vec3& position, const glm::vec3& direction, float fov, float nearPlane, float farPlane)
	{
		glm::mat4 lightView = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::perspective(fov, 1.0f, nearPlane, farPlane);
		
		return lightProjection * lightView;
	}

	float ShadowUtils::SampleShadowMap(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare)
	{
		// TODO: Implement GetPixel method for Texture2D
		// float depth = shadowMap->GetPixel(coords).r;
		float depth = 0.0f; // Placeholder
		return compare < depth ? 1.0f : 0.0f;
	}

	float ShadowUtils::SampleShadowMapPCF(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare, float texelSize)
	{
		float shadow = 0.0f;
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				glm::vec2 offset = glm::vec2(x, y) * texelSize;
				// TODO: Implement GetPixel method for Texture2D
				// float depth = shadowMap->GetPixel(coords + offset).r;
				float depth = 0.0f; // Placeholder
				shadow += compare < depth ? 1.0f : 0.0f;
			}
		}
		return shadow / 9.0f;
	}

	float ShadowUtils::SampleShadowMapPCSS(Ref<Texture2D> shadowMap, const glm::vec2& coords, float compare, float texelSize, float lightRadius)
	{
		// Percentage Closer Soft Shadows implementation
		// This is a simplified version
		return SampleShadowMapPCF(shadowMap, coords, compare, texelSize);
	}

	float ShadowUtils::CalculateShadowBias(const glm::vec3& lightDir, const glm::vec3& normal, float bias, float normalBias)
	{
		float cosTheta = glm::dot(normal, -lightDir);
		return bias * (1.0f - cosTheta) + normalBias;
	}

	glm::vec3 ShadowUtils::CalculateNormalBias(const glm::vec3& normal, float normalBias)
	{
		return normal * normalBias;
	}

	void ShadowUtils::RenderShadowMapDebug(Ref<Texture2D> shadowMap, const glm::vec2& position, const glm::vec2& size)
	{
		// Debug rendering implementation would go here
	}

}
