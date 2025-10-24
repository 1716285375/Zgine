#pragma once

#include "Zgine/Core.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Zgine {
	namespace JSON {

		// 前向声明
		class JSONManager;

		// JSON值类型枚举
		enum class JSONType {
			Null,
			Bool,
			Number,
			String,
			Array,
			Object
		};

		// JSON后端类型枚举
		enum class JSONBackend {
			Nlohmann,   // 用于配置和编辑器
			RapidJSON,  // 用于高频序列化
			Custom      // 自定义实现
		};

		// 抽象JSON值接口
		class IJSONValue {
		public:
			virtual ~IJSONValue() = default;

			// 类型查询
			virtual JSONType GetType() const = 0;
			virtual bool IsNull() const = 0;
			virtual bool IsBool() const = 0;
			virtual bool IsNumber() const = 0;
			virtual bool IsString() const = 0;
			virtual bool IsArray() const = 0;
			virtual bool IsObject() const = 0;

			// 值获取
			virtual bool AsBool() const = 0;
			virtual int AsInt() const = 0;
			virtual float AsFloat() const = 0;
			virtual double AsDouble() const = 0;
			virtual const std::string& AsString() const = 0;

			// 数组操作
			virtual size_t Size() const = 0;
			virtual IJSONValue& operator[](size_t index) = 0;
			virtual const IJSONValue& operator[](size_t index) const = 0;
			virtual void Append(Scope<IJSONValue> value) = 0;
			virtual void Append(const IJSONValue& value) = 0;

			// 对象操作
			virtual bool HasKey(const std::string& key) const = 0;
			virtual IJSONValue& operator[](const std::string& key) = 0;
			virtual const IJSONValue& operator[](const std::string& key) const = 0;
			virtual void Set(const std::string& key, Scope<IJSONValue> value) = 0;
			virtual void Set(const std::string& key, const IJSONValue& value) = 0;

			// 序列化
			virtual std::string ToString(int indent = 0) const = 0;

			// 克隆
			virtual Scope<IJSONValue> Clone() const = 0;
		};

		// JSON值工厂接口
		class IJSONValueFactory {
		public:
			virtual ~IJSONValueFactory() = default;

			virtual Scope<IJSONValue> CreateNull() = 0;
			virtual Scope<IJSONValue> CreateBool(bool value) = 0;
			virtual Scope<IJSONValue> CreateInt(int value) = 0;
			virtual Scope<IJSONValue> CreateFloat(float value) = 0;
			virtual Scope<IJSONValue> CreateDouble(double value) = 0;
			virtual Scope<IJSONValue> CreateString(const std::string& value) = 0;
			virtual Scope<IJSONValue> CreateArray() = 0;
			virtual Scope<IJSONValue> CreateObject() = 0;

			// 从字符串解析
			virtual Scope<IJSONValue> Parse(const std::string& jsonString) = 0;
			virtual Scope<IJSONValue> ParseFromFile(const std::string& filepath) = 0;
		};


	} // namespace JSON
} // namespace Zgine
