#include "zgpch.h"
#include "ObjectPool.h"
#include "BatchRenderer2D.h"
#include "../Core.h"
#include "Zgine/Core/SmartPointers.h"

namespace Zgine {

	// RenderCommandManager static members
	Scope<RenderCommandBatch> RenderCommandManager::s_CurrentBatch;
	std::vector<Scope<RenderCommandBatch>> RenderCommandManager::s_BatchPool;
	size_t RenderCommandManager::s_BatchCapacity = 1000;
	size_t RenderCommandManager::s_TotalCommands = 0;
	size_t RenderCommandManager::s_TotalBatches = 0;
	bool RenderCommandManager::s_Initialized = false;

	void RenderCommandManager::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("RenderCommandManager::Init() called multiple times");
			return;
		}
		
		s_CurrentBatch = CreateScope<RenderCommandBatch>(s_BatchCapacity);
		s_Initialized = true;
		
		ZG_CORE_INFO("RenderCommandManager::Init() completed with batch capacity: {}", s_BatchCapacity);
	}

	void RenderCommandManager::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("RenderCommandManager::Shutdown() called without initialization");
			return;
		}
		
		FlushCurrentBatch();
		s_CurrentBatch.reset();
		s_BatchPool.clear();
		s_Initialized = false;
		
		ZG_CORE_INFO("RenderCommandManager::Shutdown() completed - Total commands: {}, Total batches: {}", 
			s_TotalCommands, s_TotalBatches);
	}

	RenderCommandBatch& RenderCommandManager::GetCurrentBatch()
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("RenderCommandManager::GetCurrentBatch() called without initialization");
			static RenderCommandBatch dummy(0);
			return dummy;
		}
		
		return *s_CurrentBatch;
	}

	void RenderCommandManager::FlushCurrentBatch()
	{
		if (!s_Initialized || !s_CurrentBatch)
			return;
		
		if (s_CurrentBatch->Size() > 0)
		{
			s_CurrentBatch->Flush();
			s_TotalBatches++;
		}
	}

	void RenderCommandManager::ClearAllBatches()
	{
		if (!s_Initialized)
			return;
		
		s_CurrentBatch->Clear();
		for (auto& batch : s_BatchPool)
		{
			batch->Clear();
		}
	}

	void RenderCommandManager::SetBatchCapacity(size_t capacity)
	{
		s_BatchCapacity = capacity;
		if (s_CurrentBatch)
		{
			s_CurrentBatch->SetCapacity(capacity);
		}
	}

	size_t RenderCommandManager::GetTotalCommandCount()
	{
		return s_TotalCommands;
	}

	void RenderCommandManager::AddCommand(const RenderCommand& command)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("RenderCommandManager::AddCommand() called without initialization");
			return;
		}
		
		s_CurrentBatch->AddCommand(command);
		s_TotalCommands++;
	}

	RenderCommandManager::ManagerStats RenderCommandManager::GetStats()
	{
		return {
			s_TotalCommands,
			s_TotalBatches,
			s_TotalBatches > 0 ? s_TotalCommands / s_TotalBatches : 0,
			0.0 // TODO: Implement timing
		};
	}

	// RenderCommandBatch implementation
	void RenderCommandBatch::ProcessQuadBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawQuad(cmd.position, cmd.size, cmd.color);
		}
	}

	void RenderCommandBatch::ProcessLineBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawLine(cmd.p1, cmd.p2, cmd.color, cmd.thickness);
		}
	}

	void RenderCommandBatch::ProcessCircleBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawCircle(cmd.position, cmd.size.x, cmd.color, cmd.thickness);
		}
	}

	void RenderCommandBatch::ProcessTriangleBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawTriangle(cmd.p1, cmd.p2, cmd.p3, cmd.color);
		}
	}

	void RenderCommandBatch::ProcessEllipseBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawEllipse(cmd.position, cmd.size.x, cmd.size.y, cmd.color, cmd.segments);
		}
	}

	void RenderCommandBatch::ProcessArcBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			// Note: DrawArc method needs to be implemented in BatchRenderer2D
			// BatchRenderer2D::DrawArc(cmd.position, cmd.size.x, cmd.rotation, cmd.thickness, cmd.color, cmd.segments);
		}
	}

	void RenderCommandBatch::ProcessQuadGradientBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawQuadGradient(cmd.position, cmd.size, 
				cmd.colorTopLeft, cmd.colorTopRight, cmd.colorBottomLeft, cmd.colorBottomRight);
		}
	}

	void RenderCommandBatch::ProcessRotatedQuadBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawRotatedQuad(cmd.position, cmd.size, cmd.rotation, cmd.color);
		}
	}

	void RenderCommandBatch::ProcessRotatedQuadGradientBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawRotatedQuadGradient(cmd.position, cmd.size, cmd.rotation,
				cmd.colorTopLeft, cmd.colorTopRight, cmd.colorBottomLeft, cmd.colorBottomRight);
		}
	}

	void RenderCommandBatch::ProcessQuadTransformBatch(size_t startIndex, size_t count)
	{
		for (size_t i = startIndex; i < startIndex + count; ++i)
		{
			const auto& cmd = m_Commands[i];
			BatchRenderer2D::DrawQuad(cmd.position, cmd.size, cmd.transform, cmd.color);
		}
	}

}
