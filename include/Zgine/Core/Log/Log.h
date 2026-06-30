#pragma once

#include <Zgine/Core/Log/LogConfig.h>
#include <Zgine/Core/Log/ScopeTimer.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace Zgine {

/**
 * @brief Enhanced logging system with async support and file output
 *
 * Features:
 * - Async multi-threaded logging (non-blocking)
 * - Console output with colors
 * - Daily rotating file logs
 * - Separate core (engine) and client (app) loggers
 * - Runtime log level changes
 * - Performance profiling support
 */
class Log {
public:
    /**
     * @brief Initialize the logging system
     * @param config Configuration options for logging
     *
     * Creates async loggers with console and file sinks.
     * Must be called before using any logging macros.
     */
    static void Init(const LogConfig& config = LogConfig());

    /**
     * @brief Shutdown the logging system
     *
     * Flushes all pending logs and releases resources.
     * IMPORTANT: Must be called before program exit to ensure all logs are written.
     */
    static void Shutdown();

    /**
     * @brief Flush all loggers immediately
     *
     * Forces all pending log messages to be written.
     * Useful before exiting or after critical errors.
     */
    static void FlushAll();

    /**
     * @brief Set minimum log level for all loggers
     * @param level Minimum severity level to log
     */
    static void SetLevel(spdlog::level::level_enum level);

    /**
     * @brief Set log level for core logger only
     */
    static void SetCoreLevel(spdlog::level::level_enum level);

    /**
     * @brief Set log level for client logger only
     */
    static void SetClientLevel(spdlog::level::level_enum level);

    /**
     * @brief Get the core (engine) logger
     */
    static std::shared_ptr<spdlog::logger>& GetCoreLogger();

    /**
     * @brief Get the client (application) logger
     */
    static std::shared_ptr<spdlog::logger>& GetClientLogger();

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
    static bool s_Initialized;
};

}

// ============================================================================
// Core Engine Log Macros
// ============================================================================

#define ZGINE_CORE_TRACE(...)    ::Zgine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ZGINE_CORE_DEBUG(...)    ::Zgine::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define ZGINE_CORE_INFO(...)     ::Zgine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ZGINE_CORE_WARN(...)     ::Zgine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ZGINE_CORE_ERROR(...)    ::Zgine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ZGINE_CORE_FATAL(...)    ::Zgine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// ============================================================================
// Client Application Log Macros
// ============================================================================

#define ZGINE_TRACE(...)         ::Zgine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ZGINE_DEBUG(...)         ::Zgine::Log::GetClientLogger()->debug(__VA_ARGS__)
#define ZGINE_INFO(...)          ::Zgine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ZGINE_WARN(...)          ::Zgine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ZGINE_ERROR(...)         ::Zgine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ZGINE_FATAL(...)         ::Zgine::Log::GetClientLogger()->critical(__VA_ARGS__)

// ============================================================================
// Advanced Debug Macros with Source Location
// ============================================================================

#ifdef ZGINE_ENABLE_VERBOSE_LOGGING
    #define ZGINE_CORE_VERBOSE(...) ZGINE_CORE_TRACE(__VA_ARGS__)
    #define ZGINE_VERBOSE(...)      ZGINE_TRACE(__VA_ARGS__)
#else
    #define ZGINE_CORE_VERBOSE(...) (void)0
    #define ZGINE_VERBOSE(...)      (void)0
#endif

// Source location macros (file:line)
#define ZGINE_CORE_TRACE_LOC(...)  \
    ::Zgine::Log::GetCoreLogger()->trace("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

#define ZGINE_CORE_DEBUG_LOC(...)  \
    ::Zgine::Log::GetCoreLogger()->debug("[{}:{}] {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

// ============================================================================
// Performance Profiling Macro
// ============================================================================

#define ZGINE_PROFILE_SCOPE(name) \
    ::Zgine::ScopeTimer ZGINE_CONCAT(_timer_, __LINE__)(name)
