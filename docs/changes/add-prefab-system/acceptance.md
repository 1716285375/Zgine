# Acceptance Criteria

1. A prefab can be created from an entity hierarchy.
2. A prefab can be saved and loaded.
3. Instantiating a prefab creates a new hierarchy in World.
4. Runtime handles are fresh after instantiation.
5. Serialized prefab does not contain backend runtime objects.
6. Editor actions are undoable.
7. Tests cover hierarchy and UUID behavior.

## Current Status

As of 2026-05-29:

- Criteria 1-5 and 7 are covered by `PrefabTest.*`.
- Criterion 6 is covered by `EntityCommandsTest.*Prefab*`.
- Content Browser has basic actions for creating a prefab from the primary entity selection and instantiating the selected prefab asset.
