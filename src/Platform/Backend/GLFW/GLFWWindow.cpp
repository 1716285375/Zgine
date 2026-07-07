#include <glad/glad.h>
#include "GLFWWindow.h"
#include <Zgine/Core/Foundation/Prerequisites.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Foundation/Assert.h>
#include <Zgine/Core/Input/Input.h>

#include <Zgine/Core/Events/ApplicationEvent.h>
#include <Zgine/Core/Events/MouseEvent.h>
#include <Zgine/Core/Events/KeyEvent.h>

#include <algorithm>
#include <filesystem>
#include <stb_image.h>

namespace Zgine {

    static bool s_GLFWInitialized = false;

    namespace {
        constexpr KeyCode kPolledKeys[] = {
            KeyCode::Space,
            KeyCode::Apostrophe,
            KeyCode::Comma,
            KeyCode::Minus,
            KeyCode::Period,
            KeyCode::Slash,
            KeyCode::D0,
            KeyCode::D1,
            KeyCode::D2,
            KeyCode::D3,
            KeyCode::D4,
            KeyCode::D5,
            KeyCode::D6,
            KeyCode::D7,
            KeyCode::D8,
            KeyCode::D9,
            KeyCode::Semicolon,
            KeyCode::Equal,
            KeyCode::A,
            KeyCode::B,
            KeyCode::C,
            KeyCode::D,
            KeyCode::E,
            KeyCode::F,
            KeyCode::G,
            KeyCode::H,
            KeyCode::I,
            KeyCode::J,
            KeyCode::K,
            KeyCode::L,
            KeyCode::M,
            KeyCode::N,
            KeyCode::O,
            KeyCode::P,
            KeyCode::Q,
            KeyCode::R,
            KeyCode::S,
            KeyCode::T,
            KeyCode::U,
            KeyCode::V,
            KeyCode::W,
            KeyCode::X,
            KeyCode::Y,
            KeyCode::Z,
            KeyCode::LeftBracket,
            KeyCode::Backslash,
            KeyCode::RightBracket,
            KeyCode::GraveAccent,
            KeyCode::Escape,
            KeyCode::Enter,
            KeyCode::Tab,
            KeyCode::Backspace,
            KeyCode::Insert,
            KeyCode::Delete,
            KeyCode::Right,
            KeyCode::Left,
            KeyCode::Down,
            KeyCode::Up,
            KeyCode::PageUp,
            KeyCode::PageDown,
            KeyCode::Home,
            KeyCode::End,
            KeyCode::F1,
            KeyCode::F2,
            KeyCode::F3,
            KeyCode::F4,
            KeyCode::F5,
            KeyCode::F6,
            KeyCode::F7,
            KeyCode::F8,
            KeyCode::F9,
            KeyCode::F10,
            KeyCode::F11,
            KeyCode::F12,
            KeyCode::LeftShift,
            KeyCode::LeftControl,
            KeyCode::LeftAlt,
            KeyCode::LeftSuper,
            KeyCode::RightShift,
            KeyCode::RightControl,
            KeyCode::RightAlt,
            KeyCode::RightSuper,
        };

        const char* ToString(WindowGraphicsAPI api) {
            switch (api) {
                case WindowGraphicsAPI::None:
                    return "None";
                case WindowGraphicsAPI::OpenGL:
                    return "OpenGL";
                case WindowGraphicsAPI::DirectX12:
                    return "DirectX12";
                case WindowGraphicsAPI::Vulkan:
                    return "Vulkan";
            }

            return "Unknown";
        }

        bool LoadWindowIcon(const std::filesystem::path& path, GLFWimage& outImage) {
            if (!std::filesystem::exists(path)) return false;
            int width = 0, height = 0, channels = 0;
            unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
            if (!data) return false;
            outImage.width = width;
            outImage.height = height;
            outImage.pixels = data;
            return true;
        }
    }

    GLFWWindow::GLFWWindow(const WindowProps& props) {
        Init(props);
    }

    GLFWWindow::~GLFWWindow() {
        Shutdown();
    }

    void GLFWWindow::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.GraphicsAPI = props.GraphicsAPI;

        ZGINE_CORE_INFO("Creating window {0} ({1}, {2}) with {3} graphics API",
            props.Title, props.Width, props.Height, ToString(props.GraphicsAPI));

        if (!s_GLFWInitialized) {
            int success = glfwInit();
            ZGINE_CORE_ASSERT(success, "Could not initialize GLFW!");
            ZGINE_UNUSED(success); // Suppress unused variable warning in release builds
            s_GLFWInitialized = true;
        }

        glfwDefaultWindowHints();

        // Platform specific hints
        if (props.GraphicsAPI == WindowGraphicsAPI::OpenGL) {
#ifdef ZGINE_PLATFORM_MACOS
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on MacOS
            glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
        } else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        ZGINE_CORE_ASSERT(m_Window, "Could not create GLFW window!");

        if (props.GraphicsAPI == WindowGraphicsAPI::OpenGL) {
            glfwMakeContextCurrent(m_Window);
        }
        glfwSetWindowUserPointer(m_Window, &m_Data);

        if (props.GraphicsAPI == WindowGraphicsAPI::OpenGL) {
            int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            ZGINE_CORE_ASSERT(status, "Failed to initialize Glad!");
            ZGINE_UNUSED(status); // Suppress unused variable warning in release builds
        }

        // Icon
        {
            std::vector<std::filesystem::path> iconPaths = {
                "assets/icons/app/zgine-32.png",
                "assets/icons/app/zgine-64.png"
            };
            std::vector<GLFWimage> images;
            for (const auto& path : iconPaths) {
                GLFWimage image{};
                if (LoadWindowIcon(path, image)) images.push_back(image);
            }
            if (!images.empty()) {
                glfwSetWindowIcon(m_Window, (int)images.size(), images.data());
                for (auto& img : images) stbi_image_free(img.pixels);
            }
        }

        glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            data->Width = width;
            data->Height = height;

            WindowResizeEvent event(width, height);
            data->EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data->EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            ZGINE_UNUSED(scancode); ZGINE_UNUSED(mods); // Suppress unused parameter warnings
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, 1);
                    data->EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data->EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            ZGINE_UNUSED(mods); // Suppress unused parameter warning
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data->EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data->EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            data->ScrollDelta += static_cast<float>(yOffset);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data->EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data->EventCallback(event);
        });

        if (props.GraphicsAPI == WindowGraphicsAPI::OpenGL) {
            glfwSwapInterval(1); // VSync
        }

        UpdateInputState();
    }

    void GLFWWindow::Shutdown() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void GLFWWindow::OnUpdate() {
        glfwPollEvents();
        UpdateInputState();
        SwapBuffers();
    }

    void GLFWWindow::SwapBuffers() {
        if (m_Data.GraphicsAPI == WindowGraphicsAPI::OpenGL) {
            glfwSwapBuffers(m_Window);
        }
    }

    bool GLFWWindow::IsRunning() const {
        return !glfwWindowShouldClose(m_Window);
    }

    void GLFWWindow::SetRefreshCallback(const RefreshCallbackFn& callback) {
        m_Data.RefreshCallback = callback;
        glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            if (data->RefreshCallback) {
                data->RefreshCallback();
            }
        });
    }

    void GLFWWindow::UpdateInputState() {
        InputState state;

        for (KeyCode key : kPolledKeys) {
            const int keyCode = static_cast<int>(key);
            const int keyState = glfwGetKey(m_Window, keyCode);
            state.Keys.set(static_cast<std::size_t>(keyCode), keyState == GLFW_PRESS || keyState == GLFW_REPEAT);
        }

        const int maxButton = std::min(GLFW_MOUSE_BUTTON_LAST, static_cast<int>(InputState::kMaxButtons) - 1);
        for (int button = 0; button <= maxButton; ++button) {
            const int buttonState = glfwGetMouseButton(m_Window, button);
            state.MouseButtons.set(static_cast<std::size_t>(button), buttonState == GLFW_PRESS);
        }

        double mouseX = 0.0;
        double mouseY = 0.0;
        glfwGetCursorPos(m_Window, &mouseX, &mouseY);

        state.MousePosition = Math::Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        state.MouseDelta = state.MousePosition - Input::GetState().MousePosition;
        state.ScrollDelta = m_Data.ScrollDelta;
        m_Data.ScrollDelta = 0.0f;

        Input::UpdateState(state);
    }

    // Factory function for platform abstraction
    std::unique_ptr<Window> Window::Create(const WindowProps& props) {
        return std::make_unique<GLFWWindow>(props);
    }

}
