#pragma once

#include "../Core.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"

namespace Zgine {

	class RendererManager
	{
	public:
		static RendererManager& GetInstance()
		{
			static RendererManager instance;
			return instance;
		}

		void Init()
		{
			if (m_Initialized)
				return;
				
			ZG_CORE_INFO("RendererManager::Init() called");
			BatchRenderer2D::Init();
			BatchRenderer3D::Init();
			m_Initialized = true;
			ZG_CORE_INFO("RendererManager::Init() completed");
		}

		void Shutdown()
		{
			if (!m_Initialized)
				return;
				
			ZG_CORE_INFO("RendererManager::Shutdown() called");
			m_ShuttingDown = true;
			
			BatchRenderer3D::Shutdown();
			BatchRenderer2D::Shutdown();
			
			m_Initialized = false;
			m_ShuttingDown = false;
			ZG_CORE_INFO("RendererManager::Shutdown() completed");
		}

		bool IsInitialized() const { return m_Initialized; }
		bool IsShuttingDown() const { return m_ShuttingDown; }

	private:
		RendererManager() = default;
		~RendererManager() = default;
		RendererManager(const RendererManager&) = delete;
		RendererManager& operator=(const RendererManager&) = delete;

		bool m_Initialized = false;
		bool m_ShuttingDown = false;
	};

}
