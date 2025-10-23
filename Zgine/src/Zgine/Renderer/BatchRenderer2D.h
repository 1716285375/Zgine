#pragma once

/**
 * @file BatchRenderer2D.h
 * @brief High-performance 2D batch renderer with memory optimization
 * 
 * This file contains the BatchRenderer2D class which provides efficient 2D rendering
 * capabilities with the following features:
 * - Batch rendering of quads, circles, lines, and other 2D primitives
 * - Dynamic buffer sizing and memory management
 * - Texture batching and caching
 * - Performance monitoring and statistics
 * - Memory pool and ring buffer optimization
 * 
 * @author Zgine Engine Team
 * @version 1.0.0
 * @date 2024
 */

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Texture.h"
#include "Zgine/Renderer/VertexArray.h"
#include "Zgine/Renderer/OrthographicCamera.h"
// Forward declarations for performance optimization classes
namespace Zgine {
	template<typename T> class MemoryPool;
	template<typename T> class RingBuffer;
}

namespace Zgine {

	struct QuadVertex
	{
		glm::vec3 Position;  ///< 3D position of the vertex
		glm::vec4 Color;     ///< RGBA color of the vertex
		glm::vec2 TexCoord;  ///< UV texture coordinates
		float TexIndex;      ///< Texture slot index for batching
	};

	struct RenderStats
	{
		uint32_t DrawCalls = 0;    ///< Number of draw calls made
		uint32_t QuadCount = 0;    ///< Number of quads rendered
		uint32_t VertexCount = 0;  ///< Total number of vertices processed
		uint32_t IndexCount = 0;   ///< Total number of indices processed
		
		uint32_t GetTotalVertexCount() const { return VertexCount; }
		uint32_t GetTotalIndexCount() const { return IndexCount; }
	};

	class BatchRenderer2D
	{
	public:
		/**
		 * @brief Initialize the batch renderer
		 * 
		 * Sets up all necessary OpenGL resources including vertex buffers,
		 * index buffers, shaders, and texture slots. Must be called before
		 * any rendering operations.
		 */
		static void Init();
		
		/**
		 * @brief Shutdown the batch renderer
		 * 
		 * Cleans up all OpenGL resources and memory allocations.
		 * Should be called when the renderer is no longer needed.
		 */
		static void Shutdown();
		
		/**
		 * @brief Check if renderer is initialized
		 * @return true if initialized, false otherwise
		 */
		static bool IsInitialized() { return s_Initialized; }

		/**
		 * @brief Begin a new rendering scene
		 * @param camera The orthographic camera to use for rendering
		 * 
		 * Sets up the camera matrix and prepares the renderer for
		 * drawing operations. Must be called before any Draw* methods.
		 */
		static void BeginScene(const OrthographicCamera& camera);
		
		/**
		 * @brief End the current rendering scene
		 * 
		 * Flushes any remaining batched data and completes the scene.
		 * Should be called after all drawing operations are complete.
		 */
		static void EndScene();
		
		/**
		 * @brief Flush all batched data to the GPU
		 * 
		 * Forces all queued rendering data to be submitted to the GPU.
		 * Called automatically by EndScene() but can be called manually
		 * for more control over batching.
		 */
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

		// Performance optimization methods
		static void SetMaxQuads(uint32_t maxQuads);
		static void SetUseRingBuffer(bool useRingBuffer);
		static void SetEnableTextureCaching(bool enableCaching);
		static void OptimizeForScene(const RenderStats& expectedStats);

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
		// Dynamic buffer sizes based on performance requirements
		static uint32_t s_MaxQuads;
		static uint32_t s_MaxVertices;
		static uint32_t s_MaxIndices;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		// Core rendering objects
		static Ref<VertexArray> s_QuadVertexArray;
		static Ref<VertexBuffer> s_QuadVertexBuffer;
		static Ref<Shader> s_TextureShader;
		static Ref<Texture2D> s_WhiteTexture;

		// Memory management
		static uint32_t s_QuadIndexCount;
		static std::unique_ptr<RingBuffer<QuadVertex>> s_VertexRingBuffer;
		static std::unique_ptr<MemoryPool<QuadVertex>> s_VertexPool;
		static QuadVertex* s_QuadVertexBufferBase;
		static QuadVertex* s_QuadVertexBufferPtr;

		// Texture management
		static std::array<Ref<Texture2D>, MaxTextureSlots> s_TextureSlots;
		static uint32_t s_TextureSlotIndex;
		static std::unordered_map<uint32_t, uint32_t> s_TextureIndexCache; // Texture ID -> Index mapping

		// Precomputed data
		static glm::vec4 s_QuadVertexPositions[4];

		// Statistics and state
		static RenderStats s_Stats;
		static bool s_Initialized;
		static bool s_ShuttingDown;
		
		// Performance optimization flags
		static bool s_UseRingBuffer;
		static bool s_EnableTextureCaching;
	};

}
