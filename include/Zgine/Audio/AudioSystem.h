#pragma once

#include <Zgine/World/Systems/ISystem.h>
#include <memory>
#include <string>

// 前向声明 miniaudio 类型
struct ma_engine;
struct ma_sound;

namespace Zgine {

class World;
class Entity;

class AudioSystem : public ISystem {
public:
    AudioSystem();
    ~AudioSystem() override;

    // ISystem interface implementation
    void Initialize() override;
    void Shutdown() override;
    void Update(World* World, float deltaTime) override;
    const char* GetName() const override { return "AudioSystem"; }
    int GetPriority() const override { return 20; }  // Audio runs after physics

    void OnSceneStart(World* World) override;
    void OnSceneStop() override;

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
    void UpdateListener(World* World);

private:
    void* m_Engine = nullptr; // ma_engine* (使用 void* 避免头文件依赖)
    bool m_Initialized = false;
    World* m_World = nullptr;
};

}

