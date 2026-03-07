# Zgine 代码风格规范

## 一、C++ 标准

- **标准：C++20**，编译器标志 `/std:c++20`（MSVC）或 `-std=c++20`
- 禁止启用编译器扩展（`CMAKE_CXX_EXTENSIONS OFF`）

---

## 二、文件惯例

### 命名
| 类型 | 规则 | 示例 |
|------|------|------|
| 头文件 | `PascalCase.h` | `InputState.h` |
| 实现文件 | `PascalCase.cpp` | `AudioSystem.cpp` |
| 测试文件 | `PascalCaseTests.cpp` | `EditorContextTests.cpp` |

### 结构
每个头文件开头固定格式：
```cpp
#pragma once

// Purpose: 一句话说明本文件的职责
```

> Backend 实现文件（`src/Core/Math/Backend/GLM/`）是唯一允许 `#include <glm/...>` 的地方。

---

## 三、命名约定

```cpp
// 类 / 结构体 / 枚举：PascalCase
class AudioSystem {};
struct TransformComponent {};
enum class KeyCode {};

// 函数 / 方法：PascalCase
void Initialize();
[[nodiscard]] Math::Vector3 GetPosition() const;

// 成员变量：m_ 前缀
int m_Width;
bool m_Initialized = false;

// 静态成员变量：s_ 前缀
static InputState s_Current;

// 常量 / constexpr：kPascalCase 或全大写
constexpr float kMaxSpeed = 10.f;
static constexpr size_t kMaxKeys = 512;

// 局部变量 / 参数：camelCase
float deltaTime;
World* world;

// 枚举值：PascalCase
enum class MouseButton { Left, Right, Middle };
```

---

## 四、数学类型

**不要使用 `glm::` 类型**。使用 Zgine 的数学抽象层：

```cpp
// ✅ 正确
#include <Zgine/Core/Math/MathTypes.h>

Math::Vector2 mousePos;
Math::Vector3 position{0.f, 1.f, 0.f};
Math::Vector4 clearColor{0.1f, 0.1f, 0.12f, 1.f};
Math::Matrix4 transform = Math::Matrix4::Identity();

// ❌ 错误 — glm 是实现细节，不暴露
glm::vec3 position;
```

GLM 仅允许出现在 `src/Core/Math/Backend/GLM/` 中的 backend 实现文件。

---

## 五、C++20 特性使用

### `[[nodiscard]]`
凡是返回值含有重要信息的函数，必须标注：
```cpp
[[nodiscard]] bool Mount(std::string_view path);
[[nodiscard]] std::future<void> Submit(Job job);
```

### `std::string_view`
字符串参数优先使用 `string_view`（无需拷贝）：
```cpp
bool Exists(std::string_view filename);         // ✅
bool Exists(const std::string& filename);       // ❌（旧风格）
```

### Concepts
编译期约束优先用 concept 而非 SFINAE：
```cpp
template<typename T>
concept ScriptConcept = requires(T t) {
    { t.OnStart()  } -> std::same_as<void>;
    { t.OnUpdate() } -> std::same_as<void>;
};
```

### 并发
使用 C++20 原语：
```cpp
std::jthread  worker;          // 自动 join
std::counting_semaphore<N> sem;
std::condition_variable_any;   // 支持 stop_token
```

---

## 六、头文件包含顺序

```cpp
#pragma once

// 1. 对应头（cpp 文件中）
#include <Zgine/Audio/AudioSystem.h>

// 2. Zgine 内部头（按层序：Core → Platform → Renderer → World）
#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Core/Entity.h>

// 3. 第三方库（尽量只在 .cpp 或 Backend 中）
#include <miniaudio.h>

// 4. 标准库
#include <string>
#include <vector>
```

---

## 七、类结构顺序

```cpp
class AudioSystem final : public ISystem {
public:
    // 1. 构造 / 析构
    AudioSystem();
    ~AudioSystem() override;

    // 2. 接口实现
    void Initialize() override;

    // 3. 公开 API（动词开头）
    void PlayAudioSource(Entity entity);

protected:
    // （如有虚函数需重写）

private:
    // 4. 私有方法
    void UpdateListener(World* world);

    // 5. 成员变量（统一放最后）
    bool m_Initialized = false;
    World* m_World = nullptr;
};
```

---

## 八、架构依赖层序

```
Editor  ──────────────┐
   ↓                  ↓
World ──→ Renderer   Gui
   ↓         ↓
Resources  Platform/IO
   ↓         ↓
Physics  Core (Math / Jobs / Input / UUID / Log)
Audio
Scripting
```

- **Core 不依赖其他任何模块**
- **Platform/IO 只依赖 Core/Base**
- Editor 可依赖 World / Renderer，但 Runtime 不依赖 Editor

---

## 九、目录结构

```
include/Zgine/
├── Core/          Platform 无关基础设施
│   ├── Input/     轮询式输入（InputState / InputCodes / Input）
│   ├── Jobs/      线程池（Job / JobSystem）
│   ├── Math/      数学抽象（Vector2/3/4, Matrix3/4）
│   │   └── Backend/GLM/   ← 唯一的 glm:: 实现
│   └── ...
├── Platform/      平台相关
│   ├── Window/
│   └── IO/        (File / FileWatcher / VFS)
├── Renderer/
│   ├── RHI/       硬件抽象接口
│   └── Pipeline/  帧渲染逻辑
├── World/         ECS 运行时（原 Scene）
│   ├── Components/
│   ├── Systems/
│   └── Serialization/   WorldSerializer / JsonWorldSerializer
├── Scripting/     IScript (concept + virtual) / ScriptEngine
├── Editor/        编辑器（MVVM / Panels / Commands / Gizmo）
├── Audio/
├── Physics/
└── Gui/
```

---

## 十、注释规范

```cpp
/**
 * @brief 一句话描述接口职责（必须）
 *
 * 必要时说明：设计决策、边界条件、并发安全性
 */

// 行内注释：说明"为什么"，而非"是什么"
// 双语注释仅在同时维护英文/中文的模块中使用
```
