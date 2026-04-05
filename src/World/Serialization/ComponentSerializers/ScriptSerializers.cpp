#include <Zgine/World/Serialization/ComponentSerializers/ScriptSerializers.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// ScriptSerializer
// ============================================================================

void ScriptSerializer::Serialize(const Entity& entity, json& out) const {
    auto& script = const_cast<Entity&>(entity).GetComponent<ScriptComponent>();
    out["Script"] = json::object();
    auto& j = out["Script"];

    j["ScriptPath"] = script.ScriptPath;

    if (script.ScriptHandle.IsValid())
        j["ScriptHandle"] = script.ScriptHandle.ToString();
}

bool ScriptSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& script = entity.AddComponent<ScriptComponent>();

    if (data.contains("ScriptPath")) script.ScriptPath = data["ScriptPath"].get<std::string>();
    if (data.contains("ScriptHandle")) script.ScriptHandle = AssetHandle::FromString(data["ScriptHandle"].get<std::string>());

    return true;
}

bool ScriptSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<ScriptComponent>();
}

} // namespace Zgine
