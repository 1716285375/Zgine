# Design

## Modules

- `Resources/Core/AssetDatabase`
- `Resources/Core/AssetMetadata`
- `Platform/IO/File`
- `Platform/IO/VFS`
- Future: `Editor/Panels/AssetBrowserPanel`

## Dependency Rules

```text
Editor AssetBrowserPanel -> AssetDatabase
AssetDatabase -> Platform File/VFS
AssetDatabase -> AssetMetadata
Runtime !-> Editor
```

## Data Flow

```text
assets directory
  -> File scan
  -> classify extension
  -> build asset records
  -> query by path/type/handle
  -> editor panel or runtime service consumes records
```

## Notes

- AssetDatabase should initially be synchronous and deterministic.
- Async scanning can be added after cache commit rules are clear.
- Paths stored in metadata should use a normalized project-relative form.
