#pragma once

#include <Zgine/World/Systems/ISystem.h>
#include <memory>

namespace Zgine {
    class World;
    class Entity;
    class PhysicsSystem;
    class AudioSystem;

    class ScriptSystem : public ISystem {
    public:
        ScriptSystem();
        ~ScriptSystem() override;

        // ISystem interface implementation
        void Initialize() override;
        void Shutdown() override;
        void Update(World* World, float deltaTime) override;
        const char* GetName() const override { return "ScriptSystem"; }
        int GetPriority() const override { return 30; }  // Scripts run after physics and audio

        // Legacy interface (kept for compatibility)
        void OnSceneStart(World* World);
        void OnSceneStop();

        void SetPhysicsSystem(PhysicsSystem* physicsSystem);
        void SetAudioSystem(AudioSystem* audioSystem);

        // 脚本管理
        bool LoadScript(Entity entity);
        void UnloadScript(Entity entity);
        bool ReloadScript(Entity entity);

        // 热重载：检查文件变化
        void CheckForChanges(World* World);

    private:
        void BindAPI();
        void BindTransformAPI();
        void BindInputAPI();
        void BindTimeAPI();
        void BindEntityAPI();
        void BindPhysicsAPI();
        void BindAudioAPI();

        struct Impl;

        std::unique_ptr<Impl> m_Impl;
        bool m_Initialized = false;
        World* m_World = nullptr;
        PhysicsSystem* m_PhysicsSystem = nullptr;
        AudioSystem* m_AudioSystem = nullptr;
    };
}

