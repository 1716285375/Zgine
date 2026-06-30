# Zgine Constitution

版本日期：2026-05-25

本文档是 Zgine 的项目宪法。所有架构设计、OpenSpec change、AI 生成代码、代码评审和测试验收都必须先服从这里的约束。更细的模块规则记录在 `docs/specs/` 和 `docs/architecture/`。

## 1. 项目定位

Zgine 是学习型自研 3D 游戏引擎，不追求生产级引擎的一次性完整度。项目目标是把底层引擎机制讲清楚、写清楚、测清楚，并逐步形成可制作小型游戏的最小完备引擎。

设计优先级：

1. 可理解：实现要暴露关键机制，避免过早用大型黑盒封装隐藏学习价值。
2. 可构建：每个阶段都必须保持可配置、可编译、可测试。
3. 可演进：新增功能必须遵守模块边界，不能为了短期运行破坏长期结构。
4. 可验证：核心行为需要自动测试；图形、音频和编辑器交互允许保留手动验收，但不能替代可自动化测试。

## 2. 开发流程

Zgine 采用 OpenSpec 风格的渐进式变更管理：

- 日常功能使用 `docs/changes/<change-id>/`。
- 长期模块设计写入 `docs/specs/`。
- 项目底线写入本文档。
- 大模块设计可以局部使用 BMAD 多角色分析，并把最终结论落入对应 `design.md`。

每个非平凡功能默认包含：

```text
proposal.md
requirements.md
design.md
tasks.md
acceptance.md
```

小修复可以使用轻量 change，但必须说明：

- 为什么改。
- 改哪些模块。
- 如何验收。
- 是否需要测试。

## 3. 模块边界

强制依赖方向：

```text
ZgineEditor -> ZgineEditorCore -> ZgineRuntime
ZgineSandbox -> ZgineRuntime

ZgineRuntime:
Core <- Platform <- Resources <- World
Core <- Gui
Core/Platform/Resources/World <- Renderer
Core/World <- Physics / Audio / Scripting
```

禁止：

- Runtime 依赖 Editor。
- Renderer 依赖 Editor。
- Core 依赖 World、Renderer、Resources、Physics、Audio、Scripting、Editor。
- Platform 调用游戏逻辑。
- Resources 直接修改 World。
- Editor panel 绕过命令系统执行需要 undo/redo 的场景修改。

## 4. ECS 原则

- Component 默认只存数据。
- System 负责解释组件、维护运行时状态、和外部库交互。
- Entity 层级只能通过 `World` 的公开 API 修改。
- Runtime handle 不作为持久化身份；保存场景时使用可重建数据和 UUID。
- 不在组件里持有 OpenGL、Vulkan、Jolt、miniaudio、Lua 等 backend 对象本体。

## 5. Renderer 原则

- OpenGL 是 reference backend。
- Vulkan 是跨平台图形 API 教学主线，按 device/swapchain、resource、shader/pipeline、descriptor、draw recording、depth、GUI backend 的顺序推进。
- DirectX 12 保持显式 selectable stub，直到对应阶段实现。
- public RHI header 不暴露 `Vk*`、`ID3D12*` 或 backend 私有 include。
- 未完成 backend 功能必须显式失败或清晰告警，不能静默伪成功。
- OpenGL/Vulkan/DirectX 调用集中在对应 backend，不散落到 World、Editor 或通用 Resources。

## 6. Asset 和 Serialization 原则

- Asset 系统输出资源对象、metadata 或 handle，不直接操纵场景。
- 后台线程只能做 CPU import；cache 提交和 GPU 资源创建必须有明确线程归属。
- 场景序列化只保存可重建数据，不保存运行时 backend 状态。
- 新增组件必须同步考虑序列化、编辑器显示、测试和默认值。

## 7. Editor 原则

- Editor 是 Runtime 的消费者。
- EditorCore 可以依赖 Runtime，但 Runtime 不知道 Editor 存在。
- 所有可撤销的编辑行为必须走 command/history。
- Inspector、Hierarchy、Viewport、Asset Browser 等 panel 不直接拥有核心业务状态，状态应进入 Editor context/view-model 或 runtime service。

## 8. 测试原则

必须补测试的情况：

- 新增 public API。
- 修改 ECS、序列化、资源缓存、系统调度、编辑器 command。
- 修复 bug。
- 调整 renderer backend 选择、RHI resource 或生命周期。

最低验收：

- 相关 target 能构建。
- 相关 CTest 通过。
- 文档 change 的 `acceptance.md` 每一条能被检查或明确说明需要手动验收。

## 9. AI 编码规则

AI 在实现非平凡功能前必须：

1. 阅读本文档。
2. 阅读相关 `docs/specs/*.md`。
3. 若没有现成 change，先创建 `docs/changes/<change-id>/`。
4. 在 `design.md` 写清模块依赖。
5. 在 `tasks.md` 拆成可验证小步。
6. 实现时不得越过 Runtime/Editor、Renderer/RHI、ECS/System 边界。
7. 完成后更新相关 spec、roadmap 或 build 文档。

## 10. 修改宪法

修改本文档必须同时说明：

- 为什么旧规则不够。
- 哪些模块受影响。
- 是否需要迁移旧代码。
- 新增或调整的测试/验收方式。
