#pragma once

#include "IJSONValue.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/error/en.h>

namespace Zgine {
namespace JSON {

// RapidJSON后端实现
class RapidJSONValue : public IJSONValue {
public:
    explicit RapidJSONValue(rapidjson::Value value, rapidjson::Document::AllocatorType& allocator);
    virtual ~RapidJSONValue() = default;
    
    // 类型查询
    JSONType GetType() const override;
    bool IsNull() const override;
    bool IsBool() const override;
    bool IsNumber() const override;
    bool IsString() const override;
    bool IsArray() const override;
    bool IsObject() const override;
    
    // 值获取
    bool AsBool() const override;
    int AsInt() const override;
    float AsFloat() const override;
    double AsDouble() const override;
    const std::string& AsString() const override;
    
    // 数组操作
    size_t Size() const override;
    IJSONValue& operator[](size_t index) override;
    const IJSONValue& operator[](size_t index) const override;
    void Append(std::unique_ptr<IJSONValue> value) override;
    void Append(const IJSONValue& value) override;
    
    // 对象操作
    bool HasKey(const std::string& key) const override;
    IJSONValue& operator[](const std::string& key) override;
    const IJSONValue& operator[](const std::string& key) const override;
    void Set(const std::string& key, std::unique_ptr<IJSONValue> value) override;
    void Set(const std::string& key, const IJSONValue& value) override;
    
    // 序列化
    std::string ToString(int indent = 0) const override;
    
    // 克隆
    std::unique_ptr<IJSONValue> Clone() const override;
    
    // 获取底层rapidjson::Value对象
    const rapidjson::Value& GetRapidJSONValue() const { return m_Value; }
    rapidjson::Value& GetRapidJSONValue() { return m_Value; }

private:
    rapidjson::Value m_Value;
    rapidjson::Document::AllocatorType& m_Allocator;
    mutable std::vector<Scope<IJSONValue>> m_CachedValues;
    mutable std::unordered_map<std::string, Scope<IJSONValue>> m_CachedObjectValues;
    mutable std::string m_CachedString;
    
    IJSONValue& GetCachedValue(size_t index) const;
    IJSONValue& GetCachedValue(const std::string& key) const;
};

// RapidJSON工厂实现
class RapidJSONFactory : public IJSONValueFactory {
public:
    RapidJSONFactory();
    virtual ~RapidJSONFactory() = default;
    
    Scope<IJSONValue> CreateNull() override;
    Scope<IJSONValue> CreateBool(bool value) override;
    Scope<IJSONValue> CreateInt(int value) override;
    Scope<IJSONValue> CreateFloat(float value) override;
    Scope<IJSONValue> CreateDouble(double value) override;
    Scope<IJSONValue> CreateString(const std::string& value) override;
    Scope<IJSONValue> CreateArray() override;
    Scope<IJSONValue> CreateObject() override;
    
    Scope<IJSONValue> Parse(const std::string& jsonString) override;
    Scope<IJSONValue> ParseFromFile(const std::string& filepath) override;

private:
    Scope<rapidjson::Document> m_Document;
    rapidjson::Document::AllocatorType& GetAllocator();
    Scope<IJSONValue> CreateFromRapidJSON(const rapidjson::Value& value);
};

} // namespace JSON
} // namespace Zgine
