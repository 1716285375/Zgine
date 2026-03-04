#pragma once

namespace Zgine {

// ============================================================================
// Font Type Enumeration / 字体类型枚举
// ============================================================================

/**
 * @brief Font type enumeration for different UI contexts
 * @brief 不同 UI 上下文的字体类型枚举
 *
 * Used to identify and retrieve specific font types for different purposes
 * within the engine's UI system.
 *
 * 用于在引擎 UI 系统中识别和检索不同用途的特定字体类型。
 */
enum class FontType {
    UI,           ///< Engine UI interface - Inter / 引擎 UI 界面 - Inter
    Code,         ///< Script/code editor - JetBrains Mono / 脚本/代码编辑 - JetBrains Mono
    Debug,        ///< Debug/console output - Hack / 调试/控制台 - Hack
    Default       ///< Default font (Inter + Noto Sans CJK) / 默认字体（Inter + Noto Sans CJK）
};

} // namespace Zgine
