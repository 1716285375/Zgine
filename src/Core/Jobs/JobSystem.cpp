#include <Zgine/Core/Jobs/JobSystem.h>
#include <algorithm>

namespace Zgine {

JobSystem::JobSystem(uint32_t threadCount) {
    if (threadCount == 0)
        threadCount = std::max(1u, std::thread::hardware_concurrency());

    m_Workers.reserve(threadCount);
    for (uint32_t i = 0; i < threadCount; ++i) {
        m_Workers.emplace_back([this](std::stop_token st) {
            WorkerLoop(std::move(st));
        });
    }
}

JobSystem::~JobSystem() {
    // jthread destructors automatically request_stop() then join().
}

std::future<void> JobSystem::Submit(Job job) {
    std::packaged_task<void()> task(std::move(job));
    std::future<void> future = task.get_future();

    {
        std::scoped_lock lock(m_QueueMutex);
        m_Queue.push(std::move(task));
    }
    m_Condition.notify_one();

    return future;
}

void JobSystem::WaitAll() {
    // Simple strategy: submit a sentinel per worker and wait for all.
    std::vector<std::future<void>> barriers;
    barriers.reserve(m_Workers.size());
    for (size_t i = 0; i < m_Workers.size(); ++i)
        barriers.push_back(Submit([] {}));
    for (auto& f : barriers) f.wait();
}

void JobSystem::WorkerLoop(std::stop_token stopToken) {
    while (true) {
        std::packaged_task<void()> task;

        {
            std::unique_lock lock(m_QueueMutex);
            // Wait until there's work OR a stop was requested.
            m_Condition.wait(lock, stopToken, [this] {
                return !m_Queue.empty();
            });

            if (stopToken.stop_requested() && m_Queue.empty())
                return;

            task = std::move(m_Queue.front());
            m_Queue.pop();
        }

        task();   // Execute outside the lock
    }
}

} // namespace Zgine
