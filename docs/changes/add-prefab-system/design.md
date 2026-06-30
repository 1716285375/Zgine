# Design

## BMAD Viewpoints

- Engine Architect：Prefab 是 Asset + Serialization + World instantiation 的交汇点，必须先定义稳定数据模型。
- Runtime Developer：实例化应复用 World 创建实体和组件复制路径。
- Editor Developer：创建、应用、断开 Prefab 必须走 command。
- Tools Developer：Prefab 文件应是可读 JSON 或项目已有序列化格式。
- QA Engineer：重点测试 UUID、层级、组件 round-trip。
- Game Developer User：Prefab 应能快速复用常见实体组合。

## Modules

- `Resources/Prefab/PrefabAsset`
- `Resources/Prefab/PrefabSerializer`
- `World/Serialization`
- `World/Core/World`
- `Editor/Commands`
- `AssetDatabase`

## Dependency Rules

```text
Editor command -> Prefab service -> World/Serialization/Asset
Runtime Prefab service !-> Editor
```

## Runtime Format

Prefab files are JSON documents:

```text
Version
Type = Prefab
Handle
Name
TemplateRoot
Entities[]
```

Each entity entry stores the template UUID, tag, serialized components, and an optional parent UUID when the parent is inside the same prefab hierarchy. Runtime backend objects are excluded by component serializer policy.

## Instantiation Policy

- Instantiate into an existing `World`.
- Generate a fresh UUID for every entity entry.
- Rebuild parent links after all entities exist.
- Return the instantiated root entity.
- Use standard filesystem I/O for prefab files so the service can run without VFS initialization.

## Editor Command Policy

- `CreatePrefabFromEntityCommand` captures prefab JSON on first execute, writes the target file, and restores the previous file state on undo.
- `InstantiatePrefabCommand` loads or receives a prefab, instantiates it into the active World, and destroys the instantiated root hierarchy on undo.
- Content Browser is only a UI entry point; undoable behavior belongs to the commands.

## Data Flow

```text
selected entity hierarchy
  -> serialize prefab data
  -> save prefab asset
  -> instantiate prefab
  -> World creates new entities
  -> components copied with fresh runtime handles
```
