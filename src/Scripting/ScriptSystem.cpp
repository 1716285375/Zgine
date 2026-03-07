#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Platform/Input.h>
#include <Zgine/Core/Application/Application.h>
#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/Platform/IO/File.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

namespace Zgine {

ScriptSystem::ScriptSystem() {
}

ScriptSystem::~ScriptSystem() {
    Shutdown();
}

void ScriptSystem::Initialize() {
    if (m_Initialized) {
        return;
    }

    // 初始化 Lua 状态
    m_LuaState.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::string,
        sol::lib::math,
        sol::lib::table
    );

    // 禁用危险函数（安全考虑）
    m_LuaState["os"] = sol::nil;
    m_LuaState["io"] = sol::nil;
    m_LuaState["debug"] = sol::nil;

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
    m_LuaState = sol::state(); // 清理 Lua 状态

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
    auto transformType = m_LuaState.new_usertype<TransformComponent>("Transform",
        "translation", &TransformComponent::Translation,
        "rotation", &TransformComponent::Rotation,
        "scale", &TransformComponent::Scale
    );

    // 便捷函数
    m_LuaState["setPosition"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Translation = Math::Vector3(x, y, z);
        }
    };

    m_LuaState["getPosition"] = [this](Entity entity) -> sol::table {
        auto table = m_LuaState.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Translation.x;
            table["y"] = transform.Translation.y;
            table["z"] = transform.Translation.z;
        }
        return table;
    };

    m_LuaState["setRotation"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Rotation = Math::Vector3(x, y, z);
        }
    };

    m_LuaState["getRotation"] = [this](Entity entity) -> sol::table {
        auto table = m_LuaState.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Rotation.x;
            table["y"] = transform.Rotation.y;
            table["z"] = transform.Rotation.z;
        }
        return table;
    };

    m_LuaState["setScale"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Scale = Math::Vector3(x, y, z);
        }
    };

    m_LuaState["getScale"] = [this](Entity entity) -> sol::table {
        auto table = m_LuaState.create_table();
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
    m_LuaState["isKeyPressed"] = [](int key) -> bool {
        return Input::IsKeyPressed(key);
    };

    m_LuaState["isMouseButtonPressed"] = [](int button) -> bool {
        return Input::IsMouseButtonPressed(button);
    };

    m_LuaState["getMousePosition"] = [this]() -> sol::table {
        auto [x, y] = Input::GetMousePosition();
        auto table = m_LuaState.create_table();
        table["x"] = x;
        table["y"] = y;
        return table;
    };

    // GLFW 键码常量
    m_LuaState["KEY_W"] = GLFW_KEY_W;
    m_LuaState["KEY_S"] = GLFW_KEY_S;
    m_LuaState["KEY_A"] = GLFW_KEY_A;
    m_LuaState["KEY_D"] = GLFW_KEY_D;
    m_LuaState["KEY_SPACE"] = GLFW_KEY_SPACE;
    m_LuaState["KEY_ESCAPE"] = GLFW_KEY_ESCAPE;
}

void ScriptSystem::BindTimeAPI() {
    m_LuaState["getDeltaTime"] = []() -> float {
        return Application::Get().GetTimestep();
    };

    m_LuaState["getTime"] = []() -> float {
        return Application::Get().GetTime();
    };
}

void ScriptSystem::BindEntityAPI() {
    // 绑定 Entity 类型
    auto entityType = m_LuaState.new_usertype<Entity>("Entity");

    m_LuaState["findEntity"] = [this](const std::string& name) -> Entity {
        if (!m_World) return Entity();

        auto& registry = m_World->GetRegistry();
        auto view = registry.view<TagComponent>();

        for (auto entity : view) {
            auto& tag = registry.get<TagComponent>(entity);
            if (tag.Tag == name) {
                return Entity(entity, m_World);
            }
        }
        return Entity();
    };

    m_LuaState["createEntity"] = [this](const std::string& name) -> Entity {
        if (!m_World) return Entity();
        return m_World->CreateEntity(name);
    };

    m_LuaState["destroyEntity"] = [this](Entity entity) {
        if (!m_World) return;
        m_World->DestroyEntity(entity);
    };
}

void ScriptSystem::BindPhysicsAPI() {
    m_LuaState["applyForce"] = [this](Entity entity, float x, float y, float z) {
        // 通过 PhysicsSystem 应用力
        if (m_PhysicsSystem && entity.HasComponent<RigidbodyComponent>()) {
            // 需要 PhysicsSystem 提供应用力的接口
            // 暂时留空，等待 PhysicsSystem 扩展
            ZGINE_UNUSED(x); ZGINE_UNUSED(y); ZGINE_UNUSED(z);
        }
    };

    m_LuaState["setVelocity"] = [this](Entity entity, float x, float y, float z) {
        // 设置速度
        if (m_PhysicsSystem && entity.HasComponent<RigidbodyComponent>()) {
            // 需要 PhysicsSystem 提供设置速度的接口
            // 暂时留空，等待 PhysicsSystem 扩展
            ZGINE_UNUSED(x); ZGINE_UNUSED(y); ZGINE_UNUSED(z);
        }
    };
}

void ScriptSystem::BindAudioAPI() {
    m_LuaState["playSound"] = [this](Entity entity, const std::string& path) {
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

    m_LuaState["stopSound"] = [this](Entity entity) {
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
        auto& registry = World->GetRegistry();
        auto view = registry.view<ScriptComponent>();

        for (auto entity : view) {
            LoadScript(Entity(entity, World));
        }
    }
}

void ScriptSystem::OnSceneStop() {
    if (!m_World) {
        return;
    }

    // 卸载所有脚本
    auto& registry = m_World->GetRegistry();
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        UnloadScript(Entity(entity, m_World));
    }

    m_ScriptInstances.clear();
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

    auto& registry = World->GetRegistry();
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        auto& script = registry.get<ScriptComponent>(entity);

        if (!script.IsInitialized) {
            continue;
        }

        // 从缓存获取函数
        uint32_t entityId = static_cast<uint32_t>(entity);
        auto it = m_ScriptInstances.find(entityId);
        if (it == m_ScriptInstances.end()) {
            continue;
        }

        // 调用 OnUpdate
        try {
            if (it->second.OnUpdate.valid()) {
                auto result = it->second.OnUpdate(Entity(entity, World), deltaTime);
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

        // 执行脚本
        auto result = m_LuaState.script(scriptContent, script.ScriptPath);

        if (!result.valid()) {
            sol::error err = result;
            ZGINE_CORE_ERROR("Failed to load script: {} - {}", script.ScriptPath, err.what());
            return false;
        }

        // 缓存函数引用
        uint32_t entityId = static_cast<uint32_t>(entity);
        ScriptInstance instance;
        instance.ScriptPath = script.ScriptPath;
        instance.OnStart = m_LuaState["OnStart"];
        instance.OnUpdate = m_LuaState["OnUpdate"];
        instance.OnDestroy = m_LuaState["OnDestroy"];

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

        m_ScriptInstances[entityId] = instance;

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
        uint32_t entityId = static_cast<uint32_t>(entity);
        auto it = m_ScriptInstances.find(entityId);
        if (it != m_ScriptInstances.end()) {
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
            m_ScriptInstances.erase(it);
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

    auto& registry = World->GetRegistry();
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        auto& script = registry.get<ScriptComponent>(entity);
        if (script.ScriptPath.empty() || !script.IsInitialized) {
            continue;
        }

        uint32_t entityId = static_cast<uint32_t>(entity);
        auto it = m_ScriptInstances.find(entityId);
        if (it == m_ScriptInstances.end()) {
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
                    ReloadScript(Entity(entity, World));
                }
            }
        } catch (...) {
            // 忽略文件系统错误
        }
    }
}

}

