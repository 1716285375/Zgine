#pragma once

#include <Zgine/Resources/Core/AssetHandle.h>

namespace Zgine {

/**
 * @brief Unique ID component for entity identification
 */
struct IDComponent {
    UUID ID = UUID::New();

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
    IDComponent(UUID id) : ID(id) {}
};

} // namespace Zgine
