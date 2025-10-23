#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Texture.h"
#include "Zgine/Renderer/VertexArray.h"
#include "Zgine/Renderer/OrthographicCamera.h"

namespace Zgine {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct RenderStats
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
		
		uint32_t GetTotalVertexCount() const { return VertexCount; }
		uint32_t GetTotalIndexCount() const { return IndexCount; }
	};

	class BatchRenderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		// Check if renderer is initialized
		static bool IsInitialized() { return s_Initialized; }

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Rotated quads
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, float thickness = 0.1f);

		static void DrawCircle(const glm::vec3& position, float radius, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f);
		static void DrawCircle(const glm::vec3& position, float radius, const glm::vec4& color, int segments, float thickness = 1.0f, float fade = 0.005f);
		
		// Circle outline rendering
		static void DrawCircleOutline(const glm::vec3& position, float radius, const glm::vec4& color, float thickness = 0.1f, int segments = 32);

		// Additional 2D primitives
		static void DrawTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);
		static void DrawEllipse(const glm::vec3& position, float radiusX, float radiusY, const glm::vec4& color, int segments = 32);
		static void DrawEllipseOutline(const glm::vec3& position, float radiusX, float radiusY, const glm::vec4& color, float thickness = 0.1f, int segments = 32);
		static void DrawArc(const glm::vec3& position, float radius, float startAngle, float endAngle, const glm::vec4& color, float thickness = 0.1f, int segments = 32);

		// Advanced 2D features
		static void DrawQuadGradient(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
		static void DrawRotatedQuadGradient(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colorTopLeft, const glm::vec4& colorTopRight, const glm::vec4& colorBottomLeft, const glm::vec4& colorBottomRight);
		
		// Transform functions
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		static RenderStats GetStats();
		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();

		static void FlushAndReset();

		// Quad rendering helpers
		static void DrawQuadInternal(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawQuadInternal(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);
		static void DrawRotatedQuadInternal(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color, int entityID = -1);
		static void DrawRotatedQuadInternal(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);

		static float GetTextureIndex(const Ref<Texture2D>& texture);

	private:
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		static Ref<VertexArray> s_QuadVertexArray;
		static Ref<VertexBuffer> s_QuadVertexBuffer;
		static Ref<Shader> s_TextureShader;
		static Ref<Texture2D> s_WhiteTexture;

		static uint32_t s_QuadIndexCount;
		static ScopeArray<QuadVertex> s_QuadVertexBufferBase;
		static QuadVertex* s_QuadVertexBufferPtr;

		static std::array<Ref<Texture2D>, MaxTextureSlots> s_TextureSlots;
		static uint32_t s_TextureSlotIndex;

		static glm::vec4 s_QuadVertexPositions[4];

		static RenderStats s_Stats;
		static bool s_Initialized;
		static bool s_ShuttingDown;
	};

}
