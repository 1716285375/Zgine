#include "zgpch.h"
#include "BatchRenderer3D.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Zgine/Renderer/RendererAPI.h"
#include "Zgine/Renderer/RenderCommand.h"
#include "Zgine/Renderer/RendererManager.h"
#include "../Core.h"

namespace Zgine {

	Ref<VertexArray> BatchRenderer3D::s_VertexArray;
	Ref<VertexBuffer> BatchRenderer3D::s_VertexBuffer;
	Ref<Shader> BatchRenderer3D::s_Shader;
	Ref<Texture2D> BatchRenderer3D::s_WhiteTexture;

	uint32_t BatchRenderer3D::s_IndexCount;
	ScopeArray<Vertex3D> BatchRenderer3D::s_VertexBufferBase;
	Vertex3D* BatchRenderer3D::s_VertexBufferPtr;

	std::array<Ref<Texture2D>, BatchRenderer3D::MaxTextureSlots> BatchRenderer3D::s_TextureSlots;
	uint32_t BatchRenderer3D::s_TextureSlotIndex = 1;

	RenderStats3D BatchRenderer3D::s_Stats;
	bool BatchRenderer3D::s_Initialized = false;
	bool BatchRenderer3D::s_ShuttingDown = false;

	void BatchRenderer3D::Init()
	{
		ZG_CORE_INFO("BatchRenderer3D::Init() called");
		
		s_VertexArray.reset(VertexArray::Create());

		s_VertexBuffer.reset(VertexBuffer::Create(nullptr, MaxVertices * sizeof(Vertex3D)));
		s_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" }
		});
		s_VertexArray->AddVertexBuffer(s_VertexBuffer);

		s_VertexBufferBase = CreateScopeArray<Vertex3D>(MaxVertices);
		ZG_CORE_INFO("Created 3D vertex buffer base with {} vertices", MaxVertices);

		// Create indices array
		std::vector<uint32_t> indices(MaxIndices);
		for (uint32_t i = 0; i < MaxIndices; i++)
		{
			indices[i] = i;
		}

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices.data(), MaxIndices));
		s_VertexArray->SetIndexBuffer(indexBuffer);

		// Create white texture
		s_WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[MaxTextureSlots];
		for (uint32_t i = 0; i < MaxTextureSlots; i++)
			samplers[i] = i;

		// Create 3D shader
		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec4 a_Color;
			layout(location = 3) in vec2 a_TexCoord;
			layout(location = 4) in float a_TexIndex;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec4 v_Color;
			out vec2 v_TexCoord;
			out float v_TexIndex;
			out vec3 v_Normal;
			out vec3 v_WorldPos;

			void main()
			{
				vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
				v_WorldPos = worldPos.xyz;
				v_Normal = mat3(u_Transform) * a_Normal;
				v_Color = a_Color;
				v_TexCoord = a_TexCoord;
				v_TexIndex = a_TexIndex;
				gl_Position = u_ViewProjection * worldPos;
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec4 v_Color;
			in vec2 v_TexCoord;
			in float v_TexIndex;
			in vec3 v_Normal;
			in vec3 v_WorldPos;

			uniform sampler2D u_Textures[32];
			uniform vec3 u_LightPosition;
			uniform vec3 u_LightColor;
			uniform float u_LightIntensity;

			void main()
			{
				vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
				vec3 finalColor = texColor.rgb * v_Color.rgb;
				
				// Simple lighting calculation
				vec3 lightDir = normalize(u_LightPosition - v_WorldPos);
				float diff = max(dot(normalize(v_Normal), lightDir), 0.0);
				vec3 diffuse = diff * u_LightColor * u_LightIntensity;
				
				finalColor = finalColor * (0.3 + diffuse); // Ambient + diffuse
				color = vec4(finalColor, v_Color.a * texColor.a);
			}
		)";

		s_Shader = CreateRef<Shader>(vertexSrc, fragmentSrc);
		s_Shader->Bind();
		s_Shader->UploadUniformIntArray("u_Textures", samplers, MaxTextureSlots);
		
		// Set default lighting
		s_Shader->UploadUniformFloat3("u_LightPosition", { 0.0f, 10.0f, 0.0f });
		s_Shader->UploadUniformFloat3("u_LightColor", { 1.0f, 1.0f, 1.0f });
		s_Shader->UploadUniformFloat("u_LightIntensity", 1.0f);

		// Set all texture slots to 0
		s_TextureSlots[0] = s_WhiteTexture;
		
		// Initialize other static members
		s_IndexCount = 0;
		s_VertexBufferPtr = s_VertexBufferBase.get();
		s_TextureSlotIndex = 1;
		
		// Mark as initialized
		s_Initialized = true;
		
		ZG_CORE_INFO("BatchRenderer3D::Init() completed successfully");
	}

	void BatchRenderer3D::Shutdown()
	{
		ZG_CORE_INFO("BatchRenderer3D::Shutdown() called");
		
		// Mark as shutting down to prevent further access
		s_ShuttingDown = true;
		
		// Reset all static members to prevent access after shutdown
		s_VertexArray.reset();
		s_VertexBuffer.reset();
		s_Shader.reset();
		s_WhiteTexture.reset();
		
		// Clear vertex buffer base and pointer
		s_VertexBufferBase.reset();
		s_VertexBufferPtr = nullptr;
		
		// Clear texture slots
		for (auto& slot : s_TextureSlots)
			slot.reset();
		
		// Reset counters
		s_IndexCount = 0;
		s_TextureSlotIndex = 1;
		
		// Reset stats
		s_Stats = RenderStats3D{};
		
		// Mark as not initialized
		s_Initialized = false;
		
		ZG_CORE_INFO("BatchRenderer3D::Shutdown() completed");
	}

	void BatchRenderer3D::BeginScene(const PerspectiveCamera& camera)
	{
		// Check global application shutdown flag first
		if (g_ApplicationShuttingDown)
		{
			ZG_CORE_WARN("BatchRenderer3D::BeginScene called during application shutdown, ignoring");
			return;
		}
		
		// Check RendererManager state
		if (RendererManager::GetInstance().IsShuttingDown())
		{
			ZG_CORE_WARN("BatchRenderer3D::BeginScene called during shutdown, ignoring");
			return;
		}
		
		if (!RendererManager::GetInstance().IsInitialized())
		{
			ZG_CORE_ERROR("BatchRenderer3D::BeginScene called but renderer manager is not initialized!");
			return;
		}
		
		if (s_ShuttingDown)
		{
			ZG_CORE_WARN("BatchRenderer3D::BeginScene called during shutdown, ignoring");
			return;
		}
		
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("BatchRenderer3D::BeginScene called but renderer is not initialized!");
			return;
		}
		
		if (!s_Shader)
		{
			ZG_CORE_ERROR("BatchRenderer3D::BeginScene called but shader is not initialized!");
			return;
		}
		
		s_Shader->Bind();
		s_Shader->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void BatchRenderer3D::EndScene()
	{
		Flush();
	}

	void BatchRenderer3D::Flush()
	{
		if (s_IndexCount == 0)
			return; // Nothing to draw

		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::Flush called but vertex buffer is not initialized!");
			return;
		}

		if (!s_VertexBuffer)
		{
			ZG_CORE_ERROR("BatchRenderer3D::Flush called but vertex buffer object is not initialized!");
			return;
		}

		if (!s_VertexArray)
		{
			ZG_CORE_ERROR("BatchRenderer3D::Flush called but vertex array is not initialized!");
			return;
		}

		uint32_t dataSize = (uint32_t)((uint8_t*)s_VertexBufferPtr - (uint8_t*)s_VertexBufferBase.get());
		s_VertexBuffer->SetData(s_VertexBufferBase.get(), dataSize);

		// Bind textures
		for (uint32_t i = 0; i < s_TextureSlotIndex; i++)
		{
			if (s_TextureSlots[i])
				s_TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_VertexArray);
		
		// Update statistics
		s_Stats.DrawCalls++;
		s_Stats.TriangleCount += s_IndexCount / 3;
		s_Stats.VertexCount += s_IndexCount;
		s_Stats.IndexCount += s_IndexCount;
	}

	void BatchRenderer3D::StartBatch()
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("BatchRenderer3D::StartBatch called but renderer is not initialized!");
			return;
		}
		
		if (!s_VertexBufferBase)
		{
			ZG_CORE_ERROR("BatchRenderer3D::StartBatch called but vertex buffer is not initialized!");
			return;
		}
		
		s_IndexCount = 0;
		s_VertexBufferPtr = s_VertexBufferBase.get();
		s_TextureSlotIndex = 1;
	}

	void BatchRenderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void BatchRenderer3D::FlushAndReset()
	{
		EndScene();
		StartBatch();
	}

	float BatchRenderer3D::GetTextureIndex(const Ref<Texture2D>& texture)
	{
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_TextureSlotIndex; i++)
		{
			if (s_TextureSlots[i] && texture && s_TextureSlots[i]->GetRendererID() == texture->GetRendererID())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_TextureSlotIndex >= MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_TextureSlotIndex;
			s_TextureSlots[s_TextureSlotIndex] = texture;
			s_TextureSlotIndex++;
		}

		return textureIndex;
	}

	// Cube rendering
	void BatchRenderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
		DrawCubeInternal(position, size, transform, color);
	}

	void BatchRenderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
		DrawCubeInternal(position, size, transform, texture, tintColor);
	}

	void BatchRenderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const glm::vec4& color)
	{
		DrawCubeInternal(position, size, transform, color);
	}

	void BatchRenderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawCubeInternal(position, size, transform, texture, tintColor);
	}

	void BatchRenderer3D::DrawCubeInternal(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawCubeInternal called but vertex buffer is not initialized!");
			return;
		}

		// Check if we have enough space for 8 vertices and 36 indices
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - 8)
			NextBatch();

		float textureIndex = 0.0f; // White texture

		// Cube vertices (8 vertices)
		glm::vec3 vertices[8] = {
			{ -0.5f, -0.5f, -0.5f }, // 0
			{  0.5f, -0.5f, -0.5f }, // 1
			{  0.5f,  0.5f, -0.5f }, // 2
			{ -0.5f,  0.5f, -0.5f }, // 3
			{ -0.5f, -0.5f,  0.5f }, // 4
			{  0.5f, -0.5f,  0.5f }, // 5
			{  0.5f,  0.5f,  0.5f }, // 6
			{ -0.5f,  0.5f,  0.5f }  // 7
		};

		// Add vertices to buffer
		for (int i = 0; i < 8; i++)
		{
			s_VertexBufferPtr->Position = vertices[i];
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 0.0f, 1.0f); // Will be calculated per face
			s_VertexBufferPtr->Color = color;
			s_VertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}

		// Add indices (36 indices for 12 triangles)
		s_IndexCount += 36;

		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.CubeCount++;
	}

	void BatchRenderer3D::DrawCubeInternal(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int entityID)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawCubeInternal called but vertex buffer is not initialized!");
			return;
		}

		// Check if we have enough space for 8 vertices and 36 indices
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - 8)
			NextBatch();

		float textureIndex = GetTextureIndex(texture);

		// Same as above but with texture coordinates
		glm::vec3 vertices[8] = {
			{ -0.5f, -0.5f, -0.5f },
			{  0.5f, -0.5f, -0.5f },
			{  0.5f,  0.5f, -0.5f },
			{ -0.5f,  0.5f, -0.5f },
			{ -0.5f, -0.5f,  0.5f },
			{  0.5f, -0.5f,  0.5f },
			{  0.5f,  0.5f,  0.5f },
			{ -0.5f,  0.5f,  0.5f }
		};

		glm::vec2 texCoords[8] = {
			{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
			{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
		};

		for (int i = 0; i < 8; i++)
		{
			s_VertexBufferPtr->Position = vertices[i];
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 0.0f, 1.0f);
			s_VertexBufferPtr->Color = tintColor;
			s_VertexBufferPtr->TexCoord = texCoords[i];
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}

		// Add indices (36 indices for 12 triangles)
		s_IndexCount += 36;

		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.CubeCount++;
	}

	// Sphere rendering
	void BatchRenderer3D::DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int segments)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));
		DrawSphereInternal(position, radius, transform, color, segments);
	}

	void BatchRenderer3D::DrawSphere(const glm::vec3& position, float radius, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int segments)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));
		DrawSphereInternal(position, radius, transform, texture, tintColor, segments);
	}

	void BatchRenderer3D::DrawSphereInternal(const glm::vec3& position, float radius, const glm::mat4& transform, const glm::vec4& color, int segments, int entityID)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawSphereInternal called but vertex buffer is not initialized!");
			return;
		}

		segments = glm::clamp(segments, 8, 64);
		
		uint32_t vertexCount = (segments + 1) * (segments + 1);
		uint32_t indexCount = segments * segments * 6;
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		float textureIndex = 0.0f;

		// Generate sphere vertices
		for (int i = 0; i <= segments; i++)
		{
			float lat = glm::pi<float>() * (-0.5f + (float)i / segments);
			float y = sin(lat);
			float radiusAtY = cos(lat);

			for (int j = 0; j <= segments; j++)
			{
				float lng = 2.0f * glm::pi<float>() * (float)j / segments;
				float x = cos(lng) * radiusAtY;
				float z = sin(lng) * radiusAtY;

				s_VertexBufferPtr->Position = glm::vec3(x, y, z);
				s_VertexBufferPtr->Normal = glm::vec3(x, y, z);
				s_VertexBufferPtr->Color = color;
				s_VertexBufferPtr->TexCoord = { (float)j / segments, (float)i / segments };
				s_VertexBufferPtr->TexIndex = textureIndex;
				s_VertexBufferPtr++;
			}
		}

		// Add indices
		s_IndexCount += indexCount;

		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.SphereCount++;
	}

	void BatchRenderer3D::DrawSphereInternal(const glm::vec3& position, float radius, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int segments, int entityID)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawSphereInternal called but vertex buffer is not initialized!");
			return;
		}

		segments = glm::clamp(segments, 8, 64);
		
		uint32_t vertexCount = (segments + 1) * (segments + 1);
		uint32_t indexCount = segments * segments * 6;
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		float textureIndex = GetTextureIndex(texture);

		for (int i = 0; i <= segments; i++)
		{
			float lat = glm::pi<float>() * (-0.5f + (float)i / segments);
			float y = sin(lat);
			float radiusAtY = cos(lat);

			for (int j = 0; j <= segments; j++)
			{
				float lng = 2.0f * glm::pi<float>() * (float)j / segments;
				float x = cos(lng) * radiusAtY;
				float z = sin(lng) * radiusAtY;

				s_VertexBufferPtr->Position = glm::vec3(x, y, z);
				s_VertexBufferPtr->Normal = glm::vec3(x, y, z);
				s_VertexBufferPtr->Color = tintColor;
				s_VertexBufferPtr->TexCoord = { (float)j / segments, (float)i / segments };
				s_VertexBufferPtr->TexIndex = textureIndex;
				s_VertexBufferPtr++;
			}
		}

		// Add indices
		s_IndexCount += indexCount;

		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.SphereCount++;
	}

	// Plane rendering
	void BatchRenderer3D::DrawPlane(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawPlane called but vertex buffer is not initialized!");
			return;
		}

		// Check if we have enough space for 4 vertices and 6 indices
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - 4)
			NextBatch();

		float textureIndex = 0.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, 1.0f, size.y));

		// Plane vertices
		glm::vec3 vertices[4] = {
			{ -0.5f, 0.0f, -0.5f },
			{  0.5f, 0.0f, -0.5f },
			{  0.5f, 0.0f,  0.5f },
			{ -0.5f, 0.0f,  0.5f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_VertexBufferPtr->Position = vertices[i];
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			s_VertexBufferPtr->Color = color;
			s_VertexBufferPtr->TexCoord = { (i % 2) == 0 ? 0.0f : 1.0f, (i < 2) ? 0.0f : 1.0f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}

		s_IndexCount += 6;
		
		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.PlaneCount++;
	}

	void BatchRenderer3D::DrawPlane(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawPlane called but vertex buffer is not initialized!");
			return;
		}

		// Check if we have enough space for 4 vertices and 6 indices
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - 4)
			NextBatch();

		float textureIndex = GetTextureIndex(texture);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, 1.0f, size.y));

		glm::vec3 vertices[4] = {
			{ -0.5f, 0.0f, -0.5f },
			{  0.5f, 0.0f, -0.5f },
			{  0.5f, 0.0f,  0.5f },
			{ -0.5f, 0.0f,  0.5f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_VertexBufferPtr->Position = vertices[i];
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			s_VertexBufferPtr->Color = tintColor;
			s_VertexBufferPtr->TexCoord = { (i % 2) == 0 ? 0.0f : 1.0f, (i < 2) ? 0.0f : 1.0f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}

		s_IndexCount += 6;
		
		// Upload transform matrix
		if (s_Shader)
			s_Shader->UploadUniformMat4("u_Transform", transform);

		// Update statistics
		s_Stats.PlaneCount++;
	}

	// Statistics
	RenderStats3D BatchRenderer3D::GetStats()
	{
		return s_Stats;
	}

	void BatchRenderer3D::ResetStats()
	{
		memset(&s_Stats, 0, sizeof(RenderStats3D));
	}

}
