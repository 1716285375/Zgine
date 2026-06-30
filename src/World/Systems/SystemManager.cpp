#include <Zgine/World/Systems/SystemManager.h>
#include <Zgine/Core/Log/Log.h>
#include <limits>

namespace Zgine {

void SystemManager::InitializeAll() {
    SortSystemsByPriority();

    auto allSystems = GetAllSystems();
    for (ISystem* system : allSystems) {
        if (system && system->IsEnabled()) {
            ZGINE_CORE_INFO("Initializing system: {}", system->GetName());
            system->Initialize();
        }
    }
}

void SystemManager::UpdateAll(World* World, float deltaTime) {
    if (!m_Sorted) {
        SortSystemsByPriority();
    }

    auto allSystems = GetAllSystems();
    for (ISystem* system : allSystems) {
        if (system && system->IsEnabled()) {
            system->Update(World, deltaTime);
        }
    }
}

void SystemManager::StartScene(World* World) {
    if (!World) {
        ZGINE_CORE_WARN("SystemManager::StartScene ignored null World.");
        return;
    }

    if (m_SceneRunning && m_ActiveScene == World) {
        return;
    }

    if (m_SceneRunning) {
        StopScene();
    }

    if (!m_Sorted) {
        SortSystemsByPriority();
    }

    m_ActiveScene = World;
    m_SceneRunning = true;

    auto allSystems = GetAllSystems();
    for (ISystem* system : allSystems) {
        if (system && system->IsEnabled()) {
            ZGINE_CORE_INFO("Starting scene for system: {}", system->GetName());
            system->OnSceneStart(World);
        }
    }
}

void SystemManager::StopScene() {
    if (!m_SceneRunning) {
        m_ActiveScene = nullptr;
        return;
    }

    auto allSystems = GetAllSystems();
    for (auto it = allSystems.rbegin(); it != allSystems.rend(); ++it) {
        ISystem* system = *it;
        if (system) {
            ZGINE_CORE_INFO("Stopping scene for system: {}", system->GetName());
            system->OnSceneStop();
        }
    }

    m_ActiveScene = nullptr;
    m_SceneRunning = false;
}

void SystemManager::FixedUpdateAll(World* World, float fixedDeltaTime) {
    if (!m_Sorted) {
        SortSystemsByPriority();
    }

    auto allSystems = GetAllSystems();
    for (ISystem* system : allSystems) {
        if (system && system->IsEnabled()) {
            system->FixedUpdate(World, fixedDeltaTime);
        }
    }
}

void SystemManager::ShutdownAll() {
    StopScene();

    // Shutdown in reverse order
    auto allSystems = GetAllSystems();
    for (auto it = allSystems.rbegin(); it != allSystems.rend(); ++it) {
        ISystem* system = *it;
        if (system) {
            ZGINE_CORE_INFO("Shutting down system: {}", system->GetName());
            system->Shutdown();
        }
    }
}

void SystemManager::Clear() {
    StopScene();
    ShutdownAll();
    m_Systems.clear();
    m_ExternalSystems.clear();
    m_SystemMap.clear();
    m_RegistrationOrder.clear();
    m_NextRegistrationOrder = 0;
    m_Sorted = false;
    m_ActiveScene = nullptr;
    m_SceneRunning = false;
}

void SystemManager::SortSystemsByPriority() {
    // Sort owned systems
    std::stable_sort(m_Systems.begin(), m_Systems.end(),
        [this](const std::unique_ptr<ISystem>& a, const std::unique_ptr<ISystem>& b) {
            if (a->GetPriority() != b->GetPriority()) {
                return a->GetPriority() < b->GetPriority();
            }
            return m_RegistrationOrder[a.get()] < m_RegistrationOrder[b.get()];
        });

    // Sort external systems
    std::stable_sort(m_ExternalSystems.begin(), m_ExternalSystems.end(),
        [this](ISystem* a, ISystem* b) {
            if (a->GetPriority() != b->GetPriority()) {
                return a->GetPriority() < b->GetPriority();
            }
            return m_RegistrationOrder[a] < m_RegistrationOrder[b];
        });

    m_Sorted = true;
}

std::vector<ISystem*> SystemManager::GetAllSystems() {
    std::vector<ISystem*> allSystems;
    allSystems.reserve(m_Systems.size() + m_ExternalSystems.size());

    // Add owned systems
    for (auto& system : m_Systems) {
        allSystems.push_back(system.get());
    }

    // Add external systems
    for (auto* system : m_ExternalSystems) {
        allSystems.push_back(system);
    }

    // Sort the final execution list globally. Owned and external systems must
    // interleave by priority; registration order is the stable tie-breaker.
    std::stable_sort(allSystems.begin(), allSystems.end(),
        [this](ISystem* a, ISystem* b) {
            if (a->GetPriority() != b->GetPriority()) {
                return a->GetPriority() < b->GetPriority();
            }

            auto aOrderIt = m_RegistrationOrder.find(a);
            auto bOrderIt = m_RegistrationOrder.find(b);
            size_t aOrder = aOrderIt != m_RegistrationOrder.end()
                ? aOrderIt->second
                : std::numeric_limits<size_t>::max();
            size_t bOrder = bOrderIt != m_RegistrationOrder.end()
                ? bOrderIt->second
                : std::numeric_limits<size_t>::max();
            return aOrder < bOrder;
        });

    return allSystems;
}

} // namespace Zgine
