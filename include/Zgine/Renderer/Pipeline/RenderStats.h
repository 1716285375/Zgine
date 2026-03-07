#pragma once

#include <cstdint>

namespace Zgine {

/** @brief Per-frame rendering statistics */
struct RenderStats {
    uint32_t DrawCalls = 0;
    uint32_t Triangles = 0;
};

} // namespace Zgine
