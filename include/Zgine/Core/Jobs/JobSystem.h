#pragma once

#include <Zgine/Core/Jobs/Job.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <vector>
#include <atomic>
#include <semaphore>

namespace Zgine {

/**
 * @brief Lightweight fixed-size thread pool for background work.
 *
 * Uses C++20 std::jthread (auto-joining on destruction) and
 * std::counting_semaphore to demonstrate modern C++ concurrency primitives.
 *
 * Design goals (learning engine):
 *   - Simple to understand: one queue, N workers
 *   - Safe: no raw thread management; jthread joins automatically
 *   - Modern: leans on C++20 stop_token for cooperative cancellation
 *
 * Usage:
 *   JobSystem js(4);  // 4 worker threads
 *   auto future = js.Submit([]{ expensiveWork(); });
 *   future.wait();    // optional — block until done
 */
class JobSystem {
public:
    /**
     * @brief Create a thread pool with the given number of workers.
     * @param threadCount Number of worker threads (0 = std::thread::hardware_concurrency).
     */
    explicit JobSystem(uint32_t threadCount = 0);

    /**
     * @brief Destructor — signals all workers to stop and joins them.
     */
    ~JobSystem();

    // Non-copyable, non-movable (owns threads)
    JobSystem(const JobSystem&)            = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    /**
     * @brief Submit a job for async execution.
     * @return std::future<void> that becomes ready when the job completes.
     */
    [[nodiscard]] std::future<void> Submit(Job job);

    /**
     * @brief Wait for all pending jobs to complete.
     */
    void WaitAll();

    /**
     * @brief Number of worker threads in this pool.
     */
    [[nodiscard]] uint32_t GetThreadCount() const { return static_cast<uint32_t>(m_Workers.size()); }

private:
    void WorkerLoop(std::stop_token stopToken);

    std::vector<std::jthread>       m_Workers;
    std::queue<std::packaged_task<void()>> m_Queue;
    std::mutex                      m_QueueMutex;
    std::condition_variable_any     m_Condition;   // supports stop_token
};

} // namespace Zgine
