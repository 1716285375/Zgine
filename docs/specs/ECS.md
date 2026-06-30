# Spec: ECS

版本日期：2026-05-25

## 职责

ECS 负责 Entity、EntityHandle、World、Component、Relationship、SystemManager 和基础场景对象组织。

## 不负责

- 不直接创建 GPU、物理、音频或脚本 backend 对象。
- 不包含 Editor 头。
- 不直接渲染或播放音频。

## 规则

- Component 存数据，System 处理行为。
- `World` 是 ECS registry 的所有者。
- 外部模块优先使用 `World` 和 `Entity` public API。
- 需要访问 EnTT registry 的 runtime 内部实现通过内部访问层，不把 EnTT registry 扩散到 Editor。
- 层级关系只能通过 `World` API 修改。
- `SystemManager::InitializeAll/ShutdownAll` 表示系统资源生命周期；`StartScene/StopScene` 表示一次 runtime scene 绑定生命周期，二者不能混用。
- `StartScene` 按系统 priority 正序调用，`StopScene` 按反向顺序调用，用于教学展示系统依赖关系和清理顺序。

## 测试要求

- Entity 创建默认组件。
- Component 添加/删除。
- Parent/child 关系。
- SystemManager priority、registration order、scene start/stop 顺序和 shutdown 顺序。
