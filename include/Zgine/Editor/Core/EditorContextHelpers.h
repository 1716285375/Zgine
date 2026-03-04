#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/ViewModels/SceneViewModel.h>
#include <Zgine/Scene/Core/Scene.h>

namespace Zgine {

/**
 * @brief Helper to create SceneViewModel when scene is set
 *
 * Call this after setting EditorContext::GetSceneContext().ActiveScene
 */
inline void InitializeSceneViewModel(EditorContext& context) {
    auto& sceneContext = context.GetSceneContext();

    if (sceneContext.ActiveScene && !context.GetSceneViewModel()) {
        // Create SceneViewModel with all required dependencies
        auto viewModel = std::make_unique<SceneViewModel>(
            sceneContext.ActiveScene,
            &context.GetEventBus(),
            &context.GetCommandHistory(),
            &context.GetSelectionContext()
        );

        // Note: This requires adding a SetSceneViewModel method to EditorContext
        // For now, this is a helper function showing the pattern
    }
}

} // namespace Zgine
