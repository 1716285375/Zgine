#include "zgpch.h"
#include "NlohmannJSONBackend.h"
#include <fstream>
#include <sstream>

namespace Zgine {
namespace JSON {

// NlohmannJSONValue实现
NlohmannJSONValue::NlohmannJSONValue(nlohmann::json value) : m_Value(std::move(value)) {}

JSONType NlohmannJSONValue::GetType() const {
    if (m_Value.is_null()) return JSONType::Null;
    if (m_Value.is_boolean()) return JSONType::Bool;
    if (m_Value.is_number()) return JSONType::Number;
    if (m_Value.is_string()) return JSONType::String;
    if (m_Value.is_array()) return JSONType::Array;
    if (m_Value.is_object()) return JSONType::Object;
    return JSONType::Null;
}

bool NlohmannJSONValue::IsNull() const { return m_Value.is_null(); }
bool NlohmannJSONValue::IsBool() const { return m_Value.is_boolean(); }
bool NlohmannJSONValue::IsNumber() const { return m_Value.is_number(); }
bool NlohmannJSONValue::IsString() const { return m_Value.is_string(); }
bool NlohmannJSONValue::IsArray() const { return m_Value.is_array(); }
bool NlohmannJSONValue::IsObject() const { return m_Value.is_object(); }

bool NlohmannJSONValue::AsBool() const { return m_Value.get<bool>(); }
int NlohmannJSONValue::AsInt() const { return m_Value.get<int>(); }
float NlohmannJSONValue::AsFloat() const { return m_Value.get<float>(); }
double NlohmannJSONValue::AsDouble() const { return m_Value.get<double>(); }
const std::string& NlohmannJSONValue::AsString() const { 
    static std::string cached;
    cached = m_Value.get<std::string>();
    return cached;
}

size_t NlohmannJSONValue::Size() const { return m_Value.size(); }

IJSONValue& NlohmannJSONValue::operator[](size_t index) {
    return GetCachedValue(index);
}

const IJSONValue& NlohmannJSONValue::operator[](size_t index) const {
    return const_cast<NlohmannJSONValue*>(this)->GetCachedValue(index);
}

void NlohmannJSONValue::Append(Scope<IJSONValue> value) {
    auto nlohmannValue = dynamic_cast<NlohmannJSONValue*>(value.get());
    if (nlohmannValue) {
        m_Value.push_back(nlohmannValue->GetNlohmannJSON());
    }
}

void NlohmannJSONValue::Append(const IJSONValue& value) {
    auto nlohmannValue = dynamic_cast<const NlohmannJSONValue*>(&value);
    if (nlohmannValue) {
        m_Value.push_back(nlohmannValue->GetNlohmannJSON());
    }
}

bool NlohmannJSONValue::HasKey(const std::string& key) const {
    return m_Value.contains(key);
}

IJSONValue& NlohmannJSONValue::operator[](const std::string& key) {
    return GetCachedValue(key);
}

const IJSONValue& NlohmannJSONValue::operator[](const std::string& key) const {
    return const_cast<NlohmannJSONValue*>(this)->GetCachedValue(key);
}

void NlohmannJSONValue::Set(const std::string& key, Scope<IJSONValue> value) {
    auto nlohmannValue = dynamic_cast<NlohmannJSONValue*>(value.get());
    if (nlohmannValue) {
        m_Value[key] = nlohmannValue->GetNlohmannJSON();
    }
}

void NlohmannJSONValue::Set(const std::string& key, const IJSONValue& value) {
    auto nlohmannValue = dynamic_cast<const NlohmannJSONValue*>(&value);
    if (nlohmannValue) {
        m_Value[key] = nlohmannValue->GetNlohmannJSON();
    }
}

std::string NlohmannJSONValue::ToString(int indent) const {
    if (indent > 0) {
        return m_Value.dump(indent);
    }
    return m_Value.dump();
}

Scope<IJSONValue> NlohmannJSONValue::Clone() const {
    return CreateScope<NlohmannJSONValue>(m_Value);
}

IJSONValue& NlohmannJSONValue::GetCachedValue(size_t index) const {
    if (index >= m_CachedValues.size()) {
        m_CachedValues.resize(index + 1);
    }
    
    if (!m_CachedValues[index]) {
        m_CachedValues[index] = CreateScope<NlohmannJSONValue>(m_Value[index]);
    }
    
    return *m_CachedValues[index];
}

IJSONValue& NlohmannJSONValue::GetCachedValue(const std::string& key) const {
    auto it = m_CachedObjectValues.find(key);
    if (it == m_CachedObjectValues.end()) {
        auto value = CreateScope<NlohmannJSONValue>(m_Value[key]);
        it = m_CachedObjectValues.emplace(key, std::move(value)).first;
    }
    
    return *it->second;
}

// NlohmannJSONFactory实现
Scope<IJSONValue> NlohmannJSONFactory::CreateNull() {
    return CreateScope<NlohmannJSONValue>(nlohmann::json::value_t::null);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateBool(bool value) {
    return CreateScope<NlohmannJSONValue>(value);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateInt(int value) {
    return CreateScope<NlohmannJSONValue>(value);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateFloat(float value) {
    return CreateScope<NlohmannJSONValue>(value);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateDouble(double value) {
    return CreateScope<NlohmannJSONValue>(value);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateString(const std::string& value) {
    return CreateScope<NlohmannJSONValue>(value);
}

Scope<IJSONValue> NlohmannJSONFactory::CreateArray() {
    return CreateScope<NlohmannJSONValue>(nlohmann::json::array());
}

Scope<IJSONValue> NlohmannJSONFactory::CreateObject() {
    return CreateScope<NlohmannJSONValue>(nlohmann::json::object());
}

Scope<IJSONValue> NlohmannJSONFactory::Parse(const std::string& jsonString) {
    try {
        nlohmann::json json = nlohmann::json::parse(jsonString);
        return CreateScope<NlohmannJSONValue>(json);
    } catch (const nlohmann::json::parse_error& e) {
        ZG_CORE_ERROR("JSON parse error: {}", e.what());
        return nullptr;
    }
}

Scope<IJSONValue> NlohmannJSONFactory::ParseFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ZG_CORE_ERROR("Failed to open file: {}", filepath);
            return nullptr;
        }
        
        nlohmann::json json;
        file >> json;
        return CreateScope<NlohmannJSONValue>(json);
    } catch (const nlohmann::json::parse_error& e) {
        ZG_CORE_ERROR("JSON parse error in file {}: {}", filepath, e.what());
        return nullptr;
    } catch (const std::exception& e) {
        ZG_CORE_ERROR("Error reading file {}: {}", filepath, e.what());
        return nullptr;
    }
}

Scope<IJSONValue> NlohmannJSONFactory::CreateFromNlohmann(const nlohmann::json& json) {
    return CreateScope<NlohmannJSONValue>(json);
}

} // namespace JSON
} // namespace Zgine
