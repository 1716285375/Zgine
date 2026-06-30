# Spec: Renderer

版本日期：2026-05-25

## 职责

Renderer 提供 backend 选择、RHI 资源、渲染系统、材质、纹理、framebuffer、post-process 和后端实现。

## 当前状态

- OpenGL：reference backend，支持现有场景渲染路径。
- Vulkan：已支持 instance/device/surface/swapchain、clear-frame、resize recreation、初步 vertex-array metadata、device-local vertex/index buffer。
- DirectX12：selectable explicit stub。
- None：headless/testing。

## 不负责

- Renderer 不拥有 World。
- Renderer 不依赖 Editor。
- Renderer backend 不负责资源数据库扫描。

## 规则

- Public RHI 不暴露 backend native type。
- 通用资源层不直接调用 OpenGL/Vulkan/DirectX API。
- 未完成 backend path 必须显式失败。
- Vulkan 优先按 `docs/architecture/renderer-rules.md` 的顺序推进。

## 测试要求

- Backend name parsing 和 availability 必须测试。
- RHI layout、factory 行为必须测试。
- Vulkan GPU 行为可以先以构建和手动验收为主，但 CPU 可验证部分必须自动测试。
