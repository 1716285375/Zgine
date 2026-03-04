#include <Zgine/Editor/UI/Widgets/EntityTreeWidget.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/UI/Helpers/SelectionHelpers.h>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Components/Components.h>
#include <imgui.h>

namespace Zgine::UI::Widgets {

void EntityTree::Render() {
    if (!m_Scene) {
        ImGui::TextDisabled("No scene loaded");
        return;
    }

    // Render root entities
    auto& registry = m_Scene->GetRegistry();
    auto view = registry.view<TagComponent, RelationshipComponent>();

    for (auto entity : view) {
        auto& rel = registry.get<RelationshipComponent>(entity);

        // Only render root entities (no parent)
        if (rel.Parent != entt::null) continue;

        Entity e(entity, m_Scene);
        if (!PassFilter(e)) continue;

        DrawEntityNode(e);
    }
}

void EntityTree::RenderNode(Entity entity) {
    DrawEntityNode(entity);
}

bool EntityTree::PassFilter(Entity entity) const {
    if (!entity || !m_Scene) return false;
    if (m_Filter.empty()) return true;

    // Check entity name
    if (entity.HasComponent<TagComponent>()) {
        const auto& tag = entity.GetComponent<TagComponent>();
        if (tag.Tag.find(m_Filter) != std::string::npos) {
            return true;
        }
    }

    // Check children recursively
    if (entity.HasComponent<RelationshipComponent>()) {
        const auto& rel = entity.GetComponent<RelationshipComponent>();
        for (auto child : rel.Children) {
            Entity childEntity(child, m_Scene);
            if (PassFilter(childEntity)) {
                return true;
            }
        }
    }

    return false;
}

void EntityTree::DrawEntityNode(Entity entity) {
    if (!entity || !m_Scene) return;

    auto& registry = m_Scene->GetRegistry();
    auto* rel = registry.try_get<RelationshipComponent>(static_cast<entt::entity>(entity));

    const bool hasChildren = rel && !rel->Children.empty();

    // Setup tree node flags
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // Highlight if selected
    if (m_SelectionContext && m_SelectionContext->IsSelected(entity)) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Render tree node
    std::string label = GetEntityLabel(entity);
    bool opened = ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<entt::entity>(entity))),
        flags, "%s", label.c_str()
    );

    // Handle click
    if (ImGui::IsItemClicked()) {
        if (m_SelectionContext) {
            Utils::HandleEntityClick(*m_SelectionContext, entity);
        }
        if (OnEntityClicked) {
            OnEntityClicked(entity);
        }
    }

    // Handle double-click
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (OnEntityDoubleClicked) {
            OnEntityDoubleClicked(entity);
        }
    }

    // Drag & drop source
    if (m_DragDropEnabled && ImGui::BeginDragDropSource()) {
        entt::entity payload = static_cast<entt::entity>(entity);
        ImGui::SetDragDropPayload("ZGINE_ENTITY", &payload, sizeof(entt::entity));
        ImGui::TextUnformatted(label.c_str());
        ImGui::EndDragDropSource();
    }

    // Drag & drop target
    if (m_DragDropEnabled && ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGINE_ENTITY")) {
            entt::entity dropped = *static_cast<const entt::entity*>(payload->Data);
            if (dropped != static_cast<entt::entity>(entity)) {
                if (OnEntityDragDrop) {
                    OnEntityDragDrop(Entity(dropped, m_Scene), entity);
                } else {
                    // Default behavior: set parent
                    m_Scene->SetParent(Entity(dropped, m_Scene), entity);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Context menu
    if (ImGui::BeginPopupContextItem()) {
        bool handled = false;
        if (OnEntityContextMenu) {
            handled = OnEntityContextMenu(entity);
        }
        if (!handled) {
            ImGui::TextDisabled("No context menu");
        }
        ImGui::EndPopup();
    }

    // Render children
    if (opened && hasChildren) {
        for (auto child : rel->Children) {
            if (!registry.valid(child)) continue;
            Entity childEntity(child, m_Scene);
            if (!PassFilter(childEntity)) continue;
            DrawEntityNode(childEntity);
        }
        ImGui::TreePop();
    }
}

std::string EntityTree::GetEntityLabel(Entity entity) const {
    if (CustomLabelRenderer) {
        std::string custom = CustomLabelRenderer(entity);
        if (!custom.empty()) {
            return custom;
        }
    }

    // Default: use TagComponent
    if (entity.HasComponent<TagComponent>()) {
        return entity.GetComponent<TagComponent>().Tag;
    }

    return "Entity";
}

} // namespace Zgine::UI::Widgets
