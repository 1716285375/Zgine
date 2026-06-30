# Design

## Modules

- `Editor/Core/EditorContext`
- `Editor/Selection/SelectionContext`
- `Editor/Panels/InspectorPanel`
- `Editor/UI/Inspectors/*`
- `Editor/Commands/*`
- Future: `AssetDatabase`

## Dependency Rules

```text
InspectorPanel -> EditorContext
InspectorPanel -> Inspector drawers
Inspector drawers -> Editor commands
Editor commands -> Runtime World/Entity
Runtime !-> Editor
```

## Data Flow

```text
user selects entity or asset
  -> SelectionContext updates
  -> InspectorPanel reads selection
  -> drawer renders fields
  -> edited value creates command
  -> command mutates Runtime state
```

## Notes

- Start with explicit drawer classes.
- Reflection/property metadata can be introduced later after component surface stabilizes.
