#pragma once

// ============================================================================
// Rendering Feature Flags
// ============================================================================
#define ZGINE_BASIC_RENDERING_ONLY 0

#if ZGINE_BASIC_RENDERING_ONLY
    #define ZGINE_ENABLE_SHADOWS    0
    #define ZGINE_ENABLE_IBL        0
    #define ZGINE_ENABLE_POSTPROCESS 0
#else
    #define ZGINE_ENABLE_SHADOWS    1
    #define ZGINE_ENABLE_IBL        1
    #define ZGINE_ENABLE_POSTPROCESS 1
#endif

namespace Zgine {

/** @brief Runtime rendering path selection */
enum class RenderPath { Basic, Advanced };

/** @brief Shadow / quality knobs */
struct RenderQuality {
    int  ShadowMapSize     = 2048;
    int  CascadeCount      = 4;
    int  MSAASamples       = 4;
    bool EnablePostProcess = false;
};

/**
 * @brief Comprehensive rendering pipeline configuration.
 *
 * Combines compile-time feature flags (macros) with runtime toggles
 * so features can be enabled/disabled without recompilation.
 */
struct RenderConfig {
    RenderPath    Path = RenderPath::Basic;
    RenderQuality Quality;

    bool EnableIBL          = false;
    bool EnableShadows      = false;
    bool EnablePostProcess  = false;

    [[nodiscard]] static constexpr bool IsIBLAvailable()         { return ZGINE_ENABLE_IBL; }
    [[nodiscard]] static constexpr bool IsShadowsAvailable()     { return ZGINE_ENABLE_SHADOWS; }
    [[nodiscard]] static constexpr bool IsPostProcessAvailable() { return ZGINE_ENABLE_POSTPROCESS; }

    [[nodiscard]] static RenderConfig CreateBasic() {
        return { RenderPath::Basic, {}, false, false, false };
    }

    [[nodiscard]] static RenderConfig CreateAdvanced() {
        return { RenderPath::Advanced, {}, IsIBLAvailable(), IsShadowsAvailable(), IsPostProcessAvailable() };
    }
};

} // namespace Zgine
