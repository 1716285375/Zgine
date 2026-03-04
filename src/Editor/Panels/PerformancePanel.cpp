#include <Zgine/Editor/Panels/PerformancePanel.h>
#include <Zgine/Gui/Fonts/FontManager.h>
#include <Zgine/Gui/Fonts/FontTypes.h>
#include <imgui.h>

namespace Zgine
{

    PerformancePanel::PerformancePanel(const std::string &name, EditorContext& context, const RenderStats *stats)
        : EditorPanel(name, context), m_RenderStats(stats) {}

	void PerformancePanel::OnAttach() {
		// Initialize performance tracking if needed
	}

	void PerformancePanel::OnDetach() {
		// Cleanup performance tracking if needed
	}

	void PerformancePanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
		// Performance data is updated in OnGuiRender
	}

    void PerformancePanel::OnGuiRender()
    {
        BeginPanel();

        ImFont *debugFont = FontManager::GetFont(FontType::Debug);
        if (debugFont)
        {
            ImGui::PushFont(debugFont);
        }

        ImGuiIO &io = ImGui::GetIO();
        float frameTimeMs = (io.Framerate > 0.0f) ? (1000.0f / io.Framerate) : 0.0f;
        m_FpsHistory[m_HistoryIndex] = io.Framerate;
        m_FrameTimeHistory[m_HistoryIndex] = frameTimeMs;
        m_HistoryIndex++;
        if (m_HistoryIndex >= m_FpsHistory.size())
        {
            m_HistoryIndex = 0;
            m_HistoryFilled = true;
        }

        size_t sampleCount = m_HistoryFilled ? m_FpsHistory.size() : m_HistoryIndex;
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Frame time: %.3f ms", frameTimeMs);
        ImGui::Separator();
        if (sampleCount > 0)
        {
            ImGui::PlotLines("FPS", m_FpsHistory.data(), static_cast<int>(sampleCount), 0, nullptr, 0.0f, 240.0f, ImVec2(0, 60));
            ImGui::PlotLines("Frame Time (ms)", m_FrameTimeHistory.data(), static_cast<int>(sampleCount), 0, nullptr, 0.0f, 60.0f, ImVec2(0, 60));
        }

        if (m_RenderStats)
        {
            ImGui::Separator();
            ImGui::Text("Draw Calls: %u", m_RenderStats->DrawCalls);
            ImGui::Text("Triangles: %u", m_RenderStats->Triangles);
            ImGui::Text("GPU: N/A");
        }

        if (debugFont)
        {
            ImGui::PopFont();
        }

        EndPanel();
    }

}
