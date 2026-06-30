#pragma once

#include <Zgine/Resources/Core/AssetDatabase.h>
#include <Zgine/Resources/Core/AssetHandle.h>
#include <Zgine/Resources/Core/AssetType.h>

#include <filesystem>

namespace Zgine {

class EditorEventBus;

struct AssetSelection {
    AssetHandle Handle;
    AssetType Type = AssetType::Unknown;
    std::filesystem::path SourcePath;
    std::filesystem::path RelativePath;
    bool IsDirectory = false;
    bool HasMetadata = false;

    [[nodiscard]] bool IsValid() const { return !SourcePath.empty(); }
};

class AssetSelectionContext {
public:
    explicit AssetSelectionContext(EditorEventBus* eventBus = nullptr);
    ~AssetSelectionContext() = default;

    void Select(const AssetRecord& record);
    void Select(AssetSelection selection);
    void Clear();

    [[nodiscard]] bool HasSelection() const { return m_SelectedAsset.IsValid(); }
    [[nodiscard]] const AssetSelection& GetSelection() const { return m_SelectedAsset; }

    void SetEventBus(EditorEventBus* eventBus) { m_EventBus = eventBus; }
    void SetEventPublishingEnabled(bool enabled) { m_PublishEvents = enabled; }

private:
    void PublishSelectionChanged();
    void PublishSelectionCleared();

    AssetSelection m_SelectedAsset;
    EditorEventBus* m_EventBus = nullptr;
    bool m_PublishEvents = true;
};

} // namespace Zgine
