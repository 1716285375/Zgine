# Lua 脚本绑定集成方案

## 概述

本文档详细说明如何在 Zgine 引擎中集成 Lua 脚本绑定系统，使游戏逻辑可以通过 Lua 脚本编写，而无需重新编译引擎。

## 可行性分析

### ✅ 为什么现在适合引入 Lua？

1. **架构成熟**: 已完成 ECS 架构、物理、音频、编辑器等核心系统
2. **组件化设计**: ECS 架构天然支持脚本组件
3. **序列化支持**: 已有 JSON 序列化，可以轻松扩展支持 Lua 脚本序列化
4. **编辑器完善**: 可以在编辑器中直接编辑和测试 Lua 脚本

### ✅ Lua 的优势

- **轻量级**: 核心库仅约 200KB
- **高性能**: 解释执行速度快，适合游戏逻辑
- **易集成**: C API 简单，易于与 C++ 绑定
- **热重载**: 支持运行时加载和重载脚本
- **跨平台**: 支持 Windows、Linux、macOS

### ⚠️ 注意事项

- **性能考虑**: 频繁调用的代码仍建议用 C++
- **类型安全**: Lua 是动态类型，需要做好错误处理
- **内存管理**: 需要正确管理 Lua 状态和 C++ 对象生命周期

## 技术选型

### 方案对比

| 方案 | 优点 | 缺点 | 推荐度 |
|------|------|------|--------|
| **原生 Lua + Sol2** | 性能好，功能强大，类型安全 | 需要学习 Sol2 API | ⭐⭐⭐⭐⭐ |
| **原生 Lua + LuaBridge** | 简单易用 | 功能相对较少 | ⭐⭐⭐⭐ |
| **原生 Lua + 手动绑定** | 完全控制 | 工作量大，容易出错 | ⭐⭐ |
| **LuaJIT** | 性能极佳 | 维护较少，平台支持有限 | ⭐⭐⭐ |

### 推荐方案：Lua + Sol2

**理由**：
- Sol2 是现代化的 C++/Lua 绑定库
- 支持 C++17/20 特性
- 类型安全，自动类型转换
- API 简洁，易于使用
- 活跃维护，文档完善

## 集成方案

### 1. 架构设计

```
┌─────────────────────────────────────┐
│         Lua Script System          │
├─────────────────────────────────────┤
│  ScriptComponent (ECS Component)   │
│  - ScriptPath: string              │
│  - LuaState: lua_State*            │
│  - OnStart, OnUpdate, OnDestroy    │
├─────────────────────────────────────┤
│         ScriptSystem                │
│  - Initialize/Shutdown             │
│  - Load/Reload Scripts             │
│  - Execute Script Functions        │
│  - Bind C++ API to Lua             │
└─────────────────────────────────────┘
```

### 2. 组件设计

#### ScriptComponent

```cpp
struct ScriptComponent {
    std::string ScriptPath;           // Lua 脚本文件路径
    void* LuaState = nullptr;         // 指向 Lua 脚本实例
    bool IsInitialized = false;       // 是否已初始化
    
    // 脚本函数引用（可选，用于缓存）
    // sol::function OnStart;
    // sol::function OnUpdate;
    // sol::function OnDestroy;
    
    ScriptComponent() = default;
    ScriptComponent(const std::string& path) : ScriptPath(path) {}
};
```

### 3. 系统设计

#### ScriptSystem 核心功能

1. **Lua 状态管理**
   - 每个实体可以有独立的 Lua 状态（隔离）
   - 或共享全局 Lua 状态（性能更好）

2. **API 绑定**
   - 绑定 Transform、Physics、Audio 等系统 API
   - 绑定 Entity 操作（创建、销毁、查找）
   - 绑定输入系统
   - 绑定时间系统

3. **生命周期管理**
   - OnStart: 脚本加载时调用
   - OnUpdate: 每帧调用
   - OnDestroy: 实体销毁时调用

## 实现步骤

### 步骤 1: 集成 Sol2 和 Lua

#### CMakeLists.txt 配置

```cmake
# 10. Lua
FetchContent_Declare(lua
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lua)

# 11. Sol2
FetchContent_Declare(sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sol2)

# 创建 Lua 库
if(NOT TARGET lua::lua)
    add_library(lua STATIC
        ${lua_SOURCE_DIR}/src/lapi.c
        ${lua_SOURCE_DIR}/src/lauxlib.c
        ${lua_SOURCE_DIR}/src/lbaselib.c
        ${lua_SOURCE_DIR}/src/lcode.c
        ${lua_SOURCE_DIR}/src/lcorolib.c
        ${lua_SOURCE_DIR}/src/lctype.c
        ${lua_SOURCE_DIR}/src/ldblib.c
        ${lua_SOURCE_DIR}/src/ldebug.c
        ${lua_SOURCE_DIR}/src/ldo.c
        ${lua_SOURCE_DIR}/src/ldump.c
        ${lua_SOURCE_DIR}/src/lfunc.c
        ${lua_SOURCE_DIR}/src/lgc.c
        ${lua_SOURCE_DIR}/src/linit.c
        ${lua_SOURCE_DIR}/src/liolib.c
        ${lua_SOURCE_DIR}/src/llex.c
        ${lua_SOURCE_DIR}/src/lmathlib.c
        ${lua_SOURCE_DIR}/src/lmem.c
        ${lua_SOURCE_DIR}/src/loadlib.c
        ${lua_SOURCE_DIR}/src/lobject.c
        ${lua_SOURCE_DIR}/src/lopcodes.c
        ${lua_SOURCE_DIR}/src/loslib.c
        ${lua_SOURCE_DIR}/src/lparser.c
        ${lua_SOURCE_DIR}/src/lstate.c
        ${lua_SOURCE_DIR}/src/lstring.c
        ${lua_SOURCE_DIR}/src/lstrlib.c
        ${lua_SOURCE_DIR}/src/ltable.c
        ${lua_SOURCE_DIR}/src/ltablib.c
        ${lua_SOURCE_DIR}/src/ltm.c
        ${lua_SOURCE_DIR}/src/lundump.c
        ${lua_SOURCE_DIR}/src/lutf8lib.c
        ${lua_SOURCE_DIR}/src/lvm.c
        ${lua_SOURCE_DIR}/src/lzio.c
    )
    target_include_directories(lua PUBLIC ${lua_SOURCE_DIR}/src)
    add_library(lua::lua ALIAS lua)
endif()

# 链接配置
target_link_libraries(Zgine PRIVATE
    lua::lua
)
target_include_directories(Zgine PRIVATE
    ${sol2_SOURCE_DIR}/include
)
```

### 步骤 2: 创建 ScriptSystem

#### ScriptSystem.h

```cpp
#pragma once

#include <sol/sol.hpp>
#include <unordered_map>
#include <string>

namespace Zgine {
    class Scene;
    class Entity;

    class ScriptSystem {
    public:
        ScriptSystem();
        ~ScriptSystem();

        void Initialize();
        void Shutdown();

        void OnSceneStart(Scene* scene);
        void OnSceneStop();
        void Update(Scene* scene, float deltaTime);

        // 脚本管理
        bool LoadScript(Entity entity);
        void UnloadScript(Entity entity);
        bool ReloadScript(Entity entity);

        // 获取 Lua 状态（用于绑定 API）
        sol::state& GetLuaState() { return m_LuaState; }

    private:
        void BindAPI();
        void BindTransformAPI();
        void BindPhysicsAPI();
        void BindAudioAPI();
        void BindInputAPI();
        void BindTimeAPI();
        void BindEntityAPI();

        sol::state m_LuaState;
        bool m_Initialized = false;
        Scene* m_Scene = nullptr;
        
        // 脚本实例管理（如果使用独立状态）
        std::unordered_map<Entity, sol::state> m_ScriptInstances;
    };
}
```

#### ScriptSystem.cpp 核心实现

```cpp
#include "ScriptSystem.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Core/Log.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Physics/PhysicsSystem.h"
#include "Audio/AudioSystem.h"

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
    BindPhysicsAPI();
    BindAudioAPI();
    BindInputAPI();
    BindTimeAPI();
    BindEntityAPI();
}

void ScriptSystem::BindTransformAPI() {
    auto transform = m_LuaState.new_usertype<TransformComponent>("Transform",
        "translation", &TransformComponent::Translation,
        "rotation", &TransformComponent::Rotation,
        "scale", &TransformComponent::Scale,
        "getTransform", &TransformComponent::GetTransform
    );

    // 便捷函数
    m_LuaState["setPosition"] = [](Entity entity, float x, float y, float z) {
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Translation = glm::vec3(x, y, z);
        }
    };

    m_LuaState["getPosition"] = [](Entity entity) -> sol::table {
        sol::state_view lua = sol::state_view::from(entity);
        auto table = lua.create_table();
        if (entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            table["x"] = transform.Translation.x;
            table["y"] = transform.Translation.y;
            table["z"] = transform.Translation.z;
        }
        return table;
    };
}

void ScriptSystem::BindPhysicsAPI() {
    m_LuaState["applyForce"] = [](Entity entity, float x, float y, float z) {
        // 通过 PhysicsSystem 应用力
        // 需要传入 PhysicsSystem 引用
    };

    m_LuaState["setVelocity"] = [](Entity entity, float x, float y, float z) {
        // 设置速度
    };
}

void ScriptSystem::BindAudioAPI() {
    m_LuaState["playSound"] = [](Entity entity, const std::string& path) {
        if (entity.HasComponent<AudioSourceComponent>()) {
            auto& audio = entity.GetComponent<AudioSourceComponent>();
            audio.FilePath = path;
            // 触发 AudioSystem 重新加载
        }
    };
}

void ScriptSystem::BindInputAPI() {
    m_LuaState["isKeyPressed"] = [](int key) -> bool {
        return Input::IsKeyPressed(key);
    };

    m_LuaState["isMouseButtonPressed"] = [](int button) -> bool {
        return Input::IsMouseButtonPressed(button);
    };

    m_LuaState["getMousePosition"] = []() -> sol::table {
        auto [x, y] = Input::GetMousePosition();
        sol::state_view lua = sol::state_view::from(/*...*/);
        auto table = lua.create_table();
        table["x"] = x;
        table["y"] = y;
        return table;
    };
}

void ScriptSystem::BindTimeAPI() {
    m_LuaState["getDeltaTime"] = []() -> float {
        return Time::GetDeltaTime();
    };

    m_LuaState["getTime"] = []() -> float {
        return Time::GetTime();
    };
}

void ScriptSystem::BindEntityAPI() {
    auto entityType = m_LuaState.new_usertype<Entity>("Entity",
        "hasComponent", &Entity::HasComponent<TransformComponent>,
        "getComponent", [](Entity e) {
            // 返回组件（需要具体实现）
        }
    );

    m_LuaState["findEntity"] = [this](const std::string& name) -> Entity {
        if (!m_Scene) return Entity();
        // 实现查找逻辑
        return Entity();
    };
}

void ScriptSystem::OnSceneStart(Scene* scene) {
    m_Scene = scene;

    // 加载所有脚本组件
    if (scene) {
        auto& registry = scene->GetRegistry();
        auto view = registry.view<ScriptComponent>();
        
        for (auto entity : view) {
            LoadScript(Entity(entity, scene));
        }
    }
}

void ScriptSystem::OnSceneStop() {
    if (!m_Scene) {
        return;
    }

    // 卸载所有脚本
    auto& registry = m_Scene->GetRegistry();
    auto view = registry.view<ScriptComponent>();
    
    for (auto entity : view) {
        UnloadScript(Entity(entity, m_Scene));
    }

    m_ScriptInstances.clear();
    m_Scene = nullptr;
}

void ScriptSystem::Update(Scene* scene, float deltaTime) {
    if (!m_Initialized || !scene) {
        return;
    }

    auto& registry = scene->GetRegistry();
    auto view = registry.view<ScriptComponent>();

    for (auto entity : view) {
        auto& script = registry.get<ScriptComponent>(entity);
        
        if (!script.IsInitialized) {
            continue;
        }

        // 调用 OnUpdate
        try {
            sol::protected_function onUpdate = m_LuaState["OnUpdate"];
            if (onUpdate.valid()) {
                onUpdate(Entity(entity, scene), deltaTime);
            }
        } catch (const sol::error& e) {
            ZGINE_CORE_ERROR("Script error in OnUpdate: {}", e.what());
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
        // 加载脚本文件
        auto result = m_LuaState.script_file(script.ScriptPath);
        
        if (!result.valid()) {
            sol::error err = result;
            ZGINE_CORE_ERROR("Failed to load script: {} - {}", script.ScriptPath, err.what());
            return false;
        }

        // 调用 OnStart
        sol::protected_function onStart = m_LuaState["OnStart"];
        if (onStart.valid()) {
            onStart(entity);
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
        try {
            sol::protected_function onDestroy = m_LuaState["OnDestroy"];
            if (onDestroy.valid()) {
                onDestroy(entity);
            }
        } catch (const sol::error& e) {
            ZGINE_CORE_ERROR("Script error in OnDestroy: {}", e.what());
        }
    }

    script.IsInitialized = false;
}

bool ScriptSystem::ReloadScript(Entity entity) {
    UnloadScript(entity);
    return LoadScript(entity);
}

}
```

### 步骤 3: 示例 Lua 脚本

#### assets/scripts/player.lua

```lua
-- 玩家控制脚本

local speed = 5.0
local entity = nil

function OnStart(self)
    entity = self
    print("Player script started for entity:", entity)
end

function OnUpdate(self, deltaTime)
    if not entity then
        return
    end

    -- 移动控制
    local moveX = 0.0
    local moveZ = 0.0

    if isKeyPressed(87) then -- W
        moveZ = moveZ - speed * deltaTime
    end
    if isKeyPressed(83) then -- S
        moveZ = moveZ + speed * deltaTime
    end
    if isKeyPressed(65) then -- A
        moveX = moveX - speed * deltaTime
    end
    if isKeyPressed(68) then -- D
        moveX = moveX + speed * deltaTime
    end

    if moveX ~= 0.0 or moveZ ~= 0.0 then
        local pos = getPosition(entity)
        setPosition(entity, pos.x + moveX, pos.y, pos.z + moveZ)
    end
end

function OnDestroy(self)
    print("Player script destroyed")
end
```

### 步骤 4: 在编辑器中的集成

#### 编辑器支持

1. **脚本路径编辑**: 在 Inspector 面板中编辑 ScriptComponent 的 ScriptPath
2. **脚本重载按钮**: 提供"Reload Script"按钮
3. **脚本错误显示**: 在控制台显示 Lua 错误信息
4. **脚本编辑器**: 可选集成 Lua 脚本编辑器（如 Monaco Editor）

## 性能优化建议

### 1. 脚本实例管理

**方案 A: 共享 Lua 状态（推荐）**
- 所有脚本共享一个 Lua 状态
- 性能好，内存占用少
- 需要小心命名空间冲突

**方案 B: 独立 Lua 状态**
- 每个实体有独立的 Lua 状态
- 完全隔离，但内存占用大
- 适合需要沙箱的场景

### 2. 函数缓存

```cpp
// 缓存 Lua 函数引用，避免每次查找
struct ScriptComponent {
    sol::function OnStart;
    sol::function OnUpdate;
    sol::function OnDestroy;
};
```

### 3. 批量更新

```cpp
// 批量调用，减少 Lua/C++ 边界开销
void ScriptSystem::Update(Scene* scene, float deltaTime) {
    // 收集所有需要更新的脚本
    // 一次性调用
}
```

## 安全考虑

### 1. 沙箱模式

```lua
-- 禁用危险函数
m_LuaState["os"] = nil
m_LuaState["io"] = nil
m_LuaState["debug"] = nil
```

### 2. 资源限制

```cpp
// 限制脚本执行时间
lua_sethook(m_LuaState, timeout_hook, LUA_MASKCOUNT, 1000);
```

### 3. 错误处理

```cpp
// 使用 protected_function 捕获错误
sol::protected_function func = m_LuaState["OnUpdate"];
auto result = func(entity, deltaTime);
if (!result.valid()) {
    sol::error err = result;
    // 处理错误
}
```

## 扩展功能

### 1. 热重载

```cpp
// 监听文件变化，自动重载脚本
void ScriptSystem::CheckForChanges() {
    for (auto& [entity, script] : m_ScriptInstances) {
        if (FileUtils::IsFileModified(script.ScriptPath)) {
            ReloadScript(entity);
        }
    }
}
```

### 2. 脚本调试

- 集成 Lua Debugger（如 Lua Debug）
- 支持断点、变量查看
- 与编辑器集成

### 3. 脚本序列化

```cpp
// 在 SceneSerializer 中支持 ScriptComponent
if (e.HasComponent<ScriptComponent>()) {
    auto& script = e.GetComponent<ScriptComponent>();
    entityJson["Script"] = {
        {"Path", script.ScriptPath}
    };
}
```

## 总结

### ✅ 优势

1. **开发效率**: 游戏逻辑无需重新编译
2. **灵活性**: 支持热重载和动态修改
3. **易用性**: Lua 语法简单，易于学习
4. **扩展性**: 可以轻松添加新的 API 绑定

### ⚠️ 注意事项

1. **性能**: 频繁调用的代码仍建议用 C++
2. **类型安全**: 需要做好错误处理和类型检查
3. **调试**: Lua 调试相对 C++ 更困难
4. **内存**: 需要正确管理 Lua 状态生命周期

### 📋 实施建议

1. **阶段一**: 集成 Lua + Sol2，实现基础 ScriptSystem
2. **阶段二**: 绑定核心 API（Transform、Input、Time）
3. **阶段三**: 绑定高级 API（Physics、Audio）
4. **阶段四**: 编辑器集成和热重载
5. **阶段五**: 性能优化和调试工具

## 参考资源

- [Lua 官方文档](https://www.lua.org/manual/5.5/)
- [Sol2 文档](https://sol2.readthedocs.io/)
- [Lua 游戏编程](https://www.lua.org/games/)

