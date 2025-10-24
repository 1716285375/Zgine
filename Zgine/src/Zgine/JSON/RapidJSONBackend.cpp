#include "zgpch.h"
#include "RapidJSONBackend.h"
#include <fstream>
#include <sstream>

namespace Zgine {
namespace JSON {

// RapidJSONValue实现
RapidJSONValue::RapidJSONValue(rapidjson::Value value, rapidjson::Document::AllocatorType& allocator)
    : m_Value(std::move(value)), m_Allocator(allocator) {}

JSONType RapidJSONValue::GetType() const {
    if (m_Value.IsNull()) return JSONType::Null;
    if (m_Value.IsBool()) return JSONType::Bool;
    if (m_Value.IsNumber()) return JSONType::Number;
    if (m_Value.IsString()) return JSONType::String;
    if (m_Value.IsArray()) return JSONType::Array;
    if (m_Value.IsObject()) return JSONType::Object;
    return JSONType::Null;
}

bool RapidJSONValue::IsNull() const { return m_Value.IsNull(); }
bool RapidJSONValue::IsBool() const { return m_Value.IsBool(); }
bool RapidJSONValue::IsNumber() const { return m_Value.IsNumber(); }
bool RapidJSONValue::IsString() const { return m_Value.IsString(); }
bool RapidJSONValue::IsArray() const { return m_Value.IsArray(); }
bool RapidJSONValue::IsObject() const { return m_Value.IsObject(); }

bool RapidJSONValue::AsBool() const { return m_Value.GetBool(); }
int RapidJSONValue::AsInt() const { return m_Value.GetInt(); }
float RapidJSONValue::AsFloat() const { return m_Value.GetFloat(); }
double RapidJSONValue::AsDouble() const { return m_Value.GetDouble(); }
const std::string& RapidJSONValue::AsString() const { 
    m_CachedString = m_Value.GetString();
    return m_CachedString;
}

size_t RapidJSONValue::Size() const { return m_Value.Size(); }

IJSONValue& RapidJSONValue::operator[](size_t index) {
    return GetCachedValue(index);
}

const IJSONValue& RapidJSONValue::operator[](size_t index) const {
    return const_cast<RapidJSONValue*>(this)->GetCachedValue(index);
}

void RapidJSONValue::Append(std::unique_ptr<IJSONValue> value) {
    auto rapidValue = dynamic_cast<RapidJSONValue*>(value.get());
    if (rapidValue) {
        m_Value.PushBack(const_cast<rapidjson::Value&>(rapidValue->GetRapidJSONValue()), m_Allocator);
    }
}

void RapidJSONValue::Append(const IJSONValue& value) {
    auto rapidValue = dynamic_cast<const RapidJSONValue*>(&value);
    if (rapidValue) {
        m_Value.PushBack(const_cast<rapidjson::Value&>(rapidValue->GetRapidJSONValue()), m_Allocator);
    }
}

bool RapidJSONValue::HasKey(const std::string& key) const {
    return m_Value.HasMember(key.c_str());
}

IJSONValue& RapidJSONValue::operator[](const std::string& key) {
    return GetCachedValue(key);
}

const IJSONValue& RapidJSONValue::operator[](const std::string& key) const {
    return const_cast<RapidJSONValue*>(this)->GetCachedValue(key);
}

void RapidJSONValue::Set(const std::string& key, std::unique_ptr<IJSONValue> value) {
    auto rapidValue = dynamic_cast<RapidJSONValue*>(value.get());
    if (rapidValue) {
        rapidjson::Value keyValue(key.c_str(), m_Allocator);
        m_Value.AddMember(keyValue, const_cast<rapidjson::Value&>(rapidValue->GetRapidJSONValue()), m_Allocator);
    }
}

void RapidJSONValue::Set(const std::string& key, const IJSONValue& value) {
    auto rapidValue = dynamic_cast<const RapidJSONValue*>(&value);
    if (rapidValue) {
        rapidjson::Value keyValue(key.c_str(), m_Allocator);
        m_Value.AddMember(keyValue, const_cast<rapidjson::Value&>(rapidValue->GetRapidJSONValue()), m_Allocator);
    }
}

std::string RapidJSONValue::ToString(int indent) const {
    rapidjson::StringBuffer buffer;
    
    if (indent > 0) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        m_Value.Accept(writer);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        m_Value.Accept(writer);
    }
    
    return buffer.GetString();
}

Scope<IJSONValue> RapidJSONValue::Clone() const {
    rapidjson::Value clonedValue(m_Value, m_Allocator);
    return CreateScope<RapidJSONValue>(std::move(clonedValue), m_Allocator);
}

IJSONValue& RapidJSONValue::GetCachedValue(size_t index) const {
    if (index >= m_CachedValues.size()) {
        m_CachedValues.resize(index + 1);
    }
    
    if (!m_CachedValues[index]) {
        rapidjson::Value value(m_Value[index], m_Allocator);
        m_CachedValues[index] = CreateScope<RapidJSONValue>(std::move(value), m_Allocator);
    }
    
    return *m_CachedValues[index];
}

IJSONValue& RapidJSONValue::GetCachedValue(const std::string& key) const {
    auto it = m_CachedObjectValues.find(key);
    if (it == m_CachedObjectValues.end()) {
        auto member = const_cast<rapidjson::Value&>(m_Value).FindMember(key.c_str());
        if (member != m_Value.MemberEnd()) {
            rapidjson::Value value(member->value, m_Allocator);
            auto valuePtr = CreateScope<RapidJSONValue>(std::move(value), m_Allocator);
            it = m_CachedObjectValues.emplace(key, std::move(valuePtr)).first;
        } else {
            // 如果找不到成员，创建一个null值
            rapidjson::Value nullValue(rapidjson::kNullType);
            auto valuePtr = CreateScope<RapidJSONValue>(std::move(nullValue), m_Allocator);
            it = m_CachedObjectValues.emplace(key, std::move(valuePtr)).first;
        }
    }
    
    return *it->second;
}

// RapidJSONFactory实现
RapidJSONFactory::RapidJSONFactory() : m_Document(CreateScope<rapidjson::Document>()) {}

rapidjson::Document::AllocatorType& RapidJSONFactory::GetAllocator() {
    return m_Document->GetAllocator();
}

Scope<IJSONValue> RapidJSONFactory::CreateNull() {
    rapidjson::Value value(rapidjson::kNullType);
    return CreateScope<RapidJSONValue>(std::move(value), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateBool(bool value) {
    rapidjson::Value rapidValue(value);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateInt(int value) {
    rapidjson::Value rapidValue(value);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateFloat(float value) {
    rapidjson::Value rapidValue(value);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateDouble(double value) {
    rapidjson::Value rapidValue(value);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateString(const std::string& value) {
    rapidjson::Value rapidValue(value.c_str(), GetAllocator());
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateArray() {
    rapidjson::Value rapidValue(rapidjson::kArrayType);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateObject() {
    rapidjson::Value rapidValue(rapidjson::kObjectType);
    return CreateScope<RapidJSONValue>(std::move(rapidValue), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::Parse(const std::string& jsonString) {
    rapidjson::Document document;
    document.Parse(jsonString.c_str());
    
    if (document.HasParseError()) {
        ZG_CORE_ERROR("RapidJSON parse error: {}", rapidjson::GetParseError_En(document.GetParseError()));
        return nullptr;
    }
    
    rapidjson::Value value(std::move(document), GetAllocator());
    return CreateScope<RapidJSONValue>(std::move(value), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::ParseFromFile(const std::string& filepath) {
    FILE* file = fopen(filepath.c_str(), "rb");
    if (!file) {
        ZG_CORE_ERROR("Failed to open file: {}", filepath);
        return nullptr;
    }
    
    char readBuffer[65536];
    rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));
    
    rapidjson::Document document;
    document.ParseStream(is);
    fclose(file);
    
    if (document.HasParseError()) {
        ZG_CORE_ERROR("RapidJSON parse error in file {}: {}", filepath, rapidjson::GetParseError_En(document.GetParseError()));
        return nullptr;
    }
    
    rapidjson::Value value(std::move(document), GetAllocator());
    return CreateScope<RapidJSONValue>(std::move(value), GetAllocator());
}

Scope<IJSONValue> RapidJSONFactory::CreateFromRapidJSON(const rapidjson::Value& value) {
    rapidjson::Value clonedValue(value, GetAllocator());
    return CreateScope<RapidJSONValue>(std::move(clonedValue), GetAllocator());
}

} // namespace JSON
} // namespace Zgine
