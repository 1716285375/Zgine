#include "zgpch.h"
#include "MiniaudioBackend.h"
#include "../Core/ResourceBackendRegistry.h"

namespace Zgine {
namespace Resources {
namespace Audio {

    // 注册Miniaudio后端
    REGISTER_RESOURCE_BACKEND(MiniaudioBackend, "Miniaudio", 0);

} // namespace Audio
} // namespace Resources
} // namespace Zgine