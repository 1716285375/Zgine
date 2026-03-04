#include <Zgine/Core/Time/Clock.h>

namespace Zgine {

// Initialize application start time
Clock::TimePoint Clock::s_ApplicationStart = std::chrono::high_resolution_clock::now();

Clock::Clock()
    : m_StartTime(std::chrono::high_resolution_clock::now())
    , m_PauseTime()
    , m_PausedDuration(0)
    , m_TimeScale(1.0)
    , m_Paused(false)
{
}

double Clock::GetElapsedTime(bool scaled) const
{
    auto currentTime = m_Paused ? m_PauseTime : std::chrono::high_resolution_clock::now();
    Duration elapsed = (currentTime - m_StartTime) - m_PausedDuration;

    double seconds = elapsed.count();
    return scaled ? seconds * m_TimeScale : seconds;
}

double Clock::GetElapsedMilliseconds(bool scaled) const
{
    return GetElapsedTime(scaled) * 1000.0;
}

int64_t Clock::GetElapsedMicroseconds(bool scaled) const
{
    auto currentTime = m_Paused ? m_PauseTime : std::chrono::high_resolution_clock::now();
    auto elapsed = (currentTime - m_StartTime) - m_PausedDuration;

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    return scaled ? static_cast<int64_t>(microseconds * m_TimeScale) : microseconds;
}

void Clock::Pause()
{
    if (!m_Paused)
    {
        m_Paused = true;
        m_PauseTime = std::chrono::high_resolution_clock::now();
    }
}

void Clock::Resume()
{
    if (m_Paused)
    {
        auto resumeTime = std::chrono::high_resolution_clock::now();
        m_PausedDuration += (resumeTime - m_PauseTime);
        m_Paused = false;
    }
}

void Clock::Reset()
{
    m_StartTime = std::chrono::high_resolution_clock::now();
    m_PausedDuration = Duration(0);
    m_Paused = false;
}

void Clock::SetTimeScale(double scale)
{
    m_TimeScale = scale > 0.0 ? scale : 1.0;
}

} // namespace Zgine
