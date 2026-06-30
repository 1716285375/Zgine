# Requirements

## Functional Requirements

1. Prefab 可以保存一个 entity hierarchy。
2. Prefab 实例化时生成新的 runtime entity handle。
3. 实例化后实体拥有新的 UUID 或明确的 instance UUID 策略。
4. Prefab asset 能通过 Asset 系统引用。
5. Editor 创建/实例化 Prefab 的操作可 undo/redo。

## Non-Functional Requirements

1. Prefab serialization 不保存 runtime backend state。
2. Runtime 不依赖 Editor。
3. Prefab 数据结构必须可测试。
4. 后续可扩展 nested prefab 和 override UI。
