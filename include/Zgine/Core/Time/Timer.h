#pragma once

#include <Zgine/Core/Time/Clock.h>
#include <functional>

namespace Zgine {

/**
 * @brief General-purpose timer for performance measurement and scheduling
 *
 * Features:
 * - Start/Stop/Reset functionality
 * - Lap time recording
 * - Callback support
 */
class Timer {
public:
    using Callback = std::function<void(double)>;

    Timer();
    ~Timer() = default;

    /**
     * @brief Start the timer
     */
    void Start();

    /**
     * @brief Stop the timer and return elapsed time
     */
    double Stop();

    /**
     * @brief Reset timer to zero
     */
    void Reset();

    /**
     * @brief Record lap time without stopping
     */
    double Lap();

    /**
     * @brief Get elapsed time without stopping
     */
    double GetElapsed() const;

    /**
     * @brief Check if timer is running
     */
    bool IsRunning() const { return m_Running; }

    /**
     * @brief Set callback to be called when target time is reached
     */
    void SetCallback(double targetTime, Callback callback);

    /**
     * @brief Update timer (call every frame to check callbacks)
     */
    void Update();

private:
    Clock m_Clock;
    bool m_Running;
    double m_LastLapTime;

    // Callback support
    double m_TargetTime;
    Callback m_Callback;
    bool m_CallbackEnabled;
};

} // namespace Zgine
