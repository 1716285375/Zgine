# 阶段五：物理系统 (Physics System)

## 概述

本阶段实现了基于 Jolt Physics 的物理模拟系统，为引擎提供了完整的物理引擎集成能力，包括刚体物理、碰撞检测和物理模拟。

## 完成的任务

### 任务 9: 集成 Jolt Physics

#### 1. Jolt Physics 集成
- **版本**: v4.0.2
- **来源**: GitHub (jrouwe/JoltPhysics)
- **功能**: 
  - 高性能多线程物理引擎
  - 刚体动力学模拟
  - 碰撞检测和响应
  - 支持静态、动态、运动学刚体
- **集成方式**: 
  - 使用 FetchContent 自动下载
  - 配置为静态库构建
  - 禁用浮点异常以提升性能
  - 配置必要的编译宏

#### 2. 物理组件实现

##### RigidBodyComponent
- **文件**: `src/Scene/Components.h`
- **功能**: 刚体物理属性
- **属性**:
  - `BodyType`: 刚体类型（Static, Dynamic, Kinematic）
  - `RuntimeBodyPtr`: 运行时指向 Jolt Body 的指针/ID
  - `Mass`: 质量（默认 1.0f）
  - `Friction`: 摩擦系数（默认 0.5f）
  - `Restitution`: 弹性系数（默认 0.0f）
- **特性**:
  - 纯数据结构，符合 ECS 设计原则
  - 支持三种刚体类型
  - 可配置物理属性

##### BoxColliderComponent
- **文件**: `src/Scene/Components.h`
- **功能**: 盒形碰撞体
- **属性**:
  - `Size`: 碰撞体尺寸（默认 1.0f, 1.0f, 1.0f）
  - `Offset`: 碰撞体偏移（默认 0.0f, 0.0f, 0.0f）
- **特性**:
  - 支持尺寸和偏移配置
  - 与 TransformComponent 的 Scale 结合使用

### 任务 10: 实现 PhysicsSystem

#### 1. PhysicsSystem 类
- **文件**: `src/Physics/PhysicsSystem.h/cpp`
- **功能**: 物理系统核心管理类
- **主要方法**:
  - `Initialize()`: 初始化物理系统
  - `Shutdown()`: 关闭物理系统
  - `OnSceneStart()`: 场景启动时调用
  - `OnSceneStop()`: 场景停止时调用
  - `Step()`: 更新物理模拟
  - `CreateBody()`: 创建物理体
  - `DestroyBody()`: 销毁物理体
  - `SyncPhysicsToECS()`: 同步物理世界到 ECS

#### 2. 核心功能实现

**初始化流程**:
1. 注册 Jolt 类型系统
2. 创建工厂（Factory）
3. 创建临时分配器（TempAllocator）
4. 创建任务系统（JobSystemThreadPool）
5. 创建物理系统（PhysicsSystem）
6. 配置碰撞层和过滤器
7. 设置重力（-9.81 m/s²）

**物理体创建流程**:
1. 从 ECS 组件获取 Transform、RigidBody、Collider 数据
2. 根据 Collider 尺寸和 Transform Scale 创建碰撞形状
3. 根据 RigidBody 类型设置运动类型
4. 配置物理属性（质量、摩擦、弹性）
5. 创建 Jolt Body 并添加到物理世界
6. 将 Body ID 存储到 RuntimeBodyPtr

**物理模拟更新流程**:
1. 调用 `PhysicsSystem::Step(deltaTime)` 更新物理世界
2. 调用 `SyncPhysicsToECS()` 同步动态刚体的位置和旋转
3. 将 Jolt 的 Vec3 和 Quat 转换为 GLM 格式
4. 更新 TransformComponent 的 Translation 和 Rotation

**碰撞层系统**:
- `LAYER_NON_MOVING`: 静态物体层（0）
- `LAYER_MOVING`: 动态物体层（1）
- 实现了 BroadPhaseLayerInterface 和过滤器
- 优化碰撞检测性能

## 技术细节

### Jolt Physics 架构

#### 内存管理
- **TempAllocator**: 临时内存分配器（10MB）
- **JobSystem**: 多线程任务系统
- **Factory**: 类型注册和创建工厂

#### 碰撞检测
- **BroadPhase**: 粗检测阶段，快速筛选潜在碰撞对
- **NarrowPhase**: 精确检测阶段，计算碰撞细节
- **碰撞层**: 通过 ObjectLayer 和 BroadPhaseLayer 优化性能

#### 物理体类型
- **Static**: 静态物体，不受物理影响，用于地面、墙壁等
- **Dynamic**: 动态物体，受重力和碰撞影响
- **Kinematic**: 运动学物体，可通过代码控制，不受力影响

### 坐标系统转换

#### Jolt → GLM
- `Vec3` → `glm::vec3`: 直接转换
- `Quat` → `glm::quat` → `glm::vec3` (欧拉角): 四元数转欧拉角

#### 旋转处理
- Jolt 使用四元数存储旋转
- 转换为欧拉角（度）存储到 TransformComponent
- 简化实现，后续可优化为直接使用四元数

### 物理体生命周期

```
创建实体 → 添加组件 → OnSceneStart → CreateBody → Step → SyncPhysicsToECS
                                                              ↓
销毁实体 ← 移除组件 ← OnSceneStop ← DestroyBody ← ← ← ← ← ← ←
```

## 第三方库集成

### Jolt Physics
- **版本**: v4.0.2
- **来源**: GitHub (jrouwe/JoltPhysics)
- **用途**: 物理模拟引擎
- **集成方式**: FetchContent
- **配置**:
  - 静态库构建
  - 禁用浮点异常
  - 配置碰撞层系统
  - 多线程支持

## 代码结构

```
src/Physics/
├── PhysicsSystem.h          # 物理系统头文件
└── PhysicsSystem.cpp        # 物理系统实现

src/Scene/
└── Components.h             # 物理组件定义
    ├── RigidBodyComponent
    └── BoxColliderComponent
```

## 使用示例

### 创建静态地面

```cpp
// 创建地板实体
auto floor = scene.CreateEntity("Floor");
auto& floorTransform = floor.GetComponent<TransformComponent>();
floorTransform.Translation = glm::vec3(0.0f, -2.0f, 0.0f);
floorTransform.Scale = glm::vec3(10.0f, 0.5f, 10.0f);

// 添加物理组件
floor.AddComponent<RigidBodyComponent>(RigidBodyComponent::Type::Static);
floor.AddComponent<BoxColliderComponent>(glm::vec3(10.0f, 0.5f, 10.0f));

// 创建物理体
physicsSystem.CreateBody(floor);
```

### 创建动态立方体

```cpp
// 创建立方体实体
auto cube = scene.CreateEntity("Cube");
auto& cubeTransform = cube.GetComponent<TransformComponent>();
cubeTransform.Translation = glm::vec3(0.0f, 5.0f, 0.0f);

// 添加物理组件
cube.AddComponent<RigidBodyComponent>(RigidBodyComponent::Type::Dynamic);
auto& cubeRB = cube.GetComponent<RigidBodyComponent>();
cubeRB.Mass = 1.0f;
cubeRB.Friction = 0.5f;
cubeRB.Restitution = 0.3f; // 弹性
cube.AddComponent<BoxColliderComponent>(glm::vec3(1.0f, 1.0f, 1.0f));

// 创建物理体
physicsSystem.CreateBody(cube);
```

### 物理系统更新循环

```cpp
// 初始化
PhysicsSystem physicsSystem;
physicsSystem.Initialize();
physicsSystem.OnSceneStart(&scene);

// 主循环
while (window.IsRunning()) {
    float deltaTime = GetDeltaTime();
    
    // 更新物理模拟
    physicsSystem.Step(deltaTime);
    
    // 同步物理到 ECS
    physicsSystem.SyncPhysicsToECS(&scene);
    
    // 渲染...
}

// 清理
physicsSystem.OnSceneStop();
physicsSystem.Shutdown();
```

## CMake 配置更新

### 新增库
- `Jolt`: 物理引擎库（通过 FetchContent）

### CMake 配置
```cmake
# Jolt Physics
FetchContent_Declare(jolt
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG v4.0.2
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR Build
)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
set(JPH_FLOATING_POINT_EXCEPTIONS_ENABLED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(jolt)

# 链接配置
target_link_libraries(Zgine PRIVATE
    Jolt
)

# 编译宏
target_compile_definitions(Zgine PRIVATE
    JPH_DEBUG_RENDERER
    JPH_FLOATING_POINT_EXCEPTIONS_ENABLED=0
)
```

### 新增源文件
- `src/Physics/PhysicsSystem.cpp`
- `src/Physics/PhysicsSystem.h`

## 测试验收

### 验收标准
✅ 集成 Jolt Physics 库
✅ 实现 RigidBodyComponent 和 BoxColliderComponent
✅ 实现 PhysicsSystem 类
✅ 支持静态和动态刚体
✅ 实现物理模拟更新
✅ 实现物理到 ECS 的同步
✅ 立方体下落并停在地面

### 测试场景
1. **基础物理测试**:
   - 创建静态地面
   - 创建动态立方体（初始位置在地面上方）
   - 立方体受重力下落
   - 立方体与地面碰撞并停止

2. **物理属性测试**:
   - 测试不同质量对下落速度的影响
   - 测试摩擦系数对滑动的影响
   - 测试弹性系数对反弹的影响

3. **多物体测试**:
   - 创建多个动态物体
   - 验证物体之间的碰撞
   - 验证性能表现

## 已知限制

1. **碰撞形状**: 目前仅支持盒形碰撞体（BoxCollider），其他形状（球体、胶囊体等）待实现
2. **旋转同步**: 旋转同步使用简化的欧拉角转换，可能在某些情况下不够精确
3. **物理材质**: 物理材质系统较为基础，完整的材质系统待后续实现
4. **约束系统**: 尚未实现关节和约束（铰链、弹簧等）
5. **触发器**: 尚未实现触发器（Trigger）功能
6. **性能优化**: 物理体数量限制为 1024，可根据需求调整
7. **碰撞回调**: 尚未实现碰撞事件回调系统

## 下一步计划

阶段六将实现：
- 编辑器系统（ImGui 集成）
- 场景层次结构视图（Hierarchy）
- 属性检视面板（Inspector）
- 实时属性编辑
- 场景可视化编辑

## 提交记录

- 阶段五任务9: 集成 Jolt Physics 库
- 阶段五任务10: 实现 PhysicsSystem 和物理组件
- 阶段五验收: 支持物理模拟，立方体下落并停在地面

## 注意事项

1. **物理体创建时机**: 必须在 `OnSceneStart()` 之后创建物理体，确保物理系统已初始化
2. **组件依赖**: 创建物理体需要同时拥有 `RigidBodyComponent`、`TransformComponent` 和 `BoxColliderComponent`
3. **同步顺序**: 物理更新后必须调用 `SyncPhysicsToECS()` 才能看到物理效果
4. **内存管理**: Jolt 使用自定义内存管理，确保在 `Shutdown()` 时正确清理
5. **线程安全**: Jolt 支持多线程，但当前实现使用单线程模式
6. **坐标系统**: Jolt 使用右手坐标系，与 OpenGL 一致
7. **重力方向**: 默认重力为 -Y 方向（向下），可通过 `SetGravity()` 修改
8. **物理体销毁**: 场景停止时会自动销毁所有物理体，无需手动清理

## 性能考虑

1. **物理体数量**: 当前配置支持最多 1024 个物理体，可根据场景需求调整
2. **碰撞对数量**: 最多支持 1024 个碰撞对，复杂场景可能需要增加
3. **时间步长**: 使用固定时间步长（deltaTime），确保物理模拟稳定性
4. **多线程**: Jolt 支持多线程物理计算，当前使用硬件线程数 - 1
5. **内存分配**: 临时分配器大小为 10MB，可根据需求调整

## 调试技巧

1. **日志输出**: 物理系统初始化、创建/销毁物理体时会输出日志
2. **可视化调试**: 启用 `JPH_DEBUG_RENDERER` 宏可进行可视化调试（需要额外实现）
3. **物理体状态**: 通过 `RuntimeBodyPtr` 可以访问 Jolt Body 的详细信息
4. **碰撞检测**: 可以通过 Jolt 的调试工具查看碰撞检测过程

