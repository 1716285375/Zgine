#pragma once

#include <Zgine/World/Serialization/IComponentSerializer.h>

namespace Zgine {

class AudioSourceSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "AudioSource"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

class AudioListenerSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "AudioListener"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

} // namespace Zgine
