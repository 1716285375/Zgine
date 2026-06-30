# Requirements

## Functional Requirements

1. The editor asset browser must scan the configured assets root through `AssetDatabase`.
2. The folder tree must be generated from `AssetRecord` directory entries.
3. The content grid must display immediate children of the current directory.
4. The browser must preserve search and import behavior.
5. Selecting an asset must show basic metadata in the panel.
6. Refresh after import must rescan the database.

## Non-Functional Requirements

1. Runtime must not include or depend on Editor code.
2. Editor panel code may depend on Runtime `Resources`.
3. Asset classification must stay in Runtime `AssetType` / `AssetDatabase`.
4. The panel must compile under the Vulkan teaching backend even when texture thumbnails are unavailable.
