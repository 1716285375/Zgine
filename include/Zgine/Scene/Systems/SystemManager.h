#pragma once

#include <Zgine/Scene/Systems/ISystem.h>
#include <Zgine/Core/Base/Prerequisites.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <typeindex>
#include <unordered_map>

namespace Zgine {

/**
 * @brief Manages registration, lifecycle, and scheduling of game systems
 *
 * SystemManager handles:
 * - System registration and ownership
 * - Priority-based update order
 * - System lifecycle (Initialize/Shutdown)
 * - Type-safe system retrieval
 */
class SystemManager {
public:
    SystemManager() = default;
    ~SystemManager() = default;

    ZGINE_NON_COPYABLE(SystemManager)

    /**
     * @brief Register a new system
     * @tparam T System type (must inherit from ISystem)
     * @tparam Args Constructor argument types
     * @param args Constructor arguments
     * @return Pointer to the registered system
     */
    template<typename T, typename... Args>
    T* RegisterSystem(Args&&... args) {
        static_assert(std::is_base_of<ISystem, T>::value, "T must inherit from ISystem");

        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = system.get();

        m_Systems.push_back(std::move(system));
        m_SystemMap[std::type_index(typeid(T))] = ptr;
        m_Sorted = false;

        return ptr;
    }

    /**
     * @brief Register an existing system instance
     * @tparam T System type
     * @param system Pointer to system (manager does NOT take ownership)
     * @return Pointer to the registered system
     */
    template<typename T>
    T* RegisterExternalSystem(T* system) {
        static_assert(std::is_base_of<ISystem, T>::value, "T must inherit from ISystem");

        m_ExternalSystems.push_back(system);
        m_SystemMap[std::type_index(typeid(T))] = system;
        m_Sorted = false;

        return system;
    }

    /**
     * @brief Get a registered system by type
     * @tparam T System type
     * @return Pointer to the system, or nullptr if not found
     */
    template<typename T>
    T* GetSystem() {
        auto it = m_SystemMap.find(std::type_index(typeid(T)));
        if (it != m_SystemMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }

    /**
     * @brief Check if a system is registered
     * @tparam T System type
     * @return True if the system is registered
     */
    template<typename T>
    bool HasSystem() const {
        return m_SystemMap.find(std::type_index(typeid(T))) != m_SystemMap.end();
    }

    /**
     * @brief Remove a system by type
     * @tparam T System type
     */
    template<typename T>
    void RemoveSystem() {
        auto typeIndex = std::type_index(typeid(T));

        // Remove from map
        m_SystemMap.erase(typeIndex);

        // Remove from owned systems
        m_Systems.erase(
            std::remove_if(m_Systems.begin(), m_Systems.end(),
                [&](const std::unique_ptr<ISystem>& sys) {
                    return std::type_index(typeid(*sys)) == typeIndex;
                }),
            m_Systems.end()
        );

        // Remove from external systems
        m_ExternalSystems.erase(
            std::remove_if(m_ExternalSystems.begin(), m_ExternalSystems.end(),
                [&](ISystem* sys) {
                    return std::type_index(typeid(*sys)) == typeIndex;
                }),
            m_ExternalSystems.end()
        );

        m_Sorted = false;
    }

    /**
     * @brief Initialize all registered systems
     */
    void InitializeAll();

    /**
     * @brief Update all enabled systems with variable delta time
     * @param scene Scene to update
     * @param deltaTime Time elapsed since last frame
     */
    void UpdateAll(Scene* scene, float deltaTime);

    /**
     * @brief Fixed update for all enabled systems
     * @param scene Scene to update
     * @param fixedDeltaTime Fixed time step
     */
    void FixedUpdateAll(Scene* scene, float fixedDeltaTime);

    /**
     * @brief Shutdown all registered systems
     */
    void ShutdownAll();

    /**
     * @brief Clear all systems
     */
    void Clear();

    /**
     * @brief Get number of registered systems
     */
    size_t GetSystemCount() const {
        return m_Systems.size() + m_ExternalSystems.size();
    }

private:
    // Systems owned by manager
    std::vector<std::unique_ptr<ISystem>> m_Systems;

    // External systems (not owned)
    std::vector<ISystem*> m_ExternalSystems;

    // Type index to system pointer map for fast lookup
    std::unordered_map<std::type_index, ISystem*> m_SystemMap;

    // Whether systems are sorted by priority
    bool m_Sorted = false;

    /**
     * @brief Sort systems by priority (lower values first)
     * Uses stable sort to preserve registration order for same priority
     */
    void SortSystemsByPriority();

    /**
     * @brief Get all systems in a single vector for iteration
     */
    std::vector<ISystem*> GetAllSystems();
};

} // namespace Zgine
