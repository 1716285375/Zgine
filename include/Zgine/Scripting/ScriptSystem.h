#pragma once

#include <Zgine/Scene/Systems/ISystem.h>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>

namespace Zgine {
    class Scene;
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
        void Update(Scene* scene, float deltaTime) override;
        const char* GetName() const override { return "ScriptSystem"; }
        int GetPriority() const override { return 30; }  // Scripts run after physics and audio

        // Legacy interface (kept for compatibility)
        void OnSceneStart(Scene* scene);
        void OnSceneStop();

        void SetPhysicsSystem(PhysicsSystem* physicsSystem);
        void SetAudioSystem(AudioSystem* audioSystem);

        // 脚本管理
        bool LoadScript(Entity entity);
        void UnloadScript(Entity entity);
        bool ReloadScript(Entity entity);

        // 获取 Lua 状态（用于绑定 API）
        sol::state& GetLuaState() { return m_LuaState; }

        // 热重载：检查文件变化
        void CheckForChanges(Scene* scene);

    private:
        void BindAPI();
        void BindTransformAPI();
        void BindInputAPI();
        void BindTimeAPI();
        void BindEntityAPI();
        void BindPhysicsAPI();
        void BindAudioAPI();

        sol::state m_LuaState;
        bool m_Initialized = false;
        Scene* m_Scene = nullptr;
        PhysicsSystem* m_PhysicsSystem = nullptr;
        AudioSystem* m_AudioSystem = nullptr;

        // 脚本函数缓存（性能优化）
        struct ScriptInstance {
            sol::function OnStart;
            sol::function OnUpdate;
            sol::function OnDestroy;
            std::string ScriptPath;
            int64_t LastModified = 0;
        };
        std::unordered_map<uint32_t, ScriptInstance> m_ScriptInstances;
    };
}

