#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Texture.h"
#include "Zgine/Renderer/VertexArray.h"
#include "Zgine/Renderer/PerspectiveCamera.h"

namespace Zgine {

	struct Vertex3D
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	struct RenderStats3D
	{
		uint32_t DrawCalls = 0;
		uint32_t TriangleCount = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
		
		// Additional counters for specific shapes
		uint32_t CubeCount = 0;
		uint32_t SphereCount = 0;
		uint32_t PlaneCount = 0;
		
		uint32_t GetTotalVertexCount() const { return VertexCount; }
		uint32_t GetTotalIndexCount() const { return IndexCount; }
	};

	class BatchRenderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const PerspectiveCamera& camera);
		static void EndScene();
		static void Flush();

		// 3D Primitives
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		
		static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int segments = 32);
		static void DrawSphere(const glm::vec3& position, float radius, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), int segments = 32);
		
		static void DrawCylinder(const glm::vec3& position, float radius, float height, const glm::vec4& color, int segments = 32);
		static void DrawCylinder(const glm::vec3& position, float radius, float height, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), int segments = 32);
		
		static void DrawPlane(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawPlane(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Transform support
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const glm::vec4& color);
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Statistics
		static RenderStats3D GetStats();
		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();
		static void FlushAndReset();

		// Internal rendering helpers
		static void DrawCubeInternal(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCubeInternal(const glm::vec3& position, const glm::vec3& size, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);
		
		static void DrawSphereInternal(const glm::vec3& position, float radius, const glm::mat4& transform, const glm::vec4& color, int segments, int entityID = -1);
		static void DrawSphereInternal(const glm::vec3& position, float radius, const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, int segments, int entityID = -1);

		static float GetTextureIndex(const Ref<Texture2D>& texture);

	private:
		static const uint32_t MaxTriangles = 10000;
		static const uint32_t MaxVertices = MaxTriangles * 3;
		static const uint32_t MaxIndices = MaxTriangles * 3;
		static const uint32_t MaxTextureSlots = 32;

		static Ref<VertexArray> s_VertexArray;
		static Ref<VertexBuffer> s_VertexBuffer;
		static Ref<Shader> s_Shader;
		static Ref<Texture2D> s_WhiteTexture;

		static uint32_t s_IndexCount;
		static ScopeArray<Vertex3D> s_VertexBufferBase;
		static Vertex3D* s_VertexBufferPtr;

		static std::array<Ref<Texture2D>, MaxTextureSlots> s_TextureSlots;
		static uint32_t s_TextureSlotIndex;

		static RenderStats3D s_Stats;
	};

}
