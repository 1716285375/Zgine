# Spec: Scene

版本日期：2026-05-29

## 职责

Scene 是 World 中一组实体、组件、层级和可序列化状态的集合。当前实现以 `World` 和 serialization 层为主，后续可以引入显式 Scene facade。

## 不负责

- 不保存 runtime backend 对象。
- 不保存临时 editor selection。
- 不保存未定义生命周期的裸指针。

## 规则

- 场景文件保存可重建数据。
- Entity 长期身份使用 UUID。
- Runtime handle 只用于当前 World 生命周期。
- Play Mode 必须通过 runtime clone/snapshot 运行；runtime World 可以保留 UUID 等可重建身份，但必须拥有独立 entity handle 和组件实例。
- `World::CloneForRuntime()` 必须重建 hierarchy，并清理 physics/audio/script/render resource 等 runtime-only 字段。
- `SceneRuntime` 只负责一次 runtime World 的场景生命周期：克隆、`StartScene`、`UpdateAll`、`FixedUpdateAll`、`StopScene`；系统资源初始化由宿主程序或系统自身的幂等初始化处理。
- 新增组件时同步考虑默认值、序列化、Editor inspector 和测试。
- Prefab 从一个 entity hierarchy 生成模板数据；实例化到 World 时必须创建新的 runtime entity handle 和新的 UUID。
- Prefab serialization 可以复用 World component serializers，但不能保存 backend runtime 对象。

## 测试要求

- 保存后加载的核心组件等价。
- 父子关系能恢复。
- 缺失字段有安全默认值。
- Prefab 实例化后层级恢复、UUID 更新、Transform 等核心组件保持等价。
- Runtime clone 修改不能污染 edit World。
