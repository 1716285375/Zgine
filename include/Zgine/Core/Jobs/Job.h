#pragma once

#include <functional>

namespace Zgine {

/**
 * @brief A single unit of work submitted to the JobSystem.
 *
 * Using std::function<void()> keeps things simple and learnable.
 * For perf-critical engines you'd use a custom allocator or coroutines,
 * but for a learning engine std::function is perfect.
 */
using Job = std::function<void()>;

} // namespace Zgine
