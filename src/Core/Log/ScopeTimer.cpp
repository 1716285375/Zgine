#include <Zgine/Core/Log/ScopeTimer.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

ScopeTimer::ScopeTimer(const std::string& name)
    : m_Name(name)
    , m_Start(std::chrono::high_resolution_clock::now())
{}

ScopeTimer::~ScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_Start);

    if (duration.count() < 1000) {
        ZGINE_CORE_TRACE("{} took {} μs", m_Name, duration.count());
    } else if (duration.count() < 1000000) {
        ZGINE_CORE_TRACE("{} took {:.2f} ms", m_Name, duration.count() / 1000.0);
    } else {
        ZGINE_CORE_TRACE("{} took {:.2f} s", m_Name, duration.count() / 1000000.0);
    }
}

}
