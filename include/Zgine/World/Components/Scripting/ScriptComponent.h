#pragma once

#include <string>
#include <Zgine/Resources/Core/AssetHandle.h>

namespace Zgine {

/**
 * @brief Lua script component
 */
struct ScriptComponent {
    std::string ScriptPath;
    AssetHandle ScriptHandle;
    bool IsInitialized = false;

    ScriptComponent() = default;
    ScriptComponent(const ScriptComponent&) = default;
    ScriptComponent(const std::string& path) : ScriptPath(path) {}
};

} // namespace Zgine
