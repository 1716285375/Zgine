#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Zgine {

	/**
	 * @brief Bounding box for culling operations
	 * @details Represents an axis-aligned bounding box
	 */
	struct BoundingBox
	{
		glm::vec3 min;
		glm::vec3 max;
		
		BoundingBox() : min(0.0f), max(0.0f) {}
		BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
		
		/**
		 * @brief Expand bounding box to include a point
		 */
		void Expand(const glm::vec3& point)
		{
			min = glm::min(min, point);
			max = glm::max(max, point);
		}
		
		/**
		 * @brief Expand bounding box to include another bounding box
		 */
		void Expand(const BoundingBox& other)
		{
			min = glm::min(min, other.min);
			max = glm::max(max, other.max);
		}
		
		/**
		 * @brief Get center of the bounding box
		 */
		glm::vec3 GetCenter() const
		{
			return (min + max) * 0.5f;
		}
		
		/**
		 * @brief Get size of the bounding box
		 */
		glm::vec3 GetSize() const
		{
			return max - min;
		}
		
		/**
		 * @brief Get radius of the bounding sphere
		 */
		float GetRadius() const
		{
			return glm::length(GetSize()) * 0.5f;
		}
		
		/**
		 * @brief Check if bounding box is valid
		 */
		bool IsValid() const
		{
			return glm::all(glm::lessThanEqual(min, max));
		}
	};

	/**
	 * @brief Bounding sphere for culling operations
	 * @details Represents a sphere for more efficient culling
	 */
	struct BoundingSphere
	{
		glm::vec3 center;
		float radius;
		
		BoundingSphere() : center(0.0f), radius(0.0f) {}
		BoundingSphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}
		
		/**
		 * @brief Expand sphere to include a point
		 */
		void Expand(const glm::vec3& point)
		{
			float distance = glm::length(point - center);
			if (distance > radius)
			{
				radius = distance;
			}
		}
		
		/**
		 * @brief Expand sphere to include another sphere
		 */
		void Expand(const BoundingSphere& other)
		{
			float distance = glm::length(other.center - center);
			float newRadius = distance + other.radius;
			if (newRadius > radius)
			{
				radius = newRadius;
			}
		}
		
		/**
		 * @brief Check if sphere is valid
		 */
		bool IsValid() const
		{
			return radius >= 0.0f;
		}
	};

	/**
	 * @brief Frustum for view culling
	 * @details Represents a camera frustum for culling operations
	 */
	class Frustum
	{
	public:
		Frustum() = default;
		
		/**
		 * @brief Create frustum from view-projection matrix
		 * @param viewProj View-projection matrix
		 */
		explicit Frustum(const glm::mat4& viewProj);
		
		/**
		 * @brief Create frustum from camera parameters
		 * @param position Camera position
		 * @param direction Camera direction
		 * @param up Camera up vector
		 * @param fov Field of view in radians
		 * @param aspect Aspect ratio
		 * @param nearPlane Near plane distance
		 * @param farPlane Far plane distance
		 */
		Frustum(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up,
				float fov, float aspect, float nearPlane, float farPlane);
		
		/**
		 * @brief Check if point is inside frustum
		 */
		bool Contains(const glm::vec3& point) const;
		
		/**
		 * @brief Check if bounding box intersects frustum
		 */
		bool Intersects(const BoundingBox& box) const;
		
		/**
		 * @brief Check if bounding sphere intersects frustum
		 */
		bool Intersects(const BoundingSphere& sphere) const;
		
		/**
		 * @brief Update frustum from view-projection matrix
		 */
		void Update(const glm::mat4& viewProj);

	private:
		enum Plane
		{
			LEFT = 0,
			RIGHT = 1,
			BOTTOM = 2,
			TOP = 3,
			NEAR = 4,
			FAR = 5,
			COUNT = 6
		};
		
		struct PlaneData
		{
			glm::vec3 normal;
			float distance;
		};
		
		PlaneData m_Planes[COUNT];
		
		void ExtractPlanes(const glm::mat4& viewProj);
	};

	/**
	 * @brief Level of Detail (LOD) system
	 * @details Manages different levels of detail for objects based on distance
	 */
	class LODSystem
	{
	public:
		/**
		 * @brief LOD level definition
		 */
		struct LODLevel
		{
			float distance;
			float detail;
			std::string name;
			
			LODLevel(float dist, float det, const std::string& n) 
				: distance(dist), detail(det), name(n) {}
		};
		
		/**
		 * @brief Initialize LOD system
		 */
		static void Init();
		
		/**
		 * @brief Shutdown LOD system
		 */
		static void Shutdown();
		
		/**
		 * @brief Add LOD level
		 * @param distance Distance threshold
		 * @param detail Detail level (0.0 = lowest, 1.0 = highest)
		 * @param name Level name
		 */
		static void AddLODLevel(float distance, float detail, const std::string& name);
		
		/**
		 * @brief Get LOD level for distance
		 * @param distance Distance from camera
		 * @return LOD level index
		 */
		static int GetLODLevel(float distance);
		
		/**
		 * @brief Get LOD level data
		 * @param level LOD level index
		 * @return LOD level data
		 */
		static const LODLevel* GetLODLevelData(int level);
		
		/**
		 * @brief Get all LOD levels
		 */
		static const std::vector<LODLevel>& GetAllLODLevels();
		
		/**
		 * @brief Clear all LOD levels
		 */
		static void ClearLODLevels();
		
		/**
		 * @brief Set LOD bias (affects distance calculations)
		 * @param bias LOD bias multiplier
		 */
		static void SetLODBias(float bias);
		
		/**
		 * @brief Get LOD bias
		 */
		static float GetLODBias();

	private:
		static std::vector<LODLevel> s_LODLevels;
		static float s_LODBias;
		static bool s_Initialized;
	};

	/**
	 * @brief Culling system for efficient rendering
	 * @details Manages frustum culling, occlusion culling, and LOD selection
	 */
	class CullingSystem
	{
	public:
		/**
		 * @brief Renderable object interface
		 */
		class IRenderable
		{
		public:
			virtual ~IRenderable() = default;
			virtual BoundingBox GetBoundingBox() const = 0;
			virtual BoundingSphere GetBoundingSphere() const = 0;
			virtual void Render(int lodLevel = 0) = 0;
			virtual bool IsVisible() const = 0;
			virtual void SetVisible(bool visible) = 0;
		};
		
		/**
		 * @brief Initialize culling system
		 */
		static void Init();
		
		/**
		 * @brief Shutdown culling system
		 */
		static void Shutdown();
		
		/**
		 * @brief Add renderable object
		 * @param id Object ID
		 * @param renderable Renderable object
		 */
		static void AddRenderable(uint32_t id, std::shared_ptr<IRenderable> renderable);
		
		/**
		 * @brief Remove renderable object
		 * @param id Object ID
		 */
		static void RemoveRenderable(uint32_t id);
		
		/**
		 * @brief Update culling system
		 * @param cameraPosition Camera position
		 * @param cameraDirection Camera direction
		 * @param cameraUp Camera up vector
		 * @param fov Field of view
		 * @param aspect Aspect ratio
		 * @param nearPlane Near plane distance
		 * @param farPlane Far plane distance
		 */
		static void Update(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection,
						   const glm::vec3& cameraUp, float fov, float aspect,
						   float nearPlane, float farPlane);
		
		/**
		 * @brief Render all visible objects
		 */
		static void RenderVisible();
		
		/**
		 * @brief Enable/disable frustum culling
		 * @param enabled Whether frustum culling is enabled
		 */
		static void SetFrustumCullingEnabled(bool enabled);
		
		/**
		 * @brief Enable/disable LOD system
		 * @param enabled Whether LOD system is enabled
		 */
		static void SetLODEnabled(bool enabled);
		
		/**
		 * @brief Enable/disable occlusion culling
		 * @param enabled Whether occlusion culling is enabled
		 */
		static void SetOcclusionCullingEnabled(bool enabled);
		
		/**
		 * @brief Get culling statistics
		 */
		struct CullingStats
		{
			uint32_t TotalObjects;
			uint32_t VisibleObjects;
			uint32_t CulledObjects;
			uint32_t LODSwitches;
			double CullingTime;
			double LODTime;
		};
		
		static CullingStats GetStats();
		
		/**
		 * @brief Clear all renderable objects
		 */
		static void Clear();

	private:
		struct RenderableData
		{
			std::shared_ptr<IRenderable> renderable;
			BoundingBox boundingBox;
			BoundingSphere boundingSphere;
			bool isVisible;
			int currentLOD;
			float distanceToCamera;
		};
		
		static std::unordered_map<uint32_t, RenderableData> s_Renderables;
		static Frustum s_Frustum;
		static bool s_FrustumCullingEnabled;
		static bool s_LODEnabled;
		static bool s_OcclusionCullingEnabled;
		static bool s_Initialized;
		
		static CullingStats s_Stats;
		
		static void PerformFrustumCulling();
		static void PerformLODSelection();
		static void PerformOcclusionCulling();
	};

	/**
	 * @brief Spatial partitioning system for efficient culling
	 * @details Divides space into cells for faster culling operations
	 */
	class SpatialPartition
	{
	public:
		/**
		 * @brief Initialize spatial partition
		 * @param worldSize World size
		 * @param cellSize Cell size
		 */
		explicit SpatialPartition(const glm::vec3& worldSize, const glm::vec3& cellSize);
		
		/**
		 * @brief Add object to spatial partition
		 * @param id Object ID
		 * @param boundingBox Object bounding box
		 */
		void AddObject(uint32_t id, const BoundingBox& boundingBox);
		
		/**
		 * @brief Remove object from spatial partition
		 * @param id Object ID
		 */
		void RemoveObject(uint32_t id);
		
		/**
		 * @brief Update object position in spatial partition
		 * @param id Object ID
		 * @param boundingBox New bounding box
		 */
		void UpdateObject(uint32_t id, const BoundingBox& boundingBox);
		
		/**
		 * @brief Get objects in frustum
		 * @param frustum Frustum for culling
		 * @return Vector of object IDs
		 */
		std::vector<uint32_t> GetObjectsInFrustum(const Frustum& frustum) const;
		
		/**
		 * @brief Get objects in bounding box
		 * @param boundingBox Bounding box for query
		 * @return Vector of object IDs
		 */
		std::vector<uint32_t> GetObjectsInBox(const BoundingBox& boundingBox) const;
		
		/**
		 * @brief Get objects in sphere
		 * @param sphere Bounding sphere for query
		 * @return Vector of object IDs
		 */
		std::vector<uint32_t> GetObjectsInSphere(const BoundingSphere& sphere) const;
		
		/**
		 * @brief Clear all objects
		 */
		void Clear();
		
		/**
		 * @brief Get partition statistics
		 */
		struct PartitionStats
		{
			uint32_t TotalCells;
			uint32_t OccupiedCells;
			uint32_t TotalObjects;
			uint32_t MaxObjectsPerCell;
			double AverageObjectsPerCell;
		};
		
		PartitionStats GetStats() const;

	private:
		struct Cell
		{
			std::vector<uint32_t> objects;
		};
		
		glm::vec3 m_WorldSize;
		glm::vec3 m_CellSize;
		glm::ivec3 m_CellCount;
		std::vector<std::vector<std::vector<Cell>>> m_Cells;
		
		glm::ivec3 GetCellIndex(const glm::vec3& position) const;
		bool IsValidCellIndex(const glm::ivec3& index) const;
	};

}
