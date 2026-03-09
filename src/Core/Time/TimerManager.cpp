#include "Zgine/Core/Time/TimerManager.h"

namespace Zgine {

    TimerManager::TimerManager() : next_id_(1) {}

    bool TimerManager::CancelTimer(TimerId id) {
        // Find in id mapping. O(N) linear search is fast enough for small counts.
        // For larger scales, an unordered_map<TimerId, std::set::iterator> could be added.
        for (auto it = timers_.begin(); it != timers_.end(); ++it) {
            if (it->id == id) {
                timers_.erase(it);
                return true;
            }
        }
        return false;
    }

    std::size_t TimerManager::Tick() {
        std::size_t fired = 0;
        const double now = Timestep::GetTime();

        // Process all expired timers
        while (!timers_.empty() && timers_.begin()->expire <= now) {
            // Extract the node before executing the callback to prevent
            // iterator invalidation if the callback itself calls Add/Cancel.
            auto node = timers_.extract(timers_.begin());
            node.value().callback();
            ++fired;
        }

        return fired;
    }

}
