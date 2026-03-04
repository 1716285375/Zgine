#pragma once

#include <Zgine/Gui/Fonts/FontConfig.h>
#include <Zgine/Gui/Fonts/FontTypes.h>
#include <imgui.h>
#include <string>
#include <unordered_map>

namespace Zgine {

// ============================================================================
// Font Manager / 字体管理器
// ============================================================================

/**
 * @brief Central font management system for the engine
 * @brief 引擎的中央字体管理系统
 *
 * FontManager handles font loading, caching, and retrieval for the ImGui-based
 * UI system. It provides:
 * - Font loading with configuration options
 * - Font caching by name for quick retrieval
 * - Preset configurations for common use cases
 * - CJK (Chinese/Japanese/Korean) font support
 *
 * FontManager 处理基于 ImGui 的 UI 系统的字体加载、缓存和检索。它提供：
 * - 带配置选项的字体加载
 * - 按名称缓存字体以便快速检索
 * - 常见用例的预设配置
 * - CJK（中日韩）字体支持
 *
 * @note All fonts are owned by ImGui's font atlas. FontManager only maintains
 *       pointers to the loaded fonts.
 * @note 所有字体都由 ImGui 的字体图集拥有。FontManager 仅维护已加载字体的指针。
 *
 * @example
 * @code
 * // Initialize the font manager
 * FontManager::Initialize();
 *
 * // Get a specific font
 * ImFont* codeFont = FontManager::GetFont(FontType::Code);
 *
 * // Use the font
 * ImGui::PushFont(codeFont);
 * ImGui::Text("Monospaced text");
 * ImGui::PopFont();
 *
 * // Shutdown when done
 * FontManager::Shutdown();
 * @endcode
 */
class FontManager {
public:
    // ========================================================================
    // Lifecycle / 生命周期
    // ========================================================================

    /**
     * @brief Initialize the font management system
     * @brief 初始化字体管理系统
     *
     * Loads all default fonts including:
     * - UI font (Inter)
     * - CJK font for Chinese support (Noto Sans or system fallback)
     * - Code editor font (JetBrains Mono)
     * - Debug/console font (Hack)
     *
     * Must be called after ImGui context creation but before rendering.
     * 必须在 ImGui 上下文创建之后但渲染之前调用。
     *
     * @return true if initialization succeeded (at least default font loaded)
     * @return 如果初始化成功（至少加载了默认字体）则返回 true
     */
    static bool Initialize();

    /**
     * @brief Shutdown the font management system
     * @brief 关闭字体管理系统
     *
     * Clears all font references. Note that ImGui owns the actual font memory.
     * 清除所有字体引用。请注意，ImGui 拥有实际的字体内存。
     */
    static void Shutdown();

    // ========================================================================
    // Font Loading / 字体加载
    // ========================================================================

    /**
     * @brief Load a font with the specified configuration
     * @brief 使用指定配置加载字体
     *
     * @param config Font configuration / 字体配置
     * @return Pointer to loaded font, or nullptr on failure / 已加载字体的指针，失败时返回 nullptr
     *
     * @see FontConfig
     */
    static ImFont* LoadFont(const FontConfig& config);

    // ========================================================================
    // Font Retrieval / 字体获取
    // ========================================================================

    /**
     * @brief Get a font by its registered name
     * @brief 通过注册名称获取字体
     *
     * @param name Font name (e.g., "Inter", "JetBrainsMono") / 字体名称
     * @return Pointer to the font, or default font if not found / 字体指针，未找到时返回默认字体
     */
    static ImFont* GetFont(const std::string& name);

    /**
     * @brief Get a font by its type
     * @brief 通过类型获取字体
     *
     * @param type Font type / 字体类型
     * @return Pointer to the font, or default font if the requested type is unavailable
     * @return 字体指针，如果请求的类型不可用则返回默认字体
     *
     * @see FontType
     */
    static ImFont* GetFont(FontType type);

    /**
     * @brief Get the default font
     * @brief 获取默认字体
     *
     * @return Pointer to the default font / 默认字体的指针
     */
    static ImFont* GetDefaultFont() { return s_DefaultFont; }

    /**
     * @brief Set the default font
     * @brief 设置默认字体
     *
     * @param font Pointer to the font to set as default / 要设置为默认字体的指针
     */
    static void SetDefaultFont(ImFont* font);

    // ========================================================================
    // Preset Configurations / 预设配置
    // ========================================================================

    /**
     * @brief Get Inter font configuration (UI font)
     * @brief 获取 Inter 字体配置（UI 字体）
     *
     * @param sizePixels Font size in pixels / 字体大小（像素）
     * @return FontConfig for Inter font / Inter 字体的 FontConfig
     */
    static FontConfig GetInterConfig(float sizePixels = 16.0f);

    /**
     * @brief Get Noto Sans CJK font configuration (Chinese/Japanese/Korean support)
     * @brief 获取 Noto Sans CJK 字体配置（中日韩支持）
     *
     * @param sizePixels Font size in pixels / 字体大小（像素）
     * @return FontConfig for Noto Sans CJK font / Noto Sans CJK 字体的 FontConfig
     */
    static FontConfig GetNotoSansCJKConfig(float sizePixels = 16.0f);

    /**
     * @brief Get JetBrains Mono font configuration (code editor font)
     * @brief 获取 JetBrains Mono 字体配置（代码编辑器字体）
     *
     * @param sizePixels Font size in pixels / 字体大小（像素）
     * @return FontConfig for JetBrains Mono font / JetBrains Mono 字体的 FontConfig
     */
    static FontConfig GetJetBrainsMonoConfig(float sizePixels = 14.0f);

    /**
     * @brief Get Hack font configuration (debug/console font)
     * @brief 获取 Hack 字体配置（调试/控制台字体）
     *
     * @param sizePixels Font size in pixels / 字体大小（像素）
     * @return FontConfig for Hack font / Hack 字体的 FontConfig
     */
    static FontConfig GetHackConfig(float sizePixels = 13.0f);

private:
    // ========================================================================
    // Static Members / 静态成员
    // ========================================================================

    static ImFont* s_DefaultFont;                              ///< Default font pointer / 默认字体指针
    static ImFont* s_UIFont;                                   ///< UI font pointer / UI 字体指针
    static ImFont* s_CodeFont;                                 ///< Code editor font pointer / 代码编辑器字体指针
    static ImFont* s_DebugFont;                                ///< Debug/console font pointer / 调试/控制台字体指针
    static std::unordered_map<std::string, ImFont*> s_Fonts;   ///< Font registry by name / 按名称的字体注册表
    static ImFontAtlas* s_FontAtlas;                           ///< ImGui font atlas reference / ImGui 字体图集引用
};

} // namespace Zgine
