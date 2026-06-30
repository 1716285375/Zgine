# Design

## Modules

- `Editor/Core/AssetSelectionContext`
- `Editor/Core/EditorContext`
- `Editor/Events/AssetEvents`
- `Editor/Panels/ContentBrowserPanel`
- `Editor/Panels/InspectorPanel`

## Dependency Direction

```text
EditorContext -> AssetSelectionContext
ContentBrowserPanel -> EditorContext -> AssetSelectionContext
InspectorPanel -> EditorContext -> AssetSelectionContext
AssetSelectionContext -> EditorEventBus
Runtime -/-> Editor
```

## Selection Rules

Entity selection and asset selection are separate services. Asset Browser clears entity selection after selecting an asset so the Inspector can switch focus to asset metadata. Entity selection still takes precedence when an entity is selected.
