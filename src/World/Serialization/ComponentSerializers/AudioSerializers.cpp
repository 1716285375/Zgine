#include <Zgine/World/Serialization/ComponentSerializers/AudioSerializers.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// AudioSourceSerializer
// ============================================================================

void AudioSourceSerializer::Serialize(const Entity& entity, json& out) const {
    auto& audio = const_cast<Entity&>(entity).GetComponent<AudioSourceComponent>();
    out["AudioSource"] = json::object();
    auto& j = out["AudioSource"];

    j["FilePath"] = audio.FilePath;
    j["IsLooping"] = audio.IsLooping;
    j["Volume"] = audio.Volume;
    j["Pitch"] = audio.Pitch;
    j["MinDistance"] = audio.MinDistance;
    j["MaxDistance"] = audio.MaxDistance;
    j["Spatialized"] = audio.Spatialized;

    if (audio.AssetRef.IsValid())
        j["AssetRef"] = audio.AssetRef.ToString();
}

bool AudioSourceSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& audio = entity.AddComponent<AudioSourceComponent>();

    if (data.contains("FilePath")) audio.FilePath = data["FilePath"].get<std::string>();
    if (data.contains("IsLooping")) audio.IsLooping = data["IsLooping"].get<bool>();
    if (data.contains("Volume")) audio.Volume = data["Volume"].get<float>();
    if (data.contains("Pitch")) audio.Pitch = data["Pitch"].get<float>();
    if (data.contains("MinDistance")) audio.MinDistance = data["MinDistance"].get<float>();
    if (data.contains("MaxDistance")) audio.MaxDistance = data["MaxDistance"].get<float>();
    if (data.contains("Spatialized")) audio.Spatialized = data["Spatialized"].get<bool>();
    if (data.contains("AssetRef")) audio.AssetRef = AssetHandle::FromString(data["AssetRef"].get<std::string>());

    return true;
}

bool AudioSourceSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<AudioSourceComponent>();
}

// ============================================================================
// AudioListenerSerializer
// ============================================================================

void AudioListenerSerializer::Serialize(const Entity& /*entity*/, json& out) const {
    out["AudioListener"] = json::object();
}

bool AudioListenerSerializer::Deserialize(const json& /*data*/, Entity& entity) const {
    entity.AddComponent<AudioListenerComponent>();
    return true;
}

bool AudioListenerSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<AudioListenerComponent>();
}

} // namespace Zgine
