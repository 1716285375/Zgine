#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/AssetSelectionContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Events/AssetEvents.h>
#include <Zgine/Editor/Events/EditorEvents.h>
#include <Zgine/Editor/Events/SelectionEvents.h>
#include <Zgine/Editor/ViewModels/SceneViewModel.h>
#include <Zgine/Runtime/SceneRuntime.h>
#include <Zgine/Core/Log/Log.h>

#include <utility>

namespace Zgine {

// ============================================================================
// ViewportContext
// ============================================================================

ViewportContext::ViewportContext()
    : m_SceneTextureId(0)
    , m_ViewportSize(0.0f, 0.0f)
    , m_Focused(false)
    , m_Hovered(false)
    , m_View(1.0f)
    , m_Projection(1.0f)
    , m_ViewProjectionValid(false)
    , m_ShowGrid(true)
    , m_ShowAxes(true)
{
    m_ViewportBounds[0] = Math::Vector2(0.0f, 0.0f);
    m_ViewportBounds[1] = Math::Vector2(0.0f, 0.0f);
}

// ============================================================================
// EditorContext
// ============================================================================

EditorContext::EditorContext()
    : m_SelectionContext(nullptr)
    , m_AssetSelectionContext(nullptr)
    , m_EventBus(nullptr)
    , m_CommandHistory(nullptr)
    , m_SceneViewModel(nullptr)
    , m_Initialized(false)
{
}

EditorContext::~EditorContext() {
    Shutdown();
}

void EditorContext::Initialize() {
    if (m_Initialized) {
        return;
    }

    // Create core services
    m_EventBus = std::make_unique<EditorEventBus>();
    m_SelectionContext = std::make_unique<SelectionContext>(m_EventBus.get());
    m_AssetSelectionContext = std::make_unique<AssetSelectionContext>(m_EventBus.get());
    m_CommandHistory = std::make_unique<EditorCommandHistory>();

    m_EventBus->Subscribe<EntitySelectedEvent>([this](EntitySelectedEvent&) {
        if (m_AssetSelectionContext && m_AssetSelectionContext->HasSelection()) {
            m_AssetSelectionContext->Clear();
        }
    });

    m_EventBus->Subscribe<AssetSelectedEvent>([this](AssetSelectedEvent&) {
        if (m_SelectionContext && m_SelectionContext->HasSelection()) {
            m_SelectionContext->Clear();
        }
    });

    // Create SceneViewModel if an active World exists
    // Note: If no World is active yet, SceneViewModel will be created later
    // when SetActiveScene() is called
    if (m_SceneContext.GetActiveScene()) {
        m_SceneViewModel = std::make_unique<SceneViewModel>(
            m_SceneContext.GetActiveScene(),
            m_EventBus.get(),
            m_CommandHistory.get(),
            m_SelectionContext.get()
        );
    }

    m_Initialized = true;
}

void EditorContext::Shutdown() {
    if (!m_Initialized) {
        return;
    }

    // Clear command history
    if (m_CommandHistory) {
        m_CommandHistory->Clear();
    }

    ExitPlayMode();

    // Shutdown services in reverse order
    m_SceneViewModel.reset();
    m_PlayRuntime.reset();
    m_CommandHistory.reset();
    m_AssetSelectionContext.reset();
    m_SelectionContext.reset();
    m_EventBus.reset();

    m_Initialized = false;
}

void EditorContext::SetActiveScene(World* World) {
    // Update World context
    m_SceneContext.SetActiveScene(World);

    // Recreate SceneViewModel if editor is initialized and World is valid
    if (m_Initialized && World) {
        m_SceneViewModel = std::make_unique<SceneViewModel>(
            World,
            m_EventBus.get(),
            m_CommandHistory.get(),
            m_SelectionContext.get()
        );
    } else {
        // Clear ViewModel if no World
        m_SceneViewModel.reset();
    }
}

void EditorContext::SetPlayRuntimeConfigurator(PlayRuntimeConfigurator configurator) {
    m_PlayRuntimeConfigurator = std::move(configurator);
}

bool EditorContext::EnterPlayMode() {
    if (m_Mode == EditorMode::Play) {
        return true;
    }

    if (m_Mode == EditorMode::Pause) {
        m_Mode = EditorMode::Play;
        PublishPlayModeChanged();
        return true;
    }

    World* editWorld = m_SceneContext.GetActiveScene();
    if (!editWorld) {
        ZGINE_CORE_WARN("EditorContext::EnterPlayMode failed: no active edit World.");
        return false;
    }

    auto runtimeWorld = editWorld->CloneForRuntime();
    if (!runtimeWorld) {
        ZGINE_CORE_WARN("EditorContext::EnterPlayMode failed: runtime World could not be cloned.");
        return false;
    }

    if (m_PlayRuntimeConfigurator) {
        m_PlayRuntimeConfigurator(*runtimeWorld);
    }

    auto runtime = std::make_unique<SceneRuntime>();
    if (!runtime->Start(std::move(runtimeWorld))) {
        ZGINE_CORE_WARN("EditorContext::EnterPlayMode failed: runtime World could not start.");
        return false;
    }

    m_EditSceneBeforePlay = editWorld;
    m_PlayRuntime = std::move(runtime);
    m_Mode = EditorMode::Play;
    SetActiveScene(m_PlayRuntime->GetWorld());

    if (m_SelectionContext) {
        m_SelectionContext->Clear();
    }
    if (m_AssetSelectionContext) {
        m_AssetSelectionContext->Clear();
    }

    PublishPlayModeChanged();
    return true;
}

void EditorContext::PausePlayMode() {
    if (m_Mode != EditorMode::Play) {
        return;
    }

    m_Mode = EditorMode::Pause;
    PublishPlayModeChanged();
}

void EditorContext::ExitPlayMode() {
    if (m_Mode == EditorMode::Edit && !m_PlayRuntime) {
        return;
    }

    m_Mode = EditorMode::Edit;
    PublishPlayModeChanged();

    // The view model stores raw World references. Restore editor-facing scene
    // state before destroying the runtime World.
    World* editWorld = m_EditSceneBeforePlay;
    m_EditSceneBeforePlay = nullptr;
    SetActiveScene(editWorld);

    if (m_PlayRuntime) {
        m_PlayRuntime->Stop();
        m_PlayRuntime.reset();
    }

    if (m_SelectionContext) {
        m_SelectionContext->Clear();
    }
    if (m_AssetSelectionContext) {
        m_AssetSelectionContext->Clear();
    }
}

void EditorContext::UpdatePlayRuntime(float deltaTime) {
    if (m_Mode == EditorMode::Play && m_PlayRuntime) {
        m_PlayRuntime->Update(deltaTime);
    }
}

void EditorContext::FixedUpdatePlayRuntime(float fixedDeltaTime) {
    if (m_Mode == EditorMode::Play && m_PlayRuntime) {
        m_PlayRuntime->FixedUpdate(fixedDeltaTime);
    }
}

void EditorContext::PublishPlayModeChanged() {
    if (!m_EventBus) {
        return;
    }

    PlayModeChangedEvent event(m_Mode);
    m_EventBus->PublishImmediate(event);
}

} // namespace Zgine
