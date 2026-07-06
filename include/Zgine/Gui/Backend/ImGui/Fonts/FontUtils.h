#pragma once

#include <string>

namespace Zgine {

// ============================================================================
// Font Utilities / 字体工具
// ============================================================================

/**
 * @brief Utility class for font-related file operations
 * @brief 字体相关文件操作的工具类
 *
 * Provides utility functions for:
 * - Locating font files from various sources (project/system)
 * - Building font file paths
 * - Validating font file existence
 *
 * Uses Core/OS/File for file system operations.
 *
 * 提供以下工具功能：
 * - 从各种来源（项目/系统）定位字体文件
 * - 构建字体文件路径
 * - 验证字体文件是否存在
 *
 * 使用 Core/OS/File 进行文件系统操作。
 */
class FontUtils {
public:
    // ========================================================================
    // CJK Font Resolution / CJK 字体解析
    // ========================================================================

    /**
     * @brief Find a suitable CJK (Chinese/Japanese/Korean) font path
     * @brief 查找合适的 CJK（中日韩）字体路径
     *
     * Searches for CJK fonts in the following order:
     * 1. Project bundled Noto Sans fonts (preferred)
     * 2. Windows system fonts (fallback)
     *
     * 按以下顺序搜索 CJK 字体：
     * 1. 项目捆绑的 Noto Sans 字体（首选）
     * 2. Windows 系统字体（后备）
     *
     * @return Path to CJK font file, or empty string if not found
     * @return CJK 字体文件路径，如果未找到则返回空字符串
     */
    static std::string FindCjkFontPath();

    // ========================================================================
    // Path Utilities / 路径工具
    // ========================================================================

    /**
     * @brief Build a complete font path from directory and filename
     * @brief 从目录和文件名构建完整的字体路径
     *
     * @param directory Font directory path / 字体目录路径
     * @param filename Font file name / 字体文件名
     * @return Complete path to the font file / 字体文件的完整路径
     *
     * @example
     * @code
     * std::string path = FontUtils::BuildFontPath("assets/fonts/Inter", "Inter-Regular.ttf");
     * // Returns: "assets/fonts/Inter/Inter-Regular.ttf"
     * @endcode
     */
    static std::string BuildFontPath(const char* directory, const char* filename);
};

} // namespace Zgine
