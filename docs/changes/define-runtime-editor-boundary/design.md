# Design

## Modules

- `ZgineRuntime`
- `ZgineEditorCore`
- `ZgineEditor`
- `ZgineSandbox`
- `ZgineTests`
- `ZgineEditorTests`

## Dependency Rules

```text
ZgineEditor -> ZgineEditorCore -> ZgineRuntime
ZgineSandbox -> ZgineRuntime
ZgineTests -> ZgineRuntime
ZgineEditorTests -> ZgineEditorCore
```

Forbidden:

```text
ZgineRuntime -> ZgineEditorCore
ZgineRuntime -> ZgineEditor
```

## Implementation Notes

- Runtime source collection must filter out `src/Editor/**` and `include/Zgine/Editor/**`.
- Editor tests should not assume editor code is compiled into runtime.
- Editor-only context, selection and command logic stays in `ZgineEditorCore`.

## Related Docs

- `docs/constitution.md`
- `docs/architecture/runtime-editor-separation.md`
- `docs/architecture/module-boundaries.md`
- `docs/specs/Editor.md`
