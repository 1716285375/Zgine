#include "zgpch.h"
#include "Zgine/Logging/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Zgine {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	/**
	 * @brief Initialize the logging system
	 * @details Sets up core and client loggers with appropriate formatting
	 */
	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("ZGINE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}