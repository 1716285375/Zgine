# Zgine 游戏引擎架构基线

版本日期：2026-05-24

本文档是 Zgine 后续开发的架构基线。除非明确修改本文档并同步更新相关模块，后续功能、重构、测试和文档都应按这里的边界推进。

相关文档：

- `docs/constitution.md`：项目宪法和最高约束。
- `docs/engine-vision.md`：教学引擎定位和阶段目标。
- `docs/project-standards.md`：代码、测试和 OpenSpec 流程规范。
- `docs/architecture/`：Runtime/Editor、ECS、Renderer 等架构专题。
- `docs/specs/`：长期模块规格。
- `docs/changes/`：功能变更记录。

## 目标

Zgine 的目标是一个学习型但工程化的现代 3D 游戏引擎。当前阶段优先级如下：

1. 先建立可构建、可测试、可演进的运行时内核。
2. 再把渲染、资源、物理、音频、脚本接入统一生命周期。
3. 最后让编辑器成为运行时之上的工具层，而不是运行时的反向依赖。

本项目不追求一次性拆成大量小库。当前采用“单一运行时库 + 明确模块边界”的结构，等构建和测试稳定后再决定是否物理拆库。

## CMake 目标架构

最终目标应收敛到以下 CMake target：

```text
ZgineRuntime
  Core, Platform, Gui, Renderer, Resources, World, Physics, Audio, Scripting

ZgineEditorCore
  Editor panels, commands, event bus, view models, gizmo, inspectors

ZgineEditor
  editor/main.cpp

ZgineSandbox
  sandbox/main.cpp

ZgineTests
  runtime tests

ZgineEditorTests
  editor-core tests
```

强制规则：

- `ZgineRuntime` 不允许编译或链接 `src/Editor/**`。
- `ZgineEditorCore` 可以依赖 `ZgineRuntime`，反向不允许。
- `ZgineEditor` 只负责应用入口和工具启动，不放编辑器业务逻辑。
- `ZgineSandbox` 是运行时样例，不允许依赖 Editor。
- Tests 必须链接被测 target，不允许靠 include path 偷用未链接实现。

## 模块依赖方向

允许的依赖方向如下：

```text
Apps
  ZgineEditor -> ZgineEditorCore -> ZgineRuntime
  ZgineSandbox -> ZgineRuntime

ZgineRuntime
  Scripting -> Audio / Physics / World / Core / Platform
  Audio     -> World / Core
  Physics   -> World / Core
  Renderer  -> World / Resources / Platform / Core
  World     -> Resources / Core
  Resources -> Platform / Core
  Gui       -> Platform / Core
  Platform  -> Core
  Core      -> standard library only
```

禁止方向：

- `Core` 依赖任何 Zgine 高层模块。
- `Platform` 依赖 `World`、`Renderer`、`Editor`。
- `Resources` 依赖 `World` 或 `Renderer` 场景状态。
- `World` 依赖 `Editor`。
- `Renderer` 依赖 `Editor`。
- `Physics`、`Audio`、`Scripting` 依赖 `Editor`。
- 运行时模块包含 `Editor` 头文件。

第三方库边界：

- GLM 只允许在数学 backend 或底层实现中出现；公共 API 使用 `Zgine::Math::*`。
- ImGui/ImGuizmo 只允许在 `Gui`、`Editor` 和明确的 UI backend 中出现。
- OpenGL/GLFW/glad 只允许在 Renderer backend、Platform backend、应用入口或临时样例中出现。
- Jolt、miniaudio、Sol2/Lua 尽量只暴露在对应系统 `.cpp` 或最小必要 public header 中。

## Runtime 层职责

### Core

职责：

- Application、Layer、事件、日志、断言、时间、Job、内存工具、UUID、数学抽象。
- 不知道渲染、世界、资源、编辑器存在。

规则：

- 只依赖标准库和必要的轻量第三方头。
- Core 类型必须稳定，避免频繁破坏上层接口。
- Core 不处理游戏对象，也不访问 ECS registry。

### Platform

职责：

- Window、输入 backend、文件、文件监听、VFS。

规则：

- 只向上提供平台能力，不调用游戏逻辑。
- 平台 backend 的第三方细节留在 `.cpp` 或 backend 私有头。
- VFS 和 `File` 的路径规范必须统一，不能同一功能两套路径语义。

### Resources

职责：

- Asset metadata、importer、cache、hot reload、mesh/material 资源加载。

规则：

- 资源系统输出资源对象或 handle，不直接修改世界。
- GPU 资源创建必须明确线程归属；OpenGL 资源默认主线程/渲染线程。
- 异步加载只能把纯 CPU import 放到后台，cache 提交需要受控同步。

### World

职责：

- ECS registry、Entity、EntityHandle、组件、层级关系、系统调度、世界序列化。

规则：

- Component 默认是纯数据，不持有复杂所有权。
- Runtime handle 必须可重建，不能作为持久序列化标识。
- Entity 长期保存前必须明确生命周期风险；跨帧引用优先使用 UUID/handle 并验证。
- `SystemManager` 是运行时系统的唯一调度入口，系统顺序由 priority 决定。

### Renderer

职责：

- RHI 抽象、backend 选择、OpenGL reference backend、shader/texture/framebuffer、render pipeline、shadow、post-process。
- DirectX 12 和 Vulkan 作为教学路线中的显式后端目标存在；Vulkan 已具备 clear-frame swapchain 路径，包含 instance、surface、device、queue、swapchain、image view、render pass、framebuffer、command buffer、sync、acquire/submit/present 和 resize recreation，并已开始接入 vertex-array 元数据与 device-local vertex/index buffer。后续逐步补 shader module、pipeline、descriptor、depth 和 draw command recording。

规则：

- Renderer 读取 World 进行绘制，但不拥有 World。
- Renderer 不依赖 Editor；编辑器 viewport 只是 Renderer 的消费者。
- OpenGL 调用集中在 RHI/backend/pipeline，不散落到 World 或通用 Resources；旧 `Resources/Mesh/Mesh` 的 OpenGL 直连路径是待收敛项。
- DirectX 12 / Vulkan 类型不得泄漏到 public RHI 头文件。
- 非 OpenGL backend 未完成的 resource/pipeline 路径必须显式失败，不能静默 no-op。
- `RenderSystem` 生命周期必须幂等：重复 `Initialize`/`Shutdown` 不应破坏状态。

### Physics

职责：

- Jolt 初始化、物理体创建/销毁、fixed-step simulation、ECS transform 同步。

规则：

- Physics runtime handle 不写入持久场景数据。
- 物理体创建/销毁应由系统生命周期或组件事件触发，不能靠样例手动补流程。
- Fixed update 是物理唯一推进入口；variable update 不做仿真推进。

### Audio

职责：

- miniaudio engine、audio source、listener、3D spatial audio。

规则：

- Audio source runtime pointer 不序列化。
- 音频文件路径应走资源系统或统一 VFS 语义。
- Listener 选择策略必须确定：优先 `AudioListenerComponent`，否则 primary camera fallback。

### Scripting

职责：

- Lua state、脚本加载、脚本实例、脚本 API binding、热重载。

规则：

- 每个脚本实例必须隔离环境，禁止所有脚本共享一组全局 `OnStart/OnUpdate/OnDestroy`。
- 暴露给脚本的 API 必须有真实实现；无实现功能不能静默 no-op。
- 脚本调用引擎系统时必须走稳定 API，不直接操作 backend 指针。

### Gui

职责：

- ImGui layer、字体、主题、UI backend 工具。

规则：

- Gui 是运行时 UI 后端，不包含编辑器业务。
- Editor panel 使用 Gui，但 Gui 不依赖 Editor。

## Editor 层职责

Editor 是工具层，不是运行时的一部分。

职责：

- EditorContext、SelectionContext、EditorEventBus、CommandHistory。
- Panels、inspectors、scene view model、gizmo、内容浏览器。
- 调用 Runtime API 创建/保存/加载/运行场景。

规则：

- Editor 可以持有 `World*` 或 runtime service 指针，但不能改变 runtime 的底层依赖方向。
- Undo/redo 只通过 command 实现，禁止 panel 直接做不可撤销的实体修改。
- Editor event 只用于工具交互，不作为 runtime gameplay event bus。
- Editor tests 链接 `ZgineEditorCore`，不要求 Runtime 泄漏 Editor 源码。

## 生命周期基线

应用生命周期：

```text
Application
  Initialize Core services
  Create Window
  Create GuiLayer
  Push Layers
  Run loop
  Shutdown layers
  Shutdown platform services
```

世界运行生命周期：

```text
WorldRuntime or scene coordinator
  Register systems
  Initialize systems
  Load scene
  OnSceneStart systems
  FixedUpdate systems by priority
  Update systems by priority
  Render through RenderSystem
  OnSceneStop systems
  Shutdown systems
```

调度优先级：

```text
0-9    Input and time-sensitive pre-update systems
10-19  Physics
20-29  Audio
30-39  Scripting and gameplay
40-49  Animation and transform propagation
50-69  Rendering preparation
70+    Debug, telemetry, non-critical tooling
```

约束：

- owned system 和 external system 必须进入同一个全局排序结果。
- shutdown 按 initialize 的反向顺序执行。
- `OnSceneStart`/`OnSceneStop` 语义必须和 `Initialize`/`Shutdown` 分开。

## 数据和序列化原则

持久数据：

- UUID、Tag、Transform、Relationship。
- Camera、mesh/material 引用、physics/audio/script component 配置。
- 场景文件中的资产引用应使用稳定路径或 asset handle。

临时数据：

- Jolt body id。
- miniaudio source pointer。
- OpenGL texture/framebuffer/program id。
- Script cached function。
- Editor selection state、hover/focus state。

序列化规则：

- Runtime handle 不写入 JSON。
- 反序列化后由系统重建 runtime state。
- 组件新增字段必须给默认值，并保持旧场景可读。
- 序列化测试必须覆盖新增组件字段。

## 错误处理原则

- 可恢复错误返回 `bool`、`std::optional`、`Result` 风格对象，并记录必要日志。
- 构造失败且对象无法维持 invariant 时可以抛异常，但当前代码基线优先避免跨模块异常边界。
- public API 返回值有意义时使用 `[[nodiscard]]`。
- 后端失败不能静默吞掉；至少输出 module、资源路径、错误码。
- `assert` 用于开发期不变量，不替代运行时错误处理。

## 并发原则

- 默认假设 ECS registry 和 Renderer backend 不是线程安全的；AssetManager 入口由管理器级锁保护，但后台任务仍不能直接越过管理器修改 cache。
- 后台线程优先只做纯 CPU、无全局状态写入的任务；需要触碰全局状态时必须通过明确同步边界。
- OpenGL 资源创建和销毁默认只在拥有上下文的线程执行。
- 共享状态必须明确 mutex 或主线程提交点。
- 新并发代码优先使用 `std::jthread`、`stop_token`、RAII join；禁止裸线程失控。

## 架构落地顺序

P0：恢复架构边界和构建基线。

1. `ZgineRuntime` 排除 `src/Editor/**`。
2. 新增 `ZgineEditorCore`。
3. 修复 sandbox 和 tests 的 API 漂移。
4. 修复 `SystemManager` 全局排序并加测试。
5. 更新构建文档。

P1：统一运行时生命周期。

1. 建立 scene runtime coordinator 或 World runtime API。
2. 系统统一注册、初始化、场景启动、更新、停止、关闭。
3. 组件 runtime state 由系统重建。
4. Renderer lifecycle 幂等化。

P2：稳定资源、脚本、编辑器工作流。

1. Lua per-instance environment。
2. Editor save/open 接入 WorldSerializer。
3. Asset hot reload 事件与更高层资源测试。
4. Sandbox 成为运行时 API 编译样例和手动验收场景。

## 参考

- C++ Core Guidelines: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
- ISO C++ WG21 committee overview: https://isocpp.org/std/the-committee/
- CMake buildsystem manual: https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html
