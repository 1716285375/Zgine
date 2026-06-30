# Proposal: Add Editor Inspector Workflow

## 背景

Inspector 是编辑器可用性的核心。当前编辑器已有若干 panel 和事件/选择基础，但需要把 selection、view model、component inspector、undo/redo 和未来 asset inspector 统一成稳定工作流。

## 目标

- 明确 Inspector 的数据流。
- Entity selection 显示核心组件。
- Asset selection 显示 AssetMetadata。
- 所有可撤销修改走 command。
- Inspector 不直接依赖 renderer backend。

## 非目标

- 本次不做完整 UI 皮肤系统。
- 本次不做复杂自定义属性 drawer 反射系统。
- 本次不做 prefab override UI。

## 风险

- Panel 直接修改 World 会绕过 undo/redo。
- Inspector 同时处理 entity 和 asset 时 selection 状态可能混乱。
