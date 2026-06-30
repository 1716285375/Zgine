# ECS Rules

版本日期：2026-05-25

## 核心原则

- `World` 拥有 ECS registry。
- `Entity` 是对 `World + EntityHandle` 的轻量访问对象。
- `EntityHandle` 是运行时 handle，不是长期持久化 ID。
- Component 存数据，System 处理行为。

## Component 规则

推荐：

```cpp
struct AudioListenerComponent {
    bool Enabled = true;
};
```

禁止：

```cpp
struct MeshComponent {
    VkBuffer Buffer;
    JPH::BodyID Body;
};
```

backend runtime state 应由系统或资源对象维护，并能从组件数据重建。

## 层级关系

实体父子关系只能通过 `World` API 修改：

- `SetParent`
- `ClearParent`
- `GetChildren`
- `GetRootEntities`

不要直接改 `RelationshipComponent.Children`，否则容易破坏父子双向关系。

## 系统调度

- `SystemManager` 是系统调度入口。
- owned 和 external system 必须进入同一执行列表排序。
- priority 是主排序，注册顺序是同 priority 的 tie-breaker。

## 序列化

可保存：

- UUID。
- Tag。
- Transform。
- 可重建配置数据。

不可保存：

- runtime handle。
- backend native object。
- 跨帧裸指针。
- Lua 全局函数引用。
