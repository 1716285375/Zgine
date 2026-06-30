# Spec: Scripting

版本日期：2026-06-30

## 职责

Scripting 使用 Lua/Sol2 暴露安全的 gameplay API，驱动脚本生命周期，并和 World/Physics/Audio 交互。

## 不负责

- 不暴露 Editor-only API。
- 不提供静默 no-op 的 gameplay binding。
- 不让不同实体脚本共享同一组全局 callback。

## 规则

- 每个脚本实例必须使用独立 Lua environment；engine binding 可以通过全局 fallback 暴露，但脚本定义的变量和 lifecycle callback 不能写入共享 global table。
- 绑定 API 必须有行为或明确不可用。
- 脚本错误要附带 entity/script path 上下文。
- 脚本不能持久保存 runtime-only handle。
- `OnSceneStart/OnSceneStop` 负责脚本实例加载和卸载；`Update` 负责调用脚本帧逻辑。
- Play Mode 中 ScriptSystem 由 runtime World 的 `SystemManager::UpdateAll` 调度，Editor 不直接逐帧调用。
- Runtime clone 不得继承脚本初始化状态；进入 Play 后由 ScriptSystem 为 runtime World 初始化脚本实例。

## 测试要求

- 两个实体运行不同脚本不会互相覆盖 callback。
- 每个公开 binding 至少有 smoke test 或明确待办。
- 错误脚本不会崩溃整个运行时。
