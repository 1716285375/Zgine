#pragma once

#include "IJSONValue.h"
#include "JSONManager.h"

namespace Zgine {
	namespace JSON {

		// JSON辅助函数 - 提供便捷的JSON操作接口
		inline Scope<IJSONValue> CreateJSONNull(JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateNull(backend);
		}

		inline Scope<IJSONValue> CreateJSONBool(bool value, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateBool(value, backend);
		}

		inline Scope<IJSONValue> CreateJSONInt(int value, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateInt(value, backend);
		}

		inline Scope<IJSONValue> CreateJSONFloat(float value, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateFloat(value, backend);
		}

		inline Scope<IJSONValue> CreateJSONDouble(double value, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateDouble(value, backend);
		}

		inline Scope<IJSONValue> CreateJSONString(const std::string& value, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateString(value, backend);
		}

		inline Scope<IJSONValue> CreateJSONArray(JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateArray(backend);
		}

		inline Scope<IJSONValue> CreateJSONObject(JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().CreateObject(backend);
		}

		inline Scope<IJSONValue> ParseJSON(const std::string& jsonString, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().Parse(jsonString, backend);
		}

		inline Scope<IJSONValue> ParseJSONFromFile(const std::string& filepath, JSONBackend backend = JSONBackend::Nlohmann) {
			return JSONManager::Instance().ParseFromFile(filepath, backend);
		}

	} // namespace JSON
} // namespace Zgine
