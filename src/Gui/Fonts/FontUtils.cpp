#include <Zgine/Gui/Fonts/FontUtils.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/OS/File.h>

namespace Zgine {

// ============================================================================
// CJK Font Resolution Implementation / CJK 字体解析实现
// ============================================================================

std::string FontUtils::FindCjkFontPath() {
    ZGINE_CORE_TRACE("Searching for CJK font...");

    // Font search candidates in priority order / 按优先级排序的字体搜索候选项
    const char* candidates[] = {
        // ====================================================================
        // Project bundled Noto Sans fonts (preferred) / 项目捆绑的 Noto Sans 字体（首选）
        // ====================================================================

        // Variable fonts (most flexible) / 可变字体（最灵活）
        "assets/fonts/Noto_Sans/NotoSans-VariableFont_wdth,wght.ttf",

        // Static fonts / 静态字体
        "assets/fonts/Noto_Sans/static/NotoSans-Regular.ttf",
        "assets/fonts/Noto_Sans/static/NotoSans-Medium.ttf",
        "assets/fonts/Noto_Sans/static/NotoSans-Bold.ttf",

        // Legacy CJK font paths (backward compatibility) / 旧版 CJK 字体路径（向后兼容）
        "assets/fonts/NotoSansCJKsc-Regular.otf",
        "assets/fonts/NotoSansSC-Regular.ttf",
        "assets/fonts/NotoSansCJK-Regular.ttf",

        // ====================================================================
        // Windows system fonts (fallback) / Windows 系统字体（后备）
        // ====================================================================

        "C:/Windows/Fonts/msyh.ttc",      // Microsoft YaHei / 微软雅黑
        "C:/Windows/Fonts/msyh.ttf",
        "C:/Windows/Fonts/simsun.ttc",    // SimSun / 宋体
        "C:/Windows/Fonts/simhei.ttf",    // SimHei / 黑体
        "C:/Windows/Fonts/msjh.ttc",      // Microsoft JhengHei / 微软正黑体
    };

    // Search for the first available font using Core/OS/File
    // 使用 Core/OS/File 搜索第一个可用的字体
    for (const char* path : candidates) {
        if (File::Exists(path)) {
            ZGINE_CORE_INFO("Found CJK font: {}", path);
            return path;
        } else {
            ZGINE_CORE_TRACE("CJK font not found at: {}", path);
        }
    }

    ZGINE_CORE_WARN("No CJK font found in any search location");
    return std::string();
}

// ============================================================================
// Path Utilities Implementation / 路径工具实现
// ============================================================================

std::string FontUtils::BuildFontPath(const char* directory, const char* filename) {
    if (!directory || !filename) {
        ZGINE_CORE_WARN("FontUtils::BuildFontPath called with null argument");
        return std::string();
    }

    return std::string(directory) + "/" + filename;
}

} // namespace Zgine
