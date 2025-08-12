#pragma once


#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace Zgine {
	class ZG_API Log {
		public:
			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core
#define ZG_CORE_TRACE(...)        ::Zgine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ZG_CORE_DEBUG(...)        ::Zgine::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define ZG_CORE_INFO(...)         ::Zgine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ZG_CORE_WARN(...)         ::Zgine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ZG_CORE_ERROR(...)        ::Zgine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ZG_CORE_FATAL(...)        ::Zgine::Log::GetCoreLogger()->critical(__VA_ARGS__)


// Client

#define ZG_TRACE(...)        ::Zgine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ZG_APP_DEBUG(...)	 ::Zgine::Log::GetClientLogger()->debug(__VA_ARGS__)
#define ZG_INFO(...)         ::Zgine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ZG_WARN(...)         ::Zgine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ZG_ERROR(...)        ::Zgine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ZG_FATAL(...)        ::Zgine::Log::GetClientLogger()->critical(__VA_ARGS__)
