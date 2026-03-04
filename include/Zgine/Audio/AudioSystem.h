#pragma once

#include <Zgine/Scene/Systems/ISystem.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>

// 前向声明 miniaudio 类型
struct ma_engine;
struct ma_sound;

namespace Zgine {

class Scene;
class Entity;

class AudioSystem : public ISystem {
public:
    AudioSystem();
    ~AudioSystem() override;

    // ISystem interface implementation
    void Initialize() override;
    void Shutdown() override;
    void Update(Scene* scene, float deltaTime) override;
    const char* GetName() const override { return "AudioSystem"; }
    int GetPriority() const override { return 20; }  // Audio runs after physics

    // Legacy interface (kept for compatibility)
    void OnSceneStart(Scene* scene);
    void OnSceneStop();

    // 音频源管理
    void CreateAudioSource(Entity entity);
    void DestroyAudioSource(Entity entity);
    void PlayAudioSource(Entity entity);
    void StopAudioSource(Entity entity);
    void PauseAudioSource(Entity entity);
    void ApplyAudioSourceSettings(Entity entity);
    void RecreateAudioSource(Entity entity);

    // 更新音频源位置（3D 空间音频）
    void UpdateAudioSourcePosition(Entity entity);
    void UpdateListener(Scene* scene);

private:
    void* m_Engine = nullptr; // ma_engine* (使用 void* 避免头文件依赖)
    bool m_Initialized = false;
    Scene* m_Scene = nullptr;
};

}

