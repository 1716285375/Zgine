#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Renderer/Pipeline/RenderStats.h>
#include <array>

namespace Zgine {

/**
 * @brief Performance monitoring panel
 *
 * Displays real-time performance metrics including FPS, frame time,
 * draw calls, vertices, and other rendering statistics.
 *
 * **Features**:
 * - FPS counter with history graph
 * - Frame time tracking (ms)
 * - Draw call statistics
 * - Vertex/triangle counts
 * - Texture/shader statistics
 * - Interactive performance graphs
 *
 * **Metrics Displayed**:
 * - Frames Per Second (FPS)
 * - Frame Time (milliseconds)
 * - Draw Calls
 * - Vertices Rendered
 * - Active Textures and Shaders
 */
class PerformancePanel : public EditorPanel {
public:
    PerformancePanel(const std::string& name, EditorContext& context,
                     const RenderStats* stats = nullptr);
    ~PerformancePanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the render statistics to display */
    void SetRenderStats(const RenderStats* stats) { m_RenderStats = stats; }

private:
    const RenderStats* m_RenderStats = nullptr;
    std::array<float, 120> m_FpsHistory = {};
    std::array<float, 120> m_FrameTimeHistory = {};
    size_t m_HistoryIndex = 0;
    bool m_HistoryFilled = false;
};

} // namespace Zgine
