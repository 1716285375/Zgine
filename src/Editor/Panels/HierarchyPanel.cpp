#include <Zgine/Editor/Panels/HierarchyPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <imgui.h>
#include <algorithm>

namespace Zgine {

HierarchyPanel::HierarchyPanel(const std::string& name, EditorContext& context)
    : EditorPanel(name, context)
{
}

void HierarchyPanel::OnAttach() {
    // Subscribe to events if needed
}

void HierarchyPanel::OnDetach() {
    // Unsubscribe from events if needed
}

void HierarchyPanel::OnUpdate(float deltaTime) {
    ZGINE_UNUSED(deltaTime);
    // Hierarchy doesn't need per-frame updates
}

void HierarchyPanel::OnGuiRender() {
    if (!m_Scene) return;

    BeginPanel();

    auto& selectionContext = GetContext().GetSelectionContext();

    // Search bar and create button
    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputTextWithHint("##Search", "Search...", m_SearchQuery.data(), m_SearchQuery.capacity());

    ImGui::SameLine();
    if (ImGui::Button("+")) {
        ImGui::OpenPopup("HierarchyCreateMenu");
    }

    if (ImGui::BeginPopup("HierarchyCreateMenu")) {
        if (ImGui::MenuItem("Create Empty")) {
            Entity created = m_Scene->CreateEntity("Empty");
            selectionContext.SetPrimary(created);
        }
        if (ImGui::MenuItem("Create Cube")) {
            CreatePrimitive(m_Scene, PrimitiveType::Cube);
        }
        if (ImGui::MenuItem("Create Plane")) {
            CreatePrimitive(m_Scene, PrimitiveType::Plane);
        }
        if (ImGui::MenuItem("Create Sphere")) {
            CreatePrimitive(m_Scene, PrimitiveType::Sphere);
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

    ImGui::BeginChild("HierarchyList", ImVec2(0.0f, 0.0f), false);

    // Render root entities
    auto& registry = m_Scene->GetRegistry();
    auto view = registry.view<TagComponent, RelationshipComponent>();
    for (auto entity : view) {
        auto& rel = registry.get<RelationshipComponent>(entity);
        // Only render root entities
        if (rel.Parent != entt::null) continue;

        Entity e(entity, m_Scene);
        if (!PassHierarchyFilter(m_Scene, e)) continue;

        DrawHierarchyNode(m_Scene, e);
    }

    // Context menu for empty space
    if (ImGui::BeginPopupContextWindow("Hierarchy Context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Create Empty")) {
            Entity created = m_Scene->CreateEntity("Empty");
            selectionContext.SetPrimary(created);
        }
        if (ImGui::MenuItem("Create Cube")) {
            CreatePrimitive(m_Scene, PrimitiveType::Cube);
        }
        if (ImGui::MenuItem("Create Plane")) {
            CreatePrimitive(m_Scene, PrimitiveType::Plane);
        }
        ImGui::EndPopup();
    }

    ImGui::EndChild();
    EndPanel();
}

bool HierarchyPanel::PassHierarchyFilter(Scene* scene, Entity entity) {
    if (!scene || !entity) return false;
    if (m_SearchQuery.empty()) return true;

    if (entity.HasComponent<TagComponent>()) {
        const auto& tag = entity.GetComponent<TagComponent>();
        if (tag.Tag.find(m_SearchQuery) != std::string::npos) {
            return true;
        }
    }

    // Check children recursively
    if (entity.HasComponent<RelationshipComponent>()) {
        const auto& rel = entity.GetComponent<RelationshipComponent>();
        for (auto child : rel.Children) {
            Entity childEntity(child, scene);
            if (PassHierarchyFilter(scene, childEntity)) {
                return true;
            }
        }
    }

    return false;
}

void HierarchyPanel::DrawHierarchyNode(Scene* scene, Entity entity) {
    auto& selectionContext = GetContext().GetSelectionContext();
    auto& registry = scene->GetRegistry();
    auto& tag = registry.get<TagComponent>(static_cast<entt::entity>(entity));
    auto* rel = registry.try_get<RelationshipComponent>(static_cast<entt::entity>(entity));

    const bool hasChildren = rel && !rel->Children.empty();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool isSelected = selectionContext.IsSelected(entity);
    if (isSelected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool opened = ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<entt::entity>(entity))),
        flags, "%s", tag.Tag.c_str()
    );

    if (ImGui::IsItemClicked()) {
        bool additive = ImGui::GetIO().KeyCtrl || ImGui::GetIO().KeyShift;
        if (!additive) {
            selectionContext.Clear();
        }
        if (additive && isSelected) {
            selectionContext.Remove(entity);
        } else {
            selectionContext.Add(entity);
        }
    }

    // Drag & Drop
    if (ImGui::BeginDragDropSource()) {
        entt::entity payload = static_cast<entt::entity>(entity);
        ImGui::SetDragDropPayload("ZGINE_ENTITY", &payload, sizeof(entt::entity));
        ImGui::TextUnformatted(tag.Tag.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGINE_ENTITY")) {
            entt::entity dropped = *static_cast<const entt::entity*>(payload->Data);
            if (dropped != static_cast<entt::entity>(entity)) {
                scene->SetParent(Entity(dropped, scene), entity);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // Context menu
    bool deleteEntity = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Create Child")) {
            Entity child = scene->CreateEntity("Empty", entity);
            selectionContext.SetPrimary(child);
        }
        if (ImGui::MenuItem("Duplicate")) {
            Entity duplicate = scene->DuplicateEntity(entity);
            if (duplicate) {
                selectionContext.SetPrimary(duplicate);
            }
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete")) {
            deleteEntity = true;
        }
        ImGui::EndPopup();
    }

    // Render children
    if (opened && hasChildren) {
        for (auto child : rel->Children) {
            if (!registry.valid(child)) continue;
            Entity childEntity(child, scene);
            if (!PassHierarchyFilter(scene, childEntity)) continue;
            DrawHierarchyNode(scene, childEntity);
        }
        ImGui::TreePop();
    }

    if (deleteEntity) {
        DeleteEntity(scene, entity);
        selectionContext.Remove(entity);
    }
}

void HierarchyPanel::CreatePrimitive(Scene* scene, PrimitiveType type, Entity parent) {
    if (!scene) return;

    std::string name;
    switch (type) {
        case PrimitiveType::Cube: name = "Cube"; break;
        case PrimitiveType::Plane: name = "Plane"; break;
        case PrimitiveType::Sphere: name = "Sphere"; break;
        default: name = "Entity"; break;
    }

    // Create command for entity creation (supports undo/redo)
    auto command = std::make_unique<CreateEntityCommand>(scene, name);

    // Execute command through CommandHistory
    auto& commandHistory = GetContext().GetCommandHistory();
    if (commandHistory.Execute(std::move(command))) {
        // Get the created entity from selection context (command sets it)
        Entity created = GetContext().GetSelectionContext().GetPrimary();

        // Set parent if specified
        if (parent && created) {
            scene->SetParent(created, parent);
        }
    }
}

void HierarchyPanel::DeleteEntity(Scene* scene, Entity entity) {
    if (!scene || !entity) return;

    // Create command for entity deletion (supports undo/redo)
    auto command = std::make_unique<DeleteEntityCommand>(scene, entity);

    // Execute command through CommandHistory
    auto& commandHistory = GetContext().GetCommandHistory();
    commandHistory.Execute(std::move(command));
}

} // namespace Zgine
