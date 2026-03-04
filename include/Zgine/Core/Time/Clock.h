#pragma once

#include <chrono>
#include <cstdint>

namespace Zgine {

/**
 * @brief High-precision clock for application timing
 *
 * Provides industrial-grade time measurement with pause/resume,
 * time scaling, and microsecond precision.
 *
 * Thread-safe: No (use from main thread only)
 */
class Clock {
public:
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using Duration = std::chrono::duration<double>;

    Clock();
    ~Clock() = default;

    /**
     * @brief Get elapsed time since clock start (seconds)
     * @param scaled If true, apply time scale
     */
    double GetElapsedTime(bool scaled = false) const;

    /**
     * @brief Get elapsed time in milliseconds
     */
    double GetElapsedMilliseconds(bool scaled = false) const;

    /**
     * @brief Get elapsed time in microseconds
     */
    int64_t GetElapsedMicroseconds(bool scaled = false) const;

    /**
     * @brief Pause the clock
     */
    void Pause();

    /**
     * @brief Resume the clock from pause
     */
    void Resume();

    /**
     * @brief Check if clock is paused
     */
    bool IsPaused() const { return m_Paused; }

    /**
     * @brief Reset clock to zero
     */
    void Reset();

    /**
     * @brief Set time scale (1.0 = normal, 0.5 = slow motion, 2.0 = fast forward)
     */
    void SetTimeScale(double scale);

    /**
     * @brief Get current time scale
     */
    double GetTimeScale() const { return m_TimeScale; }

    /**
     * @brief Get the application start time
     */
    static TimePoint GetApplicationStartTime() { return s_ApplicationStart; }

private:
    TimePoint m_StartTime;
    TimePoint m_PauseTime;
    Duration m_PausedDuration;

    double m_TimeScale;
    bool m_Paused;

    static TimePoint s_ApplicationStart;
};

} // namespace Zgine
