#include "zgpch.h"
#include "JSONManager.h"
#include "NlohmannJSONBackend.h"
#include "RapidJSONBackend.h"
#include "Zgine/Log.h"
#include "Zgine/Core/SmartPointers.h"
#include <fstream>
#include <sstream>

namespace Zgine {
namespace JSON {

JSONManager& JSONManager::Instance() {
    static JSONManager instance;
    return instance;
}

JSONManager::JSONManager() : m_DefaultBackend(JSONBackend::Nlohmann) {
    InitializeDefaultBackends();
}

void JSONManager::InitializeDefaultBackends() {
    // 注册nlohmann/json后端
    m_Factories[JSONBackend::Nlohmann] = CreateScope<NlohmannJSONFactory>();
    
    // 注册RapidJSON后端
    m_Factories[JSONBackend::RapidJSON] = CreateScope<RapidJSONFactory>();
    
    ZG_CORE_INFO("Initialized JSON backends: Nlohmann, RapidJSON");
}

void JSONManager::SetDefaultBackend(JSONBackend backend) {
    if (IsBackendAvailable(backend)) {
        m_DefaultBackend = backend;
        ZG_CORE_INFO("Set default JSON backend to: {}", static_cast<int>(backend));
    } else {
        ZG_CORE_WARN("JSON backend {} is not available", static_cast<int>(backend));
    }
}

IJSONValueFactory* JSONManager::GetFactory(JSONBackend backend) {
    auto it = m_Factories.find(backend);
    if (it != m_Factories.end()) {
        return it->second.get();
    }
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateNull(JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateNull();
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateBool(bool value, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateBool(value);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateInt(int value, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateInt(value);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateFloat(float value, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateFloat(value);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateDouble(double value, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateDouble(value);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateString(const std::string& value, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateString(value);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateArray(JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateArray();
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::CreateObject(JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->CreateObject();
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::Parse(const std::string& jsonString, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->Parse(jsonString);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

Scope<IJSONValue> JSONManager::ParseFromFile(const std::string& filepath, JSONBackend backend) {
    auto factory = GetFactory(backend);
    if (factory) {
        return factory->ParseFromFile(filepath);
    }
    ZG_CORE_ERROR("JSON backend {} is not available", static_cast<int>(backend));
    return nullptr;
}

void JSONManager::RegisterBackend(JSONBackend backend, Scope<IJSONValueFactory> factory) {
    m_Factories[backend] = std::move(factory);
    ZG_CORE_INFO("Registered JSON backend: {}", static_cast<int>(backend));
}

bool JSONManager::IsBackendAvailable(JSONBackend backend) const {
    return m_Factories.find(backend) != m_Factories.end();
}

} // namespace JSON
} // namespace Zgine
