#pragma once

#include "IJSONValue.h"
#include <nlohmann/json.hpp>

namespace Zgine {
namespace JSON {

// nlohmann/json后端实现
class NlohmannJSONValue : public IJSONValue {
public:
    explicit NlohmannJSONValue(nlohmann::json value);
    virtual ~NlohmannJSONValue() noexcept = default;
    
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
    void Append(Scope<IJSONValue> value) override;
    void Append(const IJSONValue& value) override;
    
    // 对象操作
    bool HasKey(const std::string& key) const override;
    IJSONValue& operator[](const std::string& key) override;
    const IJSONValue& operator[](const std::string& key) const override;
    void Set(const std::string& key, Scope<IJSONValue> value) override;
    void Set(const std::string& key, const IJSONValue& value) override;
    
    // 序列化
    std::string ToString(int indent = 0) const override;
    
    // 克隆
    Scope<IJSONValue> Clone() const override;
    
    // 获取底层nlohmann::json对象
    const nlohmann::json& GetNlohmannJSON() const { return m_Value; }
    nlohmann::json& GetNlohmannJSON() { return m_Value; }

private:
    nlohmann::json m_Value;
    mutable std::vector<Scope<IJSONValue>> m_CachedValues;
    mutable std::unordered_map<std::string, Scope<IJSONValue>> m_CachedObjectValues;
    
    IJSONValue& GetCachedValue(size_t index) const;
    IJSONValue& GetCachedValue(const std::string& key) const;
};

// nlohmann/json工厂实现
class NlohmannJSONFactory : public IJSONValueFactory {
public:
    virtual ~NlohmannJSONFactory() = default;
    
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
    Scope<IJSONValue> CreateFromNlohmann(const nlohmann::json& json);
};

} // namespace JSON
} // namespace Zgine
