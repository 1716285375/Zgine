#pragma once

#include <imgui.h>

namespace Zgine {

enum class ImGuiThemeType {
    Light,  // 宣纸亮色
    Dark    // 墨夜暗色
};

class ImGuiTheme {
public:
    static void ApplyTheme(ImGuiThemeType theme);
    static void ApplyLightTheme();  // 宣纸亮色
    static void ApplyDarkTheme();   // 墨夜暗色
    
    // 辅助函数：将十六进制颜色转换为 ImVec4
    static ImVec4 HexToVec4(const char* hex, float alpha = 1.0f);
    
private:
    // 颜色转换辅助函数
    static ImVec4 WithAlpha(ImVec4 color, float alpha);
};

}

