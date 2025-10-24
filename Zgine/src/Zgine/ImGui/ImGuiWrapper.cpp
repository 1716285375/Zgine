#include "zgpch.h" // Precompiled header
#include "ImGuiWrapper.h"
#include "imgui.h"
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

// 其他方法的实现可以按需添加...

} // namespace ImGui
} // namespace Zgine
