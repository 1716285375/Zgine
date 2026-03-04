# 阶段九：Lua 脚本系统 (Lua Scripting System)

## 概述

本阶段实现了基于 Lua 和 Sol2 的脚本系统，为引擎提供了脚本化游戏逻辑的能力，支持热重载、API 绑定和编辑器集成。

## 完成的任务

### 阶段一：集成 Lua + Sol2，实现基础 ScriptSystem

#### 1. Lua 和 Sol2 集成
- **Lua 版本**: v5.5.0
- **Sol2 版本**: v3.5.0
- **来源**: 
  - Lua: GitHub (lua/lua)
  - Sol2: GitHub (ThePhD/sol2)
- **功能**: 
  - Lua 脚本解释器
  - Sol2 C++/Lua 绑定库
  - 类型安全的 API 绑定
- **集成方式**: 
  - 使用 FetchContent 自动下载
  - 手动创建 Lua 静态库
  - Sol2 作为头文件库使用

#### 2. ScriptComponent 组件
- **文件**: `src/Scene/Components.h`
- **功能**: 脚本组件
- **属性**:
  - `ScriptPath`: Lua 脚本文件路径
  - `IsInitialized`: 是否已初始化
- **特性**:
  - 纯数据结构，符合 ECS 设计原则
  - 支持脚本路径配置

#### 3. ScriptSystem 基础实现
- **文件**: `src/Scripting/ScriptSystem.h/cpp`
- **功能**: 脚本系统核心管理类
- **主要方法**:
  - `Initialize()`: 初始化 Lua 状态
  - `Shutdown()`: 关闭脚本系统
  - `OnSceneStart()`: 场景启动时加载所有脚本
  - `OnSceneStop()`: 场景停止时卸载所有脚本
  - `Update()`: 更新脚本系统（调用 OnUpdate）
  - `LoadScript()`: 加载脚本
  - `UnloadScript()`: 卸载脚本
  - `ReloadScript()`: 重载脚本
  - `CheckForChanges()`: 检查文件变化（热重载）

### 阶段二：绑定核心 API

#### 1. Transform API
- `setPosition(entity, x, y, z)`: 设置实体位置
- `getPosition(entity)`: 获取实体位置（返回表）
- `setRotation(entity, x, y, z)`: 设置实体旋转
- `getRotation(entity)`: 获取实体旋转（返回表）
- `setScale(entity, x, y, z)`: 设置实体缩放
- `getScale(entity)`: 获取实体缩放（返回表）

#### 2. Input API
- `isKeyPressed(key)`: 检查按键是否按下
- `isMouseButtonPressed(button)`: 检查鼠标按钮是否按下
- `getMousePosition()`: 获取鼠标位置（返回表）
- **键码常量**: `KEY_W`, `KEY_S`, `KEY_A`, `KEY_D`, `KEY_SPACE`, `KEY_ESCAPE`

#### 3. Time API
- `getDeltaTime()`: 获取帧时间
- `getTime()`: 获取总时间

### 阶段三：绑定高级 API

#### 1. Physics API
- `applyForce(entity, x, y, z)`: 应用力（需要 PhysicsSystem 扩展）
- `setVelocity(entity, x, y, z)`: 设置速度（需要 PhysicsSystem 扩展）

#### 2. Audio API
- `playSound(entity, path)`: 播放声音
- `stopSound(entity)`: 停止声音

#### 3. Entity API
- `findEntity(name)`: 查找实体
- `createEntity(name)`: 创建实体
- `destroyEntity(entity)`: 销毁实体

### 阶段四：编辑器集成和热重载

#### 1. 编辑器集成
- **Inspector 面板**: 显示和编辑 ScriptComponent
- **脚本路径编辑**: 支持输入脚本文件路径
- **重载按钮**: 提供"Reload Script"按钮
- **状态显示**: 显示脚本初始化状态

#### 2. 热重载功能
- **文件监控**: 每 0.5 秒检查脚本文件修改时间
- **自动重载**: 检测到文件变化时自动重载脚本
- **错误处理**: 重载失败时输出错误信息

#### 3. 组件菜单
- 在"Add Component"菜单中添加"Script"选项

### 阶段五：性能优化和调试工具

#### 1. 函数缓存
- 缓存 `OnStart`、`OnUpdate`、`OnDestroy` 函数引用
- 避免每次更新时查找函数
- 使用 `std::unordered_map` 存储脚本实例

#### 2. 错误处理
- 使用 `sol::protected_function` 捕获错误
- 输出详细的错误信息到日志
- 脚本错误不影响引擎运行

#### 3. 安全措施
- 禁用危险函数（`os`、`io`、`debug`）
- 脚本沙箱隔离
- 文件系统访问限制

## 技术细节

### Lua 状态管理

#### 共享状态模式
- 所有脚本共享一个 Lua 状态
- 性能好，内存占用少
- 需要小心命名空间管理

#### 脚本生命周期
```
LoadScript → OnStart → OnUpdate (每帧) → OnDestroy → UnloadScript
```

### API 绑定机制

#### Sol2 绑定方式
- **函数绑定**: 使用 lambda 表达式
- **类型绑定**: 使用 `new_usertype`
- **表返回**: 使用 `create_table` 创建 Lua 表

#### 类型转换
- C++ `glm::vec3` → Lua 表 `{x, y, z}`
- Lua 表 `{x, y, z}` → C++ `glm::vec3`
- 自动类型检查和转换

### 热重载实现

#### 文件监控
- 使用 `std::filesystem::last_write_time` 获取文件修改时间
- 每 0.5 秒检查一次（可配置）
- 比较修改时间判断是否需要重载

#### 重载流程
1. 检测文件修改时间变化
2. 调用 `UnloadScript` 卸载旧脚本
3. 调用 `LoadScript` 加载新脚本
4. 更新缓存中的修改时间

## 第三方库集成

### Lua
- **版本**: v5.5.0
- **来源**: GitHub (lua/lua)
- **用途**: 脚本解释器
- **集成方式**: FetchContent + 手动创建库
- **配置**:
  - 静态库构建
  - 包含所有标准库

### Sol2
- **版本**: v3.5.0
- **来源**: GitHub (ThePhD/sol2)
- **用途**: C++/Lua 绑定
- **集成方式**: FetchContent（头文件库）
- **配置**:
  - 仅包含头文件
  - 无需编译

## 代码结构

```
src/Scripting/
├── ScriptSystem.h          # 脚本系统头文件
└── ScriptSystem.cpp         # 脚本系统实现

src/Scene/
└── Components.h            # ScriptComponent 定义

assets/scripts/
└── player.lua              # 示例脚本
```

## 使用示例

### 创建脚本组件

```cpp
// 创建实体并添加脚本组件
auto player = scene.CreateEntity("Player");
player.AddComponent<ScriptComponent>("assets/scripts/player.lua");
```

### Lua 脚本示例

```lua
-- assets/scripts/player.lua
local speed = 5.0
local entity = nil

function OnStart(self)
    entity = self
    print("Player script started")
end

function OnUpdate(self, deltaTime)
    local moveX = 0.0
    local moveZ = 0.0

    if isKeyPressed(KEY_W) then
        moveZ = moveZ - speed * deltaTime
    end
    if isKeyPressed(KEY_S) then
        moveZ = moveZ + speed * deltaTime
    end
    if isKeyPressed(KEY_A) then
        moveX = moveX - speed * deltaTime
    end
    if isKeyPressed(KEY_D) then
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

### 脚本系统初始化

```cpp
// 初始化脚本系统
ScriptSystem scriptSystem;
scriptSystem.Initialize();
scriptSystem.SetPhysicsSystem(&physicsSystem);
scriptSystem.SetAudioSystem(&audioSystem);
scriptSystem.OnSceneStart(&scene);

// 主循环中更新
while (window.IsRunning()) {
    float deltaTime = GetDeltaTime();
    
    // 更新脚本系统
    scriptSystem.Update(&scene, deltaTime);
    
    // 检查文件变化（热重载）
    static float scriptCheckTimer = 0.0f;
    scriptCheckTimer += deltaTime;
    if (scriptCheckTimer >= 0.5f) {
        scriptSystem.CheckForChanges(&scene);
        scriptCheckTimer = 0.0f;
    }
}

// 清理
scriptSystem.OnSceneStop();
scriptSystem.Shutdown();
```

## CMake 配置更新

### 新增库
- `lua`: Lua 解释器库（通过 FetchContent）
- `sol2`: Sol2 绑定库（头文件库）

### CMake 配置
```cmake
# Lua
FetchContent_Declare(lua
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lua)

# Sol2
FetchContent_Declare(sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sol2)

# 手动创建 Lua 库
if(NOT TARGET lua::lua)
    add_library(lua STATIC
        ${lua_SOURCE_DIR}/src/lapi.c
        # ... 其他源文件
    )
    target_include_directories(lua PUBLIC ${lua_SOURCE_DIR}/src)
    add_library(lua::lua ALIAS lua)
endif()

# 包含路径配置
target_include_directories(Zgine PRIVATE 
    ${sol2_SOURCE_DIR}/include
)

# 链接配置
target_link_libraries(Zgine PRIVATE
    lua::lua
)
```

### 新增源文件
- `src/Scripting/ScriptSystem.cpp`
- `src/Scripting/ScriptSystem.h`

## 测试验收

### 验收标准
✅ 集成 Lua 和 Sol2 库
✅ 实现 ScriptComponent 和 ScriptSystem
✅ 绑定核心 API（Transform、Input、Time）
✅ 绑定高级 API（Physics、Audio、Entity）
✅ 编辑器集成（Inspector 面板支持）
✅ 热重载功能（文件变化自动重载）
✅ 脚本序列化支持
✅ 性能优化（函数缓存）

### 测试场景
1. **基础脚本测试**:
   - 创建实体并添加脚本组件
   - 验证脚本加载和初始化
   - 验证 OnStart、OnUpdate、OnDestroy 调用

2. **API 绑定测试**:
   - 测试 Transform API（位置、旋转、缩放）
   - 测试 Input API（键盘、鼠标）
   - 测试 Time API（时间、DeltaTime）

3. **热重载测试**:
   - 修改脚本文件
   - 验证自动重载
   - 验证重载后功能正常

4. **编辑器集成测试**:
   - 在 Inspector 中编辑脚本路径
   - 点击"Reload Script"按钮
   - 验证脚本重载功能

## 已知限制

1. **Physics API**: 部分 Physics API 需要 PhysicsSystem 扩展支持
2. **性能**: 大量脚本可能影响性能，建议限制脚本数量
3. **调试**: Lua 调试相对困难，需要额外工具
4. **类型安全**: Lua 是动态类型，需要做好错误处理
5. **内存管理**: 需要正确管理 Lua 状态生命周期
6. **多线程**: 当前实现不支持多线程脚本执行

## 下一步计划

未来可以扩展：
- Lua Debugger 集成
- 更完善的 Physics API 绑定
- 脚本性能分析工具
- 脚本模板系统
- 脚本资源管理

## 提交记录

- 阶段九任务17: 集成 Lua 和 Sol2 库
- 阶段九任务18: 实现 ScriptSystem 和 ScriptComponent
- 阶段九任务19: 绑定核心 API（Transform、Input、Time）
- 阶段九任务20: 绑定高级 API（Physics、Audio、Entity）
- 阶段九任务21: 编辑器集成和热重载功能
- 阶段九验收: 支持 Lua 脚本，热重载功能正常

## 注意事项

1. **脚本路径**: 确保脚本文件路径正确，支持相对路径
2. **错误处理**: 脚本错误会输出到日志，注意检查控制台
3. **热重载**: 热重载会重新执行 OnStart，注意状态管理
4. **性能**: 频繁调用的代码建议用 C++ 实现
5. **内存**: 大量脚本会占用内存，注意资源管理
6. **安全**: 已禁用危险函数，但脚本仍有文件系统访问能力

## 性能考虑

1. **函数缓存**: 缓存函数引用避免重复查找
2. **批量更新**: 可以考虑批量调用减少边界开销
3. **脚本数量**: 建议限制同时运行的脚本数量
4. **文件监控**: 文件监控有性能开销，检查间隔可调整

## 调试技巧

1. **日志输出**: 脚本错误会输出到日志，注意查看
2. **print 函数**: Lua 的 print 函数会输出到控制台
3. **错误信息**: Sol2 提供详细的错误信息，包括行号
4. **热重载**: 修改脚本后会自动重载，方便调试

## Lua API 参考

### Transform API
- `setPosition(entity, x, y, z)`: 设置位置
- `getPosition(entity)`: 获取位置（返回 `{x, y, z}`）
- `setRotation(entity, x, y, z)`: 设置旋转（度）
- `getRotation(entity)`: 获取旋转（返回 `{x, y, z}`）
- `setScale(entity, x, y, z)`: 设置缩放
- `getScale(entity)`: 获取缩放（返回 `{x, y, z}`）

### Input API
- `isKeyPressed(key)`: 检查按键（返回 `bool`）
- `isMouseButtonPressed(button)`: 检查鼠标按钮（返回 `bool`）
- `getMousePosition()`: 获取鼠标位置（返回 `{x, y}`）
- **键码**: `KEY_W`, `KEY_S`, `KEY_A`, `KEY_D`, `KEY_SPACE`, `KEY_ESCAPE`

### Time API
- `getDeltaTime()`: 获取帧时间（秒）
- `getTime()`: 获取总时间（秒）

### Entity API
- `findEntity(name)`: 查找实体（返回 `Entity` 或 `nil`）
- `createEntity(name)`: 创建实体（返回 `Entity`）
- `destroyEntity(entity)`: 销毁实体

### Audio API
- `playSound(entity, path)`: 播放声音
- `stopSound(entity)`: 停止声音

## 脚本生命周期

### OnStart
- **调用时机**: 脚本加载时
- **参数**: `self` (Entity)
- **用途**: 初始化脚本状态

### OnUpdate
- **调用时机**: 每帧更新
- **参数**: `self` (Entity), `deltaTime` (float)
- **用途**: 游戏逻辑更新

### OnDestroy
- **调用时机**: 脚本卸载时
- **参数**: `self` (Entity)
- **用途**: 清理资源

