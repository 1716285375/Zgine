#pragma once

#include <Zgine/Core/Time/Clock.h>

namespace Zgine {

/**
 * @brief Frame delta time with FPS tracking
 *
 * Industrial-grade timestep with:
 * - Multiple time unit accessors
 * - FPS calculation and smoothing
 * - Time scale support
 * - Frame statistics
 */
class Timestep {
public:
    Timestep(double deltaTime = 0.0);

    // Implicit conversion to double (seconds)
    operator double() const { return m_Time; }
    operator float() const { return static_cast<float>(m_Time); }

    /**
     * @brief Get delta time in seconds
     */
    double GetSeconds() const { return m_Time; }
    float GetSecondsF() const { return static_cast<float>(m_Time); }

    /**
     * @brief Get delta time in milliseconds
     */
    double GetMilliseconds() const { return m_Time * 1000.0; }
    float GetMillisecondsF() const { return static_cast<float>(m_Time * 1000.0); }

    /**
     * @brief Get delta time in microseconds
     */
    int64_t GetMicroseconds() const { return static_cast<int64_t>(m_Time * 1000000.0); }

    /**
     * @brief Get scaled delta time
     */
    double GetScaled(double timeScale) const { return m_Time * timeScale; }

    /**
     * @brief Get current FPS (frames per second)
     */
    float GetFPS() const;

    /**
     * @brief Update FPS counter with new frame time
     */
    void UpdateFPS();

    /**
     * @brief Get smoothed FPS (averaged over last N frames)
     */
    float GetSmoothedFPS() const { return m_SmoothedFPS; }

    /**
     * @brief Get global application time since start
     */
    static double GetTime();

    /**
     * @brief Get global application time in milliseconds
     */
    static double GetTimeMilliseconds();

private:
    double m_Time; // Delta time in seconds

    // FPS tracking
    static constexpr int FPS_SAMPLE_COUNT = 60;
    static float s_FPSSamples[FPS_SAMPLE_COUNT];
    static int s_FPSSampleIndex;
    static float s_SmoothedFPS;
    float m_SmoothedFPS;

    static Clock s_GlobalClock;
};

} // namespace Zgine
