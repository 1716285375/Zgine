#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <string>

namespace Zgine {
namespace EventUtils {

/**
 * @brief Convert event category to string for logging/debug
 * @param category Event category (may contain multiple flags)
 * @return Comma-separated category names
 */
std::string CategoryToString(EditorEventCategory category);

/**
 * @brief Convert event priority to string
 * @param priority Event priority level
 * @return Priority name
 */
const char* PriorityToString(EditorEventPriority priority);

/**
 * @brief Format event for logging with all metadata
 * @param event Event to format
 * @return Formatted string: "[Priority] EventName [ID:123] (elapsed: 0.5ms)"
 */
std::string FormatEventForLog(const EditorEvent& event);

} // namespace EventUtils
} // namespace Zgine
