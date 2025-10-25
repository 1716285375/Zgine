#pragma once

#include "Zgine/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <string>
#include <vector>

namespace Zgine {

	/**
	 * @enum LogLevel
	 * @brief Logging levels for the engine
	 */
	enum class LogLevel {
		Trace = 0,
		Debug = 1,
		Info = 2,
		Warn = 3,
		Error = 4,
		Critical = 5,
		Off = 6
	};

	/**
	 * @enum LogCategory
	 * @brief Different categories of logging for better organization
	 */
	enum class LogCategory {
		Core,       ///< Core engine systems
		Renderer,   ///< Rendering system
		Audio,      ///< Audio system
		Input,      ///< Input handling
		Physics,    ///< Physics simulation
		ECS,        ///< Entity Component System
		Resources,  ///< Resource management
		UI,         ///< User interface
		Network,    ///< Network operations
		Scripting,  ///< Scripting system
		Application ///< Application specific
	};

	/**
	 * @class LogConfig
	 * @brief Configuration structure for logging system
	 */
	struct LogConfig {
		bool enableConsoleOutput = true;           ///< Enable console output
		bool enableFileOutput = true;             ///< Enable file output
		bool enableRotatingFiles = true;           ///< Enable rotating log files
		std::string logDirectory = "logs";         ///< Directory for log files
		std::string coreLogFile = "zgine_core.log"; ///< Core log file name
		std::string clientLogFile = "zgine_app.log"; ///< Client log file name
		size_t maxFileSize = 5 * 1024 * 1024;     ///< Max file size (5MB)
		size_t maxFiles = 3;                       ///< Max number of files
		LogLevel consoleLevel = LogLevel::Info;    ///< Console log level
		LogLevel fileLevel = LogLevel::Trace;      ///< File log level
		bool enableColors = true;                  ///< Enable colored output
		bool enableTimestamps = true;              ///< Enable timestamps
		bool enableThreadIds = false;              ///< Enable thread IDs
		bool enableSourceLocation = false;         ///< Enable source file location
	};

	/**
	 * @brief Logging system class for the Zgine engine
	 * @details This class provides centralized logging functionality using spdlog,
	 *          with separate loggers for core engine and client application
	 */
	class ZG_API Log {
		public:
			/**
			 * @brief Initialize the logging system with default configuration
			 * @details Sets up core and client loggers with appropriate formatting
			 */
			static void Init();

			/**
			 * @brief Initialize the logging system with custom configuration
			 * @param config Custom logging configuration
			 */
			static void Init(const LogConfig& config);

			/**
			 * @brief Shutdown the logging system
			 * @details Flushes all loggers and cleans up resources
			 */
			static void Shutdown();

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

			/**
			 * @brief Set the log level for console output
			 * @param level The log level to set
			 */
			static void SetConsoleLevel(LogLevel level);

			/**
			 * @brief Set the log level for file output
			 * @param level The log level to set
			 */
			static void SetFileLevel(LogLevel level);

			/**
			 * @brief Set the log level for a specific logger
			 * @param logger The logger to configure
			 * @param level The log level to set
			 */
			static void SetLoggerLevel(std::shared_ptr<spdlog::logger> logger, LogLevel level);

			/**
			 * @brief Enable or disable console output
			 * @param enable Whether to enable console output
			 */
			static void EnableConsoleOutput(bool enable);

			/**
			 * @brief Enable or disable file output
			 * @param enable Whether to enable file output
			 */
			static void EnableFileOutput(bool enable);

			/**
			 * @brief Flush all loggers
			 * @details Forces all buffered log messages to be written
			 */
			static void Flush();

			/**
			 * @brief Get current logging configuration
			 * @return const LogConfig& Current configuration
			 */
			static const LogConfig& GetConfig() { return s_Config; }

			/**
			 * @brief Create a custom logger
			 * @param name Logger name
			 * @param category Logger category
			 * @return std::shared_ptr<spdlog::logger> Created logger
			 */
			static std::shared_ptr<spdlog::logger> CreateLogger(const std::string& name, LogCategory category = LogCategory::Application);

			/**
			 * @brief Get logger by name
			 * @param name Logger name
			 * @return std::shared_ptr<spdlog::logger> Logger if found, nullptr otherwise
			 */
			static std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);

			/**
			 * @brief Remove a custom logger
			 * @param name Logger name
			 */
			static void RemoveLogger(const std::string& name);

		private:
			static void SetupSinks();
			static void SetupPatterns();
			static spdlog::level::level_enum ConvertLogLevel(LogLevel level);
			static std::string GetCategoryString(LogCategory category);

			static std::shared_ptr<spdlog::logger> s_CoreLogger;    ///< Core engine logger
			static std::shared_ptr<spdlog::logger> s_ClientLogger; ///< Client application logger
			static std::vector<std::shared_ptr<spdlog::sinks::sink>> s_Sinks; ///< Shared sinks
			static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> s_CustomLoggers; ///< Custom loggers
			static LogConfig s_Config; ///< Current configuration
			static bool s_Initialized; ///< Initialization flag
	};
}

// Core logging macros with category support
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

// Category-specific logging macros
/**
 * @brief Renderer trace logging macro
 * @param ... Arguments to log
 */
#define ZG_RENDERER_TRACE(...)    ::Zgine::Log::GetLogger("Renderer")->trace(__VA_ARGS__)

/**
 * @brief Renderer info logging macro
 * @param ... Arguments to log
 */
#define ZG_RENDERER_INFO(...)     ::Zgine::Log::GetLogger("Renderer")->info(__VA_ARGS__)

/**
 * @brief Renderer warning logging macro
 * @param ... Arguments to log
 */
#define ZG_RENDERER_WARN(...)     ::Zgine::Log::GetLogger("Renderer")->warn(__VA_ARGS__)

/**
 * @brief Renderer error logging macro
 * @param ... Arguments to log
 */
#define ZG_RENDERER_ERROR(...)    ::Zgine::Log::GetLogger("Renderer")->error(__VA_ARGS__)

/**
 * @brief Audio trace logging macro
 * @param ... Arguments to log
 */
#define ZG_AUDIO_TRACE(...)       ::Zgine::Log::GetLogger("Audio")->trace(__VA_ARGS__)

/**
 * @brief Audio info logging macro
 * @param ... Arguments to log
 */
#define ZG_AUDIO_INFO(...)        ::Zgine::Log::GetLogger("Audio")->info(__VA_ARGS__)

/**
 * @brief Audio warning logging macro
 * @param ... Arguments to log
 */
#define ZG_AUDIO_WARN(...)        ::Zgine::Log::GetLogger("Audio")->warn(__VA_ARGS__)

/**
 * @brief Audio error logging macro
 * @param ... Arguments to log
 */
#define ZG_AUDIO_ERROR(...)       ::Zgine::Log::GetLogger("Audio")->error(__VA_ARGS__)

/**
 * @brief ECS trace logging macro
 * @param ... Arguments to log
 */
#define ZG_ECS_TRACE(...)         ::Zgine::Log::GetLogger("ECS")->trace(__VA_ARGS__)

/**
 * @brief ECS info logging macro
 * @param ... Arguments to log
 */
#define ZG_ECS_INFO(...)          ::Zgine::Log::GetLogger("ECS")->info(__VA_ARGS__)

/**
 * @brief ECS warning logging macro
 * @param ... Arguments to log
 */
#define ZG_ECS_WARN(...)          ::Zgine::Log::GetLogger("ECS")->warn(__VA_ARGS__)

/**
 * @brief ECS error logging macro
 * @param ... Arguments to log
 */
#define ZG_ECS_ERROR(...)         ::Zgine::Log::GetLogger("ECS")->error(__VA_ARGS__)

// Performance logging macros
/**
 * @brief Performance timing macro - start timing
 * @param name Timer name
 */
#define ZG_PERF_START(name)        auto name##_start = std::chrono::high_resolution_clock::now()

/**
 * @brief Performance timing macro - end timing and log
 * @param name Timer name
 * @param logger Logger to use
 */
#define ZG_PERF_END(name, logger)  do { \
    auto name##_end = std::chrono::high_resolution_clock::now(); \
    auto name##_duration = std::chrono::duration_cast<std::chrono::microseconds>(name##_end - name##_start).count(); \
    logger->info("PERF [{}]: {}μs", #name, name##_duration); \
} while(0)

// Conditional logging macros
/**
 * @brief Conditional trace logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_TRACE_IF(condition, ...) do { if(condition) ZG_CORE_TRACE(__VA_ARGS__); } while(0)

/**
 * @brief Conditional debug logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_DEBUG_IF(condition, ...) do { if(condition) ZG_CORE_DEBUG(__VA_ARGS__); } while(0)

/**
 * @brief Conditional info logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_INFO_IF(condition, ...) do { if(condition) ZG_CORE_INFO(__VA_ARGS__); } while(0)

/**
 * @brief Conditional warning logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_WARN_IF(condition, ...) do { if(condition) ZG_CORE_WARN(__VA_ARGS__); } while(0)

/**
 * @brief Conditional error logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_ERROR_IF(condition, ...) do { if(condition) ZG_CORE_ERROR(__VA_ARGS__); } while(0)

/**
 * @brief Conditional fatal logging macro
 * @param condition Condition to check
 * @param ... Arguments to log
 */
#define ZG_CORE_FATAL_IF(condition, ...) do { if(condition) ZG_CORE_FATAL(__VA_ARGS__); } while(0)
