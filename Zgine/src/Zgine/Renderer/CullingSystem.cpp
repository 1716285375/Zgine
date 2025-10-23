#include "CullingSystem.h"
#include "../Core.h"
#include <algorithm>
#include <cmath>

namespace Zgine {

	// LODSystem static members
	std::vector<LODSystem::LODLevel> LODSystem::s_LODLevels;
	float LODSystem::s_LODBias = 1.0f;
	bool LODSystem::s_Initialized = false;

	// CullingSystem static members
	std::unordered_map<uint32_t, CullingSystem::RenderableData> CullingSystem::s_Renderables;
	Frustum CullingSystem::s_Frustum;
	bool CullingSystem::s_FrustumCullingEnabled = true;
	bool CullingSystem::s_LODEnabled = true;
	bool CullingSystem::s_OcclusionCullingEnabled = false;
	bool CullingSystem::s_Initialized = false;
	CullingSystem::CullingStats CullingSystem::s_Stats{};

	// Frustum implementation
	Frustum::Frustum(const glm::mat4& viewProj)
	{
		ExtractPlanes(viewProj);
	}

	Frustum::Frustum(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up,
					  float fov, float aspect, float nearPlane, float farPlane)
	{
		// Calculate view-projection matrix
		glm::mat4 view = glm::lookAt(position, position + direction, up);
		glm::mat4 proj = glm::perspective(fov, aspect, nearPlane, farPlane);
		glm::mat4 viewProj = proj * view;
		
		ExtractPlanes(viewProj);
	}

	bool Frustum::Contains(const glm::vec3& point) const
	{
		for (int i = 0; i < COUNT; ++i)
		{
			float distance = glm::dot(m_Planes[i].normal, point) + m_Planes[i].distance;
			if (distance < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	bool Frustum::Intersects(const BoundingBox& box) const
	{
		for (int i = 0; i < COUNT; ++i)
		{
			const auto& plane = m_Planes[i];
			
			// Find the positive vertex (furthest in the direction of the plane normal)
			glm::vec3 positiveVertex = box.min;
			if (plane.normal.x >= 0.0f) positiveVertex.x = box.max.x;
			if (plane.normal.y >= 0.0f) positiveVertex.y = box.max.y;
			if (plane.normal.z >= 0.0f) positiveVertex.z = box.max.z;
			
			float distance = glm::dot(plane.normal, positiveVertex) + plane.distance;
			if (distance < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	bool Frustum::Intersects(const BoundingSphere& sphere) const
	{
		for (int i = 0; i < COUNT; ++i)
		{
			const auto& plane = m_Planes[i];
			float distance = glm::dot(plane.normal, sphere.center) + plane.distance;
			
			if (distance < -sphere.radius)
			{
				return false;
			}
		}
		return true;
	}

	void Frustum::Update(const glm::mat4& viewProj)
	{
		ExtractPlanes(viewProj);
	}

	void Frustum::ExtractPlanes(const glm::mat4& viewProj)
	{
		// Extract planes from view-projection matrix
		// Left plane
		m_Planes[LEFT].normal.x = viewProj[0][3] + viewProj[0][0];
		m_Planes[LEFT].normal.y = viewProj[1][3] + viewProj[1][0];
		m_Planes[LEFT].normal.z = viewProj[2][3] + viewProj[2][0];
		m_Planes[LEFT].distance = viewProj[3][3] + viewProj[3][0];
		
		// Right plane
		m_Planes[RIGHT].normal.x = viewProj[0][3] - viewProj[0][0];
		m_Planes[RIGHT].normal.y = viewProj[1][3] - viewProj[1][0];
		m_Planes[RIGHT].normal.z = viewProj[2][3] - viewProj[2][0];
		m_Planes[RIGHT].distance = viewProj[3][3] - viewProj[3][0];
		
		// Bottom plane
		m_Planes[BOTTOM].normal.x = viewProj[0][3] + viewProj[0][1];
		m_Planes[BOTTOM].normal.y = viewProj[1][3] + viewProj[1][1];
		m_Planes[BOTTOM].normal.z = viewProj[2][3] + viewProj[2][1];
		m_Planes[BOTTOM].distance = viewProj[3][3] + viewProj[3][1];
		
		// Top plane
		m_Planes[TOP].normal.x = viewProj[0][3] - viewProj[0][1];
		m_Planes[TOP].normal.y = viewProj[1][3] - viewProj[1][1];
		m_Planes[TOP].normal.z = viewProj[2][3] - viewProj[2][1];
		m_Planes[TOP].distance = viewProj[3][3] - viewProj[3][1];
		
		// Near plane
		m_Planes[NEAR].normal.x = viewProj[0][3] + viewProj[0][2];
		m_Planes[NEAR].normal.y = viewProj[1][3] + viewProj[1][2];
		m_Planes[NEAR].normal.z = viewProj[2][3] + viewProj[2][2];
		m_Planes[NEAR].distance = viewProj[3][3] + viewProj[3][2];
		
		// Far plane
		m_Planes[FAR].normal.x = viewProj[0][3] - viewProj[0][2];
		m_Planes[FAR].normal.y = viewProj[1][3] - viewProj[1][2];
		m_Planes[FAR].normal.z = viewProj[2][3] - viewProj[2][2];
		m_Planes[FAR].distance = viewProj[3][3] - viewProj[3][2];
		
		// Normalize all planes
		for (int i = 0; i < COUNT; ++i)
		{
			float length = glm::length(m_Planes[i].normal);
			if (length > 0.0f)
			{
				m_Planes[i].normal /= length;
				m_Planes[i].distance /= length;
			}
		}
	}

	// LODSystem implementation
	void LODSystem::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("LODSystem::Init() called multiple times");
			return;
		}
		
		// Add default LOD levels
		s_LODLevels.emplace_back(0.0f, 1.0f, "High");
		s_LODLevels.emplace_back(50.0f, 0.7f, "Medium");
		s_LODLevels.emplace_back(100.0f, 0.4f, "Low");
		s_LODLevels.emplace_back(200.0f, 0.1f, "Very Low");
		
		s_Initialized = true;
		ZG_CORE_INFO("LODSystem::Init() completed with {} LOD levels", s_LODLevels.size());
	}

	void LODSystem::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("LODSystem::Shutdown() called without initialization");
			return;
		}
		
		s_LODLevels.clear();
		s_Initialized = false;
		ZG_CORE_INFO("LODSystem::Shutdown() completed");
	}

	void LODSystem::AddLODLevel(float distance, float detail, const std::string& name)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("LODSystem::AddLODLevel() called without initialization");
			return;
		}
		
		s_LODLevels.emplace_back(distance, detail, name);
		
		// Sort by distance
		std::sort(s_LODLevels.begin(), s_LODLevels.end(), 
			[](const LODLevel& a, const LODLevel& b) {
				return a.distance < b.distance;
			});
		
		ZG_CORE_TRACE("LODSystem::AddLODLevel() - Added level '{}' at distance {}", name, distance);
	}

	int LODSystem::GetLODLevel(float distance)
	{
		if (!s_Initialized || s_LODLevels.empty())
		{
			return 0;
		}
		
		distance *= s_LODBias;
		
		for (int i = static_cast<int>(s_LODLevels.size()) - 1; i >= 0; --i)
		{
			if (distance >= s_LODLevels[i].distance)
			{
				return i;
			}
		}
		
		return 0;
	}

	const LODSystem::LODLevel* LODSystem::GetLODLevelData(int level)
	{
		if (!s_Initialized || level < 0 || level >= static_cast<int>(s_LODLevels.size()))
		{
			return nullptr;
		}
		
		return &s_LODLevels[level];
	}

	const std::vector<LODSystem::LODLevel>& LODSystem::GetAllLODLevels()
	{
		return s_LODLevels;
	}

	void LODSystem::ClearLODLevels()
	{
		s_LODLevels.clear();
		ZG_CORE_INFO("LODSystem::ClearLODLevels() - Cleared all LOD levels");
	}

	void LODSystem::SetLODBias(float bias)
	{
		s_LODBias = bias;
		ZG_CORE_INFO("LODSystem::SetLODBias() - Set to {}", bias);
	}

	float LODSystem::GetLODBias()
	{
		return s_LODBias;
	}

	// CullingSystem implementation
	void CullingSystem::Init()
	{
		if (s_Initialized)
		{
			ZG_CORE_WARN("CullingSystem::Init() called multiple times");
			return;
		}
		
		LODSystem::Init();
		s_Initialized = true;
		
		ZG_CORE_INFO("CullingSystem::Init() completed");
	}

	void CullingSystem::Shutdown()
	{
		if (!s_Initialized)
		{
			ZG_CORE_WARN("CullingSystem::Shutdown() called without initialization");
			return;
		}
		
		s_Renderables.clear();
		LODSystem::Shutdown();
		s_Initialized = false;
		
		ZG_CORE_INFO("CullingSystem::Shutdown() completed");
	}

	void CullingSystem::AddRenderable(uint32_t id, std::shared_ptr<IRenderable> renderable)
	{
		if (!s_Initialized)
		{
			ZG_CORE_ERROR("CullingSystem::AddRenderable() called without initialization");
			return;
		}
		
		if (!renderable)
		{
			ZG_CORE_ERROR("CullingSystem::AddRenderable() - Null renderable");
			return;
		}
		
		RenderableData data;
		data.renderable = renderable;
		data.boundingBox = renderable->GetBoundingBox();
		data.boundingSphere = renderable->GetBoundingSphere();
		data.isVisible = true;
		data.currentLOD = 0;
		data.distanceToCamera = 0.0f;
		
		s_Renderables[id] = data;
		
		ZG_CORE_TRACE("CullingSystem::AddRenderable() - Added object {}", id);
	}

	void CullingSystem::RemoveRenderable(uint32_t id)
	{
		auto it = s_Renderables.find(id);
		if (it != s_Renderables.end())
		{
			s_Renderables.erase(it);
			ZG_CORE_TRACE("CullingSystem::RemoveRenderable() - Removed object {}", id);
		}
	}

	void CullingSystem::Update(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection,
								const glm::vec3& cameraUp, float fov, float aspect,
								float nearPlane, float farPlane)
	{
		if (!s_Initialized) return;
		
		// Update frustum
		s_Frustum = Frustum(cameraPosition, cameraDirection, cameraUp, fov, aspect, nearPlane, farPlane);
		
		// Update distances to camera
		for (auto& pair : s_Renderables)
		{
			auto& data = pair.second;
			glm::vec3 center = data.boundingSphere.center;
			data.distanceToCamera = glm::length(center - cameraPosition);
		}
		
		// Perform culling operations
		if (s_FrustumCullingEnabled)
		{
			PerformFrustumCulling();
		}
		
		if (s_LODEnabled)
		{
			PerformLODSelection();
		}
		
		if (s_OcclusionCullingEnabled)
		{
			PerformOcclusionCulling();
		}
	}

	void CullingSystem::RenderVisible()
	{
		if (!s_Initialized) return;
		
		uint32_t renderedCount = 0;
		
		for (const auto& pair : s_Renderables)
		{
			const auto& data = pair.second;
			if (data.isVisible && data.renderable->IsVisible())
			{
				data.renderable->Render(data.currentLOD);
				renderedCount++;
			}
		}
		
		ZG_CORE_TRACE("CullingSystem::RenderVisible() - Rendered {} objects", renderedCount);
	}

	void CullingSystem::SetFrustumCullingEnabled(bool enabled)
	{
		s_FrustumCullingEnabled = enabled;
		ZG_CORE_INFO("CullingSystem::SetFrustumCullingEnabled() - {}", enabled ? "enabled" : "disabled");
	}

	void CullingSystem::SetLODEnabled(bool enabled)
	{
		s_LODEnabled = enabled;
		ZG_CORE_INFO("CullingSystem::SetLODEnabled() - {}", enabled ? "enabled" : "disabled");
	}

	void CullingSystem::SetOcclusionCullingEnabled(bool enabled)
	{
		s_OcclusionCullingEnabled = enabled;
		ZG_CORE_INFO("CullingSystem::SetOcclusionCullingEnabled() - {}", enabled ? "enabled" : "disabled");
	}

	CullingSystem::CullingStats CullingSystem::GetStats()
	{
		s_Stats.TotalObjects = static_cast<uint32_t>(s_Renderables.size());
		s_Stats.VisibleObjects = 0;
		s_Stats.CulledObjects = 0;
		
		for (const auto& pair : s_Renderables)
		{
			if (pair.second.isVisible)
			{
				s_Stats.VisibleObjects++;
			}
			else
			{
				s_Stats.CulledObjects++;
			}
		}
		
		return s_Stats;
	}

	void CullingSystem::Clear()
	{
		s_Renderables.clear();
		ZG_CORE_INFO("CullingSystem::Clear() - Cleared all renderables");
	}

	void CullingSystem::PerformFrustumCulling()
	{
		for (auto& pair : s_Renderables)
		{
			auto& data = pair.second;
			data.isVisible = s_Frustum.Intersects(data.boundingSphere);
		}
	}

	void CullingSystem::PerformLODSelection()
	{
		for (auto& pair : s_Renderables)
		{
			auto& data = pair.second;
			if (data.isVisible)
			{
				int newLOD = LODSystem::GetLODLevel(data.distanceToCamera);
				if (newLOD != data.currentLOD)
				{
					data.currentLOD = newLOD;
					s_Stats.LODSwitches++;
				}
			}
		}
	}

	void CullingSystem::PerformOcclusionCulling()
	{
		// Simplified occlusion culling - in practice, you'd use hardware occlusion queries
		// For now, we'll just mark objects as visible if they pass frustum culling
		ZG_CORE_TRACE("CullingSystem::PerformOcclusionCulling() - Simplified implementation");
	}

	// SpatialPartition implementation
	SpatialPartition::SpatialPartition(const glm::vec3& worldSize, const glm::vec3& cellSize)
		: m_WorldSize(worldSize), m_CellSize(cellSize)
	{
		m_CellCount = glm::ivec3(
			static_cast<int>(std::ceil(worldSize.x / cellSize.x)),
			static_cast<int>(std::ceil(worldSize.y / cellSize.y)),
			static_cast<int>(std::ceil(worldSize.z / cellSize.z))
		);
		
		m_Cells.resize(m_CellCount.x);
		for (int x = 0; x < m_CellCount.x; ++x)
		{
			m_Cells[x].resize(m_CellCount.y);
			for (int y = 0; y < m_CellCount.y; ++y)
			{
				m_Cells[x][y].resize(m_CellCount.z);
			}
		}
		
		ZG_CORE_INFO("SpatialPartition::SpatialPartition() - Created {}x{}x{} grid", 
			m_CellCount.x, m_CellCount.y, m_CellCount.z);
	}

	void SpatialPartition::AddObject(uint32_t id, const BoundingBox& boundingBox)
	{
		glm::ivec3 minCell = GetCellIndex(boundingBox.min);
		glm::ivec3 maxCell = GetCellIndex(boundingBox.max);
		
		for (int x = minCell.x; x <= maxCell.x; ++x)
		{
			for (int y = minCell.y; y <= maxCell.y; ++y)
			{
				for (int z = minCell.z; z <= maxCell.z; ++z)
				{
					glm::ivec3 cellIndex(x, y, z);
					if (IsValidCellIndex(cellIndex))
					{
						m_Cells[x][y][z].objects.push_back(id);
					}
				}
			}
		}
	}

	void SpatialPartition::RemoveObject(uint32_t id)
	{
		for (int x = 0; x < m_CellCount.x; ++x)
		{
			for (int y = 0; y < m_CellCount.y; ++y)
			{
				for (int z = 0; z < m_CellCount.z; ++z)
				{
					auto& objects = m_Cells[x][y][z].objects;
					objects.erase(std::remove(objects.begin(), objects.end(), id), objects.end());
				}
			}
		}
	}

	void SpatialPartition::UpdateObject(uint32_t id, const BoundingBox& boundingBox)
	{
		RemoveObject(id);
		AddObject(id, boundingBox);
	}

	std::vector<uint32_t> SpatialPartition::GetObjectsInFrustum(const Frustum& frustum) const
	{
		std::vector<uint32_t> result;
		
		// Simplified implementation - check all cells
		for (int x = 0; x < m_CellCount.x; ++x)
		{
			for (int y = 0; y < m_CellCount.y; ++y)
			{
				for (int z = 0; z < m_CellCount.z; ++z)
				{
					const auto& objects = m_Cells[x][y][z].objects;
					for (uint32_t id : objects)
					{
						if (std::find(result.begin(), result.end(), id) == result.end())
						{
							result.push_back(id);
						}
					}
				}
			}
		}
		
		return result;
	}

	std::vector<uint32_t> SpatialPartition::GetObjectsInBox(const BoundingBox& boundingBox) const
	{
		std::vector<uint32_t> result;
		
		glm::ivec3 minCell = GetCellIndex(boundingBox.min);
		glm::ivec3 maxCell = GetCellIndex(boundingBox.max);
		
		for (int x = minCell.x; x <= maxCell.x; ++x)
		{
			for (int y = minCell.y; y <= maxCell.y; ++y)
			{
				for (int z = minCell.z; z <= maxCell.z; ++z)
				{
					glm::ivec3 cellIndex(x, y, z);
					if (IsValidCellIndex(cellIndex))
					{
						const auto& objects = m_Cells[x][y][z].objects;
						for (uint32_t id : objects)
						{
							if (std::find(result.begin(), result.end(), id) == result.end())
							{
								result.push_back(id);
							}
						}
					}
				}
			}
		}
		
		return result;
	}

	std::vector<uint32_t> SpatialPartition::GetObjectsInSphere(const BoundingSphere& sphere) const
	{
		// Convert sphere to bounding box for simplicity
		BoundingBox box(
			sphere.center - glm::vec3(sphere.radius),
			sphere.center + glm::vec3(sphere.radius)
		);
		
		return GetObjectsInBox(box);
	}

	void SpatialPartition::Clear()
	{
		for (int x = 0; x < m_CellCount.x; ++x)
		{
			for (int y = 0; y < m_CellCount.y; ++y)
			{
				for (int z = 0; z < m_CellCount.z; ++z)
				{
					m_Cells[x][y][z].objects.clear();
				}
			}
		}
	}

	SpatialPartition::PartitionStats SpatialPartition::GetStats() const
	{
		PartitionStats stats{};
		stats.TotalCells = m_CellCount.x * m_CellCount.y * m_CellCount.z;
		stats.MaxObjectsPerCell = 0;
		uint32_t totalObjects = 0;
		uint32_t occupiedCells = 0;
		
		for (int x = 0; x < m_CellCount.x; ++x)
		{
			for (int y = 0; y < m_CellCount.y; ++y)
			{
				for (int z = 0; z < m_CellCount.z; ++z)
				{
					uint32_t objectCount = static_cast<uint32_t>(m_Cells[x][y][z].objects.size());
					if (objectCount > 0)
					{
						occupiedCells++;
						totalObjects += objectCount;
						stats.MaxObjectsPerCell = std::max(stats.MaxObjectsPerCell, objectCount);
					}
				}
			}
		}
		
		stats.OccupiedCells = occupiedCells;
		stats.TotalObjects = totalObjects;
		stats.AverageObjectsPerCell = occupiedCells > 0 ? static_cast<double>(totalObjects) / occupiedCells : 0.0;
		
		return stats;
	}

	glm::ivec3 SpatialPartition::GetCellIndex(const glm::vec3& position) const
	{
		return glm::ivec3(
			static_cast<int>(position.x / m_CellSize.x),
			static_cast<int>(position.y / m_CellSize.y),
			static_cast<int>(position.z / m_CellSize.z)
		);
	}

	bool SpatialPartition::IsValidCellIndex(const glm::ivec3& index) const
	{
		return index.x >= 0 && index.x < m_CellCount.x &&
			   index.y >= 0 && index.y < m_CellCount.y &&
			   index.z >= 0 && index.z < m_CellCount.z;
	}

}
