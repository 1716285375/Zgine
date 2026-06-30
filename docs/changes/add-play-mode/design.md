# Design

## BMAD Viewpoints

- Engine Architect：Play Mode 需要 World snapshot/copy 和系统生命周期统一。
- Runtime Developer：需要 scene runtime coordinator，避免 sandbox/editor 手写多套更新逻辑。
- Editor Developer：需要 mode event、toolbar 状态和安全停止。
- QA Engineer：重点验证编辑 world 不被 play world 污染。
- Game Developer User：Play/Stop 必须直观可靠。

## Modules

- `World/Core/World`
- `World/Systems/SystemManager`
- `Runtime/SceneRuntime` 或等价 coordinator
- `Editor/Core/EditorContext`
- `Editor/Events/PlayModeChangedEvent`

## Dependency Rules

```text
Editor play controller -> Runtime scene runtime
Runtime scene runtime -> World/SystemManager
Runtime !-> Editor
```

## Data Flow

```text
edit world
  -> enter play
  -> clone/load runtime world
  -> initialize runtime systems
  -> update/fixed update
  -> exit play
  -> shutdown systems
  -> discard runtime world
  -> return to edit world
```
