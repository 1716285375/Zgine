#include "zgpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <filesystem>
#include <chrono>

namespace Zgine {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::vector<std::shared_ptr<spdlog::sinks::sink>> Log::s_Sinks;
	std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> Log::s_CustomLoggers;
	LogConfig Log::s_Config;
	bool Log::s_Initialized = false;

	/**
	 * @brief Initialize the logging system with default configuration
	 * @details Sets up core and client loggers with appropriate formatting
	 */
	void Log::Init() {
		LogConfig defaultConfig;
		Init(defaultConfig);
	}

	/**
	 * @brief Initialize the logging system with custom configuration
	 * @param config Custom logging configuration
	 */
	void Log::Init(const LogConfig& config) {
		if (s_Initialized) {
			ZG_CORE_WARN("Log system already initialized!");
			return;
		}

		s_Config = config;

		// Create log directory if it doesn't exist
		if (s_Config.enableFileOutput) {
			std::filesystem::create_directories(s_Config.logDirectory);
		}

		SetupSinks();
		SetupPatterns();

		// Create core logger
		s_CoreLogger = std::make_shared<spdlog::logger>("ZGINE", s_Sinks.begin(), s_Sinks.end());
		s_CoreLogger->set_level(ConvertLogLevel(s_Config.consoleLevel));
		s_CoreLogger->flush_on(spdlog::level::err);

		// Create client logger
		s_ClientLogger = std::make_shared<spdlog::logger>("APP", s_Sinks.begin(), s_Sinks.end());
		s_ClientLogger->set_level(ConvertLogLevel(s_Config.consoleLevel));
		s_ClientLogger->flush_on(spdlog::level::err);

		// Register loggers
		spdlog::register_logger(s_CoreLogger);
		spdlog::register_logger(s_ClientLogger);

		// Set default logger
		spdlog::set_default_logger(s_CoreLogger);

		// Create default category loggers
		CreateLogger("Renderer", LogCategory::Renderer);
		CreateLogger("Audio", LogCategory::Audio);
		CreateLogger("ECS", LogCategory::ECS);
		CreateLogger("Resources", LogCategory::Resources);
		CreateLogger("UI", LogCategory::UI);
		CreateLogger("Physics", LogCategory::Physics);
		CreateLogger("Input", LogCategory::Input);
		CreateLogger("Network", LogCategory::Network);
		CreateLogger("Scripting", LogCategory::Scripting);

		s_Initialized = true;
		ZG_CORE_INFO("Log system initialized successfully");
	}

	/**
	 * @brief Shutdown the logging system
	 * @details Flushes all loggers and cleans up resources
	 */
	void Log::Shutdown() {
		if (!s_Initialized) {
			return;
		}

		Flush();
		
		// Clear custom loggers
		s_CustomLoggers.clear();
		
		// Clear sinks
		s_Sinks.clear();
		
		// Reset loggers
		s_CoreLogger.reset();
		s_ClientLogger.reset();
		
		s_Initialized = false;
	}

	/**
	 * @brief Set up logging sinks based on configuration
	 */
	void Log::SetupSinks() {
		s_Sinks.clear();

		// Console sink
		if (s_Config.enableConsoleOutput) {
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_level(ConvertLogLevel(s_Config.consoleLevel));
			s_Sinks.push_back(console_sink);
		}

		// File sinks
		if (s_Config.enableFileOutput) {
			if (s_Config.enableRotatingFiles) {
				// Rotating file sink for core logger
				auto core_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
					s_Config.logDirectory + "/" + s_Config.coreLogFile,
					s_Config.maxFileSize,
					s_Config.maxFiles
				);
				core_file_sink->set_level(ConvertLogLevel(s_Config.fileLevel));
				s_Sinks.push_back(core_file_sink);

				// Rotating file sink for client logger
				auto client_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
					s_Config.logDirectory + "/" + s_Config.clientLogFile,
					s_Config.maxFileSize,
					s_Config.maxFiles
				);
				client_file_sink->set_level(ConvertLogLevel(s_Config.fileLevel));
				s_Sinks.push_back(client_file_sink);
			} else {
				// Basic file sink for core logger
				auto core_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
					s_Config.logDirectory + "/" + s_Config.coreLogFile
				);
				core_file_sink->set_level(ConvertLogLevel(s_Config.fileLevel));
				s_Sinks.push_back(core_file_sink);

				// Basic file sink for client logger
				auto client_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
					s_Config.logDirectory + "/" + s_Config.clientLogFile
				);
				client_file_sink->set_level(ConvertLogLevel(s_Config.fileLevel));
				s_Sinks.push_back(client_file_sink);
			}
		}
	}

	/**
	 * @brief Set up logging patterns based on configuration
	 */
	void Log::SetupPatterns() {
		std::string pattern = "";

		// Timestamp
		if (s_Config.enableTimestamps) {
			pattern += "[%Y-%m-%d %H:%M:%S.%e] ";
		}

		// Thread ID
		if (s_Config.enableThreadIds) {
			pattern += "[%t] ";
		}

		// Source location
		if (s_Config.enableSourceLocation) {
			pattern += "[%s:%#] ";
		}

		// Level and logger name
		if (s_Config.enableColors) {
			pattern += "%^[%l] %n: %v%$";
		} else {
			pattern += "[%l] %n: %v";
		}

		// Set pattern for all sinks
		for (auto& sink : s_Sinks) {
			sink->set_pattern(pattern);
		}
	}

	/**
	 * @brief Convert LogLevel to spdlog level
	 * @param level LogLevel to convert
	 * @return spdlog::level::level_enum Converted level
	 */
	spdlog::level::level_enum Log::ConvertLogLevel(LogLevel level) {
		switch (level) {
			case LogLevel::Trace:    return spdlog::level::trace;
			case LogLevel::Debug:    return spdlog::level::debug;
			case LogLevel::Info:     return spdlog::level::info;
			case LogLevel::Warn:     return spdlog::level::warn;
			case LogLevel::Error:    return spdlog::level::err;
			case LogLevel::Critical: return spdlog::level::critical;
			case LogLevel::Off:      return spdlog::level::off;
			default:                 return spdlog::level::info;
		}
	}

	/**
	 * @brief Get category string for logging
	 * @param category LogCategory to convert
	 * @return std::string Category string
	 */
	std::string Log::GetCategoryString(LogCategory category) {
		switch (category) {
			case LogCategory::Core:       return "CORE";
			case LogCategory::Renderer:   return "RENDERER";
			case LogCategory::Audio:      return "AUDIO";
			case LogCategory::Input:      return "INPUT";
			case LogCategory::Physics:    return "PHYSICS";
			case LogCategory::ECS:        return "ECS";
			case LogCategory::Resources:  return "RESOURCES";
			case LogCategory::UI:         return "UI";
			case LogCategory::Network:    return "NETWORK";
			case LogCategory::Scripting:  return "SCRIPTING";
			case LogCategory::Application: return "APP";
			default:                      return "UNKNOWN";
		}
	}

	/**
	 * @brief Set the log level for console output
	 * @param level The log level to set
	 */
	void Log::SetConsoleLevel(LogLevel level) {
		s_Config.consoleLevel = level;
		
		// Update console sinks
		for (auto& sink : s_Sinks) {
			if (std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(sink)) {
				sink->set_level(ConvertLogLevel(level));
			}
		}
	}

	/**
	 * @brief Set the log level for file output
	 * @param level The log level to set
	 */
	void Log::SetFileLevel(LogLevel level) {
		s_Config.fileLevel = level;
		
		// Update file sinks
		for (auto& sink : s_Sinks) {
			if (std::dynamic_pointer_cast<spdlog::sinks::basic_file_sink_mt>(sink) ||
				std::dynamic_pointer_cast<spdlog::sinks::rotating_file_sink_mt>(sink)) {
				sink->set_level(ConvertLogLevel(level));
			}
		}
	}

	/**
	 * @brief Set the log level for a specific logger
	 * @param logger The logger to configure
	 * @param level The log level to set
	 */
	void Log::SetLoggerLevel(std::shared_ptr<spdlog::logger> logger, LogLevel level) {
		if (logger) {
			logger->set_level(ConvertLogLevel(level));
		}
	}

	/**
	 * @brief Enable or disable console output
	 * @param enable Whether to enable console output
	 */
	void Log::EnableConsoleOutput(bool enable) {
		if (s_Config.enableConsoleOutput == enable) {
			return;
		}

		s_Config.enableConsoleOutput = enable;
		
		if (s_Initialized) {
			// Re-setup sinks to apply changes
			SetupSinks();
			SetupPatterns();
		}
	}

	/**
	 * @brief Enable or disable file output
	 * @param enable Whether to enable file output
	 */
	void Log::EnableFileOutput(bool enable) {
		if (s_Config.enableFileOutput == enable) {
			return;
		}

		s_Config.enableFileOutput = enable;
		
		if (s_Initialized) {
			// Re-setup sinks to apply changes
			SetupSinks();
			SetupPatterns();
		}
	}

	/**
	 * @brief Flush all loggers
	 * @details Forces all buffered log messages to be written
	 */
	void Log::Flush() {
		if (s_CoreLogger) {
			s_CoreLogger->flush();
		}
		if (s_ClientLogger) {
			s_ClientLogger->flush();
		}
		
		for (auto& [name, logger] : s_CustomLoggers) {
			if (logger) {
				logger->flush();
			}
		}
	}

	/**
	 * @brief Create a custom logger
	 * @param name Logger name
	 * @param category Logger category
	 * @return std::shared_ptr<spdlog::logger> Created logger
	 */
	std::shared_ptr<spdlog::logger> Log::CreateLogger(const std::string& name, LogCategory category) {
		if (s_CustomLoggers.find(name) != s_CustomLoggers.end()) {
			ZG_CORE_WARN("Logger '{}' already exists!", name);
			return s_CustomLoggers[name];
		}

		auto logger = std::make_shared<spdlog::logger>(name, s_Sinks.begin(), s_Sinks.end());
		logger->set_level(ConvertLogLevel(s_Config.consoleLevel));
		logger->flush_on(spdlog::level::err);

		spdlog::register_logger(logger);
		s_CustomLoggers[name] = logger;

		ZG_CORE_INFO("Created custom logger: {} [{}]", name, GetCategoryString(category));
		return logger;
	}

	/**
	 * @brief Get logger by name
	 * @param name Logger name
	 * @return std::shared_ptr<spdlog::logger> Logger if found, nullptr otherwise
	 */
	std::shared_ptr<spdlog::logger> Log::GetLogger(const std::string& name) {
		auto it = s_CustomLoggers.find(name);
		if (it != s_CustomLoggers.end()) {
			return it->second;
		}
		
		// Try to get from spdlog registry
		return spdlog::get(name);
	}

	/**
	 * @brief Remove a custom logger
	 * @param name Logger name
	 */
	void Log::RemoveLogger(const std::string& name) {
		auto it = s_CustomLoggers.find(name);
		if (it != s_CustomLoggers.end()) {
			spdlog::drop(name);
			s_CustomLoggers.erase(it);
			ZG_CORE_INFO("Removed custom logger: {}", name);
		}
	}
}