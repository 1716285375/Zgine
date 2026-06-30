# Spec: Physics

版本日期：2026-06-30

## 职责

Physics 使用 Jolt 解释 physics components，创建/销毁物理体，执行 fixed-step simulation，并同步 Transform。

## 不负责

- 不直接处理 Editor UI。
- 不保存物理 body id 到场景文件。
- 不绕过 World API 修改实体生命周期。

## 规则

- Physics runtime state 由 PhysicsSystem 维护。
- Component 保存可重建配置，如 mass、shape、body type、collision 参数。
- `OnSceneStart/OnSceneStop` 负责 runtime body 创建和销毁；`FixedUpdate` 负责 step 与 ECS Transform 同步。
- Fixed update 应通过 runtime World 的 `SystemManager::FixedUpdateAll` 推进，Editor 不直接手动调用 `Step`。
- Runtime clone 不得继承 `RigidbodyComponent::RuntimeBody`；进入 Play 后由 PhysicsSystem 为 runtime World 重新创建 body。
- 面向 gameplay 和脚本的力/线速度控制必须通过 PhysicsSystem 的 runtime body API；Lua binding 不直接操作 Jolt，也不保留 body handle。
- `RigidbodyComponent` 的 mass、drag、gravity scale、fixed rotation 等配置必须在 body 创建时映射到 Jolt，而不是只保存在组件里。

## 测试要求

- 组件默认值。
- 系统初始化/关闭幂等。
- Transform 同步和 body 创建销毁需要 integration 测试或明确手动验收。
- 脚本可见的 physics helper 至少要有一个真实 body 的 smoke/integration test。
