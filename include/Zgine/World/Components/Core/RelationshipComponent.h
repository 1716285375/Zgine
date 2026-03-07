#pragma once

#include <vector>
#include <entt/entt.hpp>

namespace Zgine {

/**
 * @brief Entity hierarchy relationship component
 */
struct RelationshipComponent {
    entt::entity Parent = entt::null;
    std::vector<entt::entity> Children;

    RelationshipComponent() = default;
    RelationshipComponent(const RelationshipComponent&) = default;
};

} // namespace Zgine
