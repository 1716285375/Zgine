#include "zgpch.h"
#include "ResourceBackendRegistry.h"
#include "IResourceBackend.h"
#include <algorithm>

namespace Zgine {
namespace Resources {
namespace Core {

    ResourceBackendRegistry& ResourceBackendRegistry::GetInstance() {
        static ResourceBackendRegistry instance;
        return instance;
    }

    bool ResourceBackendRegistry::RegisterBackend(const std::string& name, 
                                                 ResourceBackendFactory factory, 
                                                 int priority) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if (m_Backends.find(name) != m_Backends.end()) {
            ZG_CORE_WARN("Resource backend '{}' is already registered", name);
            return false;
        }

        BackendInfo info;
        info.name = name;
        info.factory = factory;
        info.priority = priority;

        m_Backends[name] = info;
        
        // 重新排序后端列表
        m_BackendOrder.clear();
        for (const auto& pair : m_Backends) {
            m_BackendOrder.push_back(pair.first);
        }
        
        std::sort(m_BackendOrder.begin(), m_BackendOrder.end(), 
                 [this](const std::string& a, const std::string& b) {
                     return m_Backends[a].priority < m_Backends[b].priority;
                 });

        ZG_CORE_INFO("Registered resource backend: {} (priority: {})", name, priority);
        return true;
    }

    bool ResourceBackendRegistry::UnregisterBackend(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        auto it = m_Backends.find(name);
        if (it == m_Backends.end()) {
            ZG_CORE_WARN("Resource backend '{}' is not registered", name);
            return false;
        }

        m_Backends.erase(it);
        
        // 从排序列表中移除
        auto orderIt = std::find(m_BackendOrder.begin(), m_BackendOrder.end(), name);
        if (orderIt != m_BackendOrder.end()) {
            m_BackendOrder.erase(orderIt);
        }

        ZG_CORE_INFO("Unregistered resource backend: {}", name);
        return true;
    }

    ResourceBackendRef ResourceBackendRegistry::CreateBackend(const std::string& name) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        auto it = m_Backends.find(name);
        if (it == m_Backends.end()) {
            ZG_CORE_ERROR("Resource backend '{}' is not registered", name);
            return nullptr;
        }

        try {
            ResourceBackendRef backend = it->second.factory();
            if (backend) {
                ZG_CORE_TRACE("Created resource backend instance: {}", name);
                return backend;
            } else {
                ZG_CORE_ERROR("Failed to create resource backend instance: {}", name);
                return nullptr;
            }
        } catch (const std::exception& e) {
            ZG_CORE_ERROR("Exception while creating resource backend '{}': {}", name, e.what());
            return nullptr;
        }
    }

    std::vector<std::string> ResourceBackendRegistry::GetRegisteredBackends() const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_BackendOrder;
    }

    std::vector<std::string> ResourceBackendRegistry::GetBackendsForResourceType(ResourceType type) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::vector<std::string> supportedBackends;
        
        for (const auto& name : m_BackendOrder) {
            try {
                ResourceBackendRef backend = m_Backends.at(name).factory();
                if (backend && backend->SupportsResourceType(type)) {
                    supportedBackends.push_back(name);
                }
            } catch (const std::exception& e) {
                ZG_CORE_WARN("Exception while checking backend '{}' for resource type: {}", name, e.what());
            }
        }
        
        return supportedBackends;
    }

    std::vector<std::string> ResourceBackendRegistry::GetBackendsForFileFormat(const std::string& extension) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::vector<std::string> supportedBackends;
        
        for (const auto& name : m_BackendOrder) {
            try {
                ResourceBackendRef backend = m_Backends.at(name).factory();
                if (backend && backend->SupportsFileFormat(extension)) {
                    supportedBackends.push_back(name);
                }
            } catch (const std::exception& e) {
                ZG_CORE_WARN("Exception while checking backend '{}' for file format: {}", name, e.what());
            }
        }
        
        return supportedBackends;
    }

    bool ResourceBackendRegistry::IsBackendRegistered(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Backends.find(name) != m_Backends.end();
    }

    void ResourceBackendRegistry::Clear() {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Backends.clear();
        m_BackendOrder.clear();
        ZG_CORE_INFO("Cleared all registered resource backends");
    }

} // namespace Core
} // namespace Resources
} // namespace Zgine
