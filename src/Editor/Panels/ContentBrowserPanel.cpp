#include <Zgine/Editor/Panels/ContentBrowserPanel.h>
#include <Zgine/Renderer/RHI/Texture.h>
#include <Zgine/Core/Log/Log.h>
#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <cstring>

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
	}

	void ContentBrowserPanel::OnAttach() {
		// Initialize file watchers if needed
	}

	void ContentBrowserPanel::OnDetach() {
		// Cleanup thumbnail cache
		m_ThumbnailCache.clear();
	}

	void ContentBrowserPanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
		// Content browser doesn't need per-frame updates
	}

    void ContentBrowserPanel::OnGuiRender()
    {
        BeginPanel();

        if (!std::filesystem::exists(m_AssetsRoot))
        {
            ImGui::Text("Assets directory not found.");
            EndPanel();
            return;
        }

        if (m_AssetsCurrentDir.empty() || !std::filesystem::exists(m_AssetsCurrentDir))
        {
            m_AssetsCurrentDir = m_AssetsRoot;
        }

        ImGui::TextUnformatted("Content Browser");
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
                    }
                }
                else
                {
                    ZGINE_CORE_WARN("Import path not found: {}", m_ImportPathInput);
                }
            }
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

        std::error_code ec;
        std::string searchLower = ToLower(m_AssetSearch);
        for (const auto &entry : std::filesystem::directory_iterator(m_AssetsCurrentDir, ec))
        {
            if (ec)
            {
                break;
            }
            const auto &path = entry.path();
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
            bool isDir = entry.is_directory();
            std::shared_ptr<Texture> thumbnail;
            if (!isDir && IsImageFile(path))
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
                ImGui::Button(isDir ? "[DIR]" : "[FILE]", thumbSize);
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
        ImGui::EndChild();
        EndPanel();
    }

    void ContentBrowserPanel::RenderAssetTreeNode(const std::filesystem::path &path)
    {
        std::error_code ec;
        bool hasChildren = std::filesystem::is_directory(path, ec);
        if (ec)
        {
            return;
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
            if (hasChildren)
            {
                m_AssetsCurrentDir = path;
            }
        }

        if (opened && hasChildren)
        {
            for (const auto &entry : std::filesystem::directory_iterator(path, ec))
            {
                if (ec)
                {
                    break;
                }
                if (entry.is_directory())
                {
                    RenderAssetTreeNode(entry.path());
                }
            }
            ImGui::TreePop();
        }
    }

    bool ContentBrowserPanel::IsImageFile(const std::filesystem::path &path)
    {
        std::string ext = ToLower(path.extension().string());
        return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".bmp";
    }

}
