#pragma once

#include <Zgine/Core/Base/Macro.h>
#include <chrono>
#include <string>


namespace Zgine {

/**
 * @brief RAII timer for automatic performance profiling
 *
 * Logs elapsed time when scope exits. Use with ZGINE_PROFILE_SCOPE macro.
 *
 * Example:
 *   void ExpensiveFunction() {
 *       ZGINE_PROFILE_SCOPE("ExpensiveFunction");
 *       // ... work ...
 *   }  // Automatically logs: "ExpensiveFunction took 1234 μs"
 */
class ScopeTimer {
public:
    explicit ScopeTimer(const std::string& name);
    ~ScopeTimer();

    ZGINE_NON_COPYABLE(ScopeTimer)

private:
    std::string m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

}
