#pragma once

#include <imgui.h>

namespace Zgine::UI::Scoped {

/**
 * @brief RAII wrapper for ImGui::PushStyleVar / PopStyleVar
 */
class ScopedStyleVar {
public:
    ScopedStyleVar(ImGuiStyleVar idx, float val) {
        ImGui::PushStyleVar(idx, val);
    }

    ScopedStyleVar(ImGuiStyleVar idx, const ImVec2& val) {
        ImGui::PushStyleVar(idx, val);
    }

    ~ScopedStyleVar() {
        ImGui::PopStyleVar();
    }

    // Non-copyable, non-movable
    ScopedStyleVar(const ScopedStyleVar&) = delete;
    ScopedStyleVar& operator=(const ScopedStyleVar&) = delete;
    ScopedStyleVar(ScopedStyleVar&&) = delete;
    ScopedStyleVar& operator=(ScopedStyleVar&&) = delete;
};

/**
 * @brief RAII wrapper for ImGui::PushStyleColor / PopStyleColor
 */
class ScopedStyleColor {
public:
    ScopedStyleColor(ImGuiCol idx, ImU32 col) {
        ImGui::PushStyleColor(idx, col);
    }

    ScopedStyleColor(ImGuiCol idx, const ImVec4& col) {
        ImGui::PushStyleColor(idx, col);
    }

    ~ScopedStyleColor() {
        ImGui::PopStyleColor();
    }

    ScopedStyleColor(const ScopedStyleColor&) = delete;
    ScopedStyleColor& operator=(const ScopedStyleColor&) = delete;
    ScopedStyleColor(ScopedStyleColor&&) = delete;
    ScopedStyleColor& operator=(ScopedStyleColor&&) = delete;
};

/**
 * @brief RAII wrapper for ImGui::PushID / PopID
 */
class ScopedID {
public:
    explicit ScopedID(const char* str) {
        ImGui::PushID(str);
    }

    explicit ScopedID(const void* ptr) {
        ImGui::PushID(ptr);
    }

    explicit ScopedID(int id) {
        ImGui::PushID(id);
    }

    ~ScopedID() {
        ImGui::PopID();
    }

    ScopedID(const ScopedID&) = delete;
    ScopedID& operator=(const ScopedID&) = delete;
    ScopedID(ScopedID&&) = delete;
    ScopedID& operator=(ScopedID&&) = delete;
};

/**
 * @brief RAII wrapper for ImGui::BeginDisabled / EndDisabled
 */
class ScopedDisabled {
public:
    explicit ScopedDisabled(bool disabled = true) : m_Disabled(disabled) {
        if (m_Disabled) {
            ImGui::BeginDisabled();
        }
    }

    ~ScopedDisabled() {
        if (m_Disabled) {
            ImGui::EndDisabled();
        }
    }

    ScopedDisabled(const ScopedDisabled&) = delete;
    ScopedDisabled& operator=(const ScopedDisabled&) = delete;
    ScopedDisabled(ScopedDisabled&&) = delete;
    ScopedDisabled& operator=(ScopedDisabled&&) = delete;

private:
    bool m_Disabled;
};

/**
 * @brief RAII wrapper for ImGui::Indent / Unindent
 */
class ScopedIndent {
public:
    explicit ScopedIndent(float indent = 0.0f) {
        ImGui::Indent(indent);
    }

    ~ScopedIndent() {
        ImGui::Unindent();
    }

    ScopedIndent(const ScopedIndent&) = delete;
    ScopedIndent& operator=(const ScopedIndent&) = delete;
    ScopedIndent(ScopedIndent&&) = delete;
    ScopedIndent& operator=(ScopedIndent&&) = delete;
};

} // namespace Zgine::UI::Scoped
