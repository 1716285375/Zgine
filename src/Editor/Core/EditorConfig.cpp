#include <Zgine/Editor/Core/EditorConfig.h>
#include <Zgine/Core/Log/Log.h>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace Zgine {

EditorConfig::EditorConfig()
    : m_Theme(ImGuiThemeType::Dark)
    , m_ShowGrid(true)
    , m_ShowAxes(true)
    , m_AutoSave(false)
    , m_AutoSaveIntervalMinutes(5)
    , m_MaxUndoHistory(100)
{
}

bool EditorConfig::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ZGINE_CORE_WARN("EditorConfig: Could not open file: {}", filepath);
        return false;
    }

    try {
        json j = json::parse(file);

        if (j.contains("Theme")) m_Theme = static_cast<ImGuiThemeType>(j["Theme"].get<int>());
        if (j.contains("ShowGrid")) m_ShowGrid = j["ShowGrid"].get<bool>();
        if (j.contains("ShowAxes")) m_ShowAxes = j["ShowAxes"].get<bool>();
        if (j.contains("AutoSave")) m_AutoSave = j["AutoSave"].get<bool>();
        if (j.contains("AutoSaveIntervalMinutes")) m_AutoSaveIntervalMinutes = j["AutoSaveIntervalMinutes"].get<int>();
        if (j.contains("MaxUndoHistory")) m_MaxUndoHistory = j["MaxUndoHistory"].get<size_t>();

        ZGINE_CORE_INFO("EditorConfig loaded from: {}", filepath);
        return true;
    } catch (const json::exception& e) {
        ZGINE_CORE_ERROR("EditorConfig: JSON parse error: {}", e.what());
        return false;
    }
}

bool EditorConfig::SaveToFile(const std::string& filepath) const {
    json j;
    j["Theme"] = static_cast<int>(m_Theme);
    j["ShowGrid"] = m_ShowGrid;
    j["ShowAxes"] = m_ShowAxes;
    j["AutoSave"] = m_AutoSave;
    j["AutoSaveIntervalMinutes"] = m_AutoSaveIntervalMinutes;
    j["MaxUndoHistory"] = m_MaxUndoHistory;

    std::ofstream file(filepath);
    if (!file.is_open()) {
        ZGINE_CORE_ERROR("EditorConfig: Could not write to file: {}", filepath);
        return false;
    }

    file << j.dump(4);
    ZGINE_CORE_INFO("EditorConfig saved to: {}", filepath);
    return true;
}

} // namespace Zgine
