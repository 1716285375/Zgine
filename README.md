<p align="center">
  <img src="assets/icons/app/zgine.png" alt="Zgine Logo" width="200" />
</p>

<p align="center">
  <a href="https://isocpp.org/"><img alt="C++" src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=c%2B%2B&logoColor=white" /></a>
  <a href="https://cmake.org/"><img alt="CMake" src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=flat-square&logo=cmake&logoColor=white" /></a>
  <a href="https://visualstudio.microsoft.com/"><img alt="Visual Studio" src="https://img.shields.io/badge/VS-2022-5C2D91?style=flat-square&logo=visual-studio&logoColor=white" /></a>
  <a href="LICENSE"><img alt="License" src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square" /></a>
  <a href="https://github.com/opengl/opengl"><img alt="OpenGL" src="https://img.shields.io/badge/OpenGL-4.6-5586A4?style=flat-square&logo=opengl&logoColor=white" /></a>
</p>

<h1 align="center">⚡ Zgine</h1>

<p align="center">
  <b>一个用于学习的现代 3D 游戏引擎，基于 C++20 构建</b>
</p>

<p align="center">
  <a href="#-tech-stack">Tech Stack</a> ·
  <a href="#-architecture">Architecture</a> ·
  <a href="#-getting-started">Getting Started</a> ·
  <a href="#-documentation">Docs</a> ·
  <a href="#-license">License</a>
</p>

<br/>

<details open>
  <summary><h2>🧩 Tech Stack</h2></summary>

<h3>🎮 Core</h3>

<p>
  <a href="https://github.com/skypjack/entt"><img alt="EnTT" src="https://img.shields.io/badge/EnTT-ECS-3A3A3A?style=flat-square&logo=c%2B%2B&logoColor=white" /></a>
  <a href="https://github.com/jrouwe/JoltPhysics"><img alt="Jolt Physics" src="https://img.shields.io/badge/Jolt-Physics-FF6B00?style=flat-square&logo=c%2B%2B&logoColor=white" /></a>
  <a href="https://github.com/mackron/miniaudio"><img alt="miniaudio" src="https://img.shields.io/badge/miniaudio-Audio-FF4088?style=flat-square&logo=audiomack&logoColor=white" /></a>
</p>

<h3>🖼️ Rendering</h3>

<p>
  <a href="https://www.opengl.org/"><img alt="OpenGL" src="https://img.shields.io/badge/OpenGL-RHI-5586A4?style=flat-square&logo=opengl&logoColor=white" /></a>
  <a href="https://www.glfw.org/"><img alt="GLFW" src="https://img.shields.io/badge/GLFW-Window-FF6600?style=flat-square&logo=glfw&logoColor=white" /></a>
  <a href="https://gen.glad.sh/"><img alt="GLAD" src="https://img.shields.io/badge/GLAD-Loader-4A90D9?style=flat-square&logo=opengl&logoColor=white" /></a>
  <a href="https://github.com/g-truc/glm"><img alt="GLM" src="https://img.shields.io/badge/GLM-Math-5A0FC8?style=flat-square&logo=opengl&logoColor=white" /></a>
</p>

<h3>🛠️ Tools</h3>

<p>
  <a href="https://github.com/ocornut/imgui"><img alt="ImGui" src="https://img.shields.io/badge/ImGui-Docking-1A6E6E?style=flat-square" /></a>
  <a href="https://github.com/CedricGuillemet/ImGuizmo"><img alt="ImGuizmo" src="https://img.shields.io/badge/ImGuizmo-Gizmo-1A6E6E?style=flat-square" /></a>
  <a href="https://github.com/assimp/assimp"><img alt="Assimp" src="https://img.shields.io/badge/Assimp-Model-00599C?style=flat-square" /></a>
  <a href="https://github.com/ThePhD/sol2"><img alt="Sol2" src="https://img.shields.io/badge/Sol2-Lua-00007C?style=flat-square&logo=lua&logoColor=white" /></a>
  <a href="https://github.com/nlohmann/json"><img alt="nlohmann/json" src="https://img.shields.io/badge/nlohmann-JSON-333333?style=flat-square&logo=json&logoColor=white" /></a>
  <a href="https://github.com/icculus/physfs"><img alt="PhysicsFS" src="https://img.shields.io/badge/PhysicsFS-VFS-235789?style=flat-square" /></a>
</p>

</details>

<br/>

<details open>
  <summary><h2>🧬 Architecture</h2></summary>

```
Core/          基础设施 (Application · Events · Input · Jobs · Math · UUID)
Platform/      平台层  (Window · IO/VFS)
Renderer/      渲染层  (RHI 抽象 · Pipeline)
Renderer/RHI   后端选择 (OpenGL reference · Vulkan device/swapchain · DirectX12 stub)
World/         ECS 世界 (Entity · Components · Systems · Serialization)
Resources/     资产管理 (AssetManager · Mesh · Material)
Scripting/     脚本系统 (IScript · ScriptEngine)
Physics/       物理系统
Audio/         音频系统
Editor/        编辑器   (Panels · Commands · MVVM · Gizmo)
Gui/           UI 后端  (ImGui · Nuklear · Fonts)
```

</details>

<br/>

<details open>
  <summary><h2>🚀 Getting Started</h2></summary>

**环境要求：** Visual Studio 2022 · CMake ≥ 3.20

```powershell
# 构建编辑器
.\build-editor.bat [debug|release]

# 仅构建沙盒
.\build-sandbox.bat [debug|release]

# 仅构建运行时库
.\build-runtime.bat [debug|release]
```

</details>

<br/>

<details>
  <summary><h2>📖 Documentation</h2></summary>

| 文档 | 说明 |
|------|------|
| [游戏引擎架构基线](docs/engine-architecture.md) | 引擎架构详细设计 |
| [项目规范](docs/project-standards.md) | 项目开发规范 |
| [架构评审与路线图](docs/architecture-roadmap.md) | 架构评审与开发计划 |
| [渲染后端路线](docs/rendering-backends.md) | OpenGL / DirectX 12 / Vulkan 后端教学路线 |
| [代码风格规范](CODE_STYLE.md) | C++ 代码风格指南 |

</details>

<br/>

<h2>📄 License</h2>

<p>MIT — see <a href="LICENSE">LICENSE</a></p>

<br/>

<p align="center">
  <sub>Built with ❤️ and C++20</sub>
</p>
