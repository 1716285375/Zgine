#include <Zgine/World/Core/EntityHandle.h>
#include <Zgine/World/Core/World.h>
#include "WorldRegistryAccess.h"

namespace Zgine {

bool EntityHandle::IsValid(const World* World) const {
    if (!World || !*this) {
        return false;
    }

    return Internal::GetRegistry(*World).valid(Internal::ToEnTT(*this));
}

} // namespace Zgine
