#include <Zgine/Core/Time/Timer.h>

namespace Zgine {

Timer::Timer()
    : m_Running(false)
    , m_LastLapTime(0.0)
    , m_TargetTime(0.0)
    , m_CallbackEnabled(false)
{
}

void Timer::Start()
{
    if (!m_Running)
    {
        m_Clock.Reset();
        m_Running = true;
        m_LastLapTime = 0.0;
    }
}

double Timer::Stop()
{
    if (m_Running)
    {
        m_Running = false;
        return m_Clock.GetElapsedTime();
    }
    return 0.0;
}

void Timer::Reset()
{
    m_Clock.Reset();
    m_LastLapTime = 0.0;
    m_CallbackEnabled = false;
}

double Timer::Lap()
{
    if (m_Running)
    {
        double currentTime = m_Clock.GetElapsedTime();
        double lapTime = currentTime - m_LastLapTime;
        m_LastLapTime = currentTime;
        return lapTime;
    }
    return 0.0;
}

double Timer::GetElapsed() const
{
    return m_Running ? m_Clock.GetElapsedTime() : 0.0;
}

void Timer::SetCallback(double targetTime, Callback callback)
{
    m_TargetTime = targetTime;
    m_Callback = callback;
    m_CallbackEnabled = true;
}

void Timer::Update()
{
    if (m_Running && m_CallbackEnabled && m_Callback)
    {
        double elapsed = m_Clock.GetElapsedTime();
        if (elapsed >= m_TargetTime)
        {
            m_Callback(elapsed);
            m_CallbackEnabled = false; // One-shot callback
        }
    }
}

} // namespace Zgine
