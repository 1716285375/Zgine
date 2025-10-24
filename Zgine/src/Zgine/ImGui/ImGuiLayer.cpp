#include "zgpch.h"
#include "ImGuiLayer.h"

#include "GLFW/glfw3.h"
#include "imgui.h"

//#define IMGUI_IMPL_API
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Zgine/Application.h"

namespace Zgine {

	/**
	 * @brief Construct a new ImGuiLayer object
	 * @details Initializes the ImGui layer with default settings
	 */
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		// ImGuiLayer constructor implementation
	}

	/**
	 * @brief Destroy the ImGuiLayer object
	 * @details Properly cleans up ImGui resources
	 */
	ImGuiLayer::~ImGuiLayer()
	{
		// ImGuiLayer destructor implementation
	}

	/**
	 * @brief Called when the layer is attached to the layer stack
	 * @details Initializes ImGui and sets up the rendering context
	 */
	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// set ImGui theme style dark
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// set platform/renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 420");
	}

	/**
	 * @brief Called when the layer is detached from the layer stack
	 * @details Shuts down ImGui and cleans up resources
	 */
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	/**
	 * @brief Called every frame to render ImGui elements
	 * @details Renders ImGui debug interface and UI elements
	 */
	void ImGuiLayer::OnImGuiRender()
	{
		static int frameCount = 0;
		frameCount++;
		// if (frameCount % 60 == 0) // Every 60 frames
		// {
		//	ZG_CORE_INFO("ImGuiLayer::OnImGuiRender called - Frame: {}", frameCount);
		// }
		
		// Don't show demo window to allow custom UI
		// static bool show = true;
		// ImGui::ShowDemoWindow(&show);
	}

	/**
	 * @brief Called when an event occurs
	 * @param e The event that occurred
	 * @details Handles ImGui events and input processing
	 */
	void ImGuiLayer::OnEvent(Event& e)
	{
		// ImGui handles its own input events through GLFW callbacks
		// No need to manually dispatch events here
	}

	/**
	 * @brief Begin ImGui frame
	 * @details Starts a new ImGui frame for rendering
	 */
	void ImGuiLayer::Begin()
	{
		//ZG_INFO("ImGuiLayer::Begin called");
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	/**
	 * @brief End ImGui frame
	 * @details Ends the current ImGui frame and renders it
	 */
	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

		// rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context =glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

} // namespace Zgine
