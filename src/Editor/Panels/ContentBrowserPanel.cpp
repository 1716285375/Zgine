#include <Zgine/Editor/Panels/ContentBrowserPanel.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/PrefabCommands.h>
#include <Zgine/Editor/Core/AssetSelectionContext.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/World.h>

#include <imgui.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <system_error>

namespace Zgine
{

    namespace
    {
        std::string ToLower(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(),
                           [](unsigned char ch)
                           { return static_cast<char>(std::tolower(ch)); });
            return value;
        }
    }

    ContentBrowserPanel::ContentBrowserPanel(const std::string &name, EditorContext& context, const std::filesystem::path &assetsRoot)
        : EditorPanel(name, context), m_AssetsRoot(assetsRoot), m_AssetsCurrentDir(assetsRoot) {}

    void ContentBrowserPanel::SetAssetsRoot(const std::filesystem::path &root)
    {
        m_AssetsRoot = root;
		m_AssetsCurrentDir = root;
        GetContext().GetAssetSelectionContext().Clear();
        RefreshAssetDatabase();
	}

	void ContentBrowserPanel::OnAttach() {
        RefreshAssetDatabase();
	}

	void ContentBrowserPanel::OnDetach() {
		// Cleanup thumbnail cache
		m_ThumbnailCache.clear();
	}

	void ContentBrowserPanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
	}

    void ContentBrowserPanel::OnGuiRender()
    {
        BeginPanel();

        if (!m_AssetDatabase.IsScanned())
        {
            RefreshAssetDatabase();
        }

        if (!std::filesystem::exists(m_AssetsRoot))
        {
            ImGui::Text("Assets directory not found.");
            if (ImGui::Button("Refresh"))
            {
                RefreshAssetDatabase();
            }
            EndPanel();
            return;
        }

        if (m_AssetsCurrentDir.empty() || !std::filesystem::exists(m_AssetsCurrentDir))
        {
            m_AssetsCurrentDir = m_AssetsRoot;
        }

        ImGui::TextUnformatted("Content Browser");
        ImGui::SameLine();
        if (ImGui::Button("Refresh"))
        {
            RefreshAssetDatabase();
        }
        ImGui::SameLine();
        if (m_AssetsCurrentDir != m_AssetsRoot)
        {
            if (ImGui::Button("<-"))
            {
                m_AssetsCurrentDir = m_AssetsCurrentDir.parent_path();
            }
            ImGui::SameLine();
        }

        ImGui::TextUnformatted(m_AssetsCurrentDir.string().c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(220.0f);
        char searchBuffer[256];
        std::strncpy(searchBuffer, m_AssetSearch.c_str(), sizeof(searchBuffer) - 1);
        searchBuffer[sizeof(searchBuffer) - 1] = '\0';
        if (ImGui::InputTextWithHint("##AssetSearch", "Search...", searchBuffer, sizeof(searchBuffer)))
        {
            m_AssetSearch = std::string(searchBuffer);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(140.0f);
        ImGui::SliderFloat("##ThumbSize", &m_ContentThumbnailSize, 48.0f, 160.0f, "%.0f");

        ImGui::Separator();

        char importBuffer[512];
        std::strncpy(importBuffer, m_ImportPathInput.c_str(), sizeof(importBuffer) - 1);
        importBuffer[sizeof(importBuffer) - 1] = '\0';
        ImGui::SetNextItemWidth(360.0f);
        if (ImGui::InputTextWithHint("##AssetImport", "Import file path...", importBuffer, sizeof(importBuffer)))
        {
            m_ImportPathInput = std::string(importBuffer);
        }
        ImGui::SameLine();
        if (ImGui::Button("Import"))
        {
            if (!m_ImportPathInput.empty())
            {
                std::filesystem::path sourcePath = m_ImportPathInput;
                if (std::filesystem::exists(sourcePath))
                {
                    std::filesystem::path destination = m_AssetsCurrentDir / sourcePath.filename();
                    std::error_code copyEc;
                    std::filesystem::copy_file(sourcePath, destination, std::filesystem::copy_options::overwrite_existing, copyEc);
                    if (copyEc)
                    {
                        ZGINE_CORE_WARN("Failed to import asset: {}", copyEc.message());
                    }
                    else
                    {
                        ZGINE_CORE_INFO("Imported asset: {}", destination.string());
                        m_ImportPathInput.clear();
                        RefreshAssetDatabase();
                    }
                }
                else
                {
                    ZGINE_CORE_WARN("Import path not found: {}", m_ImportPathInput);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Create Prefab"))
        {
            CreatePrefabFromPrimarySelection();
        }

        ImGui::Separator();

        const float leftPaneWidth = 220.0f;
        ImGui::BeginChild("AssetTree", ImVec2(leftPaneWidth, 0.0f), true);
        RenderAssetTreeNode(m_AssetsRoot);
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("AssetContent", ImVec2(0.0f, 0.0f), true);

        const float cellSize = m_ContentThumbnailSize + m_ContentPadding;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1)
        {
            columnCount = 1;
        }
        ImGui::Columns(columnCount, nullptr, false);

        std::string searchLower = ToLower(m_AssetSearch);
        const auto records = GetCurrentDirectoryRecords();
        for (const AssetRecord* record : records)
        {
            const auto &path = record->SourcePath;
            std::string name = path.filename().string();
            if (!searchLower.empty())
            {
                std::string nameLower = ToLower(name);
                if (nameLower.find(searchLower) == std::string::npos)
                {
                    continue;
                }
            }

            ImGui::PushID(name.c_str());
            bool isDir = record->IsDirectory;
            std::shared_ptr<Texture> thumbnail;
            if (!isDir && record->Type == AssetType::Texture && IsImageFile(path))
            {
                const std::string key = path.string();
                auto it = m_ThumbnailCache.find(key);
                if (it != m_ThumbnailCache.end())
                {
                    thumbnail = it->second;
                }
                else
                {
                    thumbnail = Texture::Create(key);
                    m_ThumbnailCache.emplace(key, thumbnail);
                }
            }

            ImGui::BeginGroup();
            ImVec2 thumbSize = ImVec2(m_ContentThumbnailSize, m_ContentThumbnailSize);
            if (thumbnail)
            {
                ImGui::ImageButton("##Thumbnail",
                                   reinterpret_cast<void *>(static_cast<intptr_t>(thumbnail->GetID())),
                                   thumbSize, ImVec2(0, 1), ImVec2(1, 0));
            }
            else
            {
                ImGui::Button(GetAssetButtonLabel(record->Type, isDir), thumbSize);
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                GetContext().GetAssetSelectionContext().Select(*record);
                GetContext().GetSelectionContext().Clear();
            }

            if (ImGui::BeginDragDropSource())
            {
                std::string fullPath = path.string();
                ImGui::SetDragDropPayload("ZGINE_ASSET_PATH", fullPath.c_str(), fullPath.size() + 1);
                ImGui::TextUnformatted(name.c_str());
                ImGui::EndDragDropSource();
            }

            if (isDir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                m_AssetsCurrentDir = path;
            }

            ImGui::TextWrapped("%s", name.c_str());
            ImGui::EndGroup();

            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
        RenderSelectedAssetDetails();
        ImGui::EndChild();
        EndPanel();
    }

    void ContentBrowserPanel::RenderAssetTreeNode(const std::filesystem::path &path)
    {
        bool hasChildren = false;
        for (const AssetRecord& record : m_AssetDatabase.GetRecords())
        {
            if (record.IsDirectory && record.SourcePath.parent_path() == path)
            {
                hasChildren = true;
                break;
            }
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (path == m_AssetsCurrentDir)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (!hasChildren)
        {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        std::string label = path.filename().string();
        if (label.empty())
        {
            label = path.string();
        }

        bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
        if (ImGui::IsItemClicked())
        {
            m_AssetsCurrentDir = path;
        }

        if (opened && hasChildren)
        {
            for (const AssetRecord& record : m_AssetDatabase.GetRecords())
            {
                if (record.IsDirectory && record.SourcePath.parent_path() == path)
                {
                    RenderAssetTreeNode(record.SourcePath);
                }
            }
            ImGui::TreePop();
        }
    }

    void ContentBrowserPanel::RefreshAssetDatabase()
    {
        const std::filesystem::path previousRoot = m_AssetsRoot;
        const std::filesystem::path previousCurrentDir = m_AssetsCurrentDir;
        std::error_code relativeEc;
        const std::filesystem::path previousCurrentRelative =
            std::filesystem::relative(previousCurrentDir, previousRoot, relativeEc);

        AssetDatabaseConfig config;
        config.AssetsRoot = m_AssetsRoot;
        config.IncludeDirectories = true;
        config.IncludeUnknownFiles = true;
        m_AssetDatabase.Scan(config);

        if (m_AssetDatabase.IsScanned())
        {
            m_AssetsRoot = m_AssetDatabase.GetAssetsRoot();

            if (!relativeEc && previousCurrentRelative != ".")
            {
                m_AssetsCurrentDir = (m_AssetsRoot / previousCurrentRelative).lexically_normal();
            }
            else
            {
                m_AssetsCurrentDir = m_AssetsRoot;
            }

            std::error_code existsEc;
            if (!std::filesystem::exists(m_AssetsCurrentDir, existsEc) ||
                !std::filesystem::is_directory(m_AssetsCurrentDir, existsEc))
            {
                m_AssetsCurrentDir = m_AssetsRoot;
            }
        }

        m_ThumbnailCache.clear();
    }

    void ContentBrowserPanel::RenderSelectedAssetDetails()
    {
        const AssetSelectionContext& assetSelectionContext = GetContext().GetAssetSelectionContext();
        if (!assetSelectionContext.HasSelection())
        {
            return;
        }

        const AssetSelection& selected = assetSelectionContext.GetSelection();
        if (!selected.IsValid())
        {
            return;
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Selected Asset");
        ImGui::Text("Name: %s", selected.SourcePath.filename().string().c_str());
        ImGui::Text("Type: %s", AssetTypeToString(selected.Type));
        ImGui::Text("Relative: %s", selected.RelativePath.generic_string().c_str());
        ImGui::Text("Source: %s", selected.SourcePath.string().c_str());
        if (selected.Handle.IsValid())
        {
            ImGui::Text("Handle: %s", selected.Handle.ToString().c_str());
        }
        else
        {
            ImGui::TextUnformatted("Handle: <none>");
        }

        if (selected.Type == AssetType::Prefab && !selected.IsDirectory)
        {
            if (ImGui::Button("Instantiate Prefab"))
            {
                InstantiateSelectedPrefab();
            }
        }

        if (GetContext().GetSelectionContext().HasSelection())
        {
            if (ImGui::Button("Create Prefab From Entity"))
            {
                CreatePrefabFromPrimarySelection();
            }
        }
    }

    void ContentBrowserPanel::CreatePrefabFromPrimarySelection()
    {
        World* world = GetContext().GetSceneContext().GetActiveScene();
        Entity sourceRoot = GetContext().GetSelectionContext().GetPrimary();
        if (!world || !sourceRoot || !sourceRoot.IsValid()) {
            return;
        }

        std::string baseName = "Prefab";
        if (sourceRoot.HasComponent<TagComponent>()) {
            baseName = sourceRoot.GetComponent<TagComponent>().Tag;
        }

        const std::filesystem::path targetPath =
            m_AssetsCurrentDir / (SanitizeAssetFilename(baseName) + ".prefab");
        auto command = std::make_unique<CreatePrefabFromEntityCommand>(sourceRoot, targetPath);
        if (GetContext().GetCommandHistory().Execute(std::move(command))) {
            RefreshAssetDatabase();
        }
    }

    void ContentBrowserPanel::InstantiateSelectedPrefab()
    {
        World* world = GetContext().GetSceneContext().GetActiveScene();
        const AssetSelection& selected = GetContext().GetAssetSelectionContext().GetSelection();
        if (!world || selected.Type != AssetType::Prefab || selected.IsDirectory || selected.SourcePath.empty()) {
            return;
        }

        auto command = std::make_unique<InstantiatePrefabCommand>(world, selected.SourcePath);
        InstantiatePrefabCommand* commandPtr = command.get();
        if (GetContext().GetCommandHistory().Execute(std::move(command))) {
            Entity root = commandPtr->GetInstantiatedRoot();
            if (root) {
                GetContext().GetSelectionContext().Select(root);
                GetContext().GetAssetSelectionContext().Clear();
            }
        }
    }

    std::vector<const AssetRecord*> ContentBrowserPanel::GetCurrentDirectoryRecords() const
    {
        std::vector<const AssetRecord*> records;
        const std::filesystem::path currentRelativeDirectory = GetCurrentRelativeDirectory();

        for (const AssetRecord& record : m_AssetDatabase.GetRecords())
        {
            if (record.RelativePath.parent_path() == currentRelativeDirectory)
            {
                records.push_back(&record);
            }
        }

        return records;
    }

    std::filesystem::path ContentBrowserPanel::GetCurrentRelativeDirectory() const
    {
        std::error_code ec;
        std::filesystem::path relative = std::filesystem::relative(m_AssetsCurrentDir, m_AssetsRoot, ec);
        if (ec || relative == ".")
        {
            return {};
        }
        return relative.lexically_normal();
    }

    bool ContentBrowserPanel::IsImageFile(const std::filesystem::path &path)
    {
        std::string ext = ToLower(path.extension().string());
        return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".bmp";
    }

    const char* ContentBrowserPanel::GetAssetButtonLabel(AssetType type, bool isDirectory)
    {
        if (isDirectory)
        {
            return "[DIR]";
        }

        switch (type)
        {
            case AssetType::Texture: return "[TEX]";
            case AssetType::Mesh: return "[MESH]";
            case AssetType::Audio: return "[AUD]";
            case AssetType::Shader: return "[SHD]";
            case AssetType::World: return "[SCN]";
            case AssetType::Material: return "[MAT]";
            case AssetType::Prefab: return "[PFB]";
            case AssetType::Script: return "[LUA]";
            case AssetType::Folder: return "[DIR]";
            case AssetType::Unknown:
            default: return "[FILE]";
        }
    }

    std::string ContentBrowserPanel::SanitizeAssetFilename(std::string value)
    {
        if (value.empty()) {
            return "Prefab";
        }

        for (char& ch : value) {
            const unsigned char uch = static_cast<unsigned char>(ch);
            if (!std::isalnum(uch) && ch != '-' && ch != '_') {
                ch = '_';
            }
        }

        if (value == "." || value == "..") {
            return "Prefab";
        }
        return value;
    }

}
