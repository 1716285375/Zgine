# Proposal: Add Editor Asset Browser

## Background

Zgine now has a Runtime `AssetDatabase` that can scan the assets tree and classify records. The editor already has a `ContentBrowserPanel`, but it previously walked the filesystem directly, which duplicated asset classification logic and made future Inspector/drag-drop workflows harder to stabilize.

## Goals

- Use Runtime `AssetDatabase` as the source of truth for the editor asset browser.
- Keep `ContentBrowserPanel` as an Editor-only panel.
- Preserve Runtime -> Editor zero dependency.
- Show basic asset details for the selected asset.

## Non-Goals

- No thumbnail cache persistence.
- No asset import pipeline.
- No asset-specific editing tools yet.
- No drag-drop instantiation into the World yet.
