#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Zgine {

	/**
	 * @brief Logging system class for the Zgine engine
	 * @details This class provides centralized logging functionality using spdlog,
	 *          with separate loggers for core engine and client application
	 */
	class ZG_API Log {
		public:
			/**
			 * @brief Initialize the logging system
			 * @details Sets up core and client loggers with appropriate formatting
			 */
			static void Init();

			/**
			 * @brief Get the core engine logger
			 * @return std::shared_ptr<spdlog::logger>& Reference to the core logger
			 */
			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			
			/**
			 * @brief Get the client application logger
			 * @return std::shared_ptr<spdlog::logger>& Reference to the client logger
			 */
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;    ///< Core engine logger
			static std::shared_ptr<spdlog::logger> s_ClientLogger;   ///< Client application logger
	};
}

// Core logging macros
/**
 * @brief Core trace logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_TRACE(...)        ::Zgine::Log::GetCoreLogger()->trace(__VA_ARGS__)

/**
 * @brief Core debug logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_DEBUG(...)        ::Zgine::Log::GetCoreLogger()->debug(__VA_ARGS__)

/**
 * @brief Core info logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_INFO(...)         ::Zgine::Log::GetCoreLogger()->info(__VA_ARGS__)

/**
 * @brief Core warning logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_WARN(...)         ::Zgine::Log::GetCoreLogger()->warn(__VA_ARGS__)

/**
 * @brief Core error logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_ERROR(...)        ::Zgine::Log::GetCoreLogger()->error(__VA_ARGS__)

/**
 * @brief Core fatal logging macro
 * @param ... Arguments to log
 */
#define ZG_CORE_FATAL(...)        ::Zgine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client logging macros
/**
 * @brief Client trace logging macro
 * @param ... Arguments to log
 */
#define ZG_TRACE(...)        ::Zgine::Log::GetClientLogger()->trace(__VA_ARGS__)

/**
 * @brief Client debug logging macro
 * @param ... Arguments to log
 */
#define ZG_APP_DEBUG(...)	 ::Zgine::Log::GetClientLogger()->debug(__VA_ARGS__)

/**
 * @brief Client info logging macro
 * @param ... Arguments to log
 */
#define ZG_INFO(...)         ::Zgine::Log::GetClientLogger()->info(__VA_ARGS__)

/**
 * @brief Client warning logging macro
 * @param ... Arguments to log
 */
#define ZG_WARN(...)         ::Zgine::Log::GetClientLogger()->warn(__VA_ARGS__)

/**
 * @brief Client error logging macro
 * @param ... Arguments to log
 */
#define ZG_ERROR(...)        ::Zgine::Log::GetClientLogger()->error(__VA_ARGS__)

/**
 * @brief Client fatal logging macro
 * @param ... Arguments to log
 */
#define ZG_FATAL(...)        ::Zgine::Log::GetClientLogger()->critical(__VA_ARGS__)
