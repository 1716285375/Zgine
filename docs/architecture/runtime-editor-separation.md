# Runtime / Editor Separation

版本日期：2026-05-25

## 原则

Runtime 是游戏运行所需的引擎核心。Editor 是构建在 Runtime 之上的工具层。

```text
ZgineEditor -> ZgineEditorCore -> ZgineRuntime
ZgineSandbox -> ZgineRuntime
```

禁止反向依赖：

```text
ZgineRuntime -> ZgineEditorCore
ZgineRuntime -> ZgineEditor
```

## Runtime 可以包含

- Core、Platform、Resources、World、Renderer、Physics、Audio、Scripting、Gui。
- 可由游戏直接使用的 public API。
- 不依赖编辑器 UI 的调试能力。

## EditorCore 可以包含

- Editor panels。
- Command history。
- Selection context。
- Scene view model。
- Inspector workflow。
- Gizmo 和 viewport tool。

## 典型错误

- 为了让 Inspector 显示组件，把组件格式化逻辑写进 Runtime 并 include ImGui。
- 为了加载场景，让 Runtime 调用 Editor file dialog。
- 为了给 Hierarchy 拖拽排序，在 World component 中保存 Editor-only 状态。

## 推荐写法

- Runtime 暴露纯数据和服务 API。
- EditorCore 读取 Runtime 状态并转成 UI。
- 需要 undo/redo 的编辑通过 Editor command 修改 Runtime。
- Runtime 需要调试事件时定义 runtime event，Editor 可以订阅，但 Runtime 不知道 Editor。
