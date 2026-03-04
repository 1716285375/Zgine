#include <Zgine/Editor/UI/Helpers/SelectionHelpers.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Core/EditorTypes.h>
#include <imgui.h>

namespace Zgine::UI::Utils {

void HandleEntityClick(SelectionContext& ctx, Entity entity, bool forceReplace) {
    if (!entity) return;

    bool isSelected = ctx.IsSelected(entity);
    bool additive = !forceReplace && IsAdditiveSelection();

    if (!additive) {
        // Replace selection
        ctx.Select(entity, SelectionMode::Replace);
    } else {
        // Additive mode - toggle selection
        if (isSelected) {
            ctx.Select(entity, SelectionMode::Subtract);
        } else {
            ctx.Select(entity, SelectionMode::Add);
        }
    }
}

void SelectMultiple(SelectionContext& ctx, const std::vector<Entity>& entities, bool additive) {
    if (entities.empty()) return;

    SelectionMode mode = additive ? SelectionMode::Add : SelectionMode::Replace;
    ctx.Select(entities, mode);
}

void ToggleEntity(SelectionContext& ctx, Entity entity) {
    if (!entity) return;
    ctx.Select(entity, SelectionMode::Toggle);
}

bool IsAnyModifierPressed() {
    const ImGuiIO& io = ImGui::GetIO();
    return io.KeyCtrl || io.KeyShift;
}

} // namespace Zgine::UI::Utils
