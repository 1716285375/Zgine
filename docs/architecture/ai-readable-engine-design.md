# AI-Readable Engine Design

版本日期：2026-05-25

本文档给 AI 编码助手提供最短路径的项目上下文。完整规则以 `docs/constitution.md`、`docs/project-standards.md` 和 `docs/specs/` 为准。

## 必读顺序

1. `docs/constitution.md`
2. `docs/project-standards.md`
3. 与任务相关的 `docs/specs/*.md`
4. 若任务是功能开发，读取或创建 `docs/changes/<change-id>/`

## 当前架构摘要

- `ZgineRuntime` 是运行时库。
- `ZgineEditorCore` 是编辑器工具层。
- `ZgineEditor` 和 `ZgineSandbox` 是应用入口。
- World/ECS 使用 EnTT，但 public API 应尽量通过 `World`、`Entity`、`EntityHandle`。
- OpenGL 是参考渲染后端。
- Vulkan 是当前跨平台图形 API 教学主线。

## 实现前检查

- 这个改动属于哪个模块。
- 是否有对应 spec。
- 是否需要 OpenSpec change。
- 是否会让 Runtime 依赖 Editor。
- 是否会让 public header 暴露第三方 backend。
- 是否需要测试。

## 交付前检查

- 相关代码构建通过。
- 相关 CTest 通过。
- `git diff --check` 无空白错误。
- 更新了相关 spec/change/roadmap。
- 没有回退用户已有改动。
