#include <Zgine/Gui/Backend/ImGui/Themes/ImGuiTheme.h>
#include <imgui.h>
#include <cstdio>
#include <cstdlib>

namespace Zgine {

ImVec4 ImGuiTheme::HexToVec4(const char* hex, float alpha) {
    unsigned int r, g, b;
#ifdef _MSC_VER
    if (sscanf_s(hex, "#%02x%02x%02x", &r, &g, &b) == 3) {
#else
    if (sscanf(hex, "#%02x%02x%02x", &r, &g, &b) == 3) {
#endif
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, alpha);
    }
    return ImVec4(1.0f, 1.0f, 1.0f, alpha);
}

ImVec4 ImGuiTheme::WithAlpha(ImVec4 color, float alpha) {
    return ImVec4(color.x, color.y, color.z, alpha);
}

void ImGuiTheme::ApplyTheme(ImGuiThemeType theme) {
    if (theme == ImGuiThemeType::Light) {
        ApplyLightTheme();
    } else {
        ApplyDarkTheme();
    }
}

void ImGuiTheme::ApplyLightTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // 基础色板 - 宣纸亮色
    ImVec4 Paper = HexToVec4("#F6F1E6");
    ImVec4 Paper2 = HexToVec4("#EFE7D8");
    ImVec4 Ink1 = HexToVec4("#1B1B1B");
    ImVec4 Ink2 = HexToVec4("#343434");
    ImVec4 Ink3 = HexToVec4("#6E6A62");
    ImVec4 Wash = HexToVec4("#DAD1C3");
    ImVec4 SealRed = HexToVec4("#B23A2F");
    ImVec4 Indigo = HexToVec4("#2F4FA8");
    ImVec4 BambooGreen = HexToVec4("#2E7D32");
    ImVec4 Ochre = HexToVec4("#B7791F");

    ImVec4* colors = style.Colors;

    // 文本颜色
    colors[ImGuiCol_Text] = Ink1;
    colors[ImGuiCol_TextDisabled] = Ink3;

    // 窗口背景
    colors[ImGuiCol_WindowBg] = Paper;
    colors[ImGuiCol_ChildBg] = Paper;
    colors[ImGuiCol_PopupBg] = Paper2;
    colors[ImGuiCol_DockingEmptyBg] = Paper;

    // 边框
    colors[ImGuiCol_Border] = WithAlpha(Ink3, 0.45f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // 框架背景
    colors[ImGuiCol_FrameBg] = WithAlpha(Wash, 0.35f);
    colors[ImGuiCol_FrameBgHovered] = WithAlpha(Wash, 0.55f);
    colors[ImGuiCol_FrameBgActive] = WithAlpha(Wash, 0.75f);

    // 标题栏
    colors[ImGuiCol_TitleBg] = Paper2;
    colors[ImGuiCol_TitleBgActive] = Paper2;
    colors[ImGuiCol_TitleBgCollapsed] = WithAlpha(Paper2, 0.75f);

    // 菜单栏
    colors[ImGuiCol_MenuBarBg] = Paper2;

    // 滚动条
    colors[ImGuiCol_ScrollbarBg] = WithAlpha(Paper2, 0.55f);
    colors[ImGuiCol_ScrollbarGrab] = WithAlpha(Ink3, 0.35f);
    colors[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(Ink3, 0.55f);
    colors[ImGuiCol_ScrollbarGrabActive] = WithAlpha(Ink3, 0.75f);

    // 复选框/单选按钮
    colors[ImGuiCol_CheckMark] = SealRed;

    // 滑块
    colors[ImGuiCol_SliderGrab] = WithAlpha(Ink2, 0.55f);
    colors[ImGuiCol_SliderGrabActive] = WithAlpha(SealRed, 0.9f);

    // 按钮
    colors[ImGuiCol_Button] = WithAlpha(Wash, 0.35f);
    colors[ImGuiCol_ButtonHovered] = WithAlpha(Wash, 0.55f);
    colors[ImGuiCol_ButtonActive] = WithAlpha(Wash, 0.75f);

    // 标题/可折叠头部
    colors[ImGuiCol_Header] = WithAlpha(Wash, 0.35f);
    colors[ImGuiCol_HeaderHovered] = WithAlpha(Indigo, 0.18f);
    colors[ImGuiCol_HeaderActive] = WithAlpha(Indigo, 0.28f);

    // 分隔线
    colors[ImGuiCol_Separator] = WithAlpha(Ink3, 0.35f);
    colors[ImGuiCol_SeparatorHovered] = WithAlpha(Ink2, 0.55f);
    colors[ImGuiCol_SeparatorActive] = WithAlpha(Ink2, 0.75f);

    // 调整大小手柄
    colors[ImGuiCol_ResizeGrip] = WithAlpha(Ink3, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = WithAlpha(Ink3, 0.45f);
    colors[ImGuiCol_ResizeGripActive] = WithAlpha(Ink3, 0.65f);

    // 标签页
    colors[ImGuiCol_Tab] = WithAlpha(Paper2, 0.75f);
    colors[ImGuiCol_TabHovered] = WithAlpha(Indigo, 0.18f);
    colors[ImGuiCol_TabActive] = Paper2;
    colors[ImGuiCol_TabUnfocused] = WithAlpha(Paper2, 0.55f);
    colors[ImGuiCol_TabUnfocusedActive] = WithAlpha(Paper2, 0.75f);

    // 图表
    colors[ImGuiCol_PlotLines] = Ink2;
    colors[ImGuiCol_PlotLinesHovered] = SealRed;
    colors[ImGuiCol_PlotHistogram] = Ochre;
    colors[ImGuiCol_PlotHistogramHovered] = SealRed;

    // 文本选择背景
    colors[ImGuiCol_TextSelectedBg] = WithAlpha(Indigo, 0.20f);

    // 拖放目标
    colors[ImGuiCol_DragDropTarget] = WithAlpha(SealRed, 0.65f);

    // 导航高亮
    colors[ImGuiCol_NavHighlight] = WithAlpha(Indigo, 0.35f);
    colors[ImGuiCol_NavWindowingHighlight] = WithAlpha(Indigo, 0.20f);
    colors[ImGuiCol_NavWindowingDimBg] = WithAlpha(Ink1, 0.08f);

    // 模态窗口
    colors[ImGuiCol_ModalWindowDimBg] = WithAlpha(Ink1, 0.10f);

    // 表格（如果使用）
    colors[ImGuiCol_TableHeaderBg] = WithAlpha(Paper2, 0.75f);
    colors[ImGuiCol_TableBorderStrong] = WithAlpha(Ink3, 0.55f);
    colors[ImGuiCol_TableBorderLight] = WithAlpha(Ink3, 0.35f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = WithAlpha(Paper2, 0.25f);

    // 样式参数 - 水墨感
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;

    style.WindowPadding = ImVec2(10.0f, 8.0f);
    style.FramePadding = ImVec2(8.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.DockingSeparatorSize = 0.0f;

    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
}

void ImGuiTheme::ApplyDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // 基础色板 - 墨夜暗色
    ImVec4 InkPaper = HexToVec4("#121417");
    ImVec4 InkPaper2 = HexToVec4("#181B20");
    ImVec4 InkText = HexToVec4("#EDE7DD");
    ImVec4 InkText2 = HexToVec4("#CFC7BA");
    ImVec4 InkLine = HexToVec4("#5B5A58");
    ImVec4 InkWash = HexToVec4("#2A2D33");
    ImVec4 SealRed = HexToVec4("#D05A4F");
    ImVec4 Indigo = HexToVec4("#6E8BFF");
    ImVec4 BambooGreen = HexToVec4("#6BCB77");
    ImVec4 Ochre = HexToVec4("#D3A14A");

    ImVec4* colors = style.Colors;

    // 文本颜色
    colors[ImGuiCol_Text] = InkText;
    colors[ImGuiCol_TextDisabled] = WithAlpha(InkText2, 0.65f);

    // 窗口背景
    colors[ImGuiCol_WindowBg] = InkPaper;
    colors[ImGuiCol_ChildBg] = InkPaper;
    colors[ImGuiCol_PopupBg] = InkPaper2;
    colors[ImGuiCol_DockingEmptyBg] = InkPaper;

    // 边框
    colors[ImGuiCol_Border] = WithAlpha(InkLine, 0.45f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // 框架背景
    colors[ImGuiCol_FrameBg] = WithAlpha(InkWash, 0.55f);
    colors[ImGuiCol_FrameBgHovered] = WithAlpha(InkWash, 0.75f);
    colors[ImGuiCol_FrameBgActive] = WithAlpha(InkWash, 0.95f);

    // 标题栏
    colors[ImGuiCol_TitleBg] = InkPaper2;
    colors[ImGuiCol_TitleBgActive] = InkPaper2;
    colors[ImGuiCol_TitleBgCollapsed] = WithAlpha(InkPaper2, 0.65f);

    // 菜单栏
    colors[ImGuiCol_MenuBarBg] = InkPaper2;

    // 滚动条
    colors[ImGuiCol_ScrollbarBg] = WithAlpha(InkPaper2, 0.55f);
    colors[ImGuiCol_ScrollbarGrab] = WithAlpha(InkLine, 0.35f);
    colors[ImGuiCol_ScrollbarGrabHovered] = WithAlpha(InkLine, 0.55f);
    colors[ImGuiCol_ScrollbarGrabActive] = WithAlpha(InkLine, 0.75f);

    // 复选框/单选按钮
    colors[ImGuiCol_CheckMark] = SealRed;

    // 滑块
    colors[ImGuiCol_SliderGrab] = WithAlpha(InkText2, 0.35f);
    colors[ImGuiCol_SliderGrabActive] = WithAlpha(SealRed, 0.85f);

    // 按钮
    colors[ImGuiCol_Button] = WithAlpha(InkWash, 0.55f);
    colors[ImGuiCol_ButtonHovered] = WithAlpha(InkWash, 0.75f);
    colors[ImGuiCol_ButtonActive] = WithAlpha(InkWash, 0.95f);

    // 标题/可折叠头部
    colors[ImGuiCol_Header] = WithAlpha(InkWash, 0.55f);
    colors[ImGuiCol_HeaderHovered] = WithAlpha(Indigo, 0.20f);
    colors[ImGuiCol_HeaderActive] = WithAlpha(Indigo, 0.32f);

    // 分隔线
    colors[ImGuiCol_Separator] = WithAlpha(InkLine, 0.35f);
    colors[ImGuiCol_SeparatorHovered] = WithAlpha(InkLine, 0.55f);
    colors[ImGuiCol_SeparatorActive] = WithAlpha(InkLine, 0.75f);

    // 调整大小手柄
    colors[ImGuiCol_ResizeGrip] = WithAlpha(InkLine, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = WithAlpha(InkLine, 0.45f);
    colors[ImGuiCol_ResizeGripActive] = WithAlpha(InkLine, 0.65f);

    // 标签页
    colors[ImGuiCol_Tab] = WithAlpha(InkPaper2, 0.85f);
    colors[ImGuiCol_TabHovered] = WithAlpha(Indigo, 0.20f);
    colors[ImGuiCol_TabActive] = InkPaper2;
    colors[ImGuiCol_TabUnfocused] = WithAlpha(InkPaper2, 0.55f);
    colors[ImGuiCol_TabUnfocusedActive] = WithAlpha(InkPaper2, 0.75f);

    // 图表
    colors[ImGuiCol_PlotLines] = InkText2;
    colors[ImGuiCol_PlotLinesHovered] = SealRed;
    colors[ImGuiCol_PlotHistogram] = Ochre;
    colors[ImGuiCol_PlotHistogramHovered] = SealRed;

    // 文本选择背景
    colors[ImGuiCol_TextSelectedBg] = WithAlpha(Indigo, 0.25f);

    // 拖放目标
    colors[ImGuiCol_DragDropTarget] = WithAlpha(SealRed, 0.70f);

    // 导航高亮
    colors[ImGuiCol_NavHighlight] = WithAlpha(Indigo, 0.38f);
    colors[ImGuiCol_NavWindowingHighlight] = WithAlpha(Indigo, 0.22f);
    colors[ImGuiCol_NavWindowingDimBg] = WithAlpha(InkPaper, 0.22f);

    // 模态窗口
    colors[ImGuiCol_ModalWindowDimBg] = WithAlpha(InkPaper, 0.35f);

    // 表格（如果使用）
    colors[ImGuiCol_TableHeaderBg] = WithAlpha(InkPaper2, 0.75f);
    colors[ImGuiCol_TableBorderStrong] = WithAlpha(InkLine, 0.55f);
    colors[ImGuiCol_TableBorderLight] = WithAlpha(InkLine, 0.35f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = WithAlpha(InkPaper2, 0.25f);

    // 样式参数 - 水墨感
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;

    style.WindowPadding = ImVec2(10.0f, 8.0f);
    style.FramePadding = ImVec2(8.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 16.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.DockingSeparatorSize = 0.0f;

    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
}

}

