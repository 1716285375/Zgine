# Zgine 项目规范

版本日期：2026-05-29

本文档定义 Zgine 的项目规范。后续开发默认遵守本文档；如果与旧代码冲突，新代码按本文档写，旧代码在相关改动中逐步迁移。

## 开发流程规范

Zgine 使用 OpenSpec 风格管理持续演进：

- 项目底线：`docs/constitution.md`。
- 长期模块设计：`docs/specs/*.md`。
- 架构专题：`docs/architecture/*.md`。
- 功能变更：`docs/changes/<change-id>/`。

非平凡功能默认先创建 change：

```text
proposal.md
requirements.md
design.md
tasks.md
acceptance.md
```

规则：

- 小 bugfix 可以使用轻量 change，但必须说明验收方式。
- Renderer、Asset Pipeline、Prefab、Play Mode、Scripting、Editor Docking 等复杂模块可以在 `design.md` 中使用 BMAD 多角色分析，但最终必须收敛为一个具体设计。
- AI 或人工实现前必须先读 `constitution.md` 和相关 specs。
- 实现完成后必须同步更新相关 specs、architecture 或 roadmap。
- `acceptance.md` 是最终验收依据，不以“代码看起来完成”为准。

## 标准基线

语言基线：

- 强制基线：C++20。
- 编译器扩展：禁用，`CMAKE_CXX_EXTENSIONS OFF`。
- C++26：2026 年可作为前瞻参考，但不作为项目强制基线。只有在 MSVC、Clang、GCC 以及标准库支持都被验证后，才能把某个 C++26 特性引入主线。

构建基线：

- CMake 使用 target-based 写法。
- 依赖传播必须按 `PRIVATE`、`PUBLIC`、`INTERFACE` 的真实使用面声明。
- 不使用全局 include/link/compile option 影响无关 target。
- Windows/MSVC 必须从 Visual Studio 开发环境启动，或先执行 `VsDevCmd.bat`。只写 `cl.exe` 的绝对路径不足以保证链接成功，因为 `kernel32.lib` 等 SDK 库依赖 `LIB`/`INCLUDE` 环境。
- 本机已验证工具链位置记录在 `BUILD.md`，后续 Windows 构建问题优先对照该文档排查。

测试基线：

- Runtime 和 EditorCore 都必须有可独立运行的测试 target。
- 新功能默认先补最小可验证测试，再接入样例或编辑器。

## 代码风格

### 文件命名

```text
PascalCase.h
PascalCase.cpp
PascalCaseTests.cpp
```

例外：

- 第三方库原始文件保持 upstream 命名。
- 平台资源文件按平台习惯命名，例如 `.rc`、`.desktop`。

### 命名

```cpp
class RenderSystem {};
struct TransformComponent {};
enum class RenderPath {};

void Initialize();
[[nodiscard]] bool IsInitialized() const;

int m_Width = 0;
static InputState s_Current;

constexpr float kFixedDeltaTime = 1.0f / 60.0f;
float deltaTime = 0.0f;
World* world = nullptr;
```

规则：

- 类型、函数、方法：`PascalCase`。
- 局部变量、参数：`camelCase`。
- 成员变量：`m_` 前缀。
- 静态变量：`s_` 前缀。
- 常量：`kPascalCase`。
- 枚举值：`PascalCase`。
- namespace 使用 `Zgine`，内部实现细节用匿名 namespace。

### 头文件包含顺序

`.cpp` 文件：

```cpp
#include <Zgine/Module/Foo.h>

#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Core/World.h>

#include <third_party/header.h>

#include <memory>
#include <vector>
```

规则：

- 第一行包含对应头，保证头文件自洽。
- Zgine 内部头按层级从低到高。
- 第三方头尽量只放 `.cpp`。
- 标准库头最后。
- 头文件必须自包含，不依赖 include 顺序。

### 类结构顺序

```cpp
class AudioSystem final : public ISystem {
public:
    AudioSystem();
    ~AudioSystem() override;

    void Initialize() override;
    void Shutdown() override;
    void Update(World* world, float deltaTime) override;

    void PlayAudioSource(Entity entity);

private:
    void UpdateListener(World* world);

    bool m_Initialized = false;
    World* m_World = nullptr;
};
```

规则：

- 构造/析构在前。
- 接口实现紧随其后。
- public API 再后。
- private helper 和成员变量放最后。
- 有虚析构基类的派生类析构使用 `override`。

### 格式

- 缩进：4 spaces。
- 单行建议不超过 120 字符。
- 花括号沿用当前项目风格：函数和类开括号在同一行。
- 指针和引用贴类型或变量名必须在同一文件内一致；新代码推荐 `Type* name`、`Type& name`。
- 不做纯格式化大改动，除非该文件正在被重构。

## 推荐写法

### 资源和所有权

推荐：

```cpp
std::unique_ptr<System> m_System;
std::shared_ptr<Texture> texture;
std::weak_ptr<Texture> cachedTexture;
```

规则：

- 独占所有权用 `std::unique_ptr`。
- 共享资源缓存用 `std::shared_ptr`，跨缓存引用优先 `std::weak_ptr`。
- 非拥有裸指针允许用于临时访问，但变量名和上下文必须清楚表明不拥有。
- Runtime handle 必须能 reset 和重建。

禁止：

```cpp
auto* system = new PhysicsSystem();
delete system;
```

### 初始化

推荐：

```cpp
struct CameraComponent {
    bool Primary = true;
    float NearPlane = 0.1f;
    float FarPlane = 1000.0f;
};
```

规则：

- 成员变量默认值写在声明处。
- 构造函数只处理必须的 invariant。
- 初始化可能失败时提供返回值或明确日志。

### 接口

推荐：

```cpp
[[nodiscard]] bool LoadScene(std::string_view path);
[[nodiscard]] const AssetMetadata* GetMetadata(AssetHandle handle) const;
```

规则：

- 重要返回值必须 `[[nodiscard]]`。
- 字符串只读参数优先 `std::string_view`。
- 不修改入参时使用 `const&`。
- 小型数值类型直接传值。
- API 名称表达结果，不靠注释解释副作用。

### ECS

推荐：

```cpp
auto view = world.GetRegistry().view<TransformComponent, RigidbodyComponent>();
for (auto entity : view) {
    auto& transform = view.get<TransformComponent>(entity);
}
```

规则：

- Component 保持数据化。
- Component 中的 runtime 指针不得序列化。
- 系统负责解释组件并维护 backend state。
- 修改实体层级必须通过 `World::SetParent` / `ClearParent`。
- 统计存活实体数量时使用项目封装 API 或稳定组件存储；不要直接把 EnTT entity storage 的容量当作存活实体数。

### 数学

推荐：

```cpp
Math::Vector3 position{0.0f, 1.0f, 0.0f};
Math::Matrix4 projection = Math::Matrix4::Perspective(fov, aspect, nearPlane, farPlane);
```

禁止：

```cpp
glm::vec3 position;
glm::mat4 projection;
```

例外：

- `src/Core/Math/Backend/GLM/**`。
- 第三方 API 适配层内部。

### CMake

推荐：

```cmake
target_link_libraries(ZgineEditorCore
    PUBLIC ZgineRuntime imgui::imgui
    PRIVATE ImGuizmo::ImGuizmo
)
```

规则：

- target 之间表达真实依赖。
- 头文件使用的依赖是 `PUBLIC`。
- 只在 `.cpp` 使用的依赖是 `PRIVATE`。
- header-only 且只由消费者头需要的是 `INTERFACE`。
- 不用全局 `include_directories()`。
- Windows 下需要显式保留 `rc.exe` 和 `mt.exe` 的可解析路径；CMake 通过编译器检测后仍可能在链接阶段因为 SDK 环境缺失失败。

## 禁止写法

架构类：

- Runtime 包含或链接 Editor 实现。
- 在 `Core` 中 include `World`、`Renderer`、`Editor`。
- 在 `World` component 中保存 OpenGL/Jolt/miniaudio/Sol runtime 对象本体。
- 系统绕过 `SystemManager` 自己定义一套更新顺序。
- Editor panel 直接修改场景且不能 undo/redo。

C++ 类：

- 裸 `new` / `delete` 管理业务对象。
- `using namespace` 写在头文件。
- public header 暴露 backend 专属第三方类型，除非这是该模块明确 API。
- 静默 no-op 的 public API。
- 捕获 `[&]` 后把 lambda 存到跨帧对象里。
- 返回局部对象引用或悬空 `string_view`。
- 用 `reinterpret_cast` 表达业务类型转换。

渲染/资源类：

- 在没有 GL context 的线程创建或销毁 OpenGL 对象。
- 绕过 AssetManager 同步边界在后台线程直接修改全局 cache。
- shader uniform 字符串散落重复且无集中约定。
- 默认纹理、framebuffer、shader 生命周期只靠析构碰运气。
- 低层模块在测试可单独运行时直接使用日志宏，但没有保证 `Log::Init()` 已执行。
- DirectX 12 / Vulkan 未完成的资源、管线或提交路径静默返回成功。
- 在 public RHI 头中暴露 `ID3D12*`、`Vk*` 或 backend 专属 include。

测试类：

- 只跑手动样例不写自动测试。
- 测试依赖执行顺序或共享 fixture 状态。
- 用 `ASSERT_*` 替代所有检查，导致一次失败隐藏后续问题。
- 测试 include 旧路径来维持兼容假象。

## 常见坑

### CMake GLOB 会把不该进 Runtime 的源码编进去

当前项目曾出现 `src/*.cpp` 把 `src/Editor/**` 编进 `ZgineRuntime` 的问题。新增目录后必须检查 target 源文件归属。

### Windows 绝对工具路径不等于完整工具链环境

`cl.exe`、`link.exe`、`rc.exe`、`mt.exe` 可以使用绝对路径固定版本，但 MSVC 仍需要
Visual Studio/Windows SDK 注入的 `INCLUDE`、`LIB`、`PATH`。没有开发环境时常见表现是
`CMAKE_MT-NOTFOUND`、找不到 `rc`，或链接阶段 `LNK1104: cannot open file 'kernel32.lib'`。

### owned/external system 分开排序会破坏优先级

系统调度必须对最终执行列表排序。分别排序 owned 和 external，再拼接，会导致低优先级 external system 仍在 owned system 后执行。

### Runtime handle 不能序列化

物理 body id、音频 source pointer、OpenGL id、Lua function 都是运行时状态。场景保存只保存可重建配置。

### Lua 全局函数会互相覆盖

多个脚本共享同一个 Lua global `OnUpdate` 会让后加载脚本覆盖先加载脚本。脚本实例必须隔离环境。

### `glm::` 泄漏会锁死数学 backend

公共 API 一旦接受 `glm::vec3`，后续替换数学库会变成全仓库迁移。Zgine API 使用 `Math::Vector*` / `Matrix*`。

### EnTT tombstone 不等于存活实体

删除实体后，底层 entity storage 可能仍保留 tombstone 或历史标识。测试和运行时代码需要通过 `World::GetEntityCount()`、业务组件 view，或明确的 living entity API 判断存活数量。

### 日志系统不是所有单元测试的前置条件

低层类如果可在单元测试中独立构造，不能默认日志系统完整初始化。当前 `Log::GetCoreLogger()` / `GetClientLogger()` 提供 fallback logger，日志宏在 `Log::Init()` 前允许用于诊断；应用程序入口仍必须显式执行 `Log::Init()`，以启用正式 sink、日志级别和文件输出。

### 异步资源加载不等于线程安全

即使 import 是 CPU-only，cache、metadata、dirty set 仍是共享状态。后台线程只能生成结果，提交必须同步。

### Editor 不是 Runtime

编辑器可以消费运行时能力，但运行时不能为了编辑器增加反向依赖。需要编辑器专用行为时放在 `ZgineEditorCore`。

### 多渲染后端不能只改 enum

支持 OpenGL、DirectX 12、Vulkan 至少涉及四层：窗口/Surface、Device/Swapchain、RHI resource、GUI backend。当前 OpenGL 是 reference backend，Vulkan 已进入 clear-frame swapchain 阶段，DirectX 12 仍是显式 unsupported 后端；后续必须按 `docs/rendering-backends.md` 的阶段推进。

## 测试要求

### 每次改动至少满足

- 能配置：`cmake --preset dev`。
- 能构建目标：`cmake --build --preset dev` 或更小的相关 target。
- 能运行相关测试：`ctest --preset test-default` 或相关测试可执行文件。

如果本机环境阻塞，必须在提交说明或交付说明中写清楚阻塞原因。

### 必须补测试的情况

- 新增或修改 `World`、`Entity`、组件、序列化。
- 修改 `SystemManager` 调度顺序。
- 新增 public API。
- 修复 bug。
- 修改 undo/redo command。
- 修改资源缓存、热重载、异步加载。
- 修改脚本 binding。
- 修改场景保存/加载。

### 测试命名

GoogleTest：

```cpp
TEST(SystemManagerTest, InterleavesOwnedAndExternalSystemsByPriority) {
}

TEST_F(EditorCommandHistoryTest, UndoRestoresTransform) {
}
```

规则：

- suite 名称对应被测模块。
- test 名称描述行为。
- 不使用下划线。
- 多个测试共享 setup 时使用 fixture。

### `EXPECT_*` 和 `ASSERT_*`

- 默认用 `EXPECT_*`，一次测试暴露多个失败点。
- 后续代码无法继续时才用 `ASSERT_*`，例如即将解引用指针。
- 测试应尽量验证行为，而不是只验证“函数被调用”。

### 测试层级

Runtime unit tests：

- Math、UUID、Entity、World、SystemManager、serialization、AssetMetadata。

Runtime integration tests：

- 创建 world，注册系统，推进 fixed/update，验证组件状态。
- 序列化后反序列化，验证场景等价。

EditorCore tests：

- SelectionContext、EditorEventBus、CommandHistory、Entity/Transform commands、SceneViewModel。

Compile smoke tests：

- include `Zgine/Zgine.h`。
- sandbox target 编译。
- editor target 编译。

Manual validation：

- 渲染视觉、音频播放、编辑器交互可以保留手动验收，但不能替代可自动化测试。

## Review 清单

提交前检查：

- 是否破坏模块依赖方向。
- 是否引入 public header 第三方泄漏。
- 是否有裸所有权。
- 是否有静默失败。
- 是否有 runtime state 被序列化。
- 是否新增测试或解释为什么无法测试。
- 是否更新相关文档。
- 是否保持 sandbox 或 compile smoke 可用。

## 参考

- C++ Core Guidelines: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
- ISO C++ WG21 committee overview: https://isocpp.org/std/the-committee/
- CMake buildsystem manual: https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html
- GoogleTest Primer: https://google.github.io/googletest/primer.html
