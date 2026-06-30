# Zgine Architecture Review and Roadmap

Date: 2026-05-24

## Scope

This document records the current state of the Zgine codebase, the concrete risks found during static review, and a staged roadmap for getting the engine back to a reliable development baseline.

The target architecture is now defined in [engine-architecture.md](engine-architecture.md), and project-wide coding/testing rules are defined in [project-standards.md](project-standards.md). This document is the gap analysis and implementation roadmap.

Local toolchain status after cleanup:

- Visual Studio root: `C:\Program Files\Microsoft Visual Studio\2022\Community`
- MSVC compiler: `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe`
- MSVC linker: `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\link.exe`
- Windows SDK tools: `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64`
- Resource compiler: `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\rc.exe`
- Manifest tool: `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\mt.exe`
- Environment bootstrap: `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`

The earlier failure was not caused by a missing Visual Studio installation. CMake found
`cl.exe`, but the shell did not have the Visual Studio and Windows SDK environment
needed for `rc.exe`, `mt.exe`, headers, and libraries such as `kernel32.lib`.

Verified command path:

1. Configure through `VsDevCmd.bat -arch=x64 -host_arch=x64`.
2. Pass absolute CMake tool paths for `cl.exe`, `link.exe`, `rc.exe`, and `mt.exe`.
3. Build `ZgineTests` and `ZgineEditorTests`.
4. Run CTest.

Verified result on 2026-05-29:

- CMake configure succeeded in `build/vulkan-dev-check2` with `ZGINE_RENDERER_BACKEND=Vulkan` and `ZGINE_BUILD_EDITOR=ON`.
- `ZgineEditor`, `ZgineSandbox`, `ZgineTests`, and `ZgineEditorTests` built successfully.
- CTest passed: `79/79` tests, `0` failures.
- Vulkan now has clear-frame rendering, swapchain resize recreation, backend-private device context access, vertex-array metadata, and device-local vertex/index buffer creation through staging uploads. Shader/pipeline/descriptor/draw-command work remains pending.
- Runtime Prefab support now covers JSON prefab assets, entity hierarchy capture, file save/load without VFS initialization, fresh UUID/runtime handle instantiation, and AssetDatabase classification for `.prefab` / `.zgprefab`.
- Editor Prefab support now has undoable create/instantiate commands and basic Content Browser entry points.
- Core logging now provides safe fallback loggers so low-level runtime tests can use logging macros before `Log::Init()`.

## Current Architecture

Zgine is organized as a CMake/C++ engine with these visible layers:

- `Core`: application loop, events, input, time, jobs, logging, math, memory, UUID, layer stack.
- `Platform`: window/input backend and file/VFS utilities.
- `World`: EnTT-backed ECS, entity handles, hierarchy, systems, camera, scene serialization.
- `Renderer`: OpenGL RHI wrappers, primitive mesh rendering, PBR/basic shaders, shadows, post-process pipeline.
- `Resources`: asset metadata, importers, cache, hot reload watcher, mesh/material helpers.
- `Physics`: Jolt-backed rigid body system.
- `Audio`: miniaudio-backed source/listener system.
- `Scripting`: Lua/Sol2 script runtime and bindings.
- `Editor`: editor context, event bus, command history, panels, gizmo, inspectors, scene view model.
- `Sandbox`: sample executable intended to exercise runtime rendering/physics/audio/script integration.
- `Tests`: GoogleTest-based smoke tests and gated editor tests.

The intended boundary is clear in the build comments: runtime should not depend on editor code. The current CMake implementation does not enforce that boundary.

## Execution Model

`Application` owns the window, VFS initialization, GUI layer, layer stack, timer manager, and frame loop. The runtime loop is:

1. Fixed update at 60 Hz for each layer.
2. Variable update for each layer.
3. Timer manager tick.
4. GUI begin/render/end.
5. Window update and event polling.

`World` owns the ECS registry and `SystemManager`. `PhysicsSystem`, `AudioSystem`, and `ScriptSystem` implement `ISystem`, but the sandbox still drives them manually through legacy `OnSceneStart`, `Step`, `SyncPhysicsToECS`, and `Update` calls instead of using `World::GetSystemManager()`.

`RenderSystem` is not an `ISystem`; it is manually owned by the sample/editor layer. It owns shader references, shadow framebuffer, post-process pipeline, frame stats, and uses a static `RendererAPI` instance.

## Concrete Findings

### 1. Runtime/editor boundary is currently broken

Status: fixed in the architecture cleanup by excluding `src/Editor/**` and `include/Zgine/Editor/**` from `ZgineRuntime`, adding `ZgineEditorCore`, and linking editor tests to `ZgineEditorCore`.

Evidence:

- `CMakeLists.txt:82` starts `file(GLOB_RECURSE ZGINE_SOURCES ...)`.
- `CMakeLists.txt:84` includes `"${CMAKE_SOURCE_DIR}/src/*.cpp"`.
- There is no filter excluding `src/Editor`.
- `CMakeLists.txt:221-222` only controls whether the editor executable is added.
- `tests/Editor/CMakeLists.txt:12-14` explicitly assumes `ZgineRuntime` contains editor code.

Impact:

- `ZgineRuntime` is not a clean runtime library.
- Runtime builds inherit editor dependencies and compile surface.
- Editor tests pass only if this architectural leak remains.
- The documented one-way dependency is not trustworthy.

Recommended fix:

- Exclude `src/Editor/**` from `ZgineRuntime`.
- Add a separate `ZgineEditorCore` static library for `src/Editor/**`.
- Link `ZgineEditor` and `ZgineEditorTests` against `ZgineEditorCore`.
- Keep public editor headers out of the runtime umbrella unless explicitly building editor tooling.

### 2. Sandbox is stale against current public API

Status: fixed in the architecture cleanup by migrating sandbox math usage to `Math::*` and replacing the old material manager calls with `PBRMaterialPresetRegistry`.

Evidence:

- `sandbox/main.cpp:16`, `18`, `19`, `61`, `63`, `69`, `70`, `79`, `84`, `93`, `113`, and `114` use `glm::*` values where current components/camera use `Zgine::Math::*`.
- `sandbox/main.cpp:27`, `71`, and `80` call `PBRMaterialPresetManager`, but the current type is `PBRMaterialPresetRegistry`.
- `PBRMaterialPresetRegistry` exposes `Initialize`, `GetPreset`, and `GetAllPresets`, but no `ApplyPreset` helper.

Impact:

- The default sample likely fails compile after toolchain configuration is fixed.
- The sample cannot currently serve as a reliable runtime integration baseline.

Recommended fix:

- Convert sandbox math usage to `Math::Vector3` and `Math::Matrix4::Perspective`.
- Replace old material manager calls with a small helper that copies a registry preset into `PBRMaterialComponent`, or add a runtime helper API intentionally.
- Make sandbox use `World::SystemManager` once system lifecycle is unified.

### 3. Tests have outdated include paths and incomplete registration

Status: partially fixed. Runtime and editor include drift was corrected, a `SystemManager` test was added, and CTest now discovers/runs runtime plus editor-core tests. `tests/test_scene_destroy.cpp` still needs to be converted or registered deliberately.

Evidence:

- `tests/EngineSmokeTests.cpp:2-4` include `<Zgine/World/Components.h>`, `<Zgine/World/Entity.h>`, and `<Zgine/World/World.h>`, but the current paths are under `World/Components/Components.h` and `World/Core/*`.
- `tests/Editor/EditorCommandTests.cpp:2` includes `<Zgine/Editor/Commands/EditorCommand.h>`, but the actual interface is `IEditorCommand.h`.
- `tests/CMakeLists.txt` only builds `EngineSmokeTests.cpp`.
- `tests/test_scene_destroy.cpp` is not registered as a CTest target.
- Editor tests are only added when `ZGINE_BUILD_EDITOR` is enabled.

Impact:

- CI cannot validate the real runtime/editor surface.
- Tests are drifting from the public include layout.
- The current smoke test suite is too narrow for engine integration changes.

Recommended fix:

- Update include paths.
- Register `test_scene_destroy.cpp` or convert it to GoogleTest.
- Decide whether editor tests should require `ZGINE_BUILD_EDITOR` or a new `ZGINE_BUILD_EDITOR_TESTS`.
- Add smoke tests for serialization, system ordering, and sandbox-level API compile coverage.

### 4. SystemManager does not globally sort owned and external systems

Status: fixed in the architecture cleanup. The final execution list is now globally sorted by priority, with registration order as the tie-breaker. Lifecycle symmetry is now tracked per system: only systems that actually receive `Initialize` are shut down, and only systems that actually receive `OnSceneStart` are stopped.

Evidence:

- `src/World/Systems/SystemManager.cpp:64-77` sorts owned systems and external systems independently, then sets `m_Sorted = true`.
- `src/World/Systems/SystemManager.cpp:80-99` concatenates owned systems first and external systems second.
- The combined vector is sorted only if `!m_Sorted`; after `SortSystemsByPriority`, that branch is skipped.

Impact:

- A lower-priority external system always runs after all owned systems.
- The documented priority contract is broken when mixing ownership modes.
- This becomes risky when physics/audio/script/rendering are split between manager-owned and application-owned systems.

Recommended fix:

- Store systems in one scheduling list with ownership metadata, or always sort the combined iteration vector.
- Add tests proving owned/external systems interleave by priority.

### 5. Resource manager async path is not thread-safe enough yet

Status: first-pass fixed. AssetManager now protects manager state with a mutex, async loads re-enter the synchronized `LoadAsset` path, metadata writes use direct filesystem I/O instead of requiring VFS initialization, and runtime tests cover concurrent async audio loads sharing the same cache entry.

Evidence:

- `src/Resources/Core/AssetManager.cpp:315-329` uses `std::async` for thread-safe asset types.
- The async lambda calls `LoadAsset(handle)`.
- `LoadAsset` and its internal path mutate shared maps and sets such as `m_Metadata`, `m_Cache`, and `m_DirtyAssets` without synchronization.

Impact:

- Concurrent loads can race cache and metadata structures.
- Hot reload and async load may conflict.

Remaining work:

- For heavier assets, split background import from main-thread cache/resource commit instead of relying only on a coarse manager lock.
- Keep GPU-backed assets on the main/render thread.

### 6. Script API advertises physics functions that are no-ops

Status: fixed for the current force/velocity surface. `PhysicsSystem` now owns `ApplyForce`, `SetLinearVelocity`, and `GetLinearVelocity`, and Lua `applyForce` / `setVelocity` delegate to those runtime body APIs.

Original evidence:

- `src/Scripting/ScriptSystem.cpp:205` binds `applyForce`.
- `src/Scripting/ScriptSystem.cpp:214` binds `setVelocity`.
- `src/Scripting/ScriptSystem.cpp:209` and `218` state the implementation is temporarily empty.

Resolved implementation:

- `include/Zgine/Physics/PhysicsSystem.h` exposes runtime force and linear velocity controls.
- `src/Scripting/ScriptSystem.cpp` delegates Lua `applyForce` / `setVelocity` to `PhysicsSystem`.

Resolved impact:

- Lua scripts no longer call silently empty physics bindings for force and velocity.
- `ScriptSystemTest.PhysicsBindingsDriveRuntimeBodies` verifies the binding path against a real Jolt body.

Remaining work:

- Add a script binding test for every exported gameplay API.

### 7. Lua script instances share one global namespace

Status: fixed. `ScriptSystem` now loads each entity script into its own `sol::environment` with the engine API exposed through the global fallback table, then caches lifecycle callbacks from that environment.

Original evidence:

- `src/Scripting/ScriptSystem.cpp:344` executes each script in `m_LuaState`.
- `src/Scripting/ScriptSystem.cpp:356-358` reads global `OnStart`, `OnUpdate`, and `OnDestroy` after execution.

Resolved impact:

- Script-local variables and lifecycle callbacks no longer collide across entities.
- `ScriptSystemTest.ScriptInstancesUseIsolatedGlobals` covers two entities using conflicting script globals.

Remaining work:

- Define a clearer script module convention before exposing a larger gameplay API.
- Add binding-level smoke tests for each public Lua helper.

### 8. RenderSystem lifecycle is incomplete

Status: partially fixed. `RenderSystem::Initialize`/`Shutdown` are now guarded, release the owned RHI resources more explicitly, refuse unsupported renderer backends, and can initialize the Vulkan device/swapchain/resource skeleton without entering OpenGL scene-pipeline setup. Full cleanup of every OpenGL-owned helper remains a later renderer task.

Evidence:

- `src/Renderer/Pipeline/RenderSystem.cpp:29` calls `Shutdown` from the destructor.
- `src/Renderer/Pipeline/RenderSystem.cpp:40`, `89`, `105`, and `106` initialize defaults, shadows, and post-process state.
- `src/Renderer/Pipeline/RenderSystem.cpp:114-116` only logs shutdown.
- `m_Initialized` exists in the header but is not used to guard initialize/shutdown.

Impact:

- Double initialization/shutdown behavior is undefined.
- GPU resource release relies on member destruction rather than explicit lifecycle.
- `TextureDefaults` and post-process ownership need clearer cleanup semantics.

Recommended fix:

- Make `Initialize` idempotent and set `m_Initialized`.
- Explicitly reset shaders, framebuffers, defaults, post-process resources, and renderer API in `Shutdown`.
- Consider making `RenderSystem` an `ISystem` or documenting why rendering stays manually owned.

## Build and Documentation Drift

The docs say C++17 in `BUILD.md`, but the root project sets C++20. Several targets still set C++17 locally. This needs a deliberate choice:

Status: fixed for touched targets and docs. Sandbox, runtime tests, editor tests, and `BUILD.md` now use C++20 as the declared baseline.

- If C++20 is required, update `BUILD.md`, sandbox/tests target properties, and contributor docs.
- If C++17 compatibility is still desired for examples/tests, the runtime public headers need to avoid C++20-only contracts.

The default root options also differ between files:

- `CMakeLists.txt` defaults `ZGINE_BUILD_TESTS` to `ON`.
- `BUILD.md` lists `ZGINE_BUILD_TESTS` default as `OFF`.

## Roadmap

### P0: Recover a reliable build and test baseline

Goal: a clean configure/build/test loop that validates the current engine surface.

Tasks:

- Fix local Windows SDK tooling so CMake can pass compiler detection (`mt.exe` and `rc.exe` must be visible from the configured environment). Done for the verified local command path.
- Split editor code out of `ZgineRuntime` and introduce `ZgineEditorCore`. Done.
- Update sandbox to current `Math::*` and material preset APIs. Done.
- Update stale test include paths. Done.
- Register intended GoogleTest targets in CTest. Done for runtime and editor-core GoogleTests.
- Decide and document whether editor tests run by default. Done: editor tests are registered when `ZGINE_BUILD_TESTS` and `ZGINE_BUILD_EDITOR` are both enabled.
- Update `BUILD.md` to match CMake defaults, C++ standard, and local MSVC/SDK paths. Done.

Exit criteria:

- CMake configure succeeds through the documented MSVC developer environment command. Verified.
- `ZgineTests` and `ZgineEditorTests` build. Verified.
- CTest runs runtime and editor-core tests. Verified, `79/79`.
- Sandbox target compiles. Verified.
- Editor target compiles. Verified.

### P1: Stabilize runtime contracts

Goal: make core engine boundaries predictable before adding features.

Tasks:

- Extend `SystemManager` tests for enable/disable behavior and shutdown reverse order. Done.
- Move physics/audio/script lifecycle from sample code into `World` or a scene runtime coordinator.
- Define component add/remove hooks for runtime resources, especially physics bodies and audio sources.
- Evolve `AssetManager` async loading toward background import plus main-thread cache/resource commit for large assets.
- Continue hardening `RenderSystem` lifecycle and backend ownership.
- Keep OpenGL as the renderer reference backend while Vulkan advances from early buffer resources to shader modules, pipeline state, descriptors, and draw recording. DirectX 12 remains an explicit unsupported stub.

Exit criteria:

- A world can register physics/audio/script systems and update them through one path.
- System ordering is tested.
- Hot reload and async load have a documented threading model.

### P2: Make the sample and editor use real engine services

Goal: remove duplicate orchestration from sandbox/editor and turn them into consumers of stable engine APIs.

Tasks:

- Use `AssetManager` for mesh/material assignment instead of direct TODO placeholders.
- Add scene serialization tests covering core, rendering, physics, audio, and script components.
- Connect editor save/open flow to `WorldSerializer`.
- Add per-script Lua environments and remove global callback collisions.
- Implement or hide Lua physics APIs.
- Replace remaining app-level `glm` usage with engine math types except inside backend implementations.
- Continue Vulkan through the staged backend plan in `docs/rendering-backends.md`: shader modules, pipeline state, descriptors, command-buffer draw recording, depth resources, then editor ImGui integration.

Exit criteria:

- Sandbox demonstrates rendering, physics, audio, and scripting without using legacy lifecycle calls.
- Editor can create, save, load, and inspect a scene using runtime serialization.
- Two entities can run different scripts without callback collision.

### P3: Productize the development workflow

Goal: make the engine easy to validate, package, and extend.

Tasks:

- Add CI presets/jobs for runtime, editor core, tests, and install/export checks.
- Add a small public API compile test that includes `Zgine/Zgine.h`.
- Add sample scenes and documented asset import workflows.
- Add renderer smoke tests where possible, plus manual GPU validation steps where automated tests are impractical.
- Clean install/export rules after editor/runtime split.
- Reduce public dependency leakage where possible.

Exit criteria:

- A new contributor can configure, build, test, run sandbox, and run editor from the documented commands.
- Install/export validation catches public dependency regressions.

## Suggested Immediate Next Work

Start with P0 in this order:

1. Fix local MSVC/Windows SDK environment so CMake can configure.
2. Fix compile-surface drift in sandbox and tests.
3. Split editor implementation from runtime.
4. Add system-order tests before changing scheduler behavior.
5. Update build docs after the commands are proven locally.

This order keeps the first milestone focused on trust: once configure/build/test works, the later architecture changes can be verified instead of inferred.
