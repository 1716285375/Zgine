# Proposal: Define Runtime Editor Boundary

## 背景

Zgine 已经有 Runtime、Editor、Sandbox 和 Tests，但历史上曾出现 Runtime 编译 Editor 实现、测试依赖错误 target、编辑器逻辑渗入运行时边界的问题。这个 change 用来把 Runtime/Editor 边界固化成长期规则。

## 目标

- `ZgineRuntime` 不编译 `src/Editor/**`。
- `ZgineEditorCore` 承载编辑器业务逻辑。
- `ZgineEditor` 只作为应用入口。
- Editor tests 链接 `ZgineEditorCore`。
- 文档和测试能持续防止边界回退。

## 非目标

- 不重写所有 Editor panel。
- 不一次性完成 Play Mode。
- 不调整 Runtime public API 的全部历史设计。

## 风险

- CMake GLOB 新增目录时可能再次把 Editor 编进 Runtime。
- Editor panel 可能为了快速实现绕过 command/view-model 边界。
