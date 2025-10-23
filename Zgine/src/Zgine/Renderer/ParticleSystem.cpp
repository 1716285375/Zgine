#include "zgpch.h"
#include "ParticleSystem.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <algorithm>

#include "Zgine/Renderer/RenderCommand.h"
#include "Zgine/Renderer/RendererAPI.h"

namespace Zgine {

	// Static members for ParticleSystemManager
	std::vector<Ref<ParticleSystem>> ParticleSystemManager::s_ParticleSystems;
	bool ParticleSystemManager::s_Initialized = false;

	ParticleSystem::ParticleSystem(const ParticleEmitterConfig& config)
		: m_Config(config)
	{
		Init();
	}

	ParticleSystem::~ParticleSystem()
	{
		// Cleanup is handled by smart pointers
	}

	void ParticleSystem::Init()
	{
		// Initialize particles
		m_Particles.resize(m_Config.MaxParticles);
		for (auto& particle : m_Particles)
		{
			ResetParticle(particle);
		}

		// Create vertex array
		m_VertexArray = VertexArray::Create();

		// Create vertex buffer for particle quads
		// Each particle is rendered as a quad with 4 vertices
		m_VertexBuffer = VertexBuffer::Create(nullptr, m_Config.MaxParticles * 4 * sizeof(QuadVertex));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" }
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		// Create indices for particle quads
		std::vector<uint32_t> indices(m_Config.MaxParticles * 6);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_Config.MaxParticles; i++)
		{
			// Each particle quad uses 6 indices (2 triangles)
			indices[i * 6 + 0] = offset + 0;
			indices[i * 6 + 1] = offset + 1;
			indices[i * 6 + 2] = offset + 2;

			indices[i * 6 + 3] = offset + 2;
			indices[i * 6 + 4] = offset + 3;
			indices[i * 6 + 5] = offset + 0;

			offset += 4; // Each quad has 4 vertices
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Create particle shader
		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			layout(location = 2) in vec2 a_TexCoord;
			layout(location = 3) in float a_TexIndex;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;
			out vec2 v_TexCoord;
			out float v_TexIndex;

			void main()
			{
				v_Color = a_Color;
				v_TexCoord = a_TexCoord;
				v_TexIndex = a_TexIndex;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec4 v_Color;
			in vec2 v_TexCoord;
			in float v_TexIndex;

			uniform sampler2D u_Textures[32];

			void main()
			{
				vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
				color = v_Color * texColor;
			}
		)";

		m_Shader = CreateRef<Shader>(vertexSrc, fragmentSrc);
		m_Shader->Bind();

		// Set up texture samplers
		int32_t samplers[32];
		for (uint32_t i = 0; i < 32; i++)
			samplers[i] = i;
		m_Shader->SetIntArray("u_Textures", samplers, 32);
	}

	void ParticleSystem::OnUpdate(float deltaTime)
	{
		// Update emission timer
		if (m_Emitting)
		{
			m_EmissionTimer += deltaTime;
			float emissionInterval = 1.0f / m_Config.EmissionRate;
			
			while (m_EmissionTimer >= emissionInterval)
			{
				EmitParticle();
				m_EmissionTimer -= emissionInterval;
			}
		}

		// Update all active particles
		m_ActiveParticleCount = 0;
		for (auto& particle : m_Particles)
		{
			if (particle.Active)
			{
				UpdateParticle(particle, deltaTime);
				m_ActiveParticleCount++;
			}
		}
	}

	void ParticleSystem::OnRender(const glm::mat4& viewProjection)
	{
		if (m_ActiveParticleCount == 0)
			return;

		// Bind shader and set uniforms
		m_Shader->Bind();
		m_Shader->SetMat4("u_ViewProjection", viewProjection);

		// Bind texture if available
		if (m_Config.Texture)
		{
			m_Config.Texture->Bind(0);
		}

		// Prepare vertex data
		QuadVertex* vertexBuffer = new QuadVertex[m_ActiveParticleCount * 4];
		QuadVertex* vertexBufferPtr = vertexBuffer;

		// Fill vertex buffer with active particles
		for (const auto& particle : m_Particles)
		{
			if (!particle.Active)
				continue;

			// Calculate quad vertices for this particle
			glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f) * particle.Size.x * 0.5f;
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f) * particle.Size.y * 0.5f;

			// Rotate the quad if needed
			if (particle.Rotation != 0.0f)
			{
				float cosR = cos(particle.Rotation);
				float sinR = sin(particle.Rotation);
				right = glm::vec3(right.x * cosR - right.y * sinR, right.x * sinR + right.y * cosR, right.z);
				up = glm::vec3(up.x * cosR - up.y * sinR, up.x * sinR + up.y * cosR, up.z);
			}

			// Calculate quad corners
			glm::vec3 bottomLeft = particle.Position - right - up;
			glm::vec3 bottomRight = particle.Position + right - up;
			glm::vec3 topRight = particle.Position + right + up;
			glm::vec3 topLeft = particle.Position - right + up;

			// Add vertices to buffer
			vertexBufferPtr->Position = bottomLeft;
			vertexBufferPtr->Color = particle.Color;
			vertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			vertexBufferPtr->TexIndex = 0.0f;
			vertexBufferPtr++;

			vertexBufferPtr->Position = bottomRight;
			vertexBufferPtr->Color = particle.Color;
			vertexBufferPtr->TexCoord = { 1.0f, 0.0f };
			vertexBufferPtr->TexIndex = 0.0f;
			vertexBufferPtr++;

			vertexBufferPtr->Position = topRight;
			vertexBufferPtr->Color = particle.Color;
			vertexBufferPtr->TexCoord = { 1.0f, 1.0f };
			vertexBufferPtr->TexIndex = 0.0f;
			vertexBufferPtr++;

			vertexBufferPtr->Position = topLeft;
			vertexBufferPtr->Color = particle.Color;
			vertexBufferPtr->TexCoord = { 0.0f, 1.0f };
			vertexBufferPtr->TexIndex = 0.0f;
			vertexBufferPtr++;
		}

		// Update vertex buffer
		m_VertexBuffer->SetData(vertexBuffer, m_ActiveParticleCount * 4 * sizeof(QuadVertex));

		// Render
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray, m_ActiveParticleCount * 6);

		// Cleanup
		delete[] vertexBuffer;
	}

	void ParticleSystem::EmitBurst(int count)
	{
		for (int i = 0; i < count; i++)
		{
			EmitParticle();
		}
	}

	void ParticleSystem::StartEmission()
	{
		m_Emitting = true;
		m_EmissionTimer = 0.0f;
	}

	void ParticleSystem::StopEmission()
	{
		m_Emitting = false;
	}

	int ParticleSystem::GetActiveParticleCount() const
	{
		return m_ActiveParticleCount;
	}

	void ParticleSystem::EmitParticle()
	{
		// Find an inactive particle
		Particle* particle = nullptr;
		for (int i = 0; i < m_Config.MaxParticles; i++)
		{
			int index = (m_NextParticleIndex + i) % m_Config.MaxParticles;
			if (!m_Particles[index].Active)
			{
				particle = &m_Particles[index];
				m_NextParticleIndex = (index + 1) % m_Config.MaxParticles;
				break;
			}
		}

		if (!particle)
			return; // No available particles

		// Initialize particle
		particle->Position = m_Config.Position;
		particle->Velocity = RandomVector(m_Config.VelocityMin, m_Config.VelocityMax);
		particle->Acceleration = RandomVector(m_Config.AccelerationMin, m_Config.AccelerationMax);
		particle->Color = m_Config.ColorStart;
		particle->Size = m_Config.SizeStart;
		particle->Rotation = RandomFloat(m_Config.RotationMin, m_Config.RotationMax);
		particle->RotationSpeed = RandomFloat(m_Config.RotationSpeedMin, m_Config.RotationSpeedMax);
		particle->MaxLife = RandomFloat(m_Config.LifeMin, m_Config.LifeMax);
		particle->Life = particle->MaxLife;
		particle->Age = 0.0f;
		particle->Active = true;

		m_TotalEmittedParticles++;
	}

	void ParticleSystem::UpdateParticle(Particle& particle, float deltaTime)
	{
		if (!particle.Active)
			return;

		// Update age and life
		particle.Age += deltaTime;
		particle.Life = particle.MaxLife - particle.Age;

		// Check if particle should die
		if (particle.Life <= 0.0f)
		{
			ResetParticle(particle);
			return;
		}

		// Update physics
		particle.Velocity += particle.Acceleration * deltaTime;
		particle.Position += particle.Velocity * deltaTime;
		particle.Rotation += particle.RotationSpeed * deltaTime;

		// Update visual properties based on life
		float lifeRatio = particle.Age / particle.MaxLife;
		
		// Interpolate color
		particle.Color = Lerp(m_Config.ColorStart, m_Config.ColorEnd, lifeRatio);
		
		// Interpolate size
		particle.Size = Lerp(m_Config.SizeStart, m_Config.SizeEnd, lifeRatio);

		// Apply custom update function if provided
		if (m_Config.CustomUpdate)
		{
			m_Config.CustomUpdate(particle, deltaTime);
		}
	}

	void ParticleSystem::ResetParticle(Particle& particle)
	{
		particle.Active = false;
		particle.Position = { 0.0f, 0.0f, 0.0f };
		particle.Velocity = { 0.0f, 0.0f, 0.0f };
		particle.Acceleration = { 0.0f, 0.0f, 0.0f };
		particle.Color = { 0.0f, 0.0f, 0.0f, 0.0f };
		particle.Size = { 0.0f, 0.0f };
		particle.Rotation = 0.0f;
		particle.RotationSpeed = 0.0f;
		particle.Life = 0.0f;
		particle.MaxLife = 0.0f;
		particle.Age = 0.0f;
	}

	float ParticleSystem::RandomFloat(float min, float max)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(min, max);
		return dis(gen);
	}

	glm::vec3 ParticleSystem::RandomVector(const glm::vec3& min, const glm::vec3& max)
	{
		return glm::vec3(
			RandomFloat(min.x, max.x),
			RandomFloat(min.y, max.y),
			RandomFloat(min.z, max.z)
		);
	}

	// ParticleSystemManager implementation
	void ParticleSystemManager::Init()
	{
		if (s_Initialized)
			return;

		s_ParticleSystems.clear();
		s_Initialized = true;
	}

	void ParticleSystemManager::Shutdown()
	{
		s_ParticleSystems.clear();
		s_Initialized = false;
	}

	void ParticleSystemManager::OnUpdate(float deltaTime)
	{
		if (!s_Initialized)
			return;

		for (auto& system : s_ParticleSystems)
		{
			if (system)
			{
				system->OnUpdate(deltaTime);
			}
		}
	}

	void ParticleSystemManager::OnRender(const glm::mat4& viewProjection)
	{
		if (!s_Initialized)
			return;

		for (auto& system : s_ParticleSystems)
		{
			if (system)
			{
				system->OnRender(viewProjection);
			}
		}
	}

	void ParticleSystemManager::AddParticleSystem(Ref<ParticleSystem> system)
	{
		if (!s_Initialized)
			Init();

		s_ParticleSystems.push_back(system);
	}

	void ParticleSystemManager::RemoveParticleSystem(Ref<ParticleSystem> system)
	{
		if (!s_Initialized)
			return;

		auto it = std::find(s_ParticleSystems.begin(), s_ParticleSystems.end(), system);
		if (it != s_ParticleSystems.end())
		{
			s_ParticleSystems.erase(it);
		}
	}

	int ParticleSystemManager::GetTotalActiveParticleCount()
	{
		if (!s_Initialized)
			return 0;

		int total = 0;
		for (auto& system : s_ParticleSystems)
		{
			if (system)
			{
				total += system->GetActiveParticleCount();
			}
		}
		return total;
	}

	void ParticleSystemManager::ClearAll()
	{
		s_ParticleSystems.clear();
	}

} // namespace Zgine
