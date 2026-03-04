#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/ViewModels/SceneViewModel.h>
#include <Zgine/Core/Log/Log.h>

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
    m_SelectionContext = std::make_unique<SelectionContext>();
    m_EventBus = std::make_unique<EditorEventBus>();
    m_CommandHistory = std::make_unique<EditorCommandHistory>();

    // Create SceneViewModel if an active scene exists
    // Note: If no scene is active yet, SceneViewModel will be created later
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

    // Shutdown services in reverse order
    m_SceneViewModel.reset();
    m_CommandHistory.reset();
    m_EventBus.reset();
    m_SelectionContext.reset();

    m_Initialized = false;
}

void EditorContext::SetActiveScene(Scene* scene) {
    // Update scene context
    m_SceneContext.SetActiveScene(scene);

    // Recreate SceneViewModel if editor is initialized and scene is valid
    if (m_Initialized && scene) {
        m_SceneViewModel = std::make_unique<SceneViewModel>(
            scene,
            m_EventBus.get(),
            m_CommandHistory.get(),
            m_SelectionContext.get()
        );
    } else {
        // Clear ViewModel if no scene
        m_SceneViewModel.reset();
    }
}

} // namespace Zgine
