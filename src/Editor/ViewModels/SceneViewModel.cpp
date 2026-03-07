#include <Zgine/Editor/ViewModels/SceneViewModel.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Events/EntityEvents.h>
#include <Zgine/Editor/Events/SceneEvents.h>

// Alias to avoid conflict with member variable CreateEntityCommand
namespace EntityCmd = Zgine;

namespace Zgine {

SceneViewModel::SceneViewModel(
    World* World,
    EditorEventBus* eventBus,
    EditorCommandHistory* commandHistory,
    SelectionContext* selectionContext)
    : m_World(World)
    , m_EventBus(eventBus)
    , m_CommandHistory(commandHistory)
    , m_SelectionContext(selectionContext)
{
    // Initialize observable properties
    EntityCount.Set(0);
    IsDirty.Set(false);
    SceneName.Set("Untitled World");

    // Bind commands
    CreateEntityCommand.SetExecute([this](const std::string& name) {
        ExecuteCreateEntity(name);
    });

    DeleteSelectedEntityCommand.SetExecute([this]() {
        ExecuteDeleteSelectedEntity();
    });
    DeleteSelectedEntityCommand.SetCanExecute([this]() {
        return SelectedEntity.Get().IsValid();
    });

    DuplicateSelectedEntityCommand.SetExecute([this]() {
        ExecuteDuplicateSelectedEntity();
    });
    DuplicateSelectedEntityCommand.SetCanExecute([this]() {
        return SelectedEntity.Get().IsValid();
    });

    SaveSceneCommand.SetExecute([this]() {
        ExecuteSaveScene();
    });

    ClearSelectionCommand.SetExecute([this]() {
        ExecuteClearSelection();
    });

    // Setup event subscriptions
    SetupEventSubscriptions();

    // Initial sync from model
    RefreshFromModel();

    m_Initialized = true;
}

SceneViewModel::~SceneViewModel() {
    // Event subscriptions will be cleaned up automatically
}

void SceneViewModel::Update() {
    // Update entity count if changed
    size_t currentCount = m_World->GetEntityCount();
    if (EntityCount.Get() != currentCount) {
        EntityCount.Set(currentCount);
    }
}

std::vector<Entity> SceneViewModel::GetAllEntities() const {
    std::vector<Entity> result;
    auto view = m_World->GetRegistry().view<entt::entity>();
    for (auto entity : view) {
        result.emplace_back(entity, m_World);
    }
    return result;
}

void SceneViewModel::RefreshFromModel() {
    EntityCount.Set(m_World->GetEntityCount());

    // Sync selection
    if (m_SelectionContext && !m_SelectionContext->GetSelection().empty()) {
        SelectedEntity.Set(*m_SelectionContext->GetSelection().begin());
    } else {
        SelectedEntity.Set(Entity());
    }
}

void SceneViewModel::SetupEventSubscriptions() {
    if (!m_EventBus) {
        return;
    }

    // Subscribe to entity lifecycle events
    m_EventBus->Subscribe<EntityCreatedEvent>([this](EntityCreatedEvent& e) {
        OnEntityCreated(e.GetEntity());
    });

    m_EventBus->Subscribe<EntityDestroyedEvent>([this](EntityDestroyedEvent& e) {
        OnEntityDestroyed(e.GetEntity());
    });

    m_EventBus->Subscribe<SceneModifiedEvent>([this](SceneModifiedEvent&) {
        OnSceneModified();
    });
}

void SceneViewModel::OnEntityCreated(Entity entity) {
    ZGINE_UNUSED(entity);  // Suppress unused parameter warning
    EntityCount.Set(m_World->GetEntityCount());
    IsDirty.Set(true);
}

void SceneViewModel::OnEntityDestroyed(Entity entity) {
    EntityCount.Set(m_World->GetEntityCount());
    IsDirty.Set(true);

    // Clear selection if destroyed entity was selected
    if (SelectedEntity.Get() == entity) {
        SelectedEntity.Set(Entity());
    }
}

void SceneViewModel::OnSelectionChanged() {
    RefreshFromModel();
}

void SceneViewModel::OnSceneModified() {
    IsDirty.Set(true);
}

// ============================================================================
// Command Implementations
// ============================================================================

void SceneViewModel::ExecuteCreateEntity(const std::string& name) {
    auto cmd = std::make_unique<EntityCmd::CreateEntityCommand>(
        m_World,
        name.empty() ? "Entity" : name,
        PrimitiveType::None);

    if (m_CommandHistory) {
        m_CommandHistory->Execute(std::move(cmd));
    }
}

void SceneViewModel::ExecuteDeleteSelectedEntity() {
    Entity selected = SelectedEntity.Get();
    if (!selected.IsValid()) {
        return;
    }

    auto cmd = std::make_unique<DeleteEntityCommand>(m_World, selected);

    if (m_CommandHistory) {
        m_CommandHistory->Execute(std::move(cmd));
    }
}

void SceneViewModel::ExecuteDuplicateSelectedEntity() {
    Entity selected = SelectedEntity.Get();
    if (!selected.IsValid()) {
        return;
    }

    Entity duplicated = m_World->DuplicateEntity(selected);

    if (m_SelectionContext && duplicated.IsValid()) {
        m_SelectionContext->Select(duplicated, SelectionMode::Replace);
    }
}

void SceneViewModel::ExecuteSaveScene() {
    // TODO: Implement World serialization
    IsDirty.Set(false);

    if (m_EventBus && m_World) {
        // Use World path from SceneContext or default
        std::filesystem::path scenePath(SceneName.Get());
        SceneSavedEvent event(m_World, scenePath);
        m_EventBus->PublishImmediate(event);
    }
}

void SceneViewModel::ExecuteClearSelection() {
    if (m_SelectionContext) {
        m_SelectionContext->Clear();
    }
    SelectedEntity.Set(Entity());
}

} // namespace Zgine
