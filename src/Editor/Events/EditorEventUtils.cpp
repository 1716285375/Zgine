#include <Zgine/Editor/Events/EditorEventUtils.h>
#include <sstream>

namespace Zgine {
namespace EventUtils {

std::string CategoryToString(EditorEventCategory category) {
    std::stringstream ss;
    bool first = true;

    auto addCategory = [&](EditorEventCategory cat, const char* name) {
        if (static_cast<uint32_t>(category) & static_cast<uint32_t>(cat)) {
            if (!first) ss << ", ";
            ss << name;
            first = false;
        }
    };

    addCategory(EditorEventCategory::Entity, "Entity");
    addCategory(EditorEventCategory::World, "World");
    addCategory(EditorEventCategory::Selection, "Selection");
    addCategory(EditorEventCategory::Transform, "Transform");
    addCategory(EditorEventCategory::Viewport, "Viewport");
    addCategory(EditorEventCategory::Editor, "Editor");
    addCategory(EditorEventCategory::Component, "Component");
    addCategory(EditorEventCategory::Asset, "Asset");
    addCategory(EditorEventCategory::Project, "Project");
    addCategory(EditorEventCategory::UI, "UI");
    addCategory(EditorEventCategory::Input, "Input");
    addCategory(EditorEventCategory::Render, "Render");
    addCategory(EditorEventCategory::Physics, "Physics");
    addCategory(EditorEventCategory::Audio, "Audio");
    addCategory(EditorEventCategory::Script, "Script");
    addCategory(EditorEventCategory::Network, "Network");
    addCategory(EditorEventCategory::Debug, "Debug");
    addCategory(EditorEventCategory::Undo, "Undo");
    addCategory(EditorEventCategory::Settings, "Settings");
    addCategory(EditorEventCategory::Build, "Build");

    return first ? "None" : ss.str();
}

const char* PriorityToString(EditorEventPriority priority) {
    switch (priority) {
        case EditorEventPriority::Low:      return "Low";
        case EditorEventPriority::Normal:   return "Normal";
        case EditorEventPriority::High:     return "High";
        case EditorEventPriority::Critical: return "Critical";
        default:                            return "Unknown";
    }
}

std::string FormatEventForLog(const EditorEvent& event) {
    std::stringstream ss;
    ss << "[" << PriorityToString(event.GetPriority()) << "] "
       << event.ToString()
       << " (elapsed: " << event.GetElapsedMilliseconds() << "ms)";
    return ss.str();
}

} // namespace EventUtils
} // namespace Zgine
