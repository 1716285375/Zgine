# Spec: Editor

版本日期：2026-05-29

## 职责

EditorCore 提供编辑器上下文、panel、command history、selection、scene view model、inspector、viewport 和工具工作流。

## 不负责

- Runtime 不依赖 Editor。
- Editor panel 不直接拥有 runtime 核心状态。
- Editor 不绕过 command history 修改需要撤销的场景状态。

## 规则

- 可撤销操作走 command。
- Selection 状态集中在 editor context。
- ECS 实体选择和资源选择是互斥焦点；选中一种焦点时，另一种焦点必须被清空。
- Panel 只负责展示和输入，业务逻辑放 view-model/service。
- Editor-only 数据不写入 runtime component，除非通过明确 editor metadata 方案。
- Asset Browser 使用 Runtime `AssetDatabase` 作为资源清单来源。
- 资源选择状态由 Editor-only `AssetSelectionContext` 管理，并和 ECS 实体 `SelectionContext` 分离。
- Inspector 可显示实体组件或资源元数据；实体选择和资源选择不能混用同一个数据结构。
- Inspector 修改 Transform 必须通过 `TransformEntityCommand` 提交到 `EditorCommandHistory`，不能直接写组件字段。
- 创建 Prefab 文件、实例化 Prefab 到 World 等会改变资源或场景的动作必须走 `EditorCommandHistory`。
- Content Browser 可以提供 Prefab 操作入口，但不能把 undo/redo 逻辑写在 panel 内。
- Play/Pause/Edit 状态由 `EditorContext` 管理；Toolbar 只能请求状态变化，不能直接启动或停止 runtime state。
- Enter Play 必须从 edit World 创建 runtime clone 并切换 active scene；Exit Play 必须丢弃 runtime World 并恢复 edit World。
- Editor 可以通过 `SetPlayRuntimeConfigurator` 在 runtime clone 启动前注册外部教学系统；Play 中的 physics/audio/script 更新必须通过 runtime World 的 `SystemManager` 统一调度。
- Play Mode 状态变化必须发布 `PlayModeChangedEvent`。

## 测试要求

- EventBus。
- SelectionContext。
- AssetSelectionContext。
- CommandHistory。
- Entity/Transform commands。
- Prefab create/instantiate commands。
- SceneViewModel。
- Play Mode 状态切换、事件发布和 edit/runtime World 隔离。
- Play runtime configurator 必须先于 runtime scene start 执行。
