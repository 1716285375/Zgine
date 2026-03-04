#include <Zgine/Editor/UI/Helpers/PanelHelpers.h>
#include <imgui_internal.h>
#include <algorithm>

namespace Zgine {
namespace UI {
namespace Utils {

bool DrawSearchBar(std::string& query, float width, const char* hint) {
    bool changed = false;

    if (width > 0) {
        ImGui::SetNextItemWidth(width);
    }

    // Resize buffer if needed
    char buffer[256];
    std::strncpy(buffer, query.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputTextWithHint("##SearchBar", hint, buffer, sizeof(buffer))) {
        query = buffer;
        changed = true;
    }

    // Clear button
    if (!query.empty()) {
        ImGui::SameLine();
        if (ImGui::SmallButton("X")) {
            query.clear();
            changed = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Clear search");
        }
    }

    return changed;
}

void DrawEmptyState(const char* message, const char* icon) {
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize(message);

    // Center vertically and horizontally
    ImGui::SetCursorPosY((windowSize.y - textSize.y) * 0.5f);

    float textWidth = textSize.x;
    if (icon) {
        ImVec2 iconSize = ImGui::CalcTextSize(icon);
        textWidth += iconSize.x + ImGui::GetStyle().ItemSpacing.x;
    }

    ImGui::SetCursorPosX((windowSize.x - textWidth) * 0.5f);

    if (icon) {
        ImGui::TextDisabled("%s", icon);
        ImGui::SameLine();
    }

    ImGui::TextDisabled("%s", message);
}

void DrawPanelToolbar(std::span<const ToolbarButton> buttons) {
    for (size_t i = 0; i < buttons.size(); ++i) {
        const auto& btn = buttons[i];

        if (i > 0) {
            ImGui::SameLine();
        }

        ImGui::BeginDisabled(!btn.enabled);

        if (ImGui::Button(btn.label)) {
            if (btn.onClick) {
                btn.onClick();
            }
        }

        ImGui::EndDisabled();

        if (btn.tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("%s", btn.tooltip);
        }
    }
}

void DrawSeparatorText(const char* label) {
    // Draw line
    ImGui::Separator();

    // Draw text on top of line
    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    cursorPos.y -= textSize.y * 0.5f + ImGui::GetStyle().ItemSpacing.y;
    cursorPos.x += ImGui::GetStyle().ItemSpacing.x;

    // Draw background
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 labelMin(cursorPos.x - 4, cursorPos.y);
    ImVec2 labelMax(cursorPos.x + textSize.x + 4, cursorPos.y + textSize.y);
    drawList->AddRectFilled(labelMin, labelMax, ImGui::GetColorU32(ImGuiCol_WindowBg));

    // Draw text
    drawList->AddText(cursorPos, ImGui::GetColorU32(ImGuiCol_Text), label);
}

void CenterNextItem(float itemWidth) {
    float windowWidth = ImGui::GetWindowSize().x;
    ImGui::SetCursorPosX((windowWidth - itemWidth) * 0.5f);
}

bool IsItemDragging(const char* payloadType) {
    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        return false;
    }

    const ImGuiPayload* payload = ImGui::GetDragDropPayload();
    if (!payload || !payload->IsDataType(payloadType)) {
        return false;
    }

    return true;
}

} // namespace Utils
} // namespace UI
} // namespace Zgine
