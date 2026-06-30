# Acceptance Criteria

1. `ZgineRuntime` builds without compiling `src/Editor/**`.
2. `ZgineEditor` builds through `ZgineEditorCore`.
3. `ZgineSandbox` builds without Editor dependency.
4. `ZgineEditorTests` are discovered by CTest when editor build is enabled.
5. Documentation states Runtime/Editor dependency direction.
6. Future Runtime source files do not include `Zgine/Editor/**`.
