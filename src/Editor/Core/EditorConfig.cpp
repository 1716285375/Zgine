#include <Zgine/Editor/Core/EditorConfig.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Macro.h>

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
    // TODO: Implement YAML/JSON deserialization
    ZGINE_CORE_WARN("EditorConfig::LoadFromFile not implemented yet");
    ZGINE_UNUSED(filepath);
    return false;
}

bool EditorConfig::SaveToFile(const std::string& filepath) const {
    // TODO: Implement YAML/JSON serialization
    ZGINE_CORE_WARN("EditorConfig::SaveToFile not implemented yet");
    ZGINE_UNUSED(filepath);
    return false;
}

} // namespace Zgine
