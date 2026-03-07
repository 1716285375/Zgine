#include <Zgine/World/Core/EntityHandle.h>
#include <Zgine/World/Core/World.h>

namespace Zgine {

bool EntityHandle::IsValid(const World* World) const {
    if (!World || m_Handle == entt::null) {
        return false;
    }

    return World->GetRegistry().valid(m_Handle);
}

} // namespace Zgine
