#include <Zgine/Editor/UI/Widgets/EntityTreeWidget.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/UI/Helpers/SelectionHelpers.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Components/Components.h>
#include <imgui.h>

namespace Zgine::UI::Widgets {

void EntityTree::Render() {
    if (!m_World) {
        ImGui::TextDisabled("No World loaded");
        return;
    }

    for (Entity e : m_World->GetRootEntities()) {
        if (!PassFilter(e)) continue;

        DrawEntityNode(e);
    }
}

void EntityTree::RenderNode(Entity entity) {
    DrawEntityNode(entity);
}

bool EntityTree::PassFilter(Entity entity) const {
    if (!entity || !m_World) return false;
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
            Entity childEntity(child, m_World);
            if (PassFilter(childEntity)) {
                return true;
            }
        }
    }

    return false;
}

void EntityTree::DrawEntityNode(Entity entity) {
    if (!entity || !m_World) return;

    const RelationshipComponent* rel = entity.HasComponent<RelationshipComponent>()
        ? &entity.GetComponent<RelationshipComponent>()
        : nullptr;

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
        reinterpret_cast<void*>(static_cast<uintptr_t>(entity.GetHandle().GetValue())),
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
        uint32_t payload = entity.GetHandle().GetValue();
        ImGui::SetDragDropPayload("ZGINE_ENTITY", &payload, sizeof(uint32_t));
        ImGui::TextUnformatted(label.c_str());
        ImGui::EndDragDropSource();
    }

    // Drag & drop target
    if (m_DragDropEnabled && ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGINE_ENTITY")) {
            uint32_t dropped = *static_cast<const uint32_t*>(payload->Data);
            Entity droppedEntity(EntityHandle::FromValue(dropped), m_World);
            if (droppedEntity != entity) {
                if (OnEntityDragDrop) {
                    OnEntityDragDrop(droppedEntity, entity);
                } else {
                    // Default behavior: set parent
                    m_World->SetParent(droppedEntity, entity);
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
            Entity childEntity(child, m_World);
            if (!m_World->IsEntityValid(childEntity)) continue;
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
