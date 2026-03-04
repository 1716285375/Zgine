#pragma once

namespace Zgine {

// ============================================================================
// Font Constants / 字体常量
// ============================================================================

/**
 * @brief Centralized font-related constants
 * @brief 集中管理的字体相关常量
 *
 * This namespace contains all font-related constants including directory paths,
 * file names, default sizes, and font identifiers. Centralizing these constants
 * makes configuration easier and reduces magic strings throughout the codebase.
 *
 * 此命名空间包含所有字体相关常量，包括目录路径、文件名、默认大小和字体标识符。
 * 集中管理这些常量使配置更容易，并减少代码库中的魔术字符串。
 */
namespace FontConstants {

// ============================================================================
// Font Directory Paths / 字体目录路径
// ============================================================================
// Note: Paths are relative to the working directory, matching the shader
// loading pattern used throughout the engine.
// 注意：路径相对于工作目录，与引擎中使用的着色器加载模式一致。

/// Inter font directory / Inter 字体目录
inline constexpr const char* DIR_INTER = "assets/fonts/Inter";

/// JetBrains Mono font directory / JetBrains Mono 字体目录
inline constexpr const char* DIR_JETBRAINS = "assets/fonts/JetBrainsMono";

/// Hack font directory / Hack 字体目录
inline constexpr const char* DIR_HACK = "assets/fonts/Hack";

/// Noto Sans font directory / Noto Sans 字体目录
inline constexpr const char* DIR_NOTO_SANS = "assets/fonts/Noto_Sans";

// ============================================================================
// Font File Names / 字体文件名
// ============================================================================

/// Inter Regular font file / Inter Regular 字体文件
inline constexpr const char* FILE_INTER_REGULAR = "Inter-Regular.ttf";

/// JetBrains Mono Regular font file / JetBrains Mono Regular 字体文件
inline constexpr const char* FILE_JETBRAINS_REGULAR = "JetBrainsMono-Regular.ttf";

/// Hack Regular font file / Hack Regular 字体文件
inline constexpr const char* FILE_HACK_REGULAR = "Hack-Regular.ttf";

// ============================================================================
// Default Font Sizes / 默认字体大小
// ============================================================================

/// Default UI font size in pixels / 默认 UI 字体大小（像素）
inline constexpr float DEFAULT_UI_SIZE = 16.0f;

/// Default code editor font size in pixels / 默认代码编辑器字体大小（像素）
inline constexpr float DEFAULT_CODE_SIZE = 14.0f;

/// Default debug/console font size in pixels / 默认调试/控制台字体大小（像素）
inline constexpr float DEFAULT_DEBUG_SIZE = 13.0f;

// ============================================================================
// Font Names / 字体名称
// ============================================================================
// These are used as identifiers when registering and retrieving fonts.
// 这些用作注册和检索字体时的标识符。

/// Inter font name / Inter 字体名称
inline constexpr const char* NAME_INTER = "Inter";

/// Noto Sans CJK font name / Noto Sans CJK 字体名称
inline constexpr const char* NAME_NOTO_SANS_CJK = "NotoSansCJK";

/// JetBrains Mono font name / JetBrains Mono 字体名称
inline constexpr const char* NAME_JETBRAINS_MONO = "JetBrainsMono";

/// Hack font name / Hack 字体名称
inline constexpr const char* NAME_HACK = "Hack";

} // namespace FontConstants
} // namespace Zgine
