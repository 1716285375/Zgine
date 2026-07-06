#pragma once

#include <imgui.h>
#include <string>

namespace Zgine {

// ============================================================================
// Font Configuration Structure / 字体配置结构体
// ============================================================================

/**
 * @brief Font loading configuration
 * @brief 字体加载配置
 *
 * Contains all parameters needed to load and configure a font for use with ImGui.
 * This structure separates font configuration from the FontManager, allowing
 * reuse in other contexts and cleaner API design.
 *
 * 包含为 ImGui 加载和配置字体所需的所有参数。
 * 此结构将字体配置与 FontManager 分离，允许在其他上下文中复用并提供更清晰的 API 设计。
 */
struct FontConfig {
    /**
     * @brief Font identifier name (e.g., "Inter", "JetBrainsMono")
     * @brief 字体标识名称（例如，"Inter"、"JetBrainsMono"）
     */
    std::string Name;

    /**
     * @brief Path to the font file (.ttf, .otf, .ttc)
     * @brief 字体文件路径（.ttf、.otf、.ttc）
     */
    std::string Path;

    /**
     * @brief Font size in pixels
     * @brief 字体大小（像素）
     * @default 16.0f
     */
    float SizePixels = 16.0f;

    /**
     * @brief Whether to merge this font into the default font
     * @brief 是否将此字体合并到默认字体
     *
     * When true, the glyphs from this font will be merged into the previously
     * loaded font. Useful for adding CJK character support to a base font.
     *
     * 当为 true 时，此字体的字形将合并到先前加载的字体中。
     * 适用于向基础字体添加 CJK 字符支持。
     *
     * @default false
     */
    bool MergeMode = false;

    /**
     * @brief Character glyph ranges to include from this font
     * @brief 要从此字体中包含的字符字形范围
     *
     * If nullptr, the default Latin character set is used.
     * Use ImGui's GetGlyphRanges*() functions for predefined ranges:
     * - GetGlyphRangesDefault() - Basic Latin + Latin Supplement
     * - GetGlyphRangesChineseFull() - Full Chinese character set
     * - GetGlyphRangesJapanese() - Japanese characters
     * - GetGlyphRangesKorean() - Korean characters
     *
     * 如果为 nullptr，则使用默认拉丁字符集。
     * 使用 ImGui 的 GetGlyphRanges*() 函数获取预定义范围。
     *
     * @default nullptr
     */
    const ImWchar* GlyphRanges = nullptr;
};

} // namespace Zgine
