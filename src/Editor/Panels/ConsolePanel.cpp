#include <Zgine/Editor/Panels/ConsolePanel.h>
#include <Zgine/Core/Log/Log.h>
#include <imgui.h>
#include <spdlog/sinks/base_sink.h>
#include <algorithm>

namespace Zgine
{

    namespace
    {
        class ConsoleSink final : public spdlog::sinks::base_sink<std::mutex>
        {
        public:
            ConsoleSink(std::vector<ConsolePanel::Entry> *entries, size_t maxEntries)
                : m_Entries(entries), m_MaxEntries(maxEntries) {}

        protected:
            void sink_it_(const spdlog::details::log_msg &msg) override
            {
                if (!m_Entries)
                {
                    return;
                }
                std::string payload(msg.payload.data(), msg.payload.size());
                m_Entries->push_back({msg.level, std::move(payload)});
                if (m_MaxEntries > 0 && m_Entries->size() > m_MaxEntries)
                {
                    m_Entries->erase(m_Entries->begin(), m_Entries->begin() + (m_Entries->size() - m_MaxEntries));
                }
            }

            void flush_() override {}

        private:
            std::vector<ConsolePanel::Entry> *m_Entries = nullptr;
            size_t m_MaxEntries = 0;
        };
    }

    ConsolePanel::ConsolePanel(const std::string &name, EditorContext& context)
        : EditorPanel(name, context) {}

    void ConsolePanel::OnAttach()
    {
        if (m_Sink)
        {
            return;
        }

        m_Sink = std::make_shared<ConsoleSink>(&m_Entries, m_MaxEntries);
        if (auto logger = Log::GetCoreLogger())
        {
            logger->sinks().push_back(m_Sink);
        }
        if (auto logger = Log::GetClientLogger())
        {
            logger->sinks().push_back(m_Sink);
        }
    }

    void ConsolePanel::OnDetach()
    {
        if (!m_Sink)
        {
            return;
        }

        auto removeSink = [&](const std::shared_ptr<spdlog::logger> &logger)
        {
            if (!logger)
            {
                return;
            }
            auto &sinks = logger->sinks();
            sinks.erase(std::remove(sinks.begin(), sinks.end(), m_Sink), sinks.end());
        };
        removeSink(Log::GetCoreLogger());
        removeSink(Log::GetClientLogger());
        m_Sink.reset();
    }

    void ConsolePanel::OnUpdate(float deltaTime) {
        ZGINE_UNUSED(deltaTime);
        // Console doesn't need per-frame updates
    }

    void ConsolePanel::OnGuiRender()
    {
        BeginPanel();

        m_Filter.Draw("Filter", 180.0f);
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            m_Entries.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
        ImGui::SameLine();
        ImGui::Checkbox("Trace", &m_ShowTrace);
        ImGui::SameLine();
        ImGui::Checkbox("Debug", &m_ShowDebug);
        ImGui::SameLine();
        ImGui::Checkbox("Info", &m_ShowInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warn", &m_ShowWarn);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &m_ShowError);

        ImGui::Separator();
        ImGui::BeginChild("ConsoleScroll", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto &entry : m_Entries)
        {
            if (!m_Filter.PassFilter(entry.Message.c_str()))
            {
                continue;
            }
            if (entry.Level == spdlog::level::trace && !m_ShowTrace)
            {
                continue;
            }
            if (entry.Level == spdlog::level::debug && !m_ShowDebug)
            {
                continue;
            }
            if (entry.Level == spdlog::level::info && !m_ShowInfo)
            {
                continue;
            }
            if (entry.Level == spdlog::level::warn && !m_ShowWarn)
            {
                continue;
            }
            if ((entry.Level == spdlog::level::err || entry.Level == spdlog::level::critical) && !m_ShowError)
            {
                continue;
            }

            ImVec4 color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
            if (entry.Level == spdlog::level::warn)
            {
                color = ImVec4(0.95f, 0.85f, 0.3f, 1.0f);
            }
            else if (entry.Level == spdlog::level::err || entry.Level == spdlog::level::critical)
            {
                color = ImVec4(0.95f, 0.4f, 0.35f, 1.0f);
            }
            else if (entry.Level == spdlog::level::info)
            {
                color = ImVec4(0.6f, 0.8f, 0.95f, 1.0f);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(entry.Message.c_str());
            ImGui::PopStyleColor();
        }

        if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        EndPanel();
    }

}
