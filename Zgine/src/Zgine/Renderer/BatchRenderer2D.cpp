#include "zgpch.h"
#include "BatchRenderer2D.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Zgine/Renderer/RendererAPI.h"
#include "Zgine/Renderer/RenderCommand.h"

namespace Zgine {

	std::shared_ptr<VertexArray> BatchRenderer2D::s_QuadVertexArray;
	std::shared_ptr<VertexBuffer> BatchRenderer2D::s_QuadVertexBuffer;
	std::shared_ptr<Shader> BatchRenderer2D::s_TextureShader;
	std::shared_ptr<Texture2D> BatchRenderer2D::s_WhiteTexture;

	uint32_t BatchRenderer2D::s_QuadIndexCount;
	std::unique_ptr<QuadVertex[]> BatchRenderer2D::s_QuadVertexBufferBase;
	QuadVertex* BatchRenderer2D::s_QuadVertexBufferPtr;

	std::array<std::shared_ptr<Texture2D>, BatchRenderer2D::MaxTextureSlots> BatchRenderer2D::s_TextureSlots;
	uint32_t BatchRenderer2D::s_TextureSlotIndex = 1; // 0 = white texture

	glm::vec4 BatchRenderer2D::s_QuadVertexPositions[4] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	RenderStats BatchRenderer2D::s_Stats;

	void BatchRenderer2D::Init()
	{
		s_QuadVertexArray.reset(VertexArray::Create());

		s_QuadVertexBuffer.reset(VertexBuffer::Create(nullptr, MaxVertices * sizeof(QuadVertex)));
		s_QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" }
		});
		s_QuadVertexArray->AddVertexBuffer(s_QuadVertexBuffer);

		s_QuadVertexBufferBase = std::make_unique<QuadVertex[]>(MaxVertices);

		// Create indices array - keep it alive until IndexBuffer is created
		std::vector<uint32_t> quadIndices(MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB;
		quadIB.reset(IndexBuffer::Create(quadIndices.data(), MaxIndices));
		s_QuadVertexArray->SetIndexBuffer(quadIB);

		// Create white texture
		s_WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[MaxTextureSlots];
		for (uint32_t i = 0; i < MaxTextureSlots; i++)
			samplers[i] = i;

		// Create shader
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
				color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
			}
		)";

		s_TextureShader.reset(new Shader(vertexSrc, fragmentSrc));
		s_TextureShader->Bind();
		s_TextureShader->UploadUniformIntArray("u_Textures", samplers, MaxTextureSlots);

		// Set all texture slots to 0
		s_TextureSlots[0] = s_WhiteTexture;
	}

	void BatchRenderer2D::Shutdown()
	{
		// Smart pointers will automatically clean up memory
	}

	void BatchRenderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_TextureShader->Bind();
		s_TextureShader->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void BatchRenderer2D::EndScene()
	{
		Flush();
	}

	void BatchRenderer2D::Flush()
	{
		if (s_QuadIndexCount == 0)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_QuadVertexBufferPtr - (uint8_t*)s_QuadVertexBufferBase.get());
		s_QuadVertexBuffer->SetData(s_QuadVertexBufferBase.get(), dataSize);

		// Bind textures
		for (uint32_t i = 0; i < s_TextureSlotIndex; i++)
		{
			s_TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_QuadVertexArray);
		
		// Update statistics
		s_Stats.DrawCalls++;
		s_Stats.VertexCount += s_QuadIndexCount * 4 / 6; // Each quad has 4 vertices, 6 indices
		s_Stats.IndexCount += s_QuadIndexCount;          // s_QuadIndexCount is the number of indices
	}

	void BatchRenderer2D::StartBatch()
	{
		s_QuadIndexCount = 0;
		s_QuadVertexBufferPtr = s_QuadVertexBufferBase.get();
		s_TextureSlotIndex = 1;
	}

	void BatchRenderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void BatchRenderer2D::FlushAndReset()
	{
		EndScene();
		StartBatch();
	}

	void BatchRenderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position.x, position.y, 0.0f), size, color);
	}

	void BatchRenderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuadInternal(position, size, color, -1);
	}

	void BatchRenderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawQuad(glm::vec3(position.x, position.y, 0.0f), size, texture, tintColor);
	}

	void BatchRenderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawQuadInternal(position, size, texture, tintColor, -1);
	}

	void BatchRenderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(glm::vec3(position.x, position.y, 0.0f), size, rotation, color);
	}

	void BatchRenderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuadInternal(position, size, rotation, color, -1);
	}

	void BatchRenderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawRotatedQuad(glm::vec3(position.x, position.y, 0.0f), size, rotation, texture, tintColor);
	}

	void BatchRenderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor)
	{
		DrawRotatedQuadInternal(position, size, rotation, texture, tintColor, -1);
	}

	void BatchRenderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, float thickness)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		// Calculate line direction and length
		glm::vec3 direction = p1 - p0;
		float length = glm::length(direction);
		
		if (length < 0.001f) return; // Skip very short lines
		
		direction = glm::normalize(direction);
		
		// Calculate rotation angle
		float angle = atan2(direction.y, direction.x);
		
		// Calculate center position
		glm::vec3 center = (p0 + p1) * 0.5f;
		
		// Calculate size (length x thickness)
		glm::vec2 size(length, thickness);
		
		// Use the rotated quad drawing function
		DrawRotatedQuadInternal(center, size, angle, color, -1);
	}

	void BatchRenderer2D::DrawCircle(const glm::vec3& position, float radius, const glm::vec4& color, float thickness, float fade)
	{
		DrawCircle(position, radius, color, 32, thickness, fade);
	}

	void BatchRenderer2D::DrawCircle(const glm::vec3& position, float radius, const glm::vec4& color, int segments, float thickness, float fade)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		// Clamp segments to reasonable range
		segments = glm::clamp(segments, 3, 64);
		const float angleStep = 2.0f * glm::pi<float>() / segments;
		
		// Draw circle as multiple triangular sectors
		for (int i = 0; i < segments; i++)
		{
			if (s_QuadIndexCount >= MaxIndices - 6)
				NextBatch();
			
			float angle1 = i * angleStep;
			float angle2 = (i + 1) * angleStep;
			
			// Calculate sector vertices
			glm::vec3 center = position;
			glm::vec3 v1 = position + glm::vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
			glm::vec3 v2 = position + glm::vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
			
			// Create a triangular sector using center and two edge points
			// We'll create a quad that forms a triangle (two vertices are the same)
			glm::vec3 quadVertices[4] = {
				center,           // Bottom-left
				v1,              // Bottom-right  
				v2,              // Top-right
				center           // Top-left (same as center to form triangle)
			};
			
			// Add vertices to buffer
			for (int j = 0; j < 4; j++)
			{
				s_QuadVertexBufferPtr->Position = quadVertices[j];
				s_QuadVertexBufferPtr->Color = color;
				s_QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
				s_QuadVertexBufferPtr->TexIndex = 0.0f;
				s_QuadVertexBufferPtr++;
			}
			
			s_QuadIndexCount += 6;
		}
		
		s_Stats.QuadCount += segments;
	}

	void BatchRenderer2D::DrawCircleOutline(const glm::vec3& position, float radius, const glm::vec4& color, float thickness, int segments)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		// Clamp segments to reasonable range
		segments = glm::clamp(segments, 3, 64);
		const float angleStep = 2.0f * glm::pi<float>() / segments;
		
		// Draw circle outline as connected lines (quads)
		for (int i = 0; i < segments; i++)
		{
			if (s_QuadIndexCount >= MaxIndices - 6)
				NextBatch();
			
			float angle1 = i * angleStep;
			float angle2 = (i + 1) * angleStep;
			
			// Calculate line segment points
			glm::vec3 p1 = position + glm::vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
			glm::vec3 p2 = position + glm::vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
			
			// Draw line segment as a quad
			DrawLine(p1, p2, color, thickness);
		}
	}

	RenderStats BatchRenderer2D::GetStats()
	{
		return s_Stats;
	}

	void BatchRenderer2D::ResetStats()
	{
		memset(&s_Stats, 0, sizeof(RenderStats));
	}

	void BatchRenderer2D::DrawQuadInternal(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		float textureIndex = 0.0f; // White Texture

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_QuadVertexBufferPtr->Position = transform * s_QuadVertexPositions[i];
			s_QuadVertexBufferPtr->Color = color;
			s_QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			s_QuadVertexBufferPtr->TexIndex = textureIndex;
			s_QuadVertexBufferPtr++;
		}

		s_QuadIndexCount += 6;

		s_Stats.QuadCount++;
	}

	void BatchRenderer2D::DrawQuadInternal(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		float textureIndex = GetTextureIndex(texture);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_QuadVertexBufferPtr->Position = transform * s_QuadVertexPositions[i];
			s_QuadVertexBufferPtr->Color = tintColor;
			s_QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_QuadVertexBufferPtr->TexIndex = textureIndex;
			s_QuadVertexBufferPtr++;
		}

		s_QuadIndexCount += 6;

		s_Stats.QuadCount++;
	}

	void BatchRenderer2D::DrawRotatedQuadInternal(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color, int entityID)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		float textureIndex = 0.0f; // White Texture

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_QuadVertexBufferPtr->Position = transform * s_QuadVertexPositions[i];
			s_QuadVertexBufferPtr->Color = color;
			s_QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
			s_QuadVertexBufferPtr->TexIndex = textureIndex;
			s_QuadVertexBufferPtr++;
		}

		s_QuadIndexCount += 6;

		s_Stats.QuadCount++;
	}

	void BatchRenderer2D::DrawRotatedQuadInternal(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID)
	{
		if (s_QuadIndexCount >= MaxIndices)
			NextBatch();

		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		float textureIndex = GetTextureIndex(texture);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_QuadVertexBufferPtr->Position = transform * s_QuadVertexPositions[i];
			s_QuadVertexBufferPtr->Color = tintColor;
			s_QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_QuadVertexBufferPtr->TexIndex = textureIndex;
			s_QuadVertexBufferPtr++;
		}

		s_QuadIndexCount += 6;

		s_Stats.QuadCount++;
	}

	float BatchRenderer2D::GetTextureIndex(const std::shared_ptr<Texture2D>& texture)
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

}
