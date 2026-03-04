#pragma once

#include <string>

namespace Zgine {

/**
* @brief Tag component for entity naming
*/
struct TagComponent {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : Tag(tag) {}
};

} // namespace Zgine
