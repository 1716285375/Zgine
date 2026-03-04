#include <Zgine/Gui/Backend/Nuklear/NuklearLayer.h>

#include <Zgine/Core/Application/Application.h>
#include <Zgine/Core/Log/Log.h>

// Nuklear flags
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include <nuklear.h>
#include <glfw_opengl3/nuklear_glfw_gl3.h>

namespace Zgine {

    NuklearLayer::NuklearLayer()
        : GuiLayer("NuklearLayer")
    {
        m_NkGlfw = std::make_unique<nk_glfw>();
    }

    NuklearLayer::~NuklearLayer()
    {
    }

    void NuklearLayer::OnAttach()
    {
        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Initialize nk_glfw struct
        // API: struct nk_context* nk_glfw3_init(struct nk_glfw* glfw, GLFWwindow *win, enum nk_glfw_init_state);
        m_Context = nk_glfw3_init(m_NkGlfw.get(), window, NK_GLFW3_INSTALL_CALLBACKS);

        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(m_NkGlfw.get(), &atlas);
        // Explicitly add default font to ensure atlas is not empty
        struct nk_font *font = nk_font_atlas_add_default(atlas, 14.0f, 0);
        nk_glfw3_font_stash_end(m_NkGlfw.get());

        if (font)
            nk_style_set_font(m_Context, &font->handle);

        ZGINE_CORE_INFO("Nuklear initialized with default font");
    }

    void NuklearLayer::OnDetach()
    {
        nk_glfw3_shutdown(m_NkGlfw.get());
    }

    void NuklearLayer::OnEvent(Event& e)
    {
        if (m_BlockEvents)
        {
            // Nuklear's event handling is mostly handled by the GLFW callbacks if NK_GLFW3_INSTALL_CALLBACKS is used.
            // But we might want to manually block based on nk_window_is_any_hovered etc.
            if (nk_window_is_any_hovered(m_Context) || nk_item_is_any_active(m_Context))
            {
                if (e.IsInCategory(EventCategoryMouse) || e.IsInCategory(EventCategoryKeyboard))
                {
                    e.Handled = true;
                }
            }
        }
    }

    void NuklearLayer::Begin()
    {
        nk_glfw3_new_frame(m_NkGlfw.get());
    }

    void NuklearLayer::End()
    {
        nk_glfw3_render(m_NkGlfw.get(), NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
    }

    void NuklearLayer::EnableDocking(bool enable)
    {
        ZGINE_UNUSED(enable); // Nuklear doesn't natively support docking in the same way ImGui does
    }

    void NuklearLayer::EnableViewports(bool enable)
    {
        ZGINE_UNUSED(enable); // Viewports would require manual multi-window management in the Nuklear backend
    }

    void NuklearLayer::SetTheme(GuiTheme theme)
    {
        if (!m_Context) return;

        switch (theme)
        {
        case GuiTheme::Dark: nk_style_default(m_Context); break; // Basic default
        case GuiTheme::Light: nk_style_default(m_Context); break;
        case GuiTheme::Classic: nk_style_default(m_Context); break;
        case GuiTheme::Custom: SetCustomTheme(); break;
        }
    }

    void NuklearLayer::SetCustomTheme()
    {
        // Custom styling logic for Nuklear
    }

    void NuklearLayer::LoadFont(const GuiFontConfig& config)
    {
        ZGINE_UNUSED(config); // TODO: Nuklear font loading implementation
    }

    void NuklearLayer::SetGlobalFontScale(float scale)
    {
        ZGINE_UNUSED(scale); // TODO: Nuklear scaling logic
    }

    bool NuklearLayer::IsMouseCaptured() const
    {
        return nk_window_is_any_hovered(m_Context) != 0;
    }

    bool NuklearLayer::IsKeyboardCaptured() const
    {
        return nk_item_is_any_active(m_Context) != 0;
    }

}
