#pragma once

#include <string>
#include <Zgine/Resources/Core/AssetHandle.h>

namespace Zgine {

/**
 * @brief Audio source component for 3D spatial audio
 */
struct AudioSourceComponent {
    std::string FilePath;
    AssetHandle AssetRef;       // Asset handle for the audio file
    bool IsPlaying = false;
    bool IsLooping = false;
    float Volume = 1.0f;
    float Pitch = 1.0f;
    float MinDistance = 1.0f;
    float MaxDistance = 100.0f;
    bool Spatialized = true;  // 3D spatial audio

    void* RuntimeSourcePtr = nullptr;  // Runtime miniaudio sound pointer

    AudioSourceComponent() = default;
    AudioSourceComponent(const AudioSourceComponent&) = default;
    AudioSourceComponent(const std::string& path) : FilePath(path) {}
};

/**
 * @brief Audio listener component (usually attached to camera)
 */
struct AudioListenerComponent {
    AudioListenerComponent() = default;
    AudioListenerComponent(const AudioListenerComponent&) = default;
};

} // namespace Zgine
