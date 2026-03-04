#pragma once

#include <Zgine/Core/Base/Prerequisites.h>
#include <glm/glm.hpp>

namespace Zgine {

    /**
     * @brief GuiAPI provides a backend-agnostic GUI API.
     * Implementation is provided by the active GuiLayer.
     *
     * This class serves as a facade for the underlying GUI backend (e.g., ImGui).
     * Use this for backend-independent GUI operations.
     */
    class GuiAPI {
    public:
        // Windows & Containers
        static bool BeginWindow(const std::string& title, bool* open = nullptr);
        static void EndWindow();

        static void BeginChild(const std::string& id, const glm::vec2& size = {0, 0});
        static void EndChild();

        // Widgets
        static void Text(const std::string& fmt, ...);
        static bool Button(const std::string& label, const glm::vec2& size = {0, 0});
        static bool Checkbox(const std::string& label, bool* value);
        static bool SliderFloat(const std::string& label, float* value, float min, float max);
        static bool InputText(const std::string& label, char* buffer, size_t size);

        // Layout
        static void SameLine();
        static void Separator();
        static void Spacing();

        // Columns / Tables (Simplifed)
        static void Columns(int count = 1, const std::string& id = "");
        static void NextColumn();
    };

} // namespace Zgine

// Include ImGui widget helpers (provides namespace UI with helper functions)
#include <Zgine/Gui/Backend/ImGui/ImGuiWidgets.h>
