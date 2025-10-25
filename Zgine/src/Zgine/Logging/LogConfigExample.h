#pragma once

#include "Log.h"

namespace Zgine {
namespace Examples {

/**
 * @brief Example configurations for the logging system
 * @details These examples show how to configure the logging system for different scenarios
 */
class LogConfigExamples {
public:
    /**
     * @brief Development configuration - verbose logging for debugging
     * @return LogConfig Development configuration
     */
    static LogConfig DevelopmentConfig() {
        LogConfig config;
        config.enableConsoleOutput = true;
        config.enableFileOutput = true;
        config.enableRotatingFiles = true;
        config.logDirectory = "logs/dev";
        config.coreLogFile = "zgine_dev_core.log";
        config.clientLogFile = "zgine_dev_app.log";
        config.maxFileSize = 10 * 1024 * 1024; // 10MB
        config.maxFiles = 5;
        config.consoleLevel = LogLevel::Trace;
        config.fileLevel = LogLevel::Trace;
        config.enableColors = true;
        config.enableTimestamps = true;
        config.enableThreadIds = true;
        config.enableSourceLocation = true;
        return config;
    }

    /**
     * @brief Production configuration - minimal logging for performance
     * @return LogConfig Production configuration
     */
    static LogConfig ProductionConfig() {
        LogConfig config;
        config.enableConsoleOutput = false;
        config.enableFileOutput = true;
        config.enableRotatingFiles = true;
        config.logDirectory = "logs/prod";
        config.coreLogFile = "zgine_prod_core.log";
        config.clientLogFile = "zgine_prod_app.log";
        config.maxFileSize = 50 * 1024 * 1024; // 50MB
        config.maxFiles = 3;
        config.consoleLevel = LogLevel::Off;
        config.fileLevel = LogLevel::Warn;
        config.enableColors = false;
        config.enableTimestamps = true;
        config.enableThreadIds = false;
        config.enableSourceLocation = false;
        return config;
    }

    /**
     * @brief Debug configuration - maximum verbosity for troubleshooting
     * @return LogConfig Debug configuration
     */
    static LogConfig DebugConfig() {
        LogConfig config;
        config.enableConsoleOutput = true;
        config.enableFileOutput = true;
        config.enableRotatingFiles = false; // Single file for easier analysis
        config.logDirectory = "logs/debug";
        config.coreLogFile = "zgine_debug_core.log";
        config.clientLogFile = "zgine_debug_app.log";
        config.maxFileSize = 100 * 1024 * 1024; // 100MB
        config.maxFiles = 1;
        config.consoleLevel = LogLevel::Trace;
        config.fileLevel = LogLevel::Trace;
        config.enableColors = true;
        config.enableTimestamps = true;
        config.enableThreadIds = true;
        config.enableSourceLocation = true;
        return config;
    }

    /**
     * @brief Performance testing configuration - minimal overhead
     * @return LogConfig Performance configuration
     */
    static LogConfig PerformanceConfig() {
        LogConfig config;
        config.enableConsoleOutput = false;
        config.enableFileOutput = false;
        config.enableRotatingFiles = false;
        config.logDirectory = "logs/perf";
        config.coreLogFile = "zgine_perf_core.log";
        config.clientLogFile = "zgine_perf_app.log";
        config.maxFileSize = 1 * 1024 * 1024; // 1MB
        config.maxFiles = 1;
        config.consoleLevel = LogLevel::Off;
        config.fileLevel = LogLevel::Off;
        config.enableColors = false;
        config.enableTimestamps = false;
        config.enableThreadIds = false;
        config.enableSourceLocation = false;
        return config;
    }

    /**
     * @brief Console-only configuration - for simple applications
     * @return LogConfig Console-only configuration
     */
    static LogConfig ConsoleOnlyConfig() {
        LogConfig config;
        config.enableConsoleOutput = true;
        config.enableFileOutput = false;
        config.enableRotatingFiles = false;
        config.logDirectory = "";
        config.coreLogFile = "";
        config.clientLogFile = "";
        config.maxFileSize = 0;
        config.maxFiles = 0;
        config.consoleLevel = LogLevel::Info;
        config.fileLevel = LogLevel::Off;
        config.enableColors = true;
        config.enableTimestamps = true;
        config.enableThreadIds = false;
        config.enableSourceLocation = false;
        return config;
    }
};

} // namespace Examples
} // namespace Zgine
