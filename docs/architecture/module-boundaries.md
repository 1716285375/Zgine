# Module Boundaries

版本日期：2026-05-25

本文档记录 Zgine 模块依赖边界。项目宪法定义底线，本文件用于日常开发和代码评审。

## 目标边界

```text
Core
  基础设施：Application、Layer、Log、Assert、Time、Math、UUID、Job。

Platform
  Window、Input、File、VFS、FileWatcher。

Resources
  AssetHandle、AssetMetadata、AssetManager、Importer、Cache、HotReload。

World
  Entity、EntityHandle、World、Components、SystemManager、Serialization。

Renderer
  RHI、Backend、RenderSystem、Materials、Textures、Framebuffers、PostProcess。

Physics / Audio / Scripting
  对应第三方 backend 和 World component 的系统解释层。

Gui
  ImGui 或其他 GUI backend 适配。

EditorCore
  Panels、Commands、ViewModels、Editor Context、Editor EventBus。
```

## 允许依赖

```text
EditorCore -> Runtime
Sandbox -> Runtime

Renderer -> World / Resources / Platform / Core
World -> Resources / Core
Resources -> Platform / Core
Platform -> Core
Gui -> Platform / Core
Physics -> World / Core
Audio -> World / Core
Scripting -> World / Physics / Audio / Core
```

## 禁止依赖

- Runtime 模块 include `include/Zgine/Editor/**`。
- Runtime 编译 `src/Editor/**`。
- Core include 任意高层模块。
- Resources include World 场景状态。
- World component include renderer/physics/audio/script backend 私有头。
- Renderer backend include Editor panel 或 Editor context。

## Review 检查

新增文件时必须确认：

- 它属于哪个模块。
- 它是否被正确 target 编译。
- 它的 public header 是否泄漏第三方实现细节。
- 它是否让 Runtime 反向依赖 Editor。
