#include <Zgine/Gui/Fonts/FontManager.h>
#include <Zgine/Gui/Fonts/FontConstants.h>
#include <Zgine/Gui/Fonts/FontUtils.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Platform/IO/File.h>
#include <imgui.h>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

namespace Zgine {

// ============================================================================
// Static Member Initialization / 静态成员初始化
// ============================================================================

ImFont* FontManager::s_DefaultFont = nullptr;
ImFont* FontManager::s_UIFont = nullptr;
ImFont* FontManager::s_CodeFont = nullptr;
ImFont* FontManager::s_DebugFont = nullptr;
std::unordered_map<std::string, ImFont*> FontManager::s_Fonts;
ImFontAtlas* FontManager::s_FontAtlas = nullptr;

// ============================================================================
// Lifecycle Implementation / 生命周期实现
// ============================================================================

bool FontManager::Initialize() {
    ZGINE_CORE_INFO("Initializing Font Manager...");

    // Prevent re-initialization / 防止重复初始化
    if (s_FontAtlas != nullptr) {
        ZGINE_CORE_WARN("Font Manager already initialized, skipping...");
        return true;
    }

    // Get ImGui IO and font atlas / 获取 ImGui IO 和字体图集
    ImGuiIO& io = ImGui::GetIO();
    s_FontAtlas = io.Fonts;

    if (s_FontAtlas == nullptr) {
        ZGINE_CORE_ERROR("Failed to get ImGui font atlas!");
        return false;
    }

    // Font loading statistics / 字体加载统计
    int fontsLoaded = 0;
    int fontsFailed = 0;

    // ========================================================================
    // Load primary UI font (Inter) / 加载主要 UI 字体（Inter）
    // ========================================================================

    ZGINE_CORE_INFO("Loading primary UI font: {}", FontConstants::NAME_INTER);
    ImFont* interFont = LoadFont(GetInterConfig(FontConstants::DEFAULT_UI_SIZE));

    if (interFont) {
        s_DefaultFont = interFont;
        s_UIFont = interFont;
        io.FontDefault = interFont; // Set as ImGui default / 设置为 ImGui 默认字体
        fontsLoaded++;
        ZGINE_CORE_INFO("Successfully loaded UI font: {}", FontConstants::NAME_INTER);
    } else {
        // Fallback to ImGui's built-in default font / 回退到 ImGui 内置默认字体
        ZGINE_CORE_WARN("{} font not found, using ImGui built-in default font", FontConstants::NAME_INTER);
        s_DefaultFont = io.Fonts->AddFontDefault();
        s_UIFont = s_DefaultFont;
        io.FontDefault = s_DefaultFont;
        fontsFailed++;
    }

    // ========================================================================
    // Merge CJK font for Chinese/Japanese/Korean support
    // 合并 CJK 字体以支持中日韩文字
    // ========================================================================

    if (s_DefaultFont) {
        ZGINE_CORE_INFO("Loading CJK font for Chinese/Japanese/Korean support...");
        FontConfig cjkConfig = GetNotoSansCJKConfig(FontConstants::DEFAULT_UI_SIZE);

        if (!cjkConfig.Path.empty()) {
            // Set glyph ranges to Chinese Full / 设置字形范围为完整中文
            cjkConfig.GlyphRanges = io.Fonts->GetGlyphRangesChineseFull();

            ImFont* cjkFont = LoadFont(cjkConfig);
            if (cjkFont) {
                fontsLoaded++;
                ZGINE_CORE_INFO("Successfully merged CJK font support");
            } else {
                fontsFailed++;
                ZGINE_CORE_WARN("Failed to merge CJK font. Chinese UI text may not render correctly.");
            }
        } else {
            fontsFailed++;
            ZGINE_CORE_WARN("CJK font not found. Chinese UI text may not render correctly.");
        }
    } else {
        ZGINE_CORE_ERROR("Cannot merge CJK font: no base font available");
    }

    // ========================================================================
    // Pre-load additional fonts (failures are non-critical)
    // 预加载其他字体（失败不影响程序运行）
    // ========================================================================

    // Load code editor font (JetBrains Mono) / 加载代码编辑器字体（JetBrains Mono）
    ZGINE_CORE_INFO("Loading code editor font: {}", FontConstants::NAME_JETBRAINS_MONO);
    s_CodeFont = LoadFont(GetJetBrainsMonoConfig(FontConstants::DEFAULT_CODE_SIZE));
    if (s_CodeFont) {
        fontsLoaded++;
    } else {
        fontsFailed++;
        ZGINE_CORE_WARN("Code editor font ({}) not loaded, will fallback to default",
                        FontConstants::NAME_JETBRAINS_MONO);
    }

    // Load debug/console font (Hack) / 加载调试/控制台字体（Hack）
    ZGINE_CORE_INFO("Loading debug/console font: {}", FontConstants::NAME_HACK);
    s_DebugFont = LoadFont(GetHackConfig(FontConstants::DEFAULT_DEBUG_SIZE));
    if (s_DebugFont) {
        fontsLoaded++;
    } else {
        fontsFailed++;
        ZGINE_CORE_WARN("Debug/console font ({}) not loaded, will fallback to default",
                        FontConstants::NAME_HACK);
    }

    // ========================================================================
    // Initialization summary / 初始化总结
    // ========================================================================

    ZGINE_CORE_INFO("Font Manager Initialization Complete:");
    ZGINE_CORE_INFO("  - Fonts loaded: {}", fontsLoaded);
    ZGINE_CORE_INFO("  - Fonts failed: {}", fontsFailed);
    ZGINE_CORE_INFO("  - Total fonts in atlas: {}", s_Fonts.size());

    return true;
}

void FontManager::Shutdown() {
    ZGINE_CORE_INFO("Shutting down Font Manager...");

    // Clear font registry / 清除字体注册表
    s_Fonts.clear();

    // Reset font pointers (Note: ImGui owns the font memory)
    // 重置字体指针（注意：ImGui 拥有字体内存）
    s_DefaultFont = nullptr;
    s_UIFont = nullptr;
    s_CodeFont = nullptr;
    s_DebugFont = nullptr;
    s_FontAtlas = nullptr;

    ZGINE_CORE_INFO("Font Manager shutdown complete");
}

// ============================================================================
// Font Loading Implementation / 字体加载实现
// ============================================================================

ImFont* FontManager::LoadFont(const FontConfig& config) {
    // Validate font atlas / 验证字体图集
    if (!s_FontAtlas) {
        ZGINE_CORE_ERROR("Cannot load font '{}': FontManager not initialized!", config.Name);
        return nullptr;
    }

    // Validate font path is not empty / 验证字体路径不为空
    if (config.Path.empty()) {
        ZGINE_CORE_WARN("Cannot load font '{}': Path is empty", config.Name);
        return nullptr;
    }

    // Check font file existence using Platform/IO/File since ImGui uses OS paths
    // 使用 Platform/IO/File 检查字体文件是否存在，因为 ImGui 使用操作系统路径
    if (!File::Exists(config.Path)) {
        ZGINE_CORE_WARN("Cannot load font '{}': File does not exist at path: {}",
                        config.Name, config.Path);
        return nullptr;
    }

    // Validate font size / 验证字体大小
    if (config.SizePixels <= 0.0f) {
        ZGINE_CORE_ERROR("Cannot load font '{}': Invalid font size: {}",
                         config.Name, config.SizePixels);
        return nullptr;
    }

    // Configure ImGui font loading / 配置 ImGui 字体加载
    ImFontConfig fontConfig;
    fontConfig.MergeMode = config.MergeMode;
    fontConfig.OversampleH = 2; // Horizontal oversampling for better quality / 水平过采样以提高质量
    fontConfig.OversampleV = 1; // Vertical oversampling / 垂直过采样
    fontConfig.PixelSnapH = true; // Align to pixel grid / 对齐到像素网格

    // Copy font name for debugging / 复制字体名称用于调试
    std::strncpy(fontConfig.Name, config.Name.c_str(), sizeof(fontConfig.Name) - 1);
    fontConfig.Name[sizeof(fontConfig.Name) - 1] = '\0'; // Ensure null termination / 确保空终止

    // Load font from file / 从文件加载字体
    ZGINE_CORE_TRACE("Loading font '{}' from '{}' at {:.1f}px (MergeMode: {})",
                     config.Name, config.Path, config.SizePixels, config.MergeMode);

    ImFont* font = s_FontAtlas->AddFontFromFileTTF(
        config.Path.c_str(),
        config.SizePixels,
        &fontConfig,
        config.GlyphRanges
    );

    // Validate loaded font / 验证加载的字体
    if (font) {
        // Add to font registry / 添加到字体注册表
        s_Fonts[config.Name] = font;
        ZGINE_CORE_INFO("Successfully loaded font: '{}' from '{}'", config.Name, config.Path);
    } else {
        ZGINE_CORE_ERROR("Failed to load font: '{}' from '{}' (ImGui returned nullptr)",
                         config.Name, config.Path);
    }

    return font;
}

// ============================================================================
// Font Retrieval Implementation / 字体获取实现
// ============================================================================

ImFont* FontManager::GetFont(const std::string& name) {
    // Validate input / 验证输入
    if (name.empty()) {
        ZGINE_CORE_WARN("GetFont called with empty name, returning default font");
        return s_DefaultFont;
    }

    // Search in font registry / 在字体注册表中搜索
    auto it = s_Fonts.find(name);
    if (it != s_Fonts.end()) {
        return it->second;
    }

    // Font not found, return default / 未找到字体，返回默认字体
    ZGINE_CORE_TRACE("Font '{}' not found in registry, returning default font", name);
    return s_DefaultFont;
}

ImFont* FontManager::GetFont(FontType type) {
    switch (type) {
        case FontType::UI:
            return s_UIFont ? s_UIFont : s_DefaultFont;

        case FontType::Code:
            return s_CodeFont ? s_CodeFont : s_DefaultFont;

        case FontType::Debug:
            return s_DebugFont ? s_DebugFont : s_DefaultFont;

        case FontType::Default:
        default:
            return s_DefaultFont;
    }
}

void FontManager::SetDefaultFont(ImFont* font) {
    if (!font) {
        ZGINE_CORE_WARN("SetDefaultFont called with nullptr, ignoring");
        return;
    }

    s_DefaultFont = font;

    // Update ImGui's default font / 更新 ImGui 的默认字体
    ImGuiIO& io = ImGui::GetIO();
    io.FontDefault = font;

    ZGINE_CORE_INFO("Default font changed");
}

// ============================================================================
// Preset Font Configurations / 预设字体配置
// ============================================================================

FontConfig FontManager::GetInterConfig(float sizePixels) {
    FontConfig config;
    config.Name = FontConstants::NAME_INTER;
    config.Path = FontUtils::BuildFontPath(FontConstants::DIR_INTER,
                                            FontConstants::FILE_INTER_REGULAR);
    config.SizePixels = sizePixels;
    config.MergeMode = false;
    config.GlyphRanges = nullptr; // Use default Latin character set / 使用默认拉丁字符集
    return config;
}

FontConfig FontManager::GetNotoSansCJKConfig(float sizePixels) {
    FontConfig config;
    config.Name = FontConstants::NAME_NOTO_SANS_CJK;
    config.Path = FontUtils::FindCjkFontPath(); // Use smart CJK font search / 使用智能 CJK 字体搜索
    config.SizePixels = sizePixels;
    config.MergeMode = true; // Merge into default font / 合并到默认字体
    config.GlyphRanges = nullptr; // Will be set during initialization / 将在初始化期间设置
    return config;
}

FontConfig FontManager::GetJetBrainsMonoConfig(float sizePixels) {
    FontConfig config;
    config.Name = FontConstants::NAME_JETBRAINS_MONO;
    config.Path = FontUtils::BuildFontPath(FontConstants::DIR_JETBRAINS,
                                            FontConstants::FILE_JETBRAINS_REGULAR);
    config.SizePixels = sizePixels;
    config.MergeMode = false;
    config.GlyphRanges = nullptr; // Use default character set / 使用默认字符集
    return config;
}

FontConfig FontManager::GetHackConfig(float sizePixels) {
    FontConfig config;
    config.Name = FontConstants::NAME_HACK;
    config.Path = FontUtils::BuildFontPath(FontConstants::DIR_HACK,
                                            FontConstants::FILE_HACK_REGULAR);
    config.SizePixels = sizePixels;
    config.MergeMode = false;
    config.GlyphRanges = nullptr; // Use default character set / 使用默认字符集
    return config;
}

} // namespace Zgine
