#include <Zgine/Gui/Backend/ImGui/ImGuiLayer.h>


#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <Zgine/Core/Application/Application.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Renderer/RHI/RendererAPI.h>

// 暂时硬编码，后续应从 Window 或 Renderer 获取
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Zgine {

    namespace {
        class NullGuiLayer final : public GuiLayer {
        public:
            NullGuiLayer()
                : GuiLayer("NullGuiLayer")
            {
            }

            void Begin() override {}
            void End() override {}
            void BlockEvents(bool block) override { m_BlockEvents = block; }
            void EnableDocking(bool enable) override { m_DockingEnabled = enable; }
            void EnableViewports(bool enable) override { m_ViewportsEnabled = enable; }
            void SetTheme(GuiTheme theme) override { m_Theme = theme; }
            void SetCustomTheme() override { m_Theme = GuiTheme::Custom; }
            void LoadFont(const GuiFontConfig& config) override { ZGINE_UNUSED(config); }
            void SetGlobalFontScale(float scale) override { m_GlobalFontScale = scale; }
            bool IsMouseCaptured() const override { return false; }
            bool IsKeyboardCaptured() const override { return false; }

        private:
            bool m_BlockEvents = true;
            bool m_DockingEnabled = false;
            bool m_ViewportsEnabled = false;
            GuiTheme m_Theme = GuiTheme::Dark;
            float m_GlobalFontScale = 1.0f;
        };
    }

    ImGuiLayer::ImGuiLayer()
        : GuiLayer("ImGuiLayer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); ZGINE_UNUSED(io);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnEvent(Event& e)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::EnableDocking(bool enable)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (enable) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        else io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }

    void ImGuiLayer::EnableViewports(bool enable)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (enable) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        else io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }

    void ImGuiLayer::SetTheme(GuiTheme theme)
    {
        switch (theme)
        {
        case GuiTheme::Dark: ImGui::StyleColorsDark(); break;
        case GuiTheme::Light: ImGui::StyleColorsLight(); break;
        case GuiTheme::Classic: ImGui::StyleColorsClassic(); break;
        case GuiTheme::Custom: SetCustomTheme(); break;
        }
    }

    void ImGuiLayer::SetCustomTheme()
    {
        // Placeholder for user custom theme logic
    }

    void ImGuiLayer::LoadFont(const GuiFontConfig& config)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(config.FilePath.c_str(), config.Size);
    }

    void ImGuiLayer::SetGlobalFontScale(float scale)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = scale;
    }

    bool ImGuiLayer::IsMouseCaptured() const
    {
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiLayer::IsKeyboardCaptured() const
    {
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    Ref<GuiLayer> GuiLayer::Create()
    {
        if (RendererAPI::GetAPI() != RendererAPI::API::OpenGL) {
            ZGINE_CORE_WARN("GuiLayer is using NullGuiLayer because '{}' does not have an ImGui backend yet.",
                RendererAPI::ToString(RendererAPI::GetAPI()));
            return CreateRef<NullGuiLayer>();
        }

        return CreateRef<ImGuiLayer>();
    }

}
