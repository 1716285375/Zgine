#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <imgui.h>
#include <spdlog/common.h>
#include <spdlog/sinks/sink.h>
#include <memory>
#include <string>
#include <vector>

namespace Zgine {

/**
 * @brief Console panel for displaying engine log messages
 *
 * Captures and displays log messages from the engine's logging system.
 * Supports filtering by log level, text search, and auto-scrolling.
 *
 * **Features**:
 * - Color-coded log levels (Trace, Debug, Info, Warn, Error)
 * - Text filtering
 * - Level filtering
 * - Auto-scroll to latest messages
 * - Configurable maximum entry count
 * - Clear button to remove all messages
 *
 * **Usage**:
 * Messages are automatically captured from the engine's spdlog system.
 */
class ConsolePanel : public EditorPanel {
public:
    /** Log entry structure */
    struct Entry {
        spdlog::level::level_enum Level;
        std::string Message;
    };

    ConsolePanel(const std::string& name, EditorContext& context);
    ~ConsolePanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

private:
    std::vector<Entry> m_Entries;
    std::shared_ptr<spdlog::sinks::sink> m_Sink;
    ImGuiTextFilter m_Filter;
    bool m_AutoScroll = true;
    bool m_ShowTrace = true;
    bool m_ShowDebug = true;
    bool m_ShowInfo = true;
    bool m_ShowWarn = true;
    bool m_ShowError = true;
    size_t m_MaxEntries = 2000;
};

} // namespace Zgine
