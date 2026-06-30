#pragma once

#include <vector>
#include <Zgine/World/Core/EntityHandle.h>

namespace Zgine {

/**
 * @brief Entity hierarchy relationship component
 */
struct RelationshipComponent {
    EntityHandle Parent;
    std::vector<EntityHandle> Children;

    RelationshipComponent() = default;
    RelationshipComponent(const RelationshipComponent&) = default;
};

} // namespace Zgine
