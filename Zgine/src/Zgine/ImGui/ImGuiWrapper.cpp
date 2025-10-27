#include "zgpch.h" // Precompiled header
#include "ImGuiWrapper.h"
#include "imgui.h"
#include <cstdarg>
#include <glm/glm.hpp>

namespace Zgine {
namespace ImGui {

// Window management
bool ImGuiWrapper::Begin(const char* name, bool* p_open, int flags) {
    return ::ImGui::Begin(name, p_open, flags);
}

void ImGuiWrapper::End() {
    ::ImGui::End();
}

// Text and labels
void ImGuiWrapper::Text(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextV(fmt, args);
    va_end(args);
}

void ImGuiWrapper::TextUnformatted(const char* text) {
    ::ImGui::TextUnformatted(text);
}

void ImGuiWrapper::TextUnformatted(const char* text, const char* text_end) {
    ::ImGui::TextUnformatted(text, text_end);
}

void ImGuiWrapper::TextColored(const glm::vec4& color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextColoredV(ImVec4(color.r, color.g, color.b, color.a), fmt, args);
    va_end(args);
}

void ImGuiWrapper::TextDisabled(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextDisabledV(fmt, args);
    va_end(args);
}

void ImGuiWrapper::TextWrapped(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::TextWrappedV(fmt, args);
    va_end(args);
}

void ImGuiWrapper::LabelText(const char* label, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::LabelTextV(label, fmt, args);
    va_end(args);
}

void ImGuiWrapper::BulletText(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::BulletTextV(fmt, args);
    va_end(args);
}

// Buttons and inputs
bool ImGuiWrapper::Button(const char* label, const glm::vec2& size) {
    return ::ImGui::Button(label, ImVec2(size.x, size.y));
}

bool ImGuiWrapper::SmallButton(const char* label) {
    return ::ImGui::SmallButton(label);
}

bool ImGuiWrapper::InvisibleButton(const char* str_id, const glm::vec2& size, int flags) {
    return ::ImGui::InvisibleButton(str_id, ImVec2(size.x, size.y), flags);
}

bool ImGuiWrapper::ArrowButton(const char* str_id, int dir) {
    return ::ImGui::ArrowButton(str_id, static_cast<ImGuiDir>(dir));
}

bool ImGuiWrapper::Checkbox(const char* label, bool* v) {
    return ::ImGui::Checkbox(label, v);
}

bool ImGuiWrapper::CheckboxFlags(const char* label, int* flags, int flags_value) {
    return ::ImGui::CheckboxFlags(label, flags, flags_value);
}

bool ImGuiWrapper::RadioButton(const char* label, bool active) {
    return ::ImGui::RadioButton(label, active);
}

bool ImGuiWrapper::RadioButton(const char* label, int* v, int v_button) {
    return ::ImGui::RadioButton(label, v, v_button);
}

// Sliders
bool ImGuiWrapper::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, int flags) {
    return ::ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, int flags) {
    return ::ImGui::SliderFloat2(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, int flags) {
    return ::ImGui::SliderFloat3(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, int flags) {
    return ::ImGui::SliderFloat4(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, int flags) {
    return ::ImGui::SliderInt(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, int flags) {
    return ::ImGui::SliderInt2(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, int flags) {
    return ::ImGui::SliderInt3(label, v, v_min, v_max, format, flags);
}

bool ImGuiWrapper::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, int flags) {
    return ::ImGui::SliderInt4(label, v, v_min, v_max, format, flags);
}

// Selection
bool ImGuiWrapper::Selectable(const char* label, bool selected, int flags) {
    return ::ImGui::Selectable(label, selected, flags);
}

bool ImGuiWrapper::Selectable(const char* label, bool* p_selected, int flags) {
    return ::ImGui::Selectable(label, p_selected, flags);
}

// Input fields
bool ImGuiWrapper::InputText(const char* label, char* buf, size_t buf_size, int flags) {
    return ::ImGui::InputText(label, buf, buf_size, flags);
}

// Layout
void ImGuiWrapper::SameLine(float offset_from_start_x, float spacing) {
    ::ImGui::SameLine(offset_from_start_x, spacing);
}

void ImGuiWrapper::NewLine() {
    ::ImGui::NewLine();
}

void ImGuiWrapper::Spacing() {
    ::ImGui::Spacing();
}

void ImGuiWrapper::Dummy(const glm::vec2& size) {
    ::ImGui::Dummy(ImVec2(size.x, size.y));
}

void ImGuiWrapper::Indent(float indent_w) {
    ::ImGui::Indent(indent_w);
}

void ImGuiWrapper::Unindent(float indent_w) {
    ::ImGui::Unindent(indent_w);
}

void ImGuiWrapper::BeginGroup() {
    ::ImGui::BeginGroup();
}

void ImGuiWrapper::EndGroup() {
    ::ImGui::EndGroup();
}

glm::vec2 ImGuiWrapper::GetCursorPos() {
    ImVec2 pos = ::ImGui::GetCursorPos();
    return glm::vec2(pos.x, pos.y);
}

void ImGuiWrapper::SetCursorPos(const glm::vec2& local_pos) {
    ::ImGui::SetCursorPos(ImVec2(local_pos.x, local_pos.y));
}

glm::vec2 ImGuiWrapper::GetCursorStartPos() {
    ImVec2 pos = ::ImGui::GetCursorStartPos();
    return glm::vec2(pos.x, pos.y);
}

glm::vec2 ImGuiWrapper::GetCursorScreenPos() {
    ImVec2 pos = ::ImGui::GetCursorScreenPos();
    return glm::vec2(pos.x, pos.y);
}

void ImGuiWrapper::SetCursorScreenPos(const glm::vec2& pos) {
    ::ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
}

void ImGuiWrapper::AlignTextToFramePadding() {
    ::ImGui::AlignTextToFramePadding();
}

float ImGuiWrapper::GetTextLineHeight() {
    return ::ImGui::GetTextLineHeight();
}

float ImGuiWrapper::GetTextLineHeightWithSpacing() {
    return ::ImGui::GetTextLineHeightWithSpacing();
}

float ImGuiWrapper::GetFrameHeight() {
    return ::ImGui::GetFrameHeight();
}

float ImGuiWrapper::GetFrameHeightWithSpacing() {
    return ::ImGui::GetFrameHeightWithSpacing();
}

// Separators
void ImGuiWrapper::Separator() {
    ::ImGui::Separator();
}

void ImGuiWrapper::SeparatorEx(int separator_type) {
    ::ImGui::Separator();
}

// Collapsing headers
bool ImGuiWrapper::CollapsingHeader(const char* label, int flags) {
    return ::ImGui::CollapsingHeader(label, flags);
}

bool ImGuiWrapper::CollapsingHeader(const char* label, bool* p_open, int flags) {
    return ::ImGui::CollapsingHeader(label, p_open, flags);
}

// Demo and debugging
void ImGuiWrapper::ShowDemoWindow(bool* p_open) {
    ::ImGui::ShowDemoWindow(p_open);
}

void ImGuiWrapper::ShowAboutWindow(bool* p_open) {
    ::ImGui::ShowAboutWindow(p_open);
}

void ImGuiWrapper::ShowMetricsWindow(bool* p_open) {
    ::ImGui::ShowMetricsWindow(p_open);
}

void ImGuiWrapper::ShowDebugLogWindow(bool* p_open) {
    ::ImGui::ShowDebugLogWindow(p_open);
}

void ImGuiWrapper::ShowStackToolWindow(bool* p_open) {
    ::ImGui::ShowStackToolWindow(p_open);
}

void ImGuiWrapper::ShowStyleEditor(void* ref) {
    ::ImGui::ShowStyleEditor(static_cast<ImGuiStyle*>(ref));
}

bool ImGuiWrapper::ShowStyleSelector(const char* label) {
    return ::ImGui::ShowStyleSelector(label);
}

void ImGuiWrapper::ShowFontSelector(const char* label) {
    ::ImGui::ShowFontSelector(label);
}

void ImGuiWrapper::ShowUserGuide() {
    ::ImGui::ShowUserGuide();
}

void ImGuiWrapper::GetVersion() {
    ::ImGui::GetVersion();
}

// Color pickers
bool ImGuiWrapper::ColorEdit3(const char* label, float col[3], int flags) {
    return ::ImGui::ColorEdit3(label, col, flags);
}

bool ImGuiWrapper::ColorEdit4(const char* label, float col[4], int flags) {
    return ::ImGui::ColorEdit4(label, col, flags);
}

bool ImGuiWrapper::ColorPicker3(const char* label, float col[3], int flags) {
    return ::ImGui::ColorPicker3(label, col, flags);
}

bool ImGuiWrapper::ColorPicker4(const char* label, float col[4], int flags) {
    return ::ImGui::ColorPicker4(label, col, flags);
}

// Trees
bool ImGuiWrapper::TreeNode(const char* label) {
    return ::ImGui::TreeNode(label);
}

bool ImGuiWrapper::TreeNode(const char* str_id, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool result = ::ImGui::TreeNodeV(str_id, fmt, args);
    va_end(args);
    return result;
}

bool ImGuiWrapper::TreeNodeEx(const char* label, int flags) {
    return ::ImGui::TreeNodeEx(label, flags);
}

bool ImGuiWrapper::TreeNodeEx(const char* str_id, int flags, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool result = ::ImGui::TreeNodeExV(str_id, flags, fmt, args);
    va_end(args);
    return result;
}

void ImGuiWrapper::TreePush(const char* str_id) {
    ::ImGui::TreePush(str_id);
}

void ImGuiWrapper::TreePop() {
    ::ImGui::TreePop();
}

float ImGuiWrapper::GetTreeNodeToLabelSpacing() {
    return ::ImGui::GetTreeNodeToLabelSpacing();
}

bool ImGuiWrapper::CollapsingHeader_TreeNodeFlags(const char* label, int flags) {
    return ::ImGui::CollapsingHeader(label, flags);
}

// Child windows
bool ImGuiWrapper::BeginChild(const char* str_id, const ImVec2& size, bool border, int flags) {
    return ::ImGui::BeginChild(str_id, size, border, flags);
}

void ImGuiWrapper::EndChild() {
    ::ImGui::EndChild();
}

// Tabs
bool ImGuiWrapper::BeginTabBar(const char* str_id, int flags) {
    return ::ImGui::BeginTabBar(str_id, flags);
}

void ImGuiWrapper::EndTabBar() {
    ::ImGui::EndTabBar();
}

bool ImGuiWrapper::BeginTabItem(const char* label, bool* p_open, int flags) {
    return ::ImGui::BeginTabItem(label, p_open, flags);
}

void ImGuiWrapper::EndTabItem() {
    ::ImGui::EndTabItem();
}

bool ImGuiWrapper::TabItemButton(const char* label, int flags) {
    return ::ImGui::TabItemButton(label, flags);
}

void ImGuiWrapper::SetTabItemClosed(const char* tab_or_docked_window_label) {
    ::ImGui::SetTabItemClosed(tab_or_docked_window_label);
}

// Input methods
bool ImGuiWrapper::InputTextMultiline(const char* label, char* buf, size_t buf_size, const glm::vec2& size, int flags) {
    return ::ImGui::InputTextMultiline(label, buf, buf_size, ImVec2(size.x, size.y), flags);
}

bool ImGuiWrapper::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, int flags) {
    return ::ImGui::InputFloat(label, v, step, step_fast, format, flags);
}

bool ImGuiWrapper::InputFloat2(const char* label, float v[2], const char* format, int flags) {
    return ::ImGui::InputFloat2(label, v, format, flags);
}

bool ImGuiWrapper::InputFloat3(const char* label, float v[3], const char* format, int flags) {
    return ::ImGui::InputFloat3(label, v, format, flags);
}

bool ImGuiWrapper::InputFloat4(const char* label, float v[4], const char* format, int flags) {
    return ::ImGui::InputFloat4(label, v, format, flags);
}

bool ImGuiWrapper::InputInt(const char* label, int* v, int step, int step_fast, int flags) {
    return ::ImGui::InputInt(label, v, step, step_fast, flags);
}

bool ImGuiWrapper::InputInt2(const char* label, int v[2], int flags) {
    return ::ImGui::InputInt2(label, v, flags);
}

bool ImGuiWrapper::InputInt3(const char* label, int v[3], int flags) {
    return ::ImGui::InputInt3(label, v, flags);
}

bool ImGuiWrapper::InputInt4(const char* label, int v[4], int flags) {
    return ::ImGui::InputInt4(label, v, flags);
}

// Popup methods
bool ImGuiWrapper::BeginPopup(const char* str_id, int flags) {
    return ::ImGui::BeginPopup(str_id, flags);
}

bool ImGuiWrapper::BeginPopupModal(const char* name, bool* p_open, int flags) {
    return ::ImGui::BeginPopupModal(name, p_open, flags);
}

void ImGuiWrapper::EndPopup() {
    ::ImGui::EndPopup();
}

void ImGuiWrapper::OpenPopupOnItemClick(const char* str_id, int popup_flags) {
    ::ImGui::OpenPopupOnItemClick(str_id, popup_flags);
}

bool ImGuiWrapper::BeginPopupContextItem(const char* str_id, int popup_flags) {
    return ::ImGui::BeginPopupContextItem(str_id, popup_flags);
}

bool ImGuiWrapper::BeginPopupContextWindow(const char* str_id, int popup_flags) {
    return ::ImGui::BeginPopupContextWindow(str_id, popup_flags);
}

bool ImGuiWrapper::BeginPopupContextVoid(const char* str_id, int popup_flags) {
    return ::ImGui::BeginPopupContextVoid(str_id, popup_flags);
}

// Table methods
bool ImGuiWrapper::BeginTable(const char* str_id, int column, int flags, const glm::vec2& outer_size, float inner_width) {
    return ::ImGui::BeginTable(str_id, column, flags, ImVec2(outer_size.x, outer_size.y), inner_width);
}

void ImGuiWrapper::EndTable() {
    ::ImGui::EndTable();
}

void ImGuiWrapper::TableNextRow(int row_flags, float min_row_height) {
    ::ImGui::TableNextRow(row_flags, min_row_height);
}

bool ImGuiWrapper::TableNextColumn() {
    return ::ImGui::TableNextColumn();
}

bool ImGuiWrapper::TableSetColumnIndex(int column_n) {
    return ::ImGui::TableSetColumnIndex(column_n);
}

void ImGuiWrapper::TableSetupColumn(const char* label, int flags, float init_width_or_weight, uint32_t user_id) {
    ::ImGui::TableSetupColumn(label, flags, init_width_or_weight, user_id);
}

void ImGuiWrapper::TableSetupScrollFreeze(int cols, int rows) {
    ::ImGui::TableSetupScrollFreeze(cols, rows);
}

void ImGuiWrapper::TableHeader(const char* label) {
    ::ImGui::TableHeader(label);
}

void ImGuiWrapper::TableHeadersRow() {
    ::ImGui::TableHeadersRow();
}

void ImGuiWrapper::TableAngledHeadersRow() {
    ::ImGui::TableAngledHeadersRow();
}

void ImGuiWrapper::TableSetBgColor(int target, uint32_t color, int column_n) {
    ::ImGui::TableSetBgColor(target, color, column_n);
}

// Drag and Drop methods
bool ImGuiWrapper::BeginDragDropSource(int flags) {
    return ::ImGui::BeginDragDropSource(flags);
}

bool ImGuiWrapper::SetDragDropPayload(const char* type, const void* data, size_t sz, int cond) {
    return ::ImGui::SetDragDropPayload(type, data, sz, cond);
}

void ImGuiWrapper::EndDragDropSource() {
    ::ImGui::EndDragDropSource();
}

bool ImGuiWrapper::BeginDragDropTarget() {
    return ::ImGui::BeginDragDropTarget();
}

const ImGuiPayload* ImGuiWrapper::AcceptDragDropPayload(const char* type, int flags) {
    return ::ImGui::AcceptDragDropPayload(type, flags);
}

void ImGuiWrapper::EndDragDropTarget() {
    ::ImGui::EndDragDropTarget();
}

// Item methods
bool ImGuiWrapper::IsItemHovered(int flags) {
    return ::ImGui::IsItemHovered(flags);
}

bool ImGuiWrapper::IsItemActive() {
    return ::ImGui::IsItemActive();
}

bool ImGuiWrapper::IsItemFocused() {
    return ::ImGui::IsItemFocused();
}

bool ImGuiWrapper::IsItemClicked(int mouse_button) {
    return ::ImGui::IsItemClicked(mouse_button);
}

bool ImGuiWrapper::IsItemVisible() {
    return ::ImGui::IsItemVisible();
}

bool ImGuiWrapper::IsItemEdited() {
    return ::ImGui::IsItemEdited();
}

bool ImGuiWrapper::IsItemActivated() {
    return ::ImGui::IsItemActivated();
}

bool ImGuiWrapper::IsItemDeactivated() {
    return ::ImGui::IsItemDeactivated();
}

bool ImGuiWrapper::IsItemDeactivatedAfterEdit() {
    return ::ImGui::IsItemDeactivatedAfterEdit();
}

bool ImGuiWrapper::IsItemToggledOpen() {
    return ::ImGui::IsItemToggledOpen();
}

bool ImGuiWrapper::IsAnyItemHovered() {
    return ::ImGui::IsAnyItemHovered();
}

bool ImGuiWrapper::IsAnyItemActive() {
    return ::ImGui::IsAnyItemActive();
}

bool ImGuiWrapper::IsAnyItemFocused() {
    return ::ImGui::IsAnyItemFocused();
}

// Window methods
bool ImGuiWrapper::IsWindowFocused(int flags) {
    return ::ImGui::IsWindowFocused(flags);
}

bool ImGuiWrapper::IsWindowHovered(int flags) {
    return ::ImGui::IsWindowHovered(flags);
}

bool ImGuiWrapper::IsWindowCollapsed() {
    return ::ImGui::IsWindowCollapsed();
}

bool ImGuiWrapper::IsWindowAppearing() {
    return ::ImGui::IsWindowAppearing();
}

void ImGuiWrapper::SetNextWindowPos(const glm::vec2& pos, int cond, const glm::vec2& pivot) {
    ::ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), cond, ImVec2(pivot.x, pivot.y));
}

void ImGuiWrapper::SetNextWindowSize(const glm::vec2& size, int cond) {
    ::ImGui::SetNextWindowSize(ImVec2(size.x, size.y), cond);
}

void ImGuiWrapper::SetNextWindowContentSize(const glm::vec2& size) {
    ::ImGui::SetNextWindowContentSize(ImVec2(size.x, size.y));
}

void ImGuiWrapper::SetNextWindowCollapsed(bool collapsed, int cond) {
    ::ImGui::SetNextWindowCollapsed(collapsed, cond);
}

void ImGuiWrapper::SetNextWindowFocus() {
    ::ImGui::SetNextWindowFocus();
}

void ImGuiWrapper::SetNextWindowScroll(const glm::vec2& scroll) {
    ::ImGui::SetNextWindowScroll(ImVec2(scroll.x, scroll.y));
}

void ImGuiWrapper::SetNextWindowBgAlpha(float alpha) {
    ::ImGui::SetNextWindowBgAlpha(alpha);
}

void ImGuiWrapper::SetNextWindowViewport(uint32_t viewport_id) {
    ::ImGui::SetNextWindowViewport(viewport_id);
}

void ImGuiWrapper::SetWindowPos(const glm::vec2& pos, int cond) {
    ::ImGui::SetWindowPos(ImVec2(pos.x, pos.y), cond);
}

void ImGuiWrapper::SetWindowSize(const glm::vec2& size, int cond) {
    ::ImGui::SetWindowSize(ImVec2(size.x, size.y), cond);
}

void ImGuiWrapper::SetWindowCollapsed(bool collapsed, int cond) {
    ::ImGui::SetWindowCollapsed(collapsed, cond);
}

void ImGuiWrapper::SetWindowFocus() {
    ::ImGui::SetWindowFocus();
}

void ImGuiWrapper::SetWindowFontScale(float scale) {
    ::ImGui::SetWindowFontScale(scale);
}

void ImGuiWrapper::SetWindowPos(const char* name, const glm::vec2& pos, int cond) {
    ::ImGui::SetWindowPos(name, ImVec2(pos.x, pos.y), cond);
}

void ImGuiWrapper::SetWindowSize(const char* name, const glm::vec2& size, int cond) {
    ::ImGui::SetWindowSize(name, ImVec2(size.x, size.y), cond);
}

void ImGuiWrapper::SetWindowCollapsed(const char* name, bool collapsed, int cond) {
    ::ImGui::SetWindowCollapsed(name, collapsed, cond);
}

void ImGuiWrapper::SetWindowFocus(const char* name) {
    ::ImGui::SetWindowFocus(name);
}

// Style methods
void ImGuiWrapper::PushStyleColor(int idx, uint32_t col) {
    ::ImGui::PushStyleColor(idx, col);
}

void ImGuiWrapper::PushStyleColor(int idx, const glm::vec4& col) {
    ::ImGui::PushStyleColor(idx, ImVec4(col.r, col.g, col.b, col.a));
}

void ImGuiWrapper::PopStyleColor(int count) {
    ::ImGui::PopStyleColor(count);
}

void ImGuiWrapper::PushStyleVar(int idx, float val) {
    ::ImGui::PushStyleVar(idx, val);
}

void ImGuiWrapper::PushStyleVar(int idx, const glm::vec2& val) {
    ::ImGui::PushStyleVar(idx, ImVec2(val.x, val.y));
}

void ImGuiWrapper::PopStyleVar(int count) {
    ::ImGui::PopStyleVar(count);
}

const glm::vec4& ImGuiWrapper::GetStyleColorVec4(int idx) {
    const ImVec4& color = ::ImGui::GetStyleColorVec4(idx);
    static glm::vec4 result;
    result.r = color.x;
    result.g = color.y;
    result.b = color.z;
    result.a = color.w;
    return result;
}

uint32_t ImGuiWrapper::GetColorU32(int idx, float alpha_mul) {
    return ::ImGui::GetColorU32(idx, alpha_mul);
}

uint32_t ImGuiWrapper::GetColorU32(const glm::vec4& col) {
    return ::ImGui::GetColorU32(ImVec4(col.r, col.g, col.b, col.a));
}

uint32_t ImGuiWrapper::GetColorU32(uint32_t col) {
    return ::ImGui::GetColorU32(col);
}

// Advanced window functions
ImGuiID ImGuiWrapper::GetID(const char* str_id) {
    return ::ImGui::GetID(str_id);
}

ImGuiIO& ImGuiWrapper::GetIO() {
    return ::ImGui::GetIO();
}

ImGuiStyle& ImGuiWrapper::GetStyle() {
    return ::ImGui::GetStyle();
}

void ImGuiWrapper::DockSpace(ImGuiID id, const ImVec2& size, ImGuiDockNodeFlags flags) {
    ::ImGui::DockSpace(id, size, flags);
}

void ImGuiWrapper::DockSpaceOverViewport(ImGuiID id, const ImGuiViewport* viewport, ImGuiDockNodeFlags flags) {
    ::ImGui::DockSpaceOverViewport(id, viewport, flags);
}

// Advanced utilities
bool ImGuiWrapper::Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items) {
    return ::ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
}

void ImGuiWrapper::SetTooltip(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ::ImGui::SetTooltipV(fmt, args);
    va_end(args);
}

void ImGuiWrapper::PushID(const char* str_id) {
    ::ImGui::PushID(str_id);
}

void ImGuiWrapper::PushID(const void* ptr_id) {
    ::ImGui::PushID(ptr_id);
}

void ImGuiWrapper::PushID(int int_id) {
    ::ImGui::PushID(int_id);
}

void ImGuiWrapper::PopID() {
    ::ImGui::PopID();
}

ImVec2 ImGuiWrapper::GetContentRegionAvail() {
    return ::ImGui::GetContentRegionAvail();
}

void ImGuiWrapper::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride) {
    ::ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
}

void ImGuiWrapper::StyleColorsDark() {
    ::ImGui::StyleColorsDark();
}

void ImGuiWrapper::StyleColorsLight() {
    ::ImGui::StyleColorsLight();
}

void ImGuiWrapper::StyleColorsClassic() {
    ::ImGui::StyleColorsClassic();
}

// Columns
void ImGuiWrapper::Columns(int count, const char* id, bool border) {
    ::ImGui::Columns(count, id, border);
}

void ImGuiWrapper::NextColumn() {
    ::ImGui::NextColumn();
}

// Popups
void ImGuiWrapper::OpenPopup(const char* str_id, int popup_flags) {
    ::ImGui::OpenPopup(str_id, popup_flags);
}

void ImGuiWrapper::CloseCurrentPopup() {
    ::ImGui::CloseCurrentPopup();
}

// Menu methods
bool ImGuiWrapper::BeginMainMenuBar() {
    return ::ImGui::BeginMainMenuBar();
}

void ImGuiWrapper::EndMainMenuBar() {
    ::ImGui::EndMainMenuBar();
}

bool ImGuiWrapper::BeginMenuBar() {
    return ::ImGui::BeginMenuBar();
}

void ImGuiWrapper::EndMenuBar() {
    ::ImGui::EndMenuBar();
}

bool ImGuiWrapper::BeginMenu(const char* label, bool enabled) {
    return ::ImGui::BeginMenu(label, enabled);
}

void ImGuiWrapper::EndMenu() {
    ::ImGui::EndMenu();
}

bool ImGuiWrapper::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled) {
    return ::ImGui::MenuItem(label, shortcut, selected, enabled);
}

bool ImGuiWrapper::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled) {
    return ::ImGui::MenuItem(label, shortcut, p_selected, enabled);
}

} // namespace ImGui
} // namespace Zgine
