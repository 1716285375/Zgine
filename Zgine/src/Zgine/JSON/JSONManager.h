#pragma once

#include "IJSONValue.h"
#include "Zgine/Core/SmartPointers.h"
#include <unordered_map>

namespace Zgine {
namespace JSON {

// JSON管理器 - 管理多个JSON后端
class JSONManager {
public:
    static JSONManager& Instance();
    
    // 设置默认后端
    void SetDefaultBackend(JSONBackend backend);
    
    // 获取工厂
    IJSONValueFactory* GetFactory(JSONBackend backend);
    
    // 创建JSON值
    Scope<IJSONValue> CreateNull(JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateBool(bool value, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateInt(int value, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateFloat(float value, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateDouble(double value, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateString(const std::string& value, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateArray(JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> CreateObject(JSONBackend backend = JSONBackend::Nlohmann);
    
    // 解析JSON
    Scope<IJSONValue> Parse(const std::string& jsonString, JSONBackend backend = JSONBackend::Nlohmann);
    Scope<IJSONValue> ParseFromFile(const std::string& filepath, JSONBackend backend = JSONBackend::Nlohmann);
    
    // 注册后端
    void RegisterBackend(JSONBackend backend, Scope<IJSONValueFactory> factory);
    
    // 检查后端是否可用
    bool IsBackendAvailable(JSONBackend backend) const;

private:
    JSONManager();
    ~JSONManager() = default;
    
    std::unordered_map<JSONBackend, Scope<IJSONValueFactory>> m_Factories;
    JSONBackend m_DefaultBackend;
    
    void InitializeDefaultBackends();
};

} // namespace JSON
} // namespace Zgine
