#include <Zgine/World/Systems/SystemManager.h>
#include <Zgine/Core/Log/Log.h>

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
    ShutdownAll();
    m_Systems.clear();
    m_ExternalSystems.clear();
    m_SystemMap.clear();
    m_Sorted = false;
}

void SystemManager::SortSystemsByPriority() {
    // Sort owned systems
    std::stable_sort(m_Systems.begin(), m_Systems.end(),
        [](const std::unique_ptr<ISystem>& a, const std::unique_ptr<ISystem>& b) {
            return a->GetPriority() < b->GetPriority();
        });

    // Sort external systems
    std::stable_sort(m_ExternalSystems.begin(), m_ExternalSystems.end(),
        [](ISystem* a, ISystem* b) {
            return a->GetPriority() < b->GetPriority();
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

    // Sort by priority if needed
    if (!m_Sorted) {
        std::stable_sort(allSystems.begin(), allSystems.end(),
            [](ISystem* a, ISystem* b) {
                return a->GetPriority() < b->GetPriority();
            });
    }

    return allSystems;
}

} // namespace Zgine
