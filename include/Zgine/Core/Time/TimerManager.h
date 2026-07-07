#pragma once

#include <Zgine/Core/Foundation/Prerequisites.h>
#include <Zgine/Core/Time/Timestep.h>
#include <concepts>
#include <cstdint>
#include <functional>
#include <set>
#include <memory>

namespace Zgine {

    // ──────────────────────────────────────────────────────────────────────────────
    // Callback Concept: Constrains callback to be an invocable object with no args
    // ──────────────────────────────────────────────────────────────────────────────
    template <typename F>
    concept Callback = std::invocable<F>;

    // ──────────────────────────────────────────────────────────────────────────────
    // TimerEntry: Node stored in the Red-Black Tree (std::set)
    // ──────────────────────────────────────────────────────────────────────────────
    struct TimerEntry {
        double                 expire;    // Expiration time point in seconds (engine time)
        uint64_t               id;        // Globally unique ID (breaks ties for same time)
        std::function<void()>  callback;  // Task to execute

        // std::set sorting rule: first by time, then by id
        bool operator<(const TimerEntry& o) const noexcept {
            if (expire != o.expire) return expire < o.expire;
            return id < o.id;
        }
    };

    // ──────────────────────────────────────────────────────────────────────────────
    // TimerKey: Lightweight key for heterogeneous lookup (no callback copy)
    // ──────────────────────────────────────────────────────────────────────────────
    struct TimerKey {
        double    expire;
        uint64_t  id;
    };

    // ──────────────────────────────────────────────────────────────────────────────
    // Heterogeneous Comparator for std::set
    // ──────────────────────────────────────────────────────────────────────────────
    struct TimerCompare {
        using is_transparent = void;  // Enable C++14 heterogeneous lookup

        bool operator()(const TimerEntry& a, const TimerEntry& b) const noexcept {
            if (a.expire != b.expire) return a.expire < b.expire;
            return a.id < b.id;
        }
        bool operator()(const TimerKey& k, const TimerEntry& e) const noexcept {
            if (k.expire != e.expire) return k.expire < e.expire;
            return k.id < e.id;
        }
        bool operator()(const TimerEntry& e, const TimerKey& k) const noexcept {
            if (e.expire != k.expire) return e.expire < k.expire;
            return e.id < k.id;
        }
    };

    // ──────────────────────────────────────────────────────────────────────────────
    // TimerManager: Manages scheduled timers
    // ──────────────────────────────────────────────────────────────────────────────
    class TimerManager {
    public:
        using TimerId = uint64_t;

        TimerManager();
        ~TimerManager() = default;

        TimerManager(const TimerManager&) = delete;
        TimerManager& operator=(const TimerManager&) = delete;
        /**
         * @brief Add a one-shot timer.
         * @param delay_seconds Delay in seconds before execution (engine time based).
         * @param cb Callback function to execute.
         * @return TimerId Unique ID to cancel the timer.
         */
        template <Callback F>
        [[nodiscard]] TimerId AddTimer(double delay_seconds, F&& cb) {
            TimerId id = next_id_++;
            timers_.emplace(TimerEntry{
                .expire = Timestep::GetTime() + delay_seconds,
                .id = id,
                .callback = std::forward<F>(cb)
            });
            return id;
        }

        /**
         * @brief Add a repeating timer.
         * @param interval_seconds Interval in seconds between executions (engine time based).
         * @param cb Callback function to execute.
         * @return TimerId Unique ID to cancel the timer.
         */
        template <Callback F>
        [[nodiscard]] TimerId AddRepeatTimer(double interval_seconds, F&& cb) {
            TimerId id = next_id_++;

            // To support self-rescheduling, we use std::shared_ptr to capture the wrapper itself
            auto wrapper = std::make_shared<std::function<void()>>();
            *wrapper = [this, id, interval_seconds, cb = std::forward<F>(cb), wrapper]() mutable {
                cb();
                // Re-insert after execution
                timers_.emplace(TimerEntry{
                    .expire = Timestep::GetTime() + interval_seconds,
                    .id = id,
                    .callback = *wrapper
                });
            };

            timers_.emplace(TimerEntry{
                .expire = Timestep::GetTime() + interval_seconds,
                .id = id,
                .callback = *wrapper
            });
            return id;
        }

        /**
         * @brief Cancel a scheduled timer.
         * @param id The timer ID returned by Add*() methods.
         * @return bool True if canceled successfully, false if not found.
         */
        bool CancelTimer(TimerId id);

        /**
         * @brief Process all expired timers.
         *        This should be called once per frame in the main loop.
         * @return std::size_t Number of timers executed.
         */
        std::size_t Tick();

        [[nodiscard]] bool IsEmpty() const noexcept { return timers_.empty(); }
        [[nodiscard]] std::size_t GetSize() const noexcept { return timers_.size(); }

    private:
        std::set<TimerEntry, TimerCompare> timers_;
        uint64_t                           next_id_;
    };

}
