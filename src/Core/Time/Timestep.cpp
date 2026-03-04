#include <Zgine/Core/Time/Timestep.h>
#include <algorithm>

namespace Zgine {

// Static member initialization
Clock Timestep::s_GlobalClock;
float Timestep::s_FPSSamples[FPS_SAMPLE_COUNT] = {};
int Timestep::s_FPSSampleIndex = 0;
float Timestep::s_SmoothedFPS = 60.0f;

Timestep::Timestep(double deltaTime)
    : m_Time(deltaTime)
    , m_SmoothedFPS(s_SmoothedFPS)
{
}

float Timestep::GetFPS() const
{
    return m_Time > 0.0 ? static_cast<float>(1.0 / m_Time) : 0.0f;
}

void Timestep::UpdateFPS()
{
    float currentFPS = GetFPS();

    // Store FPS sample
    s_FPSSamples[s_FPSSampleIndex] = currentFPS;
    s_FPSSampleIndex = (s_FPSSampleIndex + 1) % FPS_SAMPLE_COUNT;

    // Calculate smoothed FPS (average)
    float sum = 0.0f;
    for (int i = 0; i < FPS_SAMPLE_COUNT; ++i)
    {
        sum += s_FPSSamples[i];
    }
    s_SmoothedFPS = sum / FPS_SAMPLE_COUNT;
    m_SmoothedFPS = s_SmoothedFPS;
}

double Timestep::GetTime()
{
    return s_GlobalClock.GetElapsedTime();
}

double Timestep::GetTimeMilliseconds()
{
    return s_GlobalClock.GetElapsedMilliseconds();
}

} // namespace Zgine
