# EnTT Entity Component System Integration

## 概述
成功将EnTT Entity Component System (ECS) 库集成到Zgine引擎中，为游戏开发提供了现代化的实体组件系统架构。

## 完成的工作

### 1. EnTT库集成
- ✅ 下载并配置EnTT库到 `Zgine/vendor/entt/`
- ✅ 创建EnTT的premake脚本配置 (`Zgine/vendor/entt/premake5.lua`)
- ✅ 更新主premake5.lua文件，添加EnTT的包含目录和链接配置
- ✅ 配置EnTT为header-only库，支持模块化和单头文件两种使用方式

### 2. ECS基础架构
- ✅ 创建ECS核心组件 (`Zgine/src/Zgine/ECS/ECS.h`)
  - `Position` - 位置组件
  - `Velocity` - 速度组件  
  - `Renderable` - 可渲染组件
  - `MovementSystem` - 移动系统
  - `ECSManager` - ECS管理器

### 3. ECS测试层
- ✅ 创建ECS测试层 (`Sandbox/src/Testing/ECSTestLayer.h/cpp`)
  - 实体创建和管理
  - 组件添加和查询
  - 系统更新和渲染
  - ImGui调试界面

### 4. Sandbox集成
- ✅ 将ECS测试层集成到MainControlLayer
- ✅ 添加ECS测试窗口到Sandbox应用程序
- ✅ 实现ECS实体的实时更新和调试

## 技术特性

### EnTT配置
```lua
-- EnTT premake配置
project "EnTT"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    
    includedirs
    {
        "src",                    -- 模块化头文件
        "single_include"          -- 单头文件版本
    }
    
    defines
    {
        "ENTT_STANDARD_CPP"       -- 使用标准C++特性
    }
```

### ECS组件示例
```cpp
// 位置组件
struct Position {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
};

// 速度组件
struct Velocity {
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
};

// 可渲染组件
struct Renderable {
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    float scale = 1.0f;
};
```

### ECS系统示例
```cpp
// 移动系统
class MovementSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Position, Velocity>();
        
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& vel = view.get<Velocity>(entity);
            
            pos.position += vel.velocity * deltaTime;
        }
    }
};
```

## 使用方法

### 1. 创建实体
```cpp
auto entity = m_ECSManager.CreateEntity();
```

### 2. 添加组件
```cpp
m_ECSManager.AddComponent<Position>(entity, 
    Position{{-5.0f, 0.0f, 0.0f}});
m_ECSManager.AddComponent<Velocity>(entity, 
    Velocity{{1.0f, 0.5f, 0.0f}});
```

### 3. 查询组件
```cpp
if (m_ECSManager.HasComponent<Position>(entity)) {
    auto& pos = m_ECSManager.GetComponent<Position>(entity);
    // 使用位置数据
}
```

### 4. 系统更新
```cpp
m_ECSManager.Update(deltaTime);
```

## 测试功能

ECS测试窗口提供以下功能：
- ✅ 实体统计信息显示
- ✅ 实体位置实时监控
- ✅ 移动系统开关控制
- ✅ 移动速度调节
- ✅ 随机实体创建
- ✅ 实体重置功能

## 下一步计划

1. **扩展ECS组件系统**
   - 添加更多游戏相关组件（Transform、Sprite、Animation等）
   - 实现组件序列化功能

2. **实现更多ECS系统**
   - 渲染系统（与BatchRenderer2D/3D集成）
   - 物理系统
   - 动画系统
   - 输入系统

3. **ECS与渲染器集成**
   - 将ECS实体渲染集成到现有的BatchRenderer中
   - 实现ECS驱动的2D/3D场景渲染

4. **性能优化**
   - 实现ECS查询缓存
   - 添加ECS性能分析工具
   - 优化实体和组件内存布局

## 文件结构

```
Zgine/
├── vendor/entt/                 # EnTT库
│   ├── src/entt/               # 模块化头文件
│   ├── single_include/entt/    # 单头文件版本
│   └── premake5.lua            # EnTT premake配置
├── src/Zgine/ECS/
│   └── ECS.h                   # ECS核心组件和系统
└── Sandbox/src/Testing/
    ├── ECSTestLayer.h          # ECS测试层头文件
    └── ECSTestLayer.cpp        # ECS测试层实现
```

## 编译配置

EnTT已正确配置到premake脚本中：
- 包含目录：`Zgine/vendor/entt/src`
- 链接库：`EnTT`
- C++标准：C++17
- 平台支持：Windows x64

EnTT集成完成，可以开始使用现代化的实体组件系统进行游戏开发！
