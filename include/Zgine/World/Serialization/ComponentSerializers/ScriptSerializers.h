#pragma once

#include <Zgine/World/Serialization/IComponentSerializer.h>

namespace Zgine {

class ScriptSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Script"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

} // namespace Zgine
