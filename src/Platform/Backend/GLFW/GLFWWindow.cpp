#include <glad/glad.h>
#include "GLFWWindow.h"
#include "GLFWInput.h"
#include <Zgine/Core/Base/Prerequisites.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Assert.h>

#include <Zgine/Core/Events/ApplicationEvent.h>
#include <Zgine/Core/Events/MouseEvent.h>
#include <Zgine/Core/Events/KeyEvent.h>

#include <filesystem>
#include <stb_image.h>

namespace Zgine {

    static bool s_GLFWInitialized = false;

    namespace {
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

        ZGINE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized) {
            int success = glfwInit();
            ZGINE_CORE_ASSERT(success, "Could not initialize GLFW!");
            ZGINE_UNUSED(success); // Suppress unused variable warning in release builds
            s_GLFWInitialized = true;
        }

        // Platform specific hints
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

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);

        GLFWInput::SetWindow(m_Window);

        // GLAD
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ZGINE_CORE_ASSERT(status, "Failed to initialize Glad!");
        ZGINE_UNUSED(status); // Suppress unused variable warning in release builds

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
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data->EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            auto data = (WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data->EventCallback(event);
        });

        glfwSwapInterval(1); // VSync
    }

    void GLFWWindow::Shutdown() {
        glfwDestroyWindow(m_Window);
    }

    void GLFWWindow::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
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

    // Factory function for platform abstraction
    std::unique_ptr<Window> Window::Create(const WindowProps& props) {
        return std::make_unique<GLFWWindow>(props);
    }

}
