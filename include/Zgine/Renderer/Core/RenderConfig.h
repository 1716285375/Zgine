#pragma once

// ============================================================================
// Rendering Feature Flags / 渲染特性标志
// ============================================================================

/**
 * @brief Master switch to disable all advanced rendering features
 * @brief 主开关：禁用所有高级渲染特性
 *
 * Set to 1 to use only basic PBR rendering (no shadows, no IBL, no post-processing)
 * Set to 0 to enable all advanced features
 *
 * 设置为 1 仅使用基本 PBR 渲染（无阴影、无IBL、无后处理）
 * 设置为 0 启用所有高级特性
 */
#define ZGINE_BASIC_RENDERING_ONLY 1

// Individual feature flags / 单独特性标志
#if ZGINE_BASIC_RENDERING_ONLY
    #define ZGINE_ENABLE_SHADOWS 0
    #define ZGINE_ENABLE_IBL 0
    #define ZGINE_ENABLE_POSTPROCESS 0
#else
    #define ZGINE_ENABLE_SHADOWS 1
    #define ZGINE_ENABLE_IBL 1
    #define ZGINE_ENABLE_POSTPROCESS 1
#endif


namespace Zgine {

/**
 * @brief Runtime rendering path selection
 * @brief 运行时渲染路径选择
 */
enum class RenderPath {
    Basic,      ///< 基本前向渲染 + PBR（无高级特性）
    Advanced    ///< 完整 PBR + IBL + Shadows + PostProcess
};

/**
 * @brief Rendering quality configuration
 * @brief 渲染质量配置
 */
struct RenderQuality {
    int ShadowMapSize = 2048;           ///< 阴影贴图分辨率
    int CascadeCount = 4;               ///< 级联阴影数量
    int MSAASamples = 4;                ///< MSAA 采样数
    bool EnablePostProcess = false;     ///< 启用后处理
};

/**
 * @brief Comprehensive rendering configuration
 * @brief 综合渲染配置
 *
 * This structure provides both compile-time (via macros) and runtime configuration
 * for rendering features. This allows for flexible feature toggling without recompilation
 * when needed.
 *
 * 此结构提供编译时（通过宏）和运行时配置。允许在需要时灵活切换特性，无需重新编译。
 */
struct RenderConfig {
    RenderPath Path = RenderPath::Basic;
    RenderQuality Quality;

    // Runtime feature flags (only effective if compile-time flags enable them)
    // 运行时特性标志（仅当编译时标志启用时有效）
    bool EnableIBL = false;
    bool EnableShadows = false;
    bool EnablePostProcess = false;

    /**
     * @brief Check if a feature is available (compile-time enabled)
     * @brief 检查特性是否可用（编译时启用）
     */
    static constexpr bool IsIBLAvailable() { return ZGINE_ENABLE_IBL; }
    static constexpr bool IsShadowsAvailable() { return ZGINE_ENABLE_SHADOWS; }
    static constexpr bool IsPostProcessAvailable() { return ZGINE_ENABLE_POSTPROCESS; }

    /**
     * @brief Create a basic rendering configuration
     * @brief 创建基本渲染配置
     */
    static RenderConfig CreateBasic() {
        RenderConfig config;
        config.Path = RenderPath::Basic;
        config.EnableIBL = false;
        config.EnableShadows = false;
        config.EnablePostProcess = false;
        return config;
    }

    /**
     * @brief Create an advanced rendering configuration
     * @brief 创建高级渲染配置
     */
    static RenderConfig CreateAdvanced() {
        RenderConfig config;
        config.Path = RenderPath::Advanced;
        config.EnableIBL = IsIBLAvailable();
        config.EnableShadows = IsShadowsAvailable();
        config.EnablePostProcess = IsPostProcessAvailable();
        return config;
    }
};

} // namespace Zgine
