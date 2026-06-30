#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Resources/Core/AssetDatabase.h>

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Zgine {

class Texture;

/**
 * @brief Content browser panel for asset management
 *
 * Provides a file browser interface for navigating and managing project assets.
 * Displays folders and files with thumbnails, supports search, and handles
 * asset importing and organization.
 *
 * **Features**:
 * - Hierarchical folder tree view
 * - Asset thumbnails with adjustable size
 * - Search/filter functionality
 * - Asset import dialog
 * - Drag-and-drop support (future)
 * - Thumbnail caching for performance
 *
 * **Supported Assets**:
 * - Images (PNG, JPG, etc.)
 * - 3D Models
 * - Shaders
 * - Scenes
 */
class ContentBrowserPanel : public EditorPanel {
public:
    ContentBrowserPanel(const std::string& name, EditorContext& context,
                        const std::filesystem::path& assetsRoot);
    ~ContentBrowserPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the root assets directory */
    void SetAssetsRoot(const std::filesystem::path& root);

private:
    void RenderAssetTreeNode(const std::filesystem::path& path);
    void RefreshAssetDatabase();
    void RenderSelectedAssetDetails();
    void CreatePrefabFromPrimarySelection();
    void InstantiateSelectedPrefab();
    [[nodiscard]] std::vector<const AssetRecord*> GetCurrentDirectoryRecords() const;
    [[nodiscard]] std::filesystem::path GetCurrentRelativeDirectory() const;
    static bool IsImageFile(const std::filesystem::path& path);
    static const char* GetAssetButtonLabel(AssetType type, bool isDirectory);
    static std::string SanitizeAssetFilename(std::string value);

private:
    std::filesystem::path m_AssetsRoot;
    std::filesystem::path m_AssetsCurrentDir;
    AssetDatabase m_AssetDatabase;
    std::string m_AssetSearch;
    std::string m_ImportPathInput;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_ThumbnailCache;
    float m_ContentThumbnailSize = 96.0f;
    float m_ContentPadding = 14.0f;
};

} // namespace Zgine
