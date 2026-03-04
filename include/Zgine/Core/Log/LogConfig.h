#pragma once

#include <spdlog/spdlog.h>
#include <string>

namespace Zgine {

/**
 * @brief Configuration structure for the logging system
 *
 * This struct controls various aspects of how logging behaves,
 * including output destinations, log levels, and async settings.
 */
struct LogConfig {
    // Console output
    bool enableConsole = true;
    bool enableColors = true;

    // File output
    bool enableFile = true;
    std::string logDirectory = "logs";
    size_t maxFileSize = 5 * 1024 * 1024;  // 5MB per file
    size_t maxFiles = 10;                   // Keep 10 rotated files

    // Async configuration - TEMPORARILY DISABLED FOR DEBUGGING
    bool enableAsync = false;  // TODO: Re-enable after fixing font loading crash
    size_t asyncQueueSize = 8192;           // Message queue size
    size_t asyncThreads = 1;                // 1 thread for ordered logging

    // Log levels
    spdlog::level::level_enum coreLoggerLevel = spdlog::level::trace;
    spdlog::level::level_enum clientLoggerLevel = spdlog::level::trace;

    // Advanced options
    bool flushOnError = true;               // Auto-flush on error/critical
    bool separateErrorLog = false;          // Create separate error.log file
};

}
