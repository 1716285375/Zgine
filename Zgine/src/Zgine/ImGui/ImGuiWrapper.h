#pragma once

#include "Zgine/Core.h"
#include <glm/glm.hpp>

namespace Zgine {
	namespace ImGui {

		/**
		 * @class ImGuiWrapper
		 * @brief Static wrapper class for ImGui functionality
		 * @details Provides a clean interface to ImGui functions without exposing
		 *          the raw ImGui headers to application code
		 */
		class ImGuiWrapper {
		public:
			// Window management
			static bool Begin(const char* name, bool* p_open = nullptr, int flags = 0);
			static void End();

			// Text and labels
			static void Text(const char* fmt, ...);
			static void TextColored(const glm::vec4& color, const char* fmt, ...);
			static void TextDisabled(const char* fmt, ...);
			static void TextWrapped(const char* fmt, ...);
			static void LabelText(const char* label, const char* fmt, ...);
			static void BulletText(const char* fmt, ...);

			// Buttons and inputs
			static bool Button(const char* label, const glm::vec2& size = { 0, 0 });
			static bool SmallButton(const char* label);
			static bool InvisibleButton(const char* str_id, const glm::vec2& size, int flags = 0);
			static bool ArrowButton(const char* str_id, int dir);
			static bool Checkbox(const char* label, bool* v);
			static bool CheckboxFlags(const char* label, int* flags, int flags_value);
			static bool RadioButton(const char* label, bool active);
			static bool RadioButton(const char* label, int* v, int v_button);

			// Sliders
			static bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", int flags = 0);
			static bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", int flags = 0);
			static bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", int flags = 0);
			static bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", int flags = 0);
			static bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", int flags = 0);
			static bool SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d", int flags = 0);
			static bool SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d", int flags = 0);
			static bool SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d", int flags = 0);

			// Selection
			static bool Selectable(const char* label, bool selected = false, int flags = 0);
			static bool Selectable(const char* label, bool* p_selected, int flags = 0);
			
			// Input fields
			static bool InputText(const char* label, char* buf, size_t buf_size, int flags = 0);
			static bool InputTextMultiline(const char* label, char* buf, size_t buf_size, const glm::vec2& size = { 0, 0 }, int flags = 0);
			static bool InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", int flags = 0);
			static bool InputFloat2(const char* label, float v[2], const char* format = "%.3f", int flags = 0);
			static bool InputFloat3(const char* label, float v[3], const char* format = "%.3f", int flags = 0);
			static bool InputFloat4(const char* label, float v[4], const char* format = "%.3f", int flags = 0);
			static bool InputInt(const char* label, int* v, int step = 1, int step_fast = 100, int flags = 0);
			static bool InputInt2(const char* label, int v[2], int flags = 0);
			static bool InputInt3(const char* label, int v[3], int flags = 0);
			static bool InputInt4(const char* label, int v[4], int flags = 0);

			// Color pickers
			static bool ColorEdit3(const char* label, float col[3], int flags = 0);
			static bool ColorEdit4(const char* label, float col[4], int flags = 0);
			static bool ColorPicker3(const char* label, float col[3], int flags = 0);
			static bool ColorPicker4(const char* label, float col[4], int flags = 0);

			// Layout
			static void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);
			static void NewLine();
			static void Spacing();
			static void Dummy(const glm::vec2& size);
			static void Indent(float indent_w = 0.0f);
			static void Unindent(float indent_w = 0.0f);
			static void BeginGroup();
			static void EndGroup();
			static glm::vec2 GetCursorPos();
			static void SetCursorPos(const glm::vec2& local_pos);
			static glm::vec2 GetCursorStartPos();
			static glm::vec2 GetCursorScreenPos();
			static void SetCursorScreenPos(const glm::vec2& pos);
			static void AlignTextToFramePadding();
			static float GetTextLineHeight();
			static float GetTextLineHeightWithSpacing();
			static float GetFrameHeight();
			static float GetFrameHeightWithSpacing();

			// Columns
			static void Columns(int count = 1, const char* id = nullptr, bool border = true);
			static void NextColumn();
			static int GetColumnIndex();
			static float GetColumnWidth(int column_index = -1);
			static void SetColumnWidth(int column_index, float width);
			static float GetColumnOffset(int column_index = -1);
			static void SetColumnOffset(int column_index, float offset_x);
			static int GetColumnsCount();

			// Separators
			static void Separator();
			static void SeparatorEx(int separator_type);

			// Collapsing headers
			static bool CollapsingHeader(const char* label, int flags = 0);
			static bool CollapsingHeader(const char* label, bool* p_open, int flags = 0);

			// Trees
			static bool TreeNode(const char* label);
			static bool TreeNode(const char* str_id, const char* fmt, ...);
			static bool TreeNodeEx(const char* label, int flags = 0);
			static bool TreeNodeEx(const char* str_id, int flags, const char* fmt, ...);
			static void TreePush(const char* str_id = nullptr);
			static void TreePop();
			static float GetTreeNodeToLabelSpacing();
			static bool CollapsingHeader_TreeNodeFlags(const char* label, int flags = 0);

			// Popups
			static bool BeginPopup(const char* str_id, int flags = 0);
			static bool BeginPopupModal(const char* name, bool* p_open = nullptr, int flags = 0);
			static void EndPopup();
			static void OpenPopup(const char* str_id, int popup_flags = 0);
			static void OpenPopupOnItemClick(const char* str_id = nullptr, int popup_flags = 1);
			static void CloseCurrentPopup();
			static bool BeginPopupContextItem(const char* str_id = nullptr, int popup_flags = 1);
			static bool BeginPopupContextWindow(const char* str_id = nullptr, int popup_flags = 1);
			static bool BeginPopupContextVoid(const char* str_id = nullptr, int popup_flags = 1);

			// Menus
			static bool BeginMenuBar();
			static void EndMenuBar();
			static bool BeginMainMenuBar();
			static void EndMainMenuBar();
			static bool BeginMenu(const char* label, bool enabled = true);
			static void EndMenu();
			static bool MenuItem(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true);
			static bool MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);

			// Tooltips
			static void BeginTooltip();
			static void EndTooltip();
			static void SetTooltip(const char* fmt, ...);

			// Tables
			static bool BeginTable(const char* str_id, int column, int flags = 0, const glm::vec2& outer_size = { 0.0f, 0.0f }, float inner_width = 0.0f);
			static void EndTable();
			static void TableNextRow(int row_flags = 0, float min_row_height = 0.0f);
			static bool TableNextColumn();
			static bool TableSetColumnIndex(int column_n);
			static void TableSetupColumn(const char* label, int flags = 0, float init_width_or_weight = 0.0f, uint32_t user_id = 0);
			static void TableSetupScrollFreeze(int cols, int rows);
			static void TableHeadersRow();
			static void TableHeader(const char* label);
			static int TableGetColumnCount();
			static int TableGetColumnIndex();
			static int TableGetRowIndex();
			static const char* TableGetColumnName(int column = -1);
			static int TableGetColumnFlags(int column = -1);
			static void TableSetColumnEnabled(int column, bool enabled);
			static void TableSetBgColor(int target, uint32_t color, int column = -1);

			// Tab bars
			static bool BeginTabBar(const char* str_id, int flags = 0);
			static void EndTabBar();
			static bool BeginTabItem(const char* label, bool* p_open = nullptr, int flags = 0);
			static void EndTabItem();
			static bool TabItemButton(const char* label, int flags = 0);
			static void SetTabItemClosed(const char* tab_or_docked_window_label);

			// Logging
			static void LogToTTY(int auto_open_depth = -1);
			static void LogToFile(int auto_open_depth = -1, const char* filename = nullptr);
			static void LogToClipboard(int auto_open_depth = -1);
			static void LogFinish();
			static void LogButtons();
			static void LogText(const char* fmt, ...);

			// Drag and drop
			static bool BeginDragDropSource(int flags = 0);
			static bool SetDragDropPayload(const char* type, const void* data, size_t sz, int cond = 0);
			static void EndDragDropSource();
			static bool BeginDragDropTarget();
			static const void* AcceptDragDropPayload(const char* type, int flags = 0);
			static void EndDragDropTarget();
			static const void* GetDragDropPayload();

			// Utilities
			static bool IsItemHovered(int flags = 0);
			static bool IsItemActive();
			static bool IsItemFocused();
			static bool IsItemClicked(int mouse_button = 0);
			static bool IsItemVisible();
			static bool IsItemEdited();
			static bool IsItemActivated();
			static bool IsItemDeactivated();
			static bool IsItemDeactivatedAfterEdit();
			static bool IsItemToggledOpen();
			static bool IsAnyItemHovered();
			static bool IsAnyItemActive();
			static bool IsAnyItemFocused();
			static glm::vec2 GetItemRectMin();
			static glm::vec2 GetItemRectMax();
			static glm::vec2 GetItemRectSize();
			static void SetItemAllowOverlap();

			// Window utilities
			static bool IsWindowAppearing();
			static bool IsWindowCollapsed();
			static bool IsWindowFocused(int flags = 0);
			static bool IsWindowHovered(int flags = 0);
			static glm::vec2 GetWindowPos();
			static glm::vec2 GetWindowSize();
			static float GetWindowWidth();
			static float GetWindowHeight();
			static void SetNextWindowPos(const glm::vec2& pos, int cond = 0, const glm::vec2& pivot = { 0, 0 });
			static void SetNextWindowSize(const glm::vec2& size, int cond = 0);
			static void SetNextWindowContentSize(const glm::vec2& size);
			static void SetNextWindowCollapsed(bool collapsed, int cond = 0);
			static void SetNextWindowFocus();
			static void SetNextWindowBgAlpha(float alpha);
			static void SetNextWindowViewport(uint32_t viewport_id);
			static void SetWindowPos(const glm::vec2& pos, int cond = 0);
			static void SetWindowSize(const glm::vec2& size, int cond = 0);
			static void SetWindowCollapsed(bool collapsed, int cond = 0);
			static void SetWindowFocus();
			static void SetWindowFontScale(float scale);
			static void SetWindowPos(const char* name, const glm::vec2& pos, int cond = 0);
			static void SetWindowSize(const char* name, const glm::vec2& size, int cond = 0);
			static void SetWindowCollapsed(const char* name, bool collapsed, int cond = 0);
			static void SetWindowFocus(const char* name);

			// Style
			static void PushStyleColor(int idx, uint32_t col);
			static void PopStyleColor(int count = 1);
			static void PushStyleVar(int idx, float val);
			static void PushStyleVar(int idx, const glm::vec2& val);
			static void PopStyleVar(int count = 1);
			static const glm::vec4& GetStyleColorVec4(int idx);
			static float GetFontSize();
			static glm::vec2 GetFontTexUvWhitePixel();
			static uint32_t GetColorU32(int idx, float alpha_mul = 1.0f);
			static uint32_t GetColorU32(const glm::vec4& col);
			static uint32_t GetColorU32(uint32_t col);

			// Demo and debugging
			static void ShowDemoWindow(bool* p_open = nullptr);
			static void ShowAboutWindow(bool* p_open = nullptr);
			static void ShowMetricsWindow(bool* p_open = nullptr);
			static void ShowDebugLogWindow(bool* p_open = nullptr);
			static void ShowStackToolWindow(bool* p_open = nullptr);
			static void ShowStyleEditor(void* ref = nullptr);
			static bool ShowStyleSelector(const char* label);
			static void ShowFontSelector(const char* label);
			static void ShowUserGuide();
			static void GetVersion();
		};

		// Short alias for ImGuiWrapper to reduce verbosity
		using IG = ImGuiWrapper;

	} // namespace ImGui

	// Global alias for easier access
	using IG = ImGui::ImGuiWrapper;

} // namespace Zgine
