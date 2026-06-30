#include <Zgine/Editor/Core/AssetSelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/AssetEvents.h>

#include <utility>

namespace Zgine {

AssetSelectionContext::AssetSelectionContext(EditorEventBus* eventBus)
    : m_EventBus(eventBus)
{
}

void AssetSelectionContext::Select(const AssetRecord& record) {
    AssetSelection selection;
    selection.Handle = record.Handle;
    selection.Type = record.Type;
    selection.SourcePath = record.SourcePath;
    selection.RelativePath = record.RelativePath;
    selection.IsDirectory = record.IsDirectory;
    selection.HasMetadata = record.HasMetadata;
    Select(std::move(selection));
}

void AssetSelectionContext::Select(AssetSelection selection) {
    if (!selection.IsValid()) {
        Clear();
        return;
    }

    m_SelectedAsset = std::move(selection);
    PublishSelectionChanged();
}

void AssetSelectionContext::Clear() {
    if (!m_SelectedAsset.IsValid()) {
        return;
    }

    m_SelectedAsset = {};
    PublishSelectionCleared();
}

void AssetSelectionContext::PublishSelectionChanged() {
    if (!m_PublishEvents || !m_EventBus || !m_SelectedAsset.IsValid()) {
        return;
    }

    AssetSelectedEvent event(m_SelectedAsset.RelativePath, m_SelectedAsset.Type);
    m_EventBus->PublishImmediate(event);
}

void AssetSelectionContext::PublishSelectionCleared() {
    if (!m_PublishEvents || !m_EventBus) {
        return;
    }

    AssetSelectionClearedEvent event;
    m_EventBus->PublishImmediate(event);
}

} // namespace Zgine
