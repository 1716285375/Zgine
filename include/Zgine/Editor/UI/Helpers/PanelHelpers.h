#pragma once

#include <imgui.h>
#include <string>
#include <functional>
#include <span>

namespace Zgine::UI::Utils {

/**
 * @brief Draw a search bar with clear button
 */
bool DrawSearchBar(std::string& query, float width = -1.0f, const char* hint = "Search...");

/**
 * @brief Draw empty state message
 */
void DrawEmptyState(const char* message, const char* icon = nullptr);

/**
 * @brief Toolbar button definition
 */
struct ToolbarButton {
    const char* label;
    std::function<void()> onClick;
    bool enabled = true;
    const char* tooltip = nullptr;
};

/**
 * @brief Draw a panel toolbar with buttons
 */
void DrawPanelToolbar(std::span<const ToolbarButton> buttons);

/**
 * @brief Draw a separator with text
 */
void DrawSeparatorText(const char* label);

/**
 * @brief Center next item horizontally
 */
void CenterNextItem(float itemWidth);

/**
 * @brief Check if item is being dragged
 */
bool IsItemDragging(const char* payloadType);

} // namespace Zgine::UI::Utils
