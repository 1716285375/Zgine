#include <Zgine/Scene/Core/EntityHandle.h>
#include <Zgine/Scene/Core/Scene.h>

namespace Zgine {

bool EntityHandle::IsValid(const Scene* scene) const {
    if (!scene || m_Handle == entt::null) {
        return false;
    }

    return scene->GetRegistry().valid(m_Handle);
}

} // namespace Zgine
