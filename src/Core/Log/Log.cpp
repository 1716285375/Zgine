#include <Zgine/Core/Log/Log.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <vector>

namespace Zgine {

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
bool Log::s_Initialized = false;

void Log::Init(const LogConfig& config) {
    if (s_Initialized) {
        return;
    }

    // Initialize async thread pool if enabled
    if (config.enableAsync) {
        spdlog::init_thread_pool(config.asyncQueueSize, config.asyncThreads);
    }

    // ========================================================================
    // Create Sinks
    // ========================================================================

    std::vector<spdlog::sink_ptr> coreSinks;
    std::vector<spdlog::sink_ptr> clientSinks;

    // Console Sink (with colors)
    if (config.enableConsole) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);

        // Simple pattern for console: [HH:MM:SS] LOGGER: message
        console_sink->set_pattern("%^[%T] %n: %v%$");

        coreSinks.push_back(console_sink);

        // Separate console sink for client
        auto client_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        client_console_sink->set_level(spdlog::level::trace);
        client_console_sink->set_pattern("%^[%T] %n: %v%$");
        clientSinks.push_back(client_console_sink);
    }

    // File Sink (daily rotation)
    if (config.enableFile) {
        // Core engine log file
        std::string coreLogPath = config.logDirectory + "/zgine";
        auto core_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            coreLogPath, 23, 59  // Rotate at 23:59 daily
        );
        core_file_sink->set_level(spdlog::level::trace);

        // Detailed pattern for files: [YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] [thread ID] message
        core_file_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [thread %t] %v");
        coreSinks.push_back(core_file_sink);

        // Client application log file
        std::string clientLogPath = config.logDirectory + "/app";
        auto client_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            clientLogPath, 23, 59
        );
        client_file_sink->set_level(spdlog::level::trace);
        client_file_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [thread %t] %v");
        clientSinks.push_back(client_file_sink);

        // Optional: Separate error log
        if (config.separateErrorLog) {
            auto error_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                config.logDirectory + "/errors.log",
                config.maxFileSize,  // Max file size
                config.maxFiles       // Max files
            );
            error_sink->set_level(spdlog::level::err);
            error_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [thread %t] %v");

            coreSinks.push_back(error_sink);
            clientSinks.push_back(error_sink);
        }
    }

    // ========================================================================
    // Create Loggers
    // ========================================================================

    if (config.enableAsync) {
        // Async loggers (non-blocking)
        s_CoreLogger = std::make_shared<spdlog::async_logger>(
            "ZGINE",
            coreSinks.begin(),
            coreSinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block  // Block if queue full (safer)
        );

        s_ClientLogger = std::make_shared<spdlog::async_logger>(
            "APP",
            clientSinks.begin(),
            clientSinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );
    } else {
        // Synchronous loggers
        s_CoreLogger = std::make_shared<spdlog::logger>(
            "ZGINE",
            coreSinks.begin(),
            coreSinks.end()
        );

        s_ClientLogger = std::make_shared<spdlog::logger>(
            "APP",
            clientSinks.begin(),
            clientSinks.end()
        );
    }

    // Set log levels
    s_CoreLogger->set_level(config.coreLoggerLevel);
    s_ClientLogger->set_level(config.clientLoggerLevel);

    // Auto-flush on error/critical if configured
    if (config.flushOnError) {
        s_CoreLogger->flush_on(spdlog::level::err);
        s_ClientLogger->flush_on(spdlog::level::err);
    }

    // Register loggers with spdlog
    spdlog::register_logger(s_CoreLogger);
    spdlog::register_logger(s_ClientLogger);

    s_Initialized = true;

    // Log initialization success
    ZGINE_CORE_INFO("Logging system initialized");
    if (config.enableAsync) {
        ZGINE_CORE_INFO("  - Async mode: {} threads, {} queue size",
                        config.asyncThreads, config.asyncQueueSize);
    }
    if (config.enableFile) {
        ZGINE_CORE_INFO("  - Log files: {}/", config.logDirectory);
    }
}

void Log::Shutdown() {
    if (!s_Initialized) {
        return;
    }

    ZGINE_CORE_INFO("Logging system shutting down...");

    // Step 1: Flush all pending logs (while loggers are still valid)
    FlushAll();

    // Step 2: Reset shared_ptr references BEFORE dropping or shutting down
    // This ensures no new log calls can be made
    auto coreLogger = s_CoreLogger;
    auto clientLogger = s_ClientLogger;

    s_CoreLogger.reset();
    s_ClientLogger.reset();

    // Step 3: Drop from spdlog registry (removes internal references)
    if (coreLogger) {
        spdlog::drop("ZGINE");
    }
    if (clientLogger) {
        spdlog::drop("APP");
    }

    // Step 4: NOW it's safe to shutdown the thread pool
    // All loggers have been flushed and released
    spdlog::shutdown();

    s_Initialized = false;
}

void Log::FlushAll() {
    if (!s_Initialized) {
        return;
    }

    // Explicitly check if loggers are valid before flushing
    if (s_CoreLogger && s_CoreLogger.use_count() > 0) {
        s_CoreLogger->flush();
    }

    if (s_ClientLogger && s_ClientLogger.use_count() > 0) {
        s_ClientLogger->flush();
    }
}

void Log::SetLevel(spdlog::level::level_enum level) {
    if (s_CoreLogger) {
        s_CoreLogger->set_level(level);
    }

    if (s_ClientLogger) {
        s_ClientLogger->set_level(level);
    }
}

void Log::SetCoreLevel(spdlog::level::level_enum level) {
    if (s_CoreLogger) {
        s_CoreLogger->set_level(level);
    }
}

void Log::SetClientLevel(spdlog::level::level_enum level) {
    if (s_ClientLogger) {
        s_ClientLogger->set_level(level);
    }
}

}
