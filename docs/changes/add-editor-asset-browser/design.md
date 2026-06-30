# Design

## Modules

- `Resources/Core/AssetDatabase`
- `Editor/Panels/ContentBrowserPanel`

## Dependency Direction

```text
ContentBrowserPanel -> AssetDatabase -> AssetType / AssetMetadata
Runtime -/-> Editor
```

## Data Flow

```text
assets root
  -> AssetDatabase::Scan()
  -> sorted AssetRecord list
  -> ContentBrowserPanel tree/content grid
  -> local selected asset details
```

## Current Scope

The panel owns a local `AssetDatabase` instance and rescans on attach, refresh, and successful import. Asset selection is promoted into `EditorContext` through `AssetSelectionContext` by the follow-up `add-editor-asset-selection-inspector` change.
