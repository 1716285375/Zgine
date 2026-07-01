#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Input/Input.h>
#include <Zgine/Core/Application/Application.h>
#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/Platform/IO/File.h>
#include <World/Core/WorldRegistryAccess.h>

#include <sol/sol.hpp>

#include <ctime>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace Zgine {

namespace {
    KeyCode ToKeyCode(int key) {
        if (key < 0 || key >= static_cast<int>(InputState::kMaxKeys)) {
            return KeyCode::None;
        }
        return static_cast<KeyCode>(key);
    }

    MouseButton ToMouseButton(int button) {
        if (button < 0 || button >= static_cast<int>(InputState::kMaxButtons)) {
            return MouseButton::None;
        }
        return static_cast<MouseButton>(button);
    }
}

struct ScriptSystem::Impl {
    sol::state LuaState;

    struct ScriptInstance {
        sol::environment Environment;
        sol::function OnStart;
        sol::function OnUpdate;
        sol::function OnDestroy;
        std::string ScriptPath;
        int64_t LastModified = 0;
    };

    std::unordered_map<uint32_t, ScriptInstance> ScriptInstances;
};

ScriptSystem::ScriptSystem()
    : m_Impl(std::make_unique<Impl>())
{
}

ScriptSystem::~ScriptSystem() {
    Shutdown();
}

void ScriptSystem::Initialize() {
    if (m_Initialized) {
        return;
    }

    // 初始化 Lua 状态
    m_Impl->LuaState.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::string,
        sol::lib::math,
        sol::lib::table
    );

    // 禁用危险函数（安全考虑）
    m_Impl->LuaState["os"] = sol::nil;
    m_Impl->LuaState["io"] = sol::nil;
    m_Impl->LuaState["debug"] = sol::nil;

    // 绑定 C++ API
    BindAPI();

    m_Initialized = true;
    ZGINE_CORE_INFO("Script System Initialized");
}

void ScriptSystem::Shutdown() {
    if (!m_Initialized) {
        return;
    }

    OnSceneStop();
    m_Impl->LuaState = sol::state(); // 清理 Lua 状态

    m_Initialized = false;
    ZGINE_CORE_INFO("Script System Shutdown");
}

void ScriptSystem::BindAPI() {
    BindTransformAPI();
    BindInputAPI();
    BindTimeAPI();
    BindEntityAPI();
    BindPhysicsAPI();
    BindAudioAPI();
}

void ScriptSystem::BindTransformAPI() {
    // 绑定 TransformComponent
    auto transformType = m_Impl->LuaState.new_usertype<TransformComponent>("Transform",
        "translation", &TransformComponent::Translation,
        "rotation", &TransformComponent::Rotation,
        "scale", &TransformComponent::Scale
    );

    // 便捷函数
    m_Impl->LuaState["setPosition"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Translation = Math::Vector3(x, y, z);
        }
    };

    m_Impl->LuaState["getPosition"] = [this](Entity entity) -> sol::table {
        auto table = m_Impl->LuaState.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Translation.x;
            table["y"] = transform.Translation.y;
            table["z"] = transform.Translation.z;
        }
        return table;
    };

    m_Impl->LuaState["setRotation"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Rotation = Math::Vector3(x, y, z);
        }
    };

    m_Impl->LuaState["getRotation"] = [this](Entity entity) -> sol::table {
        auto table = m_Impl->LuaState.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Rotation.x;
            table["y"] = transform.Rotation.y;
            table["z"] = transform.Rotation.z;
        }
        return table;
    };

    m_Impl->LuaState["setScale"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Scale = Math::Vector3(x, y, z);
        }
    };

    m_Impl->LuaState["getScale"] = [this](Entity entity) -> sol::table {
        auto table = m_Impl->LuaState.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Scale.x;
            table["y"] = transform.Scale.y;
            table["z"] = transform.Scale.z;
        }
        return table;
    };
}

void ScriptSystem::BindInputAPI() {
    m_Impl->LuaState["isKeyPressed"] = [](int key) -> bool {
        return Input::IsKeyDown(ToKeyCode(key));
    };

    m_Impl->LuaState["isMouseButtonPressed"] = [](int button) -> bool {
        return Input::IsMouseButtonDown(ToMouseButton(button));
    };

    m_Impl->LuaState["getMousePosition"] = [this]() -> sol::table {
        const Math::Vector2 position = Input::GetMousePosition();
        auto table = m_Impl->LuaState.create_table();
        table["x"] = position.x;
        table["y"] = position.y;
        return table;
    };

    m_Impl->LuaState["KEY_W"] = static_cast<int>(KeyCode::W);
    m_Impl->LuaState["KEY_S"] = static_cast<int>(KeyCode::S);
    m_Impl->LuaState["KEY_A"] = static_cast<int>(KeyCode::A);
    m_Impl->LuaState["KEY_D"] = static_cast<int>(KeyCode::D);
    m_Impl->LuaState["KEY_SPACE"] = static_cast<int>(KeyCode::Space);
    m_Impl->LuaState["KEY_ESCAPE"] = static_cast<int>(KeyCode::Escape);
}

void ScriptSystem::BindTimeAPI() {
    m_Impl->LuaState["getDeltaTime"] = []() -> float {
        return Application::Get().GetTimestep();
    };

    m_Impl->LuaState["getTime"] = []() -> float {
        return Application::Get().GetTime();
    };
}

void ScriptSystem::BindEntityAPI() {
    // 绑定 Entity 类型
    auto entityType = m_Impl->LuaState.new_usertype<Entity>("Entity");

    m_Impl->LuaState["findEntity"] = [this](const std::string& name) -> Entity {
        if (!m_World) return Entity();

        auto& registry = Internal::GetRegistry(*m_World);
        auto view = registry.view<TagComponent>();

        for (auto entity : view) {
            auto& tag = registry.get<TagComponent>(entity);
            if (tag.Tag == name) {
                return Entity(Internal::FromEnTT(entity), m_World);
            }
        }
        return Entity();
    };

    m_Impl->LuaState["createEntity"] = [this](const std::string& name) -> Entity {
        if (!m_World) return Entity();
        return m_World->CreateEntity(name);
    };

    m_Impl->LuaState["destroyEntity"] = [this](Entity entity) {
        if (!m_World) return;
        m_World->DestroyEntity(entity);
    };
}

void ScriptSystem::BindPhysicsAPI() {
    m_Impl->LuaState["applyForce"] = [this](Entity entity, float x, float y, float z) {
        if (m_PhysicsSystem && entity.HasComponent<RigidbodyComponent>()) {
            m_PhysicsSystem->ApplyForce(entity, Math::Vector3(x, y, z));
        }
    };

    m_Impl->LuaState["setVelocity"] = [this](Entity entity, float x, float y, float z) {
        if (m_PhysicsSystem && entity.HasComponent<RigidbodyComponent>()) {
            m_PhysicsSystem->SetLinearVelocity(entity, Math::Vector3(x, y, z));
        }
    };
}

void ScriptSystem::BindAudioAPI() {
    m_Impl->LuaState["playSound"] = [this](Entity entity, const std::string& path) {
        if (entity.HasComponent<AudioSourceComponent>()) {
            auto& audio = entity.GetComponent<AudioSourceComponent>();
            audio.FilePath = path;
            audio.IsPlaying = true;
            // 触发 AudioSystem 重新加载
            if (m_AudioSystem) {
                m_AudioSystem->RecreateAudioSource(entity);
                m_AudioSystem->PlayAudioSource(entity);
            }
        }
    };

    m_Impl->LuaState["stopSound"] = [this](Entity entity) {
        if (entity.HasComponent<AudioSourceComponent>()) {
            if (m_AudioSystem) {
                m_AudioSystem->StopAudioSource(entity);
            }
        }
    };
}

void ScriptSystem::OnSceneStart(World* World) {
    m_World = World;

    // 加载所有脚本组件
    if (World) {
        auto& registry = Internal::GetRegistry(*World);
        auto view = registry.view<ScriptComponent>();

        for (auto entity : view) {
            LoadScript(Entity(Internal::FromEnTT(entity), World));
        }
    }
}

void ScriptSystem::OnSceneStop() {
    if (!m_World) {
        return;
    }

    // 卸载所有脚本
    auto& registry = Internal::GetRegistry(*m_World);
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        UnloadScript(Entity(Internal::FromEnTT(entity), m_World));
    }

    m_Impl->ScriptInstances.clear();
    m_World = nullptr;
}

void ScriptSystem::SetPhysicsSystem(PhysicsSystem* physicsSystem) {
    m_PhysicsSystem = physicsSystem;
}

void ScriptSystem::SetAudioSystem(AudioSystem* audioSystem) {
    m_AudioSystem = audioSystem;
}

void ScriptSystem::Update(World* World, float deltaTime) {
    if (!m_Initialized || !World) {
        return;
    }

    auto& registry = Internal::GetRegistry(*World);
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        auto& script = registry.get<ScriptComponent>(entity);

        if (!script.IsInitialized) {
            continue;
        }

        // 从缓存获取函数
        const Entity scriptEntity(Internal::FromEnTT(entity), World);
        const uint32_t entityId = scriptEntity.GetHandle().GetValue();
        auto it = m_Impl->ScriptInstances.find(entityId);
        if (it == m_Impl->ScriptInstances.end()) {
            continue;
        }

        // 调用 OnUpdate
        try {
            if (it->second.OnUpdate.valid()) {
                auto result = it->second.OnUpdate(scriptEntity, deltaTime);
                if (!result.valid()) {
                    sol::error err = result;
                    ZGINE_CORE_ERROR("Script error in OnUpdate (entity {}): {}", entityId, err.what());
                }
            }
        } catch (const sol::error& e) {
            ZGINE_CORE_ERROR("Script exception in OnUpdate (entity {}): {}", entityId, e.what());
        }
    }
}

bool ScriptSystem::LoadScript(Entity entity) {
    if (!entity.HasComponent<ScriptComponent>()) {
        return false;
    }

    auto& script = entity.GetComponent<ScriptComponent>();

    if (script.ScriptPath.empty()) {
        ZGINE_CORE_WARN("ScriptComponent has no script path");
        return false;
    }

    try {
        // 读取脚本文件
        std::string scriptContent = File::ReadFile(script.ScriptPath);
        if (scriptContent.empty()) {
            ZGINE_CORE_ERROR("Failed to read script file: {}", script.ScriptPath);
            return false;
        }

        // Each entity script gets its own environment. Engine bindings remain
        // available through the global fallback, while script globals stay local.
        sol::environment environment(m_Impl->LuaState, sol::create, m_Impl->LuaState.globals());
        auto result = m_Impl->LuaState.script(
            scriptContent,
            environment,
            sol::script_default_on_error,
            script.ScriptPath);

        if (!result.valid()) {
            sol::error err = result;
            ZGINE_CORE_ERROR("Failed to load script: {} - {}", script.ScriptPath, err.what());
            return false;
        }

        // 缓存函数引用
        uint32_t entityId = entity.GetHandle().GetValue();
        Impl::ScriptInstance instance;
        instance.Environment = environment;
        instance.ScriptPath = script.ScriptPath;
        instance.OnStart = environment["OnStart"];
        instance.OnUpdate = environment["OnUpdate"];
        instance.OnDestroy = environment["OnDestroy"];

        // 获取文件修改时间
        try {
            if (fs::exists(script.ScriptPath)) {
                auto fileTime = fs::last_write_time(script.ScriptPath);
                auto timeSinceEpoch = fileTime.time_since_epoch();
                instance.LastModified = std::chrono::duration_cast<std::chrono::seconds>(
                    timeSinceEpoch).count();
            } else {
                instance.LastModified = 0;
            }
        } catch (...) {
            instance.LastModified = 0;
        }

        m_Impl->ScriptInstances[entityId] = instance;

        // 调用 OnStart
        if (instance.OnStart.valid()) {
            auto startResult = instance.OnStart(entity);
            if (!startResult.valid()) {
                sol::error err = startResult;
                ZGINE_CORE_ERROR("Script error in OnStart: {}", err.what());
            }
        }

        script.IsInitialized = true;
        ZGINE_CORE_INFO("Loaded script: {}", script.ScriptPath);
        return true;
    } catch (const sol::error& e) {
        ZGINE_CORE_ERROR("Script error: {}", e.what());
        return false;
    }
}

void ScriptSystem::UnloadScript(Entity entity) {
    if (!entity.HasComponent<ScriptComponent>()) {
        return;
    }

    auto& script = entity.GetComponent<ScriptComponent>();

    if (script.IsInitialized) {
        // 调用 OnDestroy
        uint32_t entityId = entity.GetHandle().GetValue();
        auto it = m_Impl->ScriptInstances.find(entityId);
        if (it != m_Impl->ScriptInstances.end()) {
            try {
                if (it->second.OnDestroy.valid()) {
                    auto result = it->second.OnDestroy(entity);
                    if (!result.valid()) {
                        sol::error err = result;
                        ZGINE_CORE_ERROR("Script error in OnDestroy: {}", err.what());
                    }
                }
            } catch (const sol::error& e) {
                ZGINE_CORE_ERROR("Script exception in OnDestroy: {}", e.what());
            }
            m_Impl->ScriptInstances.erase(it);
        }
    }

    script.IsInitialized = false;
}

bool ScriptSystem::ReloadScript(Entity entity) {
    UnloadScript(entity);
    return LoadScript(entity);
}

void ScriptSystem::CheckForChanges(World* World) {
    if (!m_Initialized || !World) {
        return;
    }

    auto& registry = Internal::GetRegistry(*World);
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        auto& script = registry.get<ScriptComponent>(entity);
        if (script.ScriptPath.empty() || !script.IsInitialized) {
            continue;
        }

        const Entity scriptEntity(Internal::FromEnTT(entity), World);
        const uint32_t entityId = scriptEntity.GetHandle().GetValue();
        auto it = m_Impl->ScriptInstances.find(entityId);
        if (it == m_Impl->ScriptInstances.end()) {
            continue;
        }

        try {
            if (fs::exists(script.ScriptPath)) {
                auto fileTime = fs::last_write_time(script.ScriptPath);
                auto timeSinceEpoch = fileTime.time_since_epoch();
                auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(
                    timeSinceEpoch).count();
                if (currentTime != it->second.LastModified) {
                    ZGINE_CORE_INFO("Script file changed, reloading: {}", script.ScriptPath);
                    ReloadScript(Entity(Internal::FromEnTT(entity), World));
                }
            }
        } catch (...) {
            // 忽略文件系统错误
        }
    }
}

}

