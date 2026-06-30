# Requirements

## Functional Requirements

1. `EditorContext` must expose a dedicated asset selection context.
2. Asset selection must store path, type, optional handle, and metadata state.
3. Selecting an asset in Asset Browser must update the shared asset selection context.
4. Selecting an asset must clear entity selection to avoid ambiguous Inspector focus.
5. Inspector must show asset details when no entity is selected and an asset is selected.
6. Asset selection must publish selected/cleared editor events.

## Non-Functional Requirements

1. Asset selection remains Editor-only.
2. Runtime resources must not depend on Editor selection state.
3. ECS entity selection and asset selection must not share the same data structure.
4. Tests must cover context state and events.
