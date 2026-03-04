#pragma once

#include <cstdint>

namespace Zgine {

struct RenderStats {
    uint32_t DrawCalls = 0;
    uint32_t Triangles = 0;
};

}
