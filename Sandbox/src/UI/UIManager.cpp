#include "UIManager.h"
#include "Zgine/Log.h"
#include "Zgine/Application.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Renderer/PerformanceMonitorUI.h"
#include "../Testing/Test2DModule.h"
#include "../Testing/Test3DModule.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace Sandbox {

	UIManager::UIManager()
	{
		ZG_CORE_INFO("UIManager created!");
	}

	UIManager::~UIManager()
	{
		ZG_CORE_INFO("UIManager destroyed!");
	}

	void UIManager::OnAttach()
	{
		ZG_CORE_INFO("UIManager attached!");
		
		// Enable docking
		if (m_EnableDocking)
		{
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}
		
		// Apply default theme
		SetTheme(m_CurrentTheme);
		
		// Create default layouts
		CreateDefaultLayouts();
		
		// Register default windows
		RegisterWindow("2D Test", [this]() { Render2DTestWindow(); }, true);
		RegisterWindow("3D Test", [this]() { Render3DTestWindow(); }, true);
		RegisterWindow("Performance", [this]() { RenderPerformanceOverlay(); }, false);
		RegisterWindow("Settings", [this]() { 
			if (m_SettingsManager) {
				// Settings window is handled by SettingsManager
			}
		}, false);
	}

	void UIManager::OnUpdate(float ts)
	{
		// Update UI logic if needed
	}

	void UIManager::OnImGuiRender()
	{
		// Enable docking - TEMPORARILY DISABLED to show 2D rendering content
		if (m_EnableDocking && false) // Disabled for now
		{
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);
		}

		// Render main menu
		if (m_ShowMainMenu)
		{
			RenderMainMenuBar();
		}

		// Render registered windows
		for (auto& [name, windowInfo] : m_Windows)
		{
			if (windowInfo.isVisible)
			{
				windowInfo.renderFunction();
			}
		}

		// Render built-in windows
		if (m_ShowDemoWindow)
			RenderDemoWindow();
		if (m_ShowMetricsWindow)
			RenderMetricsWindow();
		if (m_ShowStyleEditor)
			RenderStyleEditor();
		if (m_ShowAboutWindow)
			RenderAboutWindow();
		if (m_ShowLayoutManager)
			RenderLayoutManager();

		// Render status bar
		if (m_ShowStatusBar)
		{
			RenderStatusBar();
		}

		// Render performance overlay
		if (m_ShowPerformanceOverlay)
		{
			RenderPerformanceOverlay();
		}
	}

	void UIManager::OnEvent(Zgine::Event& e)
	{
		// Handle UI events if needed
	}

	void UIManager::RegisterWindow(const std::string& name, std::function<void()> renderFunc, bool defaultVisible)
	{
		m_Windows[name] = { renderFunc, defaultVisible, nullptr, true };
		ZG_CORE_INFO("Registered UI window: {} (default visible: {})", name, defaultVisible);
	}

	void UIManager::ShowWindow(const std::string& name, bool show)
	{
		auto it = m_Windows.find(name);
		if (it != m_Windows.end())
		{
			it->second.isVisible = show;
		}
	}

	bool UIManager::IsWindowVisible(const std::string& name) const
	{
		auto it = m_Windows.find(name);
		return it != m_Windows.end() ? it->second.isVisible : false;
	}

	void UIManager::ToggleWindow(const std::string& name)
	{
		auto it = m_Windows.find(name);
		if (it != m_Windows.end())
		{
			it->second.isVisible = !it->second.isVisible;
		}
	}

	void UIManager::HideAllWindows()
	{
		for (auto& [name, windowInfo] : m_Windows)
		{
			windowInfo.isVisible = false;
		}
	}

	void UIManager::ShowAllWindows()
	{
		for (auto& [name, windowInfo] : m_Windows)
		{
			windowInfo.isVisible = true;
		}
	}

	void UIManager::AddMenuCategory(const std::string& category, std::function<void()> renderFunc)
	{
		m_MenuCategories[category] = { category, renderFunc, {} };
	}

	void UIManager::AddMenuItem(const std::string& category, const std::string& name, std::function<void()> callback)
	{
		auto it = m_MenuCategories.find(category);
		if (it != m_MenuCategories.end())
		{
			it->second.items.push_back({ name, callback });
		}
	}

	void UIManager::SetTheme(UITheme theme)
	{
		m_CurrentTheme = theme;
		
		switch (theme)
		{
			case UITheme::Dark:
				ApplyDarkTheme();
				break;
			case UITheme::Light:
				ApplyLightTheme();
				break;
			case UITheme::Classic:
				ApplyClassicTheme();
				break;
			case UITheme::Custom:
				ApplyCustomTheme();
				break;
		}
	}

	void UIManager::ApplyCustomTheme()
	{
		// Custom theme implementation
		ImGui::StyleColorsDark();
	}

	void UIManager::SaveCurrentLayout(const std::string& name)
	{
		UILayout layout;
		layout.name = name;
		
		// Save window states
		SaveWindowStates();
		
		m_Layouts[name] = layout;
		ZG_CORE_INFO("Saved layout: {}", name);
	}

	void UIManager::LoadLayout(const std::string& name)
	{
		auto it = m_Layouts.find(name);
		if (it != m_Layouts.end())
		{
			RestoreWindowStates(it->second);
			ZG_CORE_INFO("Loaded layout: {}", name);
		}
	}

	void UIManager::DeleteLayout(const std::string& name)
	{
		auto it = m_Layouts.find(name);
		if (it != m_Layouts.end())
		{
			m_Layouts.erase(it);
			ZG_CORE_INFO("Deleted layout: {}", name);
		}
	}

	std::vector<std::string> UIManager::GetAvailableLayouts() const
	{
		std::vector<std::string> layouts;
		for (const auto& [name, layout] : m_Layouts)
		{
			layouts.push_back(name);
		}
		return layouts;
	}

	void UIManager::RenderMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					// Application::Get().Close(); // Commented out for now
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				// Render registered windows
				for (auto& [name, windowInfo] : m_Windows)
				{
					ImGui::MenuItem(name.c_str(), nullptr, &windowInfo.isVisible);
				}
				ImGui::Separator();
				ImGui::MenuItem("Demo Window", nullptr, &m_ShowDemoWindow);
				ImGui::MenuItem("Metrics", nullptr, &m_ShowMetricsWindow);
				ImGui::MenuItem("Style Editor", nullptr, &m_ShowStyleEditor);
				ImGui::MenuItem("About", nullptr, &m_ShowAboutWindow);
				ImGui::MenuItem("Layout Manager", nullptr, &m_ShowLayoutManager);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Main Menu", nullptr, &m_ShowMainMenu);
				ImGui::MenuItem("Status Bar", nullptr, &m_ShowStatusBar);
				ImGui::MenuItem("Performance Overlay", nullptr, &m_ShowPerformanceOverlay);
				ImGui::Separator();
				ImGui::MenuItem("Hide All Windows", nullptr);
				ImGui::MenuItem("Show All Windows", nullptr);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Theme"))
			{
				if (ImGui::MenuItem("Dark", nullptr, m_CurrentTheme == UITheme::Dark))
					SetTheme(UITheme::Dark);
				if (ImGui::MenuItem("Light", nullptr, m_CurrentTheme == UITheme::Light))
					SetTheme(UITheme::Light);
				if (ImGui::MenuItem("Classic", nullptr, m_CurrentTheme == UITheme::Classic))
					SetTheme(UITheme::Classic);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Layout"))
			{
				for (const auto& [name, layout] : m_Layouts)
				{
					if (ImGui::MenuItem(name.c_str()))
					{
						LoadLayout(name);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save Current Layout..."))
				{
					// TODO: Show save dialog
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					m_ShowAboutWindow = true;
				}
				ImGui::EndMenu();
			}

			// Status information in the menu bar
			ImGui::Separator();
			if (m_Test2DModule)
			{
				ImGui::Text("2D: %.1f FPS | %d Objects", m_Test2DModule->GetFPS(), m_Test2DModule->GetObjectCount());
			}
			if (m_Test3DModule)
			{
				ImGui::Text("3D: %.1f FPS | %d Objects", m_Test3DModule->GetFPS(), m_Test3DModule->GetObjectCount());
			}

			ImGui::EndMainMenuBar();
		}
	}

	void UIManager::RenderStatusBar()
	{
		// Create a more flexible status bar that can be moved and resized
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
		
		if (ImGui::Begin("Status Bar", &m_ShowStatusBar, flags))
		{
			// Main status information
			ImGui::Text("Zgine Engine - Sandbox Mode");
			ImGui::SameLine();
			ImGui::Text("| FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::SameLine();
			ImGui::Text("| Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			
			// Add more detailed information if there's space
			if (ImGui::GetContentRegionAvail().x > 300)
			{
				ImGui::SameLine();
				ImGui::Text("| Renderer: OpenGL 4.6");
				
				if (m_Test2DModule)
				{
					ImGui::SameLine();
					ImGui::Text("| 2D Objects: %d", m_Test2DModule->GetObjectCount());
				}
				
				if (m_Test3DModule)
				{
					ImGui::SameLine();
					ImGui::Text("| 3D Objects: %d", m_Test3DModule->GetObjectCount());
				}
			}
			
			// Add a separator and additional controls
			ImGui::Separator();
			
			// Quick access buttons
			if (ImGui::Button("Toggle 2D Test"))
			{
				ToggleWindow("2D Test");
			}
			ImGui::SameLine();
			if (ImGui::Button("Toggle 3D Test"))
			{
				ToggleWindow("3D Test");
			}
			ImGui::SameLine();
			if (ImGui::Button("Toggle Performance"))
			{
				ToggleWindow("Performance");
			}
		}
		ImGui::End();
	}

	void UIManager::RenderPerformanceOverlay()
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(300, 200));
		
		if (ImGui::Begin("Performance Overlay", &m_ShowPerformanceOverlay))
		{
			ImGui::Text("Performance Metrics");
			ImGui::Separator();
			
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("Draw Calls: %d", ImGui::GetIO().MetricsRenderVertices);
			ImGui::Text("Vertices: %d", ImGui::GetIO().MetricsRenderVertices);
			ImGui::Text("Indices: %d", ImGui::GetIO().MetricsRenderIndices);
			
			// Performance graph
			static float values[100] = {};
			static int values_offset = 0;
			values[values_offset] = ImGui::GetIO().Framerate;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			
			ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, 
				"FPS", 0.0f, 200.0f, ImVec2(0, 80));
		}
		ImGui::End();
	}

	void UIManager::RenderLayoutManager()
	{
		if (ImGui::Begin("Layout Manager", &m_ShowLayoutManager))
		{
			ImGui::Text("Layout Management");
			ImGui::Separator();
			
			// List available layouts
			ImGui::Text("Available Layouts:");
			for (const auto& [name, layout] : m_Layouts)
			{
				ImGui::PushID(name.c_str());
				if (ImGui::Button("Load"))
				{
					LoadLayout(name);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					DeleteLayout(name);
				}
				ImGui::SameLine();
				ImGui::Text("%s", name.c_str());
				ImGui::PopID();
			}
			
			ImGui::Separator();
			
			// Save new layout
			static char layoutName[128] = "";
			ImGui::InputText("Layout Name", layoutName, sizeof(layoutName));
			if (ImGui::Button("Save Current Layout"))
			{
				if (strlen(layoutName) > 0)
				{
					SaveCurrentLayout(layoutName);
					layoutName[0] = '\0';
				}
			}
		}
		ImGui::End();
	}

	void UIManager::Render2DTestWindow()
	{
		if (m_Test2DModule)
		{
			if (ImGui::Begin("2D Rendering Test", &m_Windows["2D Test"].isVisible))
			{
				auto& config = m_Test2DModule->GetConfig();
				
				// Header 区 - 显示FPS、Objects、Draw Calls
				ImGui::Text("2D Rendering Test Module");
				ImGui::SameLine();
				ImGui::Text("| FPS: %.1f", m_Test2DModule->GetFPS());
				ImGui::SameLine();
				ImGui::Text("| Objects: %d", m_Test2DModule->GetObjectCount());
				ImGui::SameLine();
				ImGui::Text("| Draw Calls: %d", m_Test2DModule->GetObjectCount() / 4); // Approximate draw calls
				
				// Tooltips for header info
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Current rendering performance metrics");
				}
				
				ImGui::Separator();
				
				// Quick Preset Buttons - 重新添加预设按钮
				ImGui::Text("Quick Presets:");
				ImGui::SameLine();
				if (ImGui::Button("Basic"))
				{
					config.showQuads = true;
					config.showLines = true;
					config.showCircles = true;
					config.showTriangles = false;
					config.showEllipses = false;
					config.showAdvanced = false;
					config.showArcs = false;
					config.showGradients = false;
					config.animateQuads = false;
					config.animateCircles = false;
					config.animateAll = false;
					config.animationPaused = false;
					config.animationSpeed = 1.0f;
					m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				ImGui::SameLine();
				if (ImGui::Button("Advanced"))
				{
					// Only enable advanced shapes, not basic shapes
					config.showQuads = false;
					config.showLines = false;
					config.showCircles = false;
					config.showTriangles = true;
					config.showEllipses = true;
					config.showAdvanced = true;
					config.showArcs = true;
					config.showGradients = true;
					config.animateQuads = false;
					config.animateCircles = false;
					config.animateAll = true;
					config.animationPaused = false;
					config.animationSpeed = 1.5f;
					m_Test2DModule->SetActiveScene("Advanced Shapes");
					ZG_CORE_INFO("Advanced preset applied - Only advanced shapes enabled");
				}
				ImGui::SameLine();
				if (ImGui::Button("Mixed"))
				{
					// Enable both basic and advanced shapes
					config.showQuads = true;
					config.showLines = true;
					config.showCircles = true;
					config.showTriangles = true;
					config.showEllipses = true;
					config.showAdvanced = true;
					config.showArcs = true;
					config.showGradients = true;
					config.animateQuads = true;
					config.animateCircles = true;
					config.animateAll = true;
					config.animationPaused = false;
					config.animationSpeed = 1.5f;
					m_Test2DModule->SetActiveScene("Advanced Shapes");
					ZG_CORE_INFO("Mixed preset applied - Both basic and advanced shapes enabled");
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear All"))
				{
					config.showQuads = false;
					config.showLines = false;
					config.showCircles = false;
					config.showTriangles = false;
					config.showEllipses = false;
					config.showAdvanced = false;
					config.showArcs = false;
					config.showGradients = false;
					config.animateQuads = false;
					config.animateCircles = false;
					config.animateAll = false;
					config.animationPaused = false;
					config.animationSpeed = 1.0f;
					m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				
				ImGui::Separator();
				
				// Shape 控制区 - 两列布局
				ImGui::Text("Shape Controls");
				ImGui::Columns(2, "2D_ShapeControls", false);
				
				// Left column - Basic Shapes
				ImGui::Text("Basic Shapes:");
				if (ImGui::Checkbox("Quads", &config.showQuads))
				{
					// Auto-switch scene based on configuration
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				if (ImGui::Checkbox("Lines", &config.showLines))
				{
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				if (ImGui::Checkbox("Circles", &config.showCircles))
				{
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				
				ImGui::NextColumn();
				
				// Right column - Advanced Shapes
				ImGui::Text("Advanced Shapes:");
				if (ImGui::Checkbox("Triangles", &config.showTriangles))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (ImGui::Checkbox("Ellipses", &config.showEllipses))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (ImGui::Checkbox("Arcs", &config.showArcs))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (ImGui::Checkbox("Gradients", &config.showGradients))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (ImGui::Checkbox("Advanced Effects", &config.showAdvanced))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				
				ImGui::Columns(1);
				ImGui::Separator();
				
				// Animation Controls 区
				ImGui::Text("Animation Controls");
				ImGui::Checkbox("Animate Quads", &config.animateQuads);
				ImGui::SameLine();
				ImGui::Checkbox("Animate Circles", &config.animateCircles);
				ImGui::SameLine();
				ImGui::Checkbox("Animate All", &config.animateAll);
				
				// Animation speed slider
				if (config.animateQuads || config.animateCircles || config.animateAll)
				{
					ImGui::SliderFloat("Animation Speed", &config.animationSpeed, 0.1f, 3.0f);
					ImGui::SameLine();
					if (ImGui::Button(config.animationPaused ? "Resume" : "Pause"))
					{
						config.animationPaused = !config.animationPaused;
					}
				}
				
				ImGui::Separator();
				
				// Render Options 区 - 可折叠
				if (ImGui::CollapsingHeader("Render Options"))
				{
					ImGui::Checkbox("Wireframe Mode", &config.wireframeMode);
					ImGui::SameLine();
					ImGui::Checkbox("Show Bounding Boxes", &config.showBoundingBoxes);
					
					// Render mode dropdown
					const char* renderModes[] = { "Normal", "Wireframe", "Points" };
					ImGui::Combo("Render Mode", &config.renderMode, renderModes, IM_ARRAYSIZE(renderModes));
				}
				
				// Settings section
				if (ImGui::CollapsingHeader("2D Settings"))
				{
					ImGui::SliderFloat("Line Thickness", &config.lineThickness, 0.01f, 0.2f);
					ImGui::SliderFloat("Circle Radius", &config.circleRadius, 0.1f, 1.0f);
					ImGui::SliderInt("Circle Segments", &config.circleSegments, 8, 64);
					ImGui::SliderInt("Ellipse Segments", &config.ellipseSegments, 8, 32);
				}
				
				// Footer 区 - 按钮组
				ImGui::Separator();
				ImGui::Text("Actions:");
				
				if (ImGui::Button("Apply"))
				{
					// Apply current configuration - 确保场景切换正确
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
					{
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					}
					else
					{
						m_Test2DModule->SetActiveScene("Basic Shapes");
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					// Reset to default configuration
					config.showQuads = false;
					config.showLines = false;
					config.showCircles = false;
					config.showTriangles = false;
					config.showEllipses = false;
					config.showArcs = false;
					config.showGradients = false;
					config.showAdvanced = false;
					config.animateQuads = false;
					config.animateCircles = false;
					config.animateAll = false;
					config.animationPaused = false;
					config.animationSpeed = 1.0f;
					config.wireframeMode = false;
					config.showBoundingBoxes = false;
					config.renderMode = 0;
					m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				ImGui::SameLine();
				if (ImGui::Button("Export"))
				{
					// Export current configuration to file
					Export2DConfiguration(config);
				}
				
				// Performance section
				if (ImGui::CollapsingHeader("Performance Details"))
				{
					ImGui::Text("FPS: %.1f", m_Test2DModule->GetFPS());
					ImGui::Text("Objects: %d", m_Test2DModule->GetObjectCount());
					ImGui::Text("Active Scene: %s", m_Test2DModule->GetActiveScene().c_str());
					
					// Show current configuration status
					ImGui::Separator();
					ImGui::Text("Current Configuration:");
					ImGui::Text("Quads: %s | Lines: %s | Circles: %s", 
						config.showQuads ? "ON" : "OFF",
						config.showLines ? "ON" : "OFF", 
						config.showCircles ? "ON" : "OFF");
					ImGui::Text("Advanced: %s | Triangles: %s | Ellipses: %s", 
						config.showAdvanced ? "ON" : "OFF",
						config.showTriangles ? "ON" : "OFF", 
						config.showEllipses ? "ON" : "OFF");
					ImGui::Text("Animation: %s | Paused: %s | Speed: %.1f", 
						(config.animateQuads || config.animateCircles || config.animateAll) ? "ON" : "OFF",
						config.animationPaused ? "YES" : "NO",
						config.animationSpeed);
				}
			}
			ImGui::End();
		}
	}

	void UIManager::Render3DTestWindow()
	{
		if (m_Test3DModule)
		{
			if (ImGui::Begin("3D Rendering Test", &m_Windows["3D Test"].isVisible))
			{
				auto& config = m_Test3DModule->GetConfig();
				
				// Header with status
				ImGui::Text("3D Rendering Test Module");
				ImGui::SameLine();
				ImGui::Text("| FPS: %.1f | Objects: %d", m_Test3DModule->GetFPS(), m_Test3DModule->GetObjectCount());
				ImGui::Separator();
				
				// Tab-based preset selection
				if (ImGui::BeginTabBar("3D_PresetTabs"))
				{
					if (ImGui::BeginTabItem("Basic Preset"))
					{
						// Basic preset button - only set scene, don't force config values
						if (ImGui::Button("Apply Basic Preset"))
						{
							config.showCubes = true;
							config.showSpheres = true;
							config.showPlanes = false;
							config.showEnvironment = false;
							config.animateObjects = false;
							config.wireframeMode = false;
							m_Test3DModule->SetActiveScene("Basic Shapes");
						}
						
						// Basic 3D shapes controls
						ImGui::Separator();
						ImGui::Text("Basic 3D Shape Controls:");
						ImGui::Columns(2, "3D_BasicControls", false);
						ImGui::Checkbox("Show Cubes", &config.showCubes);
						ImGui::Checkbox("Show Spheres", &config.showSpheres);
						ImGui::NextColumn();
						ImGui::Checkbox("Show Planes", &config.showPlanes);
						ImGui::Checkbox("Show Environment", &config.showEnvironment);
						ImGui::Columns(1);
						
						ImGui::EndTabItem();
					}
					
					if (ImGui::BeginTabItem("Advanced Preset"))
					{
						// Advanced preset button - only set scene, don't force config values
						if (ImGui::Button("Apply Advanced Preset"))
						{
							config.showCubes = true;
							config.showSpheres = true;
							config.showPlanes = true;
							config.showEnvironment = true;
							config.animateObjects = true;
							config.wireframeMode = false;
							m_Test3DModule->SetActiveScene("Environment");
						}
						
						// Advanced 3D features controls
						ImGui::Separator();
						ImGui::Text("Advanced 3D Feature Controls:");
						ImGui::Columns(2, "3D_AdvancedControls", false);
						ImGui::Checkbox("Animate Objects", &config.animateObjects);
						ImGui::Checkbox("Wireframe Mode", &config.wireframeMode);
						ImGui::NextColumn();
						ImGui::SliderFloat("Light Intensity", &config.lightIntensity, 0.0f, 2.0f);
						if (config.animateObjects)
						{
							ImGui::SliderFloat("Animation Speed", &config.cameraSpeed, 0.1f, 10.0f);
						}
						ImGui::Columns(1);
						
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				
				// Scene selection - REMOVED to avoid duplication with Tab-style UI
				// The scene selection is now handled through the Tab-style CollapsingHeader interface
				
				// Performance section
				if (ImGui::CollapsingHeader("3D Performance Details"))
				{
					ImGui::Text("FPS: %.1f", m_Test3DModule->GetFPS());
					ImGui::Text("Objects: %d", m_Test3DModule->GetObjectCount());
					ImGui::Text("Active Scene: %s", m_Test3DModule->GetActiveScene().c_str());
					
					// Show current configuration status
					ImGui::Separator();
					ImGui::Text("Current Configuration:");
					ImGui::Text("Cubes: %s | Spheres: %s | Planes: %s", 
						config.showCubes ? "ON" : "OFF",
						config.showSpheres ? "ON" : "OFF", 
						config.showPlanes ? "ON" : "OFF");
					ImGui::Text("Environment: %s | Animation: %s | Wireframe: %s", 
						config.showEnvironment ? "ON" : "OFF",
						config.animateObjects ? "ON" : "OFF", 
						config.wireframeMode ? "ON" : "OFF");
				}
			}
			ImGui::End();
		}
	}

	void UIManager::RenderTestModuleWindows()
	{
		// This method is now deprecated and will be removed once all test logic is migrated.
		// For now, it remains to avoid compilation errors if still referenced.
		// REMOVED: Render2DTestWindow(); and Render3DTestWindow(); calls to prevent duplication
		// The windows are now properly registered and rendered through the window system
	}

	void UIManager::RenderTestWindows()
	{
		// This method is now deprecated and will be removed once all test logic is migrated.
		// The windows are now properly registered and rendered through the window system
		// REMOVED: Duplicate window rendering logic to prevent duplication
	}

	void UIManager::RenderDemoWindow()
	{
		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
	}

	void UIManager::RenderMetricsWindow()
	{
		ImGui::ShowMetricsWindow(&m_ShowMetricsWindow);
	}

	void UIManager::RenderStyleEditor()
	{
		if (ImGui::Begin("Style Editor", &m_ShowStyleEditor))
		{
			ImGui::ShowStyleEditor();
		}
		ImGui::End();
	}

	void UIManager::RenderAboutWindow()
	{
		if (ImGui::Begin("About Zgine", &m_ShowAboutWindow))
		{
			ImGui::Text("Zgine Engine");
			ImGui::Text("A modern C++ game engine");
			ImGui::Separator();
			ImGui::Text("Version: 1.0.0");
			ImGui::Text("Build: Debug");
			ImGui::Text("Platform: Windows x64");
			ImGui::Separator();
			ImGui::Text("Features:");
			ImGui::BulletText("2D/3D Batch Rendering");
			ImGui::BulletText("Advanced Rendering Pipeline");
			ImGui::BulletText("Performance Monitoring");
			ImGui::BulletText("Modular Architecture");
			ImGui::BulletText("ImGui Integration");
		}
		ImGui::End();
	}

	void UIManager::ApplyDarkTheme()
	{
		ImGui::StyleColorsDark();
	}

	void UIManager::ApplyLightTheme()
	{
		ImGui::StyleColorsLight();
	}

	void UIManager::ApplyClassicTheme()
	{
		ImGui::StyleColorsClassic();
	}

	void UIManager::SaveWindowStates()
	{
		// Implementation for saving window states
	}

	void UIManager::RestoreWindowStates(const UILayout& layout)
	{
		// Implementation for restoring window states
	}

	void UIManager::CreateDefaultLayouts()
	{
		// Create default layouts
		UILayout defaultLayout;
		defaultLayout.name = "Default";
		defaultLayout.isDefault = true;
		m_Layouts["Default"] = defaultLayout;
		
		UILayout testLayout;
		testLayout.name = "Testing";
		m_Layouts["Testing"] = testLayout;
	}

	void UIManager::Export2DConfiguration(const struct Test2DConfig& config)
	{
		// Create a timestamp for the filename
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto tm = *std::localtime(&time_t);
		
		// Create filename with timestamp
		std::stringstream filename;
		filename << "2D_Config_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".json";
		
		// Create JSON content
		std::stringstream json;
		json << "{\n";
		json << "  \"2D_Rendering_Configuration\": {\n";
		json << "    \"render_options\": {\n";
		json << "      \"showQuads\": " << (config.showQuads ? "true" : "false") << ",\n";
		json << "      \"showLines\": " << (config.showLines ? "true" : "false") << ",\n";
		json << "      \"showCircles\": " << (config.showCircles ? "true" : "false") << ",\n";
		json << "      \"showAdvanced\": " << (config.showAdvanced ? "true" : "false") << ",\n";
		json << "      \"showTriangles\": " << (config.showTriangles ? "true" : "false") << ",\n";
		json << "      \"showEllipses\": " << (config.showEllipses ? "true" : "false") << ",\n";
		json << "      \"showArcs\": " << (config.showArcs ? "true" : "false") << ",\n";
		json << "      \"showGradients\": " << (config.showGradients ? "true" : "false") << "\n";
		json << "    },\n";
		json << "    \"animation_options\": {\n";
		json << "      \"animateCircles\": " << (config.animateCircles ? "true" : "false") << ",\n";
		json << "      \"animateQuads\": " << (config.animateQuads ? "true" : "false") << ",\n";
		json << "      \"animateAll\": " << (config.animateAll ? "true" : "false") << ",\n";
		json << "      \"animationPaused\": " << (config.animationPaused ? "true" : "false") << ",\n";
		json << "      \"animationSpeed\": " << config.animationSpeed << "\n";
		json << "    },\n";
		json << "    \"settings\": {\n";
		json << "      \"lineThickness\": " << config.lineThickness << ",\n";
		json << "      \"circleRadius\": " << config.circleRadius << ",\n";
		json << "      \"circleSegments\": " << config.circleSegments << ",\n";
		json << "      \"ellipseSegments\": " << config.ellipseSegments << "\n";
		json << "    },\n";
		json << "    \"render_options\": {\n";
		json << "      \"wireframeMode\": " << (config.wireframeMode ? "true" : "false") << ",\n";
		json << "      \"showBoundingBoxes\": " << (config.showBoundingBoxes ? "true" : "false") << ",\n";
		json << "      \"renderMode\": " << config.renderMode << "\n";
		json << "    },\n";
		json << "    \"camera_settings\": {\n";
		json << "      \"cameraSpeed\": " << config.cameraSpeed << ",\n";
		json << "      \"cameraPosition\": [\n";
		json << "        " << config.cameraPosition.x << ",\n";
		json << "        " << config.cameraPosition.y << ",\n";
		json << "        " << config.cameraPosition.z << "\n";
		json << "      ]\n";
		json << "    }\n";
		json << "  },\n";
		json << "  \"export_info\": {\n";
		json << "    \"timestamp\": \"" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\",\n";
		json << "    \"engine_version\": \"Zgine 1.0.0\",\n";
		json << "    \"active_scene\": \"" << (m_Test2DModule ? m_Test2DModule->GetActiveScene() : "Unknown") << "\"\n";
		json << "  }\n";
		json << "}\n";
		
		// Write to file
		std::ofstream file(filename.str());
		if (file.is_open())
		{
			file << json.str();
			file.close();
			ZG_CORE_INFO("2D Configuration exported to: {}", filename.str());
			
			// Show success message in ImGui
			ImGui::OpenPopup("Export Success");
		}
		else
		{
			ZG_CORE_ERROR("Failed to export 2D configuration to: {}", filename.str());
			ImGui::OpenPopup("Export Failed");
		}
		
		// Show popup messages
		if (ImGui::BeginPopupModal("Export Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Configuration exported successfully!");
			ImGui::Text("File: %s", filename.str().c_str());
			ImGui::Separator();
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		
		if (ImGui::BeginPopupModal("Export Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Failed to export configuration!");
			ImGui::Text("Please check file permissions.");
			ImGui::Separator();
			if (ImGui::Button("OK"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

}