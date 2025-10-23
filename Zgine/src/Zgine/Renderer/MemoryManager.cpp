#include "MemoryManager.h"

namespace Zgine {

	// PerformanceTracker static members
	PerformanceTracker::FrameStats PerformanceTracker::s_CurrentFrame{};
	std::vector<PerformanceTracker::FrameStats> PerformanceTracker::s_FrameHistory{};

}
