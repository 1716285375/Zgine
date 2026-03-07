# Zgine

> 一个用于学习的现代 3D 游戏引擎，基于 C++20 构建。

## 技术栈

| 领域 | 库 |
|------|-----|
| ECS  | [EnTT](https://github.com/skypjack/entt) |
| 渲染 | OpenGL · GLFW · GLAD · GLM |
| 物理 | [Jolt Physics](https://github.com/jrouwe/JoltPhysics) |
| 音频 | [miniaudio](https://github.com/mackron/miniaudio) |
| 脚本 | Lua · [Sol2](https://github.com/ThePhD/sol2) |
| 模型 | [Assimp](https://github.com/assimp/assimp) |
| UI   | [ImGui](https://github.com/ocornut/imgui) (docking) · ImGuizmo |
| 序列化 | [nlohmann/json](https://github.com/nlohmann/json) |
| 虚拟文件系统 | [PhysicsFS](https://github.com/icculus/physfs) |

## 架构概览

```
Core/          基础设施 (Application · Events · Input · Jobs · Math · UUID)
Platform/      平台层  (Window · IO/VFS)
Renderer/      渲染层  (RHI 抽象 · Pipeline)
World/         ECS 世界 (Entity · Components · Systems · Serialization)
Resources/     资产管理 (AssetManager · Mesh · Material)
Scripting/     脚本系统 (IScript · ScriptEngine)
Physics/       物理系统
Audio/         音频系统
Editor/        编辑器   (Panels · Commands · MVVM · Gizmo)
Gui/           UI 后端  (ImGui · Nuklear · Fonts)
```

## 构建

**环境要求：** Visual Studio 2022 · CMake ≥ 3.20

```powershell
# 构建编辑器
.\build-editor.bat [debug|release]

# 仅构建沙盒
.\build-sandbox.bat [debug|release]

# 仅构建运行时库
.\build-runtime.bat [debug|release]
```

## License

MIT — see [LICENSE](LICENSE)
