# Zgine OpenSpec Changes

版本日期：2026-05-25

本目录记录 Zgine 的功能变更。每个非平凡功能、架构调整或大模块设计都应创建一个 change 目录。

## 目录格式

```text
docs/changes/<change-id>/
  proposal.md
  requirements.md
  design.md
  tasks.md
  acceptance.md
```

## 命名规则

- 使用动词开头：`add-`、`refactor-`、`define-`、`fix-`、`remove-`。
- 使用 kebab-case。
- 名称表达目标，不表达实现细节。

示例：

```text
define-runtime-editor-boundary
add-asset-database
add-prefab-system
add-editor-inspector-workflow
add-play-mode
```

## 使用流程

1. 创建 change。
2. 写 `proposal.md` 说明为什么做。
3. 写 `requirements.md` 明确必须实现什么。
4. 写 `design.md` 明确模块和依赖关系。
5. 写 `tasks.md` 拆小步。
6. 写 `acceptance.md` 定义验收标准。
7. 实现代码。
8. 更新相关 `docs/specs/*.md`。
9. 验收通过后归档或保留为历史记录。
