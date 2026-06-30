# Proposal: Add Editor Asset Selection Inspector

## Background

The Asset Browser now uses Runtime `AssetDatabase`, but selected asset state was still local to the panel. That blocks Inspector integration and makes future drag-drop or asset-specific editing workflows harder to share.

## Goals

- Add an Editor-owned asset selection context.
- Keep asset selection separate from ECS entity selection.
- Let Asset Browser publish selected asset data into `EditorContext`.
- Let Inspector show selected asset metadata when no entity is selected.

## Non-Goals

- No asset editing commands yet.
- No thumbnail generation pipeline.
- No prefab/material-specific Inspector editors yet.
- No persistent editor layout or selection restore.
