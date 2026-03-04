#pragma once

#include <string>
#include <filesystem>
#include <Zgine/Gui/Themes/ImGuiTheme.h>

namespace Zgine {

/**
 * @brief Editor configuration and preferences
 *
 * Manages editor settings, preferences, and serialization.
 */
class EditorConfig {
public:
    EditorConfig();
    ~EditorConfig() = default;

    // Theme
    ImGuiThemeType GetTheme() const { return m_Theme; }
    void SetTheme(ImGuiThemeType theme) { m_Theme = theme; }

    // Grid and axes
    bool GetShowGrid() const { return m_ShowGrid; }
    void SetShowGrid(bool show) { m_ShowGrid = show; }

    bool GetShowAxes() const { return m_ShowAxes; }
    void SetShowAxes(bool show) { m_ShowAxes = show; }

    // Auto-save
    bool GetAutoSave() const { return m_AutoSave; }
    void SetAutoSave(bool enable) { m_AutoSave = enable; }

    int GetAutoSaveIntervalMinutes() const { return m_AutoSaveIntervalMinutes; }
    void SetAutoSaveIntervalMinutes(int minutes) { m_AutoSaveIntervalMinutes = minutes; }

    // Command history
    size_t GetMaxUndoHistory() const { return m_MaxUndoHistory; }
    void SetMaxUndoHistory(size_t max) { m_MaxUndoHistory = max; }

    // Serialization
    bool LoadFromFile(const std::string& filepath);
    bool SaveToFile(const std::string& filepath) const;

private:
    ImGuiThemeType m_Theme = ImGuiThemeType::Dark;
    bool m_ShowGrid = true;
    bool m_ShowAxes = true;
    bool m_AutoSave = false;
    int m_AutoSaveIntervalMinutes = 5;
    size_t m_MaxUndoHistory = 100;
};

} // namespace Zgine
