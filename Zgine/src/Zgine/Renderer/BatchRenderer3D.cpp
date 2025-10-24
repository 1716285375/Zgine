#include "zgpch.h"
#include "BatchRenderer3D.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Zgine/Renderer/RendererAPI.h"
#include "Zgine/Renderer/RenderCommand.h"
#include "Zgine/Renderer/RendererManager.h"
#include "Zgine/Renderer/Lighting.h"
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
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float3, "a_Normal" }
		});
		s_VertexArray->AddVertexBuffer(s_VertexBuffer);

		s_VertexBufferBase = CreateScopeArray<Vertex3D>(MaxVertices);
		ZG_CORE_INFO("Created 3D vertex buffer base with {} vertices", MaxVertices);

		// Create indices array - use proper quad indexing pattern
		std::vector<uint32_t> indices(MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			// Each quad uses 6 indices (2 triangles)
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4; // Each quad has 4 vertices
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
			#version 420 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			layout(location = 2) in vec2 a_TexCoord;
			layout(location = 3) in float a_TexIndex;
			layout(location = 4) in vec3 a_Normal;

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
			#version 420 core
			
			layout(location = 0) out vec4 color;
			
			in vec4 v_Color;
			in vec2 v_TexCoord;
			in float v_TexIndex;
			in vec3 v_Normal;
			in vec3 v_WorldPos;

			layout(binding = 0) uniform sampler2D u_Textures[32];
			
			// Only declare uniforms that are actually used
			uniform vec3 u_Material_albedo;
			uniform int u_Material_hasAlbedoTexture;
			
			// Basic lighting
			uniform vec3 u_AmbientColor;
			uniform float u_AmbientIntensity;

			void main()
			{
				vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
				vec3 albedo = u_Material_hasAlbedoTexture > 0 ? texColor.rgb * u_Material_albedo : u_Material_albedo;
				
				vec3 normal = normalize(v_Normal);
				
				vec3 finalColor = vec3(0.0);
				
				// Ambient lighting
				finalColor += u_AmbientColor * u_AmbientIntensity * albedo;
				
				// Simple directional light
				vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
				float NdotL = max(dot(normal, lightDir), 0.0);
				finalColor += albedo * NdotL * 0.5;
				
				// Apply vertex color tint
				finalColor *= v_Color.rgb;
				
				color = vec4(finalColor, v_Color.a * texColor.a);
			}
		)";

		s_Shader = CreateRef<Shader>(vertexSrc, fragmentSrc);
		s_Shader->Bind();
		
		// Debug: Check shader compilation
		ZG_CORE_INFO("BatchRenderer3D shader created with ID: {}", s_Shader->GetRendererID());
		
		s_Shader->UploadUniformIntArray("u_Textures", samplers, MaxTextureSlots);
		
		// Set default material (only what we actually use)
		s_Shader->UploadUniformFloat3("u_Material_albedo", { 0.8f, 0.8f, 0.8f });
		s_Shader->UploadUniformInt("u_Material_hasAlbedoTexture", 0);
		
		// Set default lighting
		s_Shader->UploadUniformFloat3("u_AmbientColor", { 0.1f, 0.1f, 0.1f });
		s_Shader->UploadUniformFloat("u_AmbientIntensity", 0.3f);

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
		
		// Update lighting system uniforms
		LightingSystem::GetInstance().UpdateShaderUniforms(s_Shader.get());

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

		// Check if we have enough space for 24 vertices and 36 indices
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - 24)
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

		// Add vertices to buffer (24 vertices for 6 faces, each face has 4 vertices)
		glm::vec3 faceVertices[24];
		glm::vec3 faceNormals[6] = {
			{ 0.0f, 0.0f, -1.0f }, // Front
			{ 0.0f, 0.0f, 1.0f },  // Back
			{ -1.0f, 0.0f, 0.0f }, // Left
			{ 1.0f, 0.0f, 0.0f },  // Right
			{ 0.0f, -1.0f, 0.0f }, // Bottom
			{ 0.0f, 1.0f, 0.0f }   // Top
		};
		
		// Front face (0,1,2,3)
		faceVertices[0] = vertices[0]; faceVertices[1] = vertices[1];
		faceVertices[2] = vertices[2]; faceVertices[3] = vertices[3];
		// Back face (4,5,6,7)
		faceVertices[4] = vertices[4]; faceVertices[5] = vertices[5];
		faceVertices[6] = vertices[6]; faceVertices[7] = vertices[7];
		// Left face (0,3,7,4)
		faceVertices[8] = vertices[0]; faceVertices[9] = vertices[3];
		faceVertices[10] = vertices[7]; faceVertices[11] = vertices[4];
		// Right face (1,2,6,5)
		faceVertices[12] = vertices[1]; faceVertices[13] = vertices[2];
		faceVertices[14] = vertices[6]; faceVertices[15] = vertices[5];
		// Bottom face (0,1,5,4)
		faceVertices[16] = vertices[0]; faceVertices[17] = vertices[1];
		faceVertices[18] = vertices[5]; faceVertices[19] = vertices[4];
		// Top face (3,2,6,7)
		faceVertices[20] = vertices[3]; faceVertices[21] = vertices[2];
		faceVertices[22] = vertices[6]; faceVertices[23] = vertices[7];

		for (int i = 0; i < 24; i++)
		{
			s_VertexBufferPtr->Position = faceVertices[i];
			s_VertexBufferPtr->Color = color;
			s_VertexBufferPtr->TexCoord = { (i % 2) == 0 ? 0.0f : 1.0f, (i % 4) < 2 ? 0.0f : 1.0f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr->Normal = faceNormals[i / 4];
			s_VertexBufferPtr++;
		}

		// Add indices (36 indices for 6 faces * 2 triangles per face)
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

		segments = glm::clamp(segments, 8, 32); // Reduce max segments for performance
		
		// Use a simplified sphere approach - render as multiple quads
		uint32_t quadCount = segments * segments;
		uint32_t vertexCount = quadCount * 4; // 4 vertices per quad
		uint32_t indexCount = quadCount * 6;  // 6 indices per quad
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		float textureIndex = 0.0f;

		// Generate sphere as multiple quads
		for (int i = 0; i < segments; i++)
		{
			float lat1 = glm::pi<float>() * (-0.5f + (float)i / segments);
			float lat2 = glm::pi<float>() * (-0.5f + (float)(i + 1) / segments);
			
			float y1 = sin(lat1);
			float y2 = sin(lat2);
			float radiusAtY1 = cos(lat1);
			float radiusAtY2 = cos(lat2);

			for (int j = 0; j < segments; j++)
			{
				float lng1 = 2.0f * glm::pi<float>() * (float)j / segments;
				float lng2 = 2.0f * glm::pi<float>() * (float)(j + 1) / segments;
				
				// Calculate quad vertices
				glm::vec3 v1 = glm::vec3(cos(lng1) * radiusAtY1, y1, sin(lng1) * radiusAtY1);
				glm::vec3 v2 = glm::vec3(cos(lng2) * radiusAtY1, y1, sin(lng2) * radiusAtY1);
				glm::vec3 v3 = glm::vec3(cos(lng2) * radiusAtY2, y2, sin(lng2) * radiusAtY2);
				glm::vec3 v4 = glm::vec3(cos(lng1) * radiusAtY2, y2, sin(lng1) * radiusAtY2);
				
				// Calculate normal (average of the four vertices)
				glm::vec3 normal = glm::normalize((v1 + v2 + v3 + v4) * 0.25f);
				
				// Add quad vertices
				glm::vec3 quadVertices[4] = { v1, v2, v3, v4 };
				for (int k = 0; k < 4; k++)
				{
					s_VertexBufferPtr->Position = quadVertices[k];
					s_VertexBufferPtr->Normal = normal;
					s_VertexBufferPtr->Color = color;
					s_VertexBufferPtr->TexCoord = { (k % 2) == 0 ? 0.0f : 1.0f, (k < 2) ? 0.0f : 1.0f };
					s_VertexBufferPtr->TexIndex = textureIndex;
					s_VertexBufferPtr++;
				}
			}
		}

		// Add indices (each quad uses 6 indices)
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

		segments = glm::clamp(segments, 8, 32); // Reduce max segments for performance
		
		// Use a simplified sphere approach - render as multiple quads
		uint32_t quadCount = segments * segments;
		uint32_t vertexCount = quadCount * 4; // 4 vertices per quad
		uint32_t indexCount = quadCount * 6;  // 6 indices per quad
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		float textureIndex = GetTextureIndex(texture);

		// Generate sphere as multiple quads
		for (int i = 0; i < segments; i++)
		{
			float lat1 = glm::pi<float>() * (-0.5f + (float)i / segments);
			float lat2 = glm::pi<float>() * (-0.5f + (float)(i + 1) / segments);
			
			float y1 = sin(lat1);
			float y2 = sin(lat2);
			float radiusAtY1 = cos(lat1);
			float radiusAtY2 = cos(lat2);

			for (int j = 0; j < segments; j++)
			{
				float lng1 = 2.0f * glm::pi<float>() * (float)j / segments;
				float lng2 = 2.0f * glm::pi<float>() * (float)(j + 1) / segments;
				
				// Calculate quad vertices
				glm::vec3 v1 = glm::vec3(cos(lng1) * radiusAtY1, y1, sin(lng1) * radiusAtY1);
				glm::vec3 v2 = glm::vec3(cos(lng2) * radiusAtY1, y1, sin(lng2) * radiusAtY1);
				glm::vec3 v3 = glm::vec3(cos(lng2) * radiusAtY2, y2, sin(lng2) * radiusAtY2);
				glm::vec3 v4 = glm::vec3(cos(lng1) * radiusAtY2, y2, sin(lng1) * radiusAtY2);
				
				// Calculate normal (average of the four vertices)
				glm::vec3 normal = glm::normalize((v1 + v2 + v3 + v4) * 0.25f);
				
				// Add quad vertices with proper texture coordinates
				glm::vec3 quadVertices[4] = { v1, v2, v3, v4 };
				glm::vec2 texCoords[4] = {
					{ (float)j / segments, (float)i / segments },
					{ (float)(j + 1) / segments, (float)i / segments },
					{ (float)(j + 1) / segments, (float)(i + 1) / segments },
					{ (float)j / segments, (float)(i + 1) / segments }
				};
				
				for (int k = 0; k < 4; k++)
				{
					s_VertexBufferPtr->Position = quadVertices[k];
					s_VertexBufferPtr->Normal = normal;
					s_VertexBufferPtr->Color = tintColor;
					s_VertexBufferPtr->TexCoord = texCoords[k];
					s_VertexBufferPtr->TexIndex = textureIndex;
					s_VertexBufferPtr++;
				}
			}
		}

		// Add indices (each quad uses 6 indices)
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

	// Cylinder implementations
	void BatchRenderer3D::DrawCylinder(const glm::vec3& position, float radius, float height, const glm::vec4& color, int segments)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawCylinder called but vertex buffer is not initialized!");
			return;
		}

		// Clamp segments to reasonable range
		segments = glm::clamp(segments, 3, 64);
		
		// Calculate vertex count: 2 circles (top + bottom) + side quads
		// Each circle: segments vertices (center + segments edge points)
		// Side: segments quads = segments * 4 vertices
		uint32_t vertexCount = (segments + 1) * 2 + segments * 4; // Top circle + bottom circle + side quads
		uint32_t indexCount = segments * 6 * 3; // 6 indices per quad, 3 sets (top, bottom, side)
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		// Store the starting vertex index for this cylinder
		uint32_t startVertexIndex = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());

		float textureIndex = 0.0f; // White texture
		const float angleStep = 2.0f * glm::pi<float>() / segments;
		
		// Generate top circle (Y = +height/2)
		glm::vec3 topCenter = position + glm::vec3(0.0f, height * 0.5f, 0.0f);
		glm::vec3 bottomCenter = position + glm::vec3(0.0f, -height * 0.5f, 0.0f);
		
		// Top circle center
		s_VertexBufferPtr->Position = topCenter;
		s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		s_VertexBufferPtr->Color = color;
		s_VertexBufferPtr->TexCoord = { 0.5f, 0.5f };
		s_VertexBufferPtr->TexIndex = textureIndex;
		s_VertexBufferPtr++;
		
		// Top circle edge vertices
		for (int i = 0; i < segments; i++)
		{
			float angle = i * angleStep;
			glm::vec3 vertex = topCenter + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			
			s_VertexBufferPtr->Position = vertex;
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			s_VertexBufferPtr->Color = color;
			s_VertexBufferPtr->TexCoord = { 0.5f + cos(angle) * 0.5f, 0.5f + sin(angle) * 0.5f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}
		
		// Bottom circle center
		s_VertexBufferPtr->Position = bottomCenter;
		s_VertexBufferPtr->Normal = glm::vec3(0.0f, -1.0f, 0.0f);
		s_VertexBufferPtr->Color = color;
		s_VertexBufferPtr->TexCoord = { 0.5f, 0.5f };
		s_VertexBufferPtr->TexIndex = textureIndex;
		s_VertexBufferPtr++;
		
		// Bottom circle edge vertices
		for (int i = 0; i < segments; i++)
		{
			float angle = i * angleStep;
			glm::vec3 vertex = bottomCenter + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			
			s_VertexBufferPtr->Position = vertex;
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, -1.0f, 0.0f);
			s_VertexBufferPtr->Color = color;
			s_VertexBufferPtr->TexCoord = { 0.5f + cos(angle) * 0.5f, 0.5f + sin(angle) * 0.5f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}
		
		// Side quads (connecting top and bottom circles)
		for (int i = 0; i < segments; i++)
		{
			int next = (i + 1) % segments;
			
			// Calculate side quad vertices
			float angle1 = i * angleStep;
			float angle2 = next * angleStep;
			
			glm::vec3 top1 = topCenter + glm::vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
			glm::vec3 top2 = topCenter + glm::vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
			glm::vec3 bottom1 = bottomCenter + glm::vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
			glm::vec3 bottom2 = bottomCenter + glm::vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
			
			// Calculate normal for this side quad
			glm::vec3 sideNormal = glm::normalize(glm::vec3(cos(angle1), 0.0f, sin(angle1)));
			
			// Add quad vertices (two triangles)
			glm::vec3 quadVertices[4] = { bottom1, bottom2, top2, top1 };
			for (int j = 0; j < 4; j++)
			{
				s_VertexBufferPtr->Position = quadVertices[j];
				s_VertexBufferPtr->Normal = sideNormal;
				s_VertexBufferPtr->Color = color;
				s_VertexBufferPtr->TexCoord = { (float)j / 3.0f, 0.0f }; // Simple UV mapping
				s_VertexBufferPtr->TexIndex = textureIndex;
				s_VertexBufferPtr++;
			}
		}
		
		// Add indices for all triangles (using the pre-generated pattern)
		// Each quad uses 6 indices, and we have segments quads for each part
		s_IndexCount += segments * 6 * 3; // Top circle + bottom circle + side quads
		
		// Update statistics
		s_Stats.CylinderCount++;
	}

	void BatchRenderer3D::DrawCylinder(const glm::vec3& position, float radius, float height, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int segments)
	{
		if (!s_VertexBufferBase || !s_VertexBufferPtr)
		{
			ZG_CORE_ERROR("BatchRenderer3D::DrawCylinder called but vertex buffer is not initialized!");
			return;
		}

		// Clamp segments to reasonable range
		segments = glm::clamp(segments, 3, 64);
		
		// Calculate vertex count: 2 circles (top + bottom) + side quads
		uint32_t vertexCount = (segments + 1) * 2 + segments * 4;
		uint32_t indexCount = segments * 6 * 3;
		
		// Check if we have enough space
		uint32_t currentVertexCount = (uint32_t)(s_VertexBufferPtr - s_VertexBufferBase.get());
		if (currentVertexCount >= MaxVertices - vertexCount)
			NextBatch();

		float textureIndex = GetTextureIndex(texture);
		const float angleStep = 2.0f * glm::pi<float>() / segments;
		
		// Generate top circle (Y = +height/2)
		glm::vec3 topCenter = position + glm::vec3(0.0f, height * 0.5f, 0.0f);
		glm::vec3 bottomCenter = position + glm::vec3(0.0f, -height * 0.5f, 0.0f);
		
		// Top circle center
		s_VertexBufferPtr->Position = topCenter;
		s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		s_VertexBufferPtr->Color = tintColor;
		s_VertexBufferPtr->TexCoord = { 0.5f, 0.5f };
		s_VertexBufferPtr->TexIndex = textureIndex;
		s_VertexBufferPtr++;
		
		// Top circle edge vertices
		for (int i = 0; i < segments; i++)
		{
			float angle = i * angleStep;
			glm::vec3 vertex = topCenter + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			
			s_VertexBufferPtr->Position = vertex;
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			s_VertexBufferPtr->Color = tintColor;
			s_VertexBufferPtr->TexCoord = { 0.5f + cos(angle) * 0.5f, 0.5f + sin(angle) * 0.5f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}
		
		// Bottom circle center
		s_VertexBufferPtr->Position = bottomCenter;
		s_VertexBufferPtr->Normal = glm::vec3(0.0f, -1.0f, 0.0f);
		s_VertexBufferPtr->Color = tintColor;
		s_VertexBufferPtr->TexCoord = { 0.5f, 0.5f };
		s_VertexBufferPtr->TexIndex = textureIndex;
		s_VertexBufferPtr++;
		
		// Bottom circle edge vertices
		for (int i = 0; i < segments; i++)
		{
			float angle = i * angleStep;
			glm::vec3 vertex = bottomCenter + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			
			s_VertexBufferPtr->Position = vertex;
			s_VertexBufferPtr->Normal = glm::vec3(0.0f, -1.0f, 0.0f);
			s_VertexBufferPtr->Color = tintColor;
			s_VertexBufferPtr->TexCoord = { 0.5f + cos(angle) * 0.5f, 0.5f + sin(angle) * 0.5f };
			s_VertexBufferPtr->TexIndex = textureIndex;
			s_VertexBufferPtr++;
		}
		
		// Side quads (connecting top and bottom circles)
		for (int i = 0; i < segments; i++)
		{
			int next = (i + 1) % segments;
			
			// Calculate side quad vertices
			float angle1 = i * angleStep;
			float angle2 = next * angleStep;
			
			glm::vec3 top1 = topCenter + glm::vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
			glm::vec3 top2 = topCenter + glm::vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
			glm::vec3 bottom1 = bottomCenter + glm::vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
			glm::vec3 bottom2 = bottomCenter + glm::vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
			
			// Calculate normal for this side quad
			glm::vec3 sideNormal = glm::normalize(glm::vec3(cos(angle1), 0.0f, sin(angle1)));
			
			// Add quad vertices (two triangles)
			glm::vec3 quadVertices[4] = { bottom1, bottom2, top2, top1 };
			for (int j = 0; j < 4; j++)
			{
				s_VertexBufferPtr->Position = quadVertices[j];
				s_VertexBufferPtr->Normal = sideNormal;
				s_VertexBufferPtr->Color = tintColor;
				s_VertexBufferPtr->TexCoord = { (float)j / 3.0f, 0.0f }; // Simple UV mapping
				s_VertexBufferPtr->TexIndex = textureIndex;
				s_VertexBufferPtr++;
			}
		}
		
		// Add indices for all triangles (using the pre-generated pattern)
		// Each quad uses 6 indices, and we have segments quads for each part
		s_IndexCount += segments * 6 * 3; // Top circle + bottom circle + side quads
		
		// Update statistics
		s_Stats.CylinderCount++;
	}

}
