#pragma once

#include <imgui.h>
#include <Zgine/Core/Math/MathTypes.h>
#include <string>
#include <cstring>

namespace Zgine {
namespace UI {

/**
 * @brief Draw a Vec3 control with individual axis labels
 * @param label Control label
 * @param values Vector to edit
 * @param speed Drag speed
 * @param min Minimum value
 * @param max Maximum value
 * @return true if value was modified
 */
inline bool DrawVec3Control(const char* label, Math::Vector3& values,
                           float speed = 0.1f, float min = 0.0f, float max = 0.0f) {
    return ImGui::DragFloat3(label, &values.x, speed, min, max);
}

/**
 * @brief Draw a file path input field with buffer management
 * @param label Input label
 * @param path String to edit
 * @param bufferSize Maximum buffer size
 * @return true if path was modified
 */
inline bool DrawFilePathInput(const char* label, std::string& path, size_t bufferSize = 256) {
    char buffer[512];
    size_t actualSize = bufferSize < sizeof(buffer) ? bufferSize : sizeof(buffer);
    std::strncpy(buffer, path.c_str(), actualSize - 1);
    buffer[actualSize - 1] = '\0';

    if (ImGui::InputText(label, buffer, actualSize)) {
        path = std::string(buffer);
        return true;
    }
    return false;
}

/**
 * @brief Draw a float slider with range
 * @param label Slider label
 * @param value Float value to edit
 * @param min Minimum value
 * @param max Maximum value
 * @return true if value was modified
 */
inline bool DrawFloatSlider(const char* label, float& value, float min, float max) {
    return ImGui::SliderFloat(label, &value, min, max);
}

/**
 * @brief Draw a float drag control
 * @param label Drag label
 * @param value Float value to edit
 * @param speed Drag speed
 * @param min Minimum value
 * @param max Maximum value
 * @return true if value was modified
 */
inline bool DrawFloatDrag(const char* label, float& value,
                         float speed = 0.1f, float min = 0.0f, float max = 0.0f) {
    return ImGui::DragFloat(label, &value, speed, min, max);
}

/**
 * @brief Draw a color picker (RGB)
 * @param label Color picker label
 * @param color Vector3 to edit (r, g, b)
 * @return true if color was modified
 */
inline bool DrawColorEdit3(const char* label, Math::Vector3& color) {
    return ImGui::ColorEdit3(label, &color.x);
}

/**
 * @brief Draw a color picker (RGBA) with single parameter
 * @param label Color picker label
 * @param color Vector4 to edit (r, g, b, a)
 * @return true if color was modified
 */
inline bool DrawColorEdit4(const char* label, Math::Vector4& color) {
    return ImGui::ColorEdit4(label, &color.x);
}

/**
 * @brief Draw a color picker (RGBA) with flags
 * @param label Color picker label
 * @param color Vector4 to edit (r, g, b, a)
 * @param flags ImGui color edit flags
 * @return true if color was modified
 */
inline bool DrawColorEdit4(const char* label, Math::Vector4& color, ImGuiColorEditFlags flags) {
    return ImGui::ColorEdit4(label, &color.x, flags);
}

} // namespace UI
} // namespace Zgine
