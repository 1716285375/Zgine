#include "sandbox_pch.h"
#include "UIManager.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Application.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Renderer/PerformanceMonitorUI.h"
#include "../Testing/Test2DModule.h"
#include "../Testing/Test3DModule.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <imgui.h> // For ImVec2, ImGuiID, etc.

namespace Sandbox {

	UIManager::UIManager()
	{
		ZG_CORE_INFO("UIManager created!");
	}

	UIManager::~UIManager()
	{
		ZG_CORE_INFO("UIManager destroyed!");
		
		// Clear windows and menu categories to prevent dangling pointer issues
		// The lambda expressions in m_Windows and m_MenuCategories hold [this] captures
		// which become invalid when this object is destroyed
		m_Windows.clear();
		m_MenuCategories.clear();
	}

	void UIManager::OnAttach()
	{
		ZG_CORE_INFO("UIManager attached!");
		
		// Enable docking
		if (m_EnableDocking)
		{
			Zgine::IG::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
		// DISABLED: Do not create DockSpace that covers the entire window
		// DockSpace with ImVec2(0, 0) covers the entire viewport and blocks rendering
		// This was causing the main window to show no rendering content
		
		// The DockSpace should only be created when we have a specific rendering viewport area
		// For now, just render the UI windows without DockSpace

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
		Zgine::IG::StyleColorsDark();
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
		if (Zgine::IG::BeginMainMenuBar())
		{
			if (Zgine::IG::BeginMenu("File"))
			{
				if (Zgine::IG::MenuItem("Exit"))
				{
					// Application::Get().Close(); // Commented out for now
				}
				Zgine::IG::EndMenu();
			}

			if (Zgine::IG::BeginMenu("Windows"))
			{
				// Render registered windows
				for (auto& [name, windowInfo] : m_Windows)
				{
					Zgine::IG::MenuItem(name.c_str(), nullptr, &windowInfo.isVisible);
				}
				Zgine::IG::Separator();
				Zgine::IG::MenuItem("Demo Window", nullptr, &m_ShowDemoWindow);
				Zgine::IG::MenuItem("Metrics", nullptr, &m_ShowMetricsWindow);
				Zgine::IG::MenuItem("Style Editor", nullptr, &m_ShowStyleEditor);
				Zgine::IG::MenuItem("About", nullptr, &m_ShowAboutWindow);
				Zgine::IG::MenuItem("Layout Manager", nullptr, &m_ShowLayoutManager);
				Zgine::IG::EndMenu();
			}

			if (Zgine::IG::BeginMenu("View"))
			{
				Zgine::IG::MenuItem("Main Menu", nullptr, &m_ShowMainMenu);
				Zgine::IG::MenuItem("Status Bar", nullptr, &m_ShowStatusBar);
				Zgine::IG::MenuItem("Performance Overlay", nullptr, &m_ShowPerformanceOverlay);
				Zgine::IG::Separator();
				Zgine::IG::MenuItem("Hide All Windows", nullptr);
				Zgine::IG::MenuItem("Show All Windows", nullptr);
				Zgine::IG::EndMenu();
			}

			if (Zgine::IG::BeginMenu("Theme"))
			{
				if (Zgine::IG::MenuItem("Dark", nullptr, m_CurrentTheme == UITheme::Dark))
					SetTheme(UITheme::Dark);
				if (Zgine::IG::MenuItem("Light", nullptr, m_CurrentTheme == UITheme::Light))
					SetTheme(UITheme::Light);
				if (Zgine::IG::MenuItem("Classic", nullptr, m_CurrentTheme == UITheme::Classic))
					SetTheme(UITheme::Classic);
				Zgine::IG::EndMenu();
			}

			if (Zgine::IG::BeginMenu("Layout"))
			{
				for (const auto& [name, layout] : m_Layouts)
				{
					if (Zgine::IG::MenuItem(name.c_str()))
					{
						LoadLayout(name);
					}
				}
				Zgine::IG::Separator();
				if (Zgine::IG::MenuItem("Save Current Layout..."))
				{
					// TODO: Show save dialog
				}
				Zgine::IG::EndMenu();
			}

			if (Zgine::IG::BeginMenu("Help"))
			{
				if (Zgine::IG::MenuItem("About"))
				{
					m_ShowAboutWindow = true;
				}
				Zgine::IG::EndMenu();
			}

			// Status information in the menu bar
			Zgine::IG::Separator();
			if (m_Test2DModule)
			{
				Zgine::IG::Text("2D: %.1f FPS | %d Objects", m_Test2DModule->GetFPS(), m_Test2DModule->GetObjectCount());
			}
			if (m_Test3DModule)
			{
				Zgine::IG::Text("3D: %.1f FPS | %d Objects", m_Test3DModule->GetFPS(), m_Test3DModule->GetObjectCount());
			}

			Zgine::IG::EndMainMenuBar();
		}
	}

	void UIManager::RenderStatusBar()
	{
		// Create a more flexible status bar that can be moved and resized
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
		
		if (Zgine::IG::Begin("Status Bar", &m_ShowStatusBar, flags))
		{
			// Main status information
			Zgine::IG::Text("Zgine Engine - Sandbox Mode");
			Zgine::IG::SameLine();
			Zgine::IG::Text("| FPS: %.1f", Zgine::IG::GetIO().Framerate);
			Zgine::IG::SameLine();
			Zgine::IG::Text("| Frame Time: %.3f ms", 1000.0f / Zgine::IG::GetIO().Framerate);
			
			// Add more detailed information if there's space
			if (Zgine::IG::GetContentRegionAvail().x > 300)
			{
				Zgine::IG::SameLine();
				Zgine::IG::Text("| Renderer: OpenGL 4.6");
				
				if (m_Test2DModule)
				{
					Zgine::IG::SameLine();
					Zgine::IG::Text("| 2D Objects: %d", m_Test2DModule->GetObjectCount());
				}
				
				if (m_Test3DModule)
				{
					Zgine::IG::SameLine();
					Zgine::IG::Text("| 3D Objects: %d", m_Test3DModule->GetObjectCount());
				}
			}
			
			// Add a separator and additional controls
			Zgine::IG::Separator();
			
			// Quick access buttons
			if (Zgine::IG::Button("Toggle 2D Test"))
			{
				ToggleWindow("2D Test");
			}
			Zgine::IG::SameLine();
			if (Zgine::IG::Button("Toggle 3D Test"))
			{
				ToggleWindow("3D Test");
			}
			Zgine::IG::SameLine();
			if (Zgine::IG::Button("Toggle Performance"))
			{
				ToggleWindow("Performance");
			}
		}
		Zgine::IG::End();
	}

	void UIManager::RenderPerformanceOverlay()
	{
		Zgine::IG::SetNextWindowPos(glm::vec2(10, 10));
		Zgine::IG::SetNextWindowSize(glm::vec2(300, 200));
		
		if (Zgine::IG::Begin("Performance Overlay", &m_ShowPerformanceOverlay))
		{
			Zgine::IG::Text("Performance Metrics");
			Zgine::IG::Separator();
			
			Zgine::IG::Text("FPS: %.1f", Zgine::IG::GetIO().Framerate);
			Zgine::IG::Text("Frame Time: %.3f ms", 1000.0f / Zgine::IG::GetIO().Framerate);
			Zgine::IG::Text("Draw Calls: %d", Zgine::IG::GetIO().MetricsRenderVertices);
			Zgine::IG::Text("Vertices: %d", Zgine::IG::GetIO().MetricsRenderVertices);
			Zgine::IG::Text("Indices: %d", Zgine::IG::GetIO().MetricsRenderIndices);
			
			// Performance graph
			static float values[100] = {};
			static int values_offset = 0;
			values[values_offset] = Zgine::IG::GetIO().Framerate;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			
			Zgine::IG::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, 
				"FPS", 0.0f, 200.0f, ImVec2(0.0f, 80.0f));
		}
		Zgine::IG::End();
	}

	void UIManager::RenderLayoutManager()
	{
		if (Zgine::IG::Begin("Layout Manager", &m_ShowLayoutManager))
		{
			Zgine::IG::Text("Layout Management");
			Zgine::IG::Separator();
			
			// List available layouts
			Zgine::IG::Text("Available Layouts:");
			for (const auto& [name, layout] : m_Layouts)
			{
				Zgine::IG::PushID(name.c_str());
				if (Zgine::IG::Button("Load"))
				{
					LoadLayout(name);
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Delete"))
				{
					DeleteLayout(name);
				}
				Zgine::IG::SameLine();
				Zgine::IG::Text("%s", name.c_str());
				Zgine::IG::PopID();
			}
			
			Zgine::IG::Separator();
			
			// Save new layout
			static char layoutName[128] = "";
			Zgine::IG::InputText("Layout Name", layoutName, sizeof(layoutName));
			if (Zgine::IG::Button("Save Current Layout"))
			{
				if (strlen(layoutName) > 0)
				{
					SaveCurrentLayout(layoutName);
					layoutName[0] = '\0';
				}
			}
		}
		Zgine::IG::End();
	}

	void UIManager::Render2DTestWindow()
	{
		if (m_Test2DModule)
		{
			if (Zgine::IG::Begin("2D Rendering Test", &m_Windows["2D Test"].isVisible))
			{
				auto& config = m_Test2DModule->GetConfig();
				
				// Header 区 - 显示FPS、Objects、Draw Calls
				Zgine::IG::Text("2D Rendering Test Module");
				Zgine::IG::SameLine();
				Zgine::IG::Text("| FPS: %.1f", m_Test2DModule->GetFPS());
				Zgine::IG::SameLine();
				Zgine::IG::Text("| Objects: %d", m_Test2DModule->GetObjectCount());
				Zgine::IG::SameLine();
				Zgine::IG::Text("| Draw Calls: %d", m_Test2DModule->GetObjectCount() / 4); // Approximate draw calls
				
				// Tooltips for header info
				if (Zgine::IG::IsItemHovered())
				{
					Zgine::IG::SetTooltip("Current rendering performance metrics");
				}
				
				Zgine::IG::Separator();
				
				// Quick Preset Buttons - 重新添加预设按钮
				Zgine::IG::Text("Quick Presets:");
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Basic"))
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
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Advanced"))
				{
					ZG_CORE_INFO("Advanced button clicked! Setting config...");
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
					ZG_CORE_INFO("Advanced preset applied - showTriangles: {}, showEllipses: {}, showAdvanced: {}", 
						config.showTriangles, config.showEllipses, config.showAdvanced);
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Mixed"))
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
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Clear All"))
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
				
				Zgine::IG::Separator();
				
				// Shape 控制区 - 两列布局
				Zgine::IG::Text("Shape Controls");
				Zgine::IG::Columns(2, "2D_ShapeControls", false);
				
				// Left column - Basic Shapes
				Zgine::IG::Text("Basic Shapes:");
				if (Zgine::IG::Checkbox("Quads", &config.showQuads))
				{
					// Auto-switch scene based on configuration
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				if (Zgine::IG::Checkbox("Lines", &config.showLines))
				{
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				if (Zgine::IG::Checkbox("Circles", &config.showCircles))
				{
					if (config.showAdvanced || config.showTriangles || config.showEllipses || config.showArcs || config.showGradients)
						m_Test2DModule->SetActiveScene("Advanced Shapes");
					else
						m_Test2DModule->SetActiveScene("Basic Shapes");
				}
				
				Zgine::IG::NextColumn();
				
				// Right column - Advanced Shapes
				Zgine::IG::Text("Advanced Shapes:");
				if (Zgine::IG::Checkbox("Triangles", &config.showTriangles))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (Zgine::IG::Checkbox("Ellipses", &config.showEllipses))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (Zgine::IG::Checkbox("Arcs", &config.showArcs))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (Zgine::IG::Checkbox("Gradients", &config.showGradients))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				if (Zgine::IG::Checkbox("Advanced Effects", &config.showAdvanced))
				{
					m_Test2DModule->SetActiveScene("Advanced Shapes");
				}
				
				Zgine::IG::Columns(1);
				Zgine::IG::Separator();
				
				// Animation Controls 区
				Zgine::IG::Text("Animation Controls");
				Zgine::IG::Checkbox("Animate Quads", &config.animateQuads);
				Zgine::IG::SameLine();
				Zgine::IG::Checkbox("Animate Circles", &config.animateCircles);
				Zgine::IG::SameLine();
				Zgine::IG::Checkbox("Animate All", &config.animateAll);
				
				// Animation speed slider
				if (config.animateQuads || config.animateCircles || config.animateAll)
				{
					Zgine::IG::SliderFloat("Animation Speed", &config.animationSpeed, 0.1f, 3.0f);
					Zgine::IG::SameLine();
					if (Zgine::IG::Button(config.animationPaused ? "Resume" : "Pause"))
					{
						config.animationPaused = !config.animationPaused;
					}
				}
				
				Zgine::IG::Separator();
				
				// Render Options 区 - 可折叠
				if (Zgine::IG::CollapsingHeader("Render Options"))
				{
					Zgine::IG::Checkbox("Wireframe Mode", &config.wireframeMode);
					Zgine::IG::SameLine();
					Zgine::IG::Checkbox("Show Bounding Boxes", &config.showBoundingBoxes);
					
					// Render mode dropdown
					const char* renderModes[] = { "Normal", "Wireframe", "Points" };
					Zgine::IG::Combo("Render Mode", &config.renderMode, renderModes, IM_ARRAYSIZE(renderModes));
				}
				
				// Settings section
				if (Zgine::IG::CollapsingHeader("2D Settings"))
				{
					Zgine::IG::SliderFloat("Line Thickness", &config.lineThickness, 0.01f, 0.2f);
					Zgine::IG::SliderFloat("Circle Radius", &config.circleRadius, 0.1f, 1.0f);
					Zgine::IG::SliderInt("Circle Segments", &config.circleSegments, 8, 64);
					Zgine::IG::SliderInt("Ellipse Segments", &config.ellipseSegments, 8, 32);
				}
				
				// Footer 区 - 按钮组
				Zgine::IG::Separator();
				Zgine::IG::Text("Actions:");
				
				if (Zgine::IG::Button("Apply"))
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
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Reset"))
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
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("Export"))
				{
					// Export current configuration to file
					Export2DConfiguration(config);
				}
				
				// Performance section
				if (Zgine::IG::CollapsingHeader("Performance Details"))
				{
					Zgine::IG::Text("FPS: %.1f", m_Test2DModule->GetFPS());
					Zgine::IG::Text("Objects: %d", m_Test2DModule->GetObjectCount());
					Zgine::IG::Text("Active Scene: %s", m_Test2DModule->GetActiveScene().c_str());
					
					// Show current configuration status
					Zgine::IG::Separator();
					Zgine::IG::Text("Current Configuration:");
					Zgine::IG::Text("Quads: %s | Lines: %s | Circles: %s", 
						config.showQuads ? "ON" : "OFF",
						config.showLines ? "ON" : "OFF", 
						config.showCircles ? "ON" : "OFF");
					Zgine::IG::Text("Advanced: %s | Triangles: %s | Ellipses: %s", 
						config.showAdvanced ? "ON" : "OFF",
						config.showTriangles ? "ON" : "OFF", 
						config.showEllipses ? "ON" : "OFF");
					Zgine::IG::Text("Animation: %s | Paused: %s | Speed: %.1f", 
						(config.animateQuads || config.animateCircles || config.animateAll) ? "ON" : "OFF",
						config.animationPaused ? "YES" : "NO",
						config.animationSpeed);
				}
			}
			Zgine::IG::End();
		}
	}

	void UIManager::Render3DTestWindow()
	{
		if (m_Test3DModule)
		{
			if (Zgine::IG::Begin("3D Rendering Test", &m_Windows["3D Test"].isVisible))
			{
				auto& config = m_Test3DModule->GetConfig();
				
				// Debug: Log current configuration values
				// ZG_CORE_INFO("UIManager::Render3DTestWindow - Current config: showCubes={}, showSpheres={}, showPlanes={}", 
				//	config.showCubes, config.showSpheres, config.showPlanes);
				
				// Debug: Check if all shapes are disabled
				if (!config.showCubes && !config.showSpheres && !config.showPlanes)
				{
					ZG_CORE_WARN("UIManager: All 3D shapes are disabled in UI! User needs to click '3D Basic' button.");
				}
				
				// Header 区 - 显示FPS、Objects、Draw Calls
				Zgine::IG::Text("3D Rendering Test Module");
				Zgine::IG::SameLine();
				Zgine::IG::Text("| FPS: %.1f", m_Test3DModule->GetFPS());
				Zgine::IG::SameLine();
				Zgine::IG::Text("| Objects: %d", m_Test3DModule->GetObjectCount());
				Zgine::IG::SameLine();
				Zgine::IG::Text("| Draw Calls: %d", m_Test3DModule->GetObjectCount() / 4); // Approximate draw calls
				
				// Tooltips for header info
				if (Zgine::IG::IsItemHovered())
				{
					Zgine::IG::SetTooltip("Current 3D rendering performance metrics");
				}
				
				Zgine::IG::Separator();
				
				// Quick Preset Buttons - 与2D测试保持一致的设计
				Zgine::IG::Text("3D Quick Presets:");
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Basic"))
				{
					ZG_CORE_INFO("3D Basic button clicked!");
					config.showCubes = true;
					config.showSpheres = true;
					config.showPlanes = true;
					config.showEnvironment = false;
					config.animateObjects = false;
					config.wireframeMode = false;
					m_Test3DModule->SetActiveScene("Basic Shapes");
					// Update camera immediately
					m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
					m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					ZG_CORE_INFO("3D Basic preset applied - Basic shapes enabled, ActiveScene: {}", m_Test3DModule->GetActiveScene());
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Advanced"))
				{
					config.showCubes = true;
					config.showSpheres = true;
					config.showPlanes = true;
					config.showEnvironment = true;
					config.animateObjects = true;
					config.wireframeMode = false;
					m_Test3DModule->SetActiveScene("Environment");
					// Update camera immediately
					m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
					m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					ZG_CORE_INFO("3D Advanced preset applied - All features enabled");
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Performance"))
				{
					config.showCubes = true;
					config.showSpheres = false;
					config.showPlanes = false;
					config.showEnvironment = false;
					config.animateObjects = false;
					config.wireframeMode = false;
					m_Test3DModule->SetActiveScene("Performance Test");
					// Update camera immediately
					m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
					m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					ZG_CORE_INFO("3D Performance preset applied - Performance test scene");
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Clear All"))
				{
					config.showCubes = false;
					config.showSpheres = false;
					config.showPlanes = false;
					config.showEnvironment = false;
					config.animateObjects = false;
					config.wireframeMode = false;
					m_Test3DModule->SetActiveScene("Basic Shapes");
					// Update camera immediately
					m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
					m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					ZG_CORE_INFO("3D Clear All preset applied - All shapes disabled");
				}
				
				Zgine::IG::Separator();
				
				// Shape 控制区 - 两列布局
				Zgine::IG::Text("3D Shape Controls");
				Zgine::IG::Columns(2, "3D_ShapeControls", false);
				
				// Left column - Basic Shapes
				Zgine::IG::Text("3D Basic Shapes:");
				if (Zgine::IG::Checkbox("3D Cubes", &config.showCubes))
				{
					// Auto-switch scene based on configuration
					if (config.showEnvironment || config.animateObjects)
						m_Test3DModule->SetActiveScene("Environment");
					else
						m_Test3DModule->SetActiveScene("Basic Shapes");
				}
				if (Zgine::IG::Checkbox("3D Spheres", &config.showSpheres))
				{
					if (config.showEnvironment || config.animateObjects)
						m_Test3DModule->SetActiveScene("Environment");
					else
						m_Test3DModule->SetActiveScene("Basic Shapes");
				}
				if (Zgine::IG::Checkbox("3D Planes", &config.showPlanes))
				{
					if (config.showEnvironment || config.animateObjects)
						m_Test3DModule->SetActiveScene("Environment");
					else
						m_Test3DModule->SetActiveScene("Basic Shapes");
				}
				
				Zgine::IG::NextColumn();
				
				// Right column - Advanced Features
				Zgine::IG::Text("3D Advanced Features:");
				if (Zgine::IG::Checkbox("3D Environment", &config.showEnvironment))
				{
					m_Test3DModule->SetActiveScene("Environment");
				}
				if (Zgine::IG::Checkbox("3D Animate Objects", &config.animateObjects))
				{
					if (config.animateObjects)
						m_Test3DModule->SetActiveScene("Animated Shapes");
					else if (config.showEnvironment)
						m_Test3DModule->SetActiveScene("Environment");
					else
						m_Test3DModule->SetActiveScene("Basic Shapes");
				}
				if (Zgine::IG::Checkbox("3D Wireframe Mode", &config.wireframeMode))
				{
					// Wireframe mode can be applied to any scene
				}
				
				Zgine::IG::Columns(1);
				Zgine::IG::Separator();
				
				// Animation Controls 区
				if (config.animateObjects)
				{
					Zgine::IG::Text("3D Animation Controls");
					Zgine::IG::SliderFloat("3D Animation Speed", &config.cameraSpeed, 0.1f, 10.0f);
					Zgine::IG::SameLine();
					if (Zgine::IG::Button("3D Reset Animation"))
					{
						// Reset animation state if needed
					}
				}
				
				Zgine::IG::Separator();
				
				// Render Options 区 - 可折叠
				if (Zgine::IG::CollapsingHeader("3D Render Options"))
				{
					Zgine::IG::Checkbox("3D Wireframe Mode", &config.wireframeMode);
					Zgine::IG::SameLine();
					Zgine::IG::Checkbox("3D Show Bounding Boxes", &config.showEnvironment); // Reuse for now
					
					// Render mode dropdown
					const char* renderModes[] = { "Normal", "Wireframe", "Points" };
					int renderMode = config.wireframeMode ? 1 : 0;
					if (Zgine::IG::Combo("3D Render Mode", &renderMode, renderModes, IM_ARRAYSIZE(renderModes)))
					{
						config.wireframeMode = (renderMode == 1);
					}
				}
				
				// Lighting Options 区 - 可折叠
				if (Zgine::IG::CollapsingHeader("3D Lighting Options"))
				{
					Zgine::IG::SliderFloat("3D Light Intensity", &config.lightIntensity, 0.0f, 5.0f);
					Zgine::IG::SliderFloat3("3D Light Position", &config.lightPosition.x, -20.0f, 20.0f);
					Zgine::IG::ColorEdit3("3D Light Color", &config.lightColor.x);
					
					if (Zgine::IG::Button("3D Reset Lighting"))
					{
						config.lightIntensity = 1.0f;
						config.lightPosition = { 0.0f, 10.0f, 0.0f };
						config.lightColor = { 1.0f, 1.0f, 1.0f };
					}
				}
				
				// Camera Controls 区 - 可折叠
				if (Zgine::IG::CollapsingHeader("3D Camera Controls"))
				{
					// Camera control enable/disable
					Zgine::IG::Checkbox("3D Enable Keyboard Movement", &config.enableKeyboardMovement);
					Zgine::IG::SameLine();
					Zgine::IG::Checkbox("3D Enable Mouse Look", &config.enableMouseLook);
					
					Zgine::IG::Separator();
					
					// Camera settings
					Zgine::IG::SliderFloat("3D Camera Speed", &config.cameraSpeed, 0.1f, 20.0f);
					Zgine::IG::SliderFloat("3D Mouse Sensitivity", &config.mouseSensitivity, 0.01f, 1.0f);
					Zgine::IG::SliderFloat("3D Rotation Speed", &config.rotationSpeed, 10.0f, 180.0f);
					
					Zgine::IG::Separator();
					
					// Manual camera controls
					Zgine::IG::SliderFloat3("3D Camera Position", &config.cameraPosition.x, -50.0f, 50.0f);
					Zgine::IG::SliderFloat3("3D Camera Rotation", &config.cameraRotation.x, -180.0f, 180.0f);
					
					// Update camera immediately when sliders change
					m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
					m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					
					Zgine::IG::Separator();
					
					// Camera control buttons
					if (Zgine::IG::Button("3D Reset Camera"))
					{
						m_Test3DModule->ResetCamera();
						config.cameraPosition = { 0.0f, 2.0f, 8.0f };
						config.cameraRotation = { -15.0f, 0.0f, 0.0f };
						// Update camera immediately
						m_Test3DModule->GetCamera().SetPosition(config.cameraPosition);
						m_Test3DModule->GetCamera().SetRotation(config.cameraRotation);
					}
					Zgine::IG::SameLine();
					if (Zgine::IG::Button("3D Look at Origin"))
					{
						m_Test3DModule->SetCameraLookAt({ 0.0f, 0.0f, 0.0f });
						// Update UI to reflect new rotation
						config.cameraRotation = m_Test3DModule->GetCamera().GetRotation();
					}
					
					Zgine::IG::Separator();
					
					// Help text
					Zgine::IG::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, "Controls:");
					Zgine::IG::TextColored({ 0.6f, 0.6f, 0.6f, 1.0f }, "WASD - Move, Space/Shift - Up/Down");
					Zgine::IG::TextColored({ 0.6f, 0.6f, 0.6f, 1.0f }, "Right Mouse - Look around");
					Zgine::IG::TextColored({ 0.6f, 0.6f, 0.6f, 1.0f }, "Q/E - Decrease/Increase speed");
					Zgine::IG::TextColored({ 0.6f, 0.6f, 0.6f, 1.0f }, "R - Reset camera");
				}
				
				Zgine::IG::Separator();
				
				// Actions 区
				Zgine::IG::Text("3D Actions:");
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Apply"))
				{
					// Apply current configuration
					ZG_CORE_INFO("3D configuration applied");
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Reset"))
				{
					// Reset to default configuration
					config.showCubes = true;
					config.showSpheres = true;
					config.showPlanes = true;
					config.showEnvironment = true;
					config.animateObjects = false;
					config.wireframeMode = false;
					config.lightIntensity = 1.0f;
					config.lightPosition = { 0.0f, 10.0f, 0.0f };
					config.lightColor = { 1.0f, 1.0f, 1.0f };
					config.cameraPosition = { 0.0f, 2.0f, 8.0f };
					config.cameraRotation = { -15.0f, 0.0f, 0.0f };
					m_Test3DModule->SetActiveScene("Basic Shapes");
					ZG_CORE_INFO("3D configuration reset to defaults");
				}
				Zgine::IG::SameLine();
				if (Zgine::IG::Button("3D Export"))
				{
					Export3DConfiguration(config);
				}
			}
			Zgine::IG::End();
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
		Zgine::IG::ShowDemoWindow(&m_ShowDemoWindow);
	}

	void UIManager::RenderMetricsWindow()
	{
		Zgine::IG::ShowMetricsWindow(&m_ShowMetricsWindow);
	}

	void UIManager::RenderStyleEditor()
	{
		if (Zgine::IG::Begin("Style Editor", &m_ShowStyleEditor))
		{
			Zgine::IG::ShowStyleEditor();
		}
		Zgine::IG::End();
	}

	void UIManager::RenderAboutWindow()
	{
		if (Zgine::IG::Begin("About Zgine", &m_ShowAboutWindow))
		{
			Zgine::IG::Text("Zgine Engine");
			Zgine::IG::Text("A modern C++ game engine");
			Zgine::IG::Separator();
			Zgine::IG::Text("Version: 1.0.0");
			Zgine::IG::Text("Build: Debug");
			Zgine::IG::Text("Platform: Windows x64");
			Zgine::IG::Separator();
			Zgine::IG::Text("Features:");
			Zgine::IG::BulletText("2D/3D Batch Rendering");
			Zgine::IG::BulletText("Advanced Rendering Pipeline");
			Zgine::IG::BulletText("Performance Monitoring");
			Zgine::IG::BulletText("Modular Architecture");
			Zgine::IG::BulletText("ImGui Integration");
		}
		Zgine::IG::End();
	}

	void UIManager::ApplyDarkTheme()
	{
		Zgine::IG::StyleColorsDark();
	}

	void UIManager::ApplyLightTheme()
	{
		Zgine::IG::StyleColorsLight();
	}

	void UIManager::ApplyClassicTheme()
	{
		Zgine::IG::StyleColorsClassic();
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
			Zgine::IG::OpenPopup("Export Success");
		}
		else
		{
			ZG_CORE_ERROR("Failed to export 2D configuration to: {}", filename.str());
			Zgine::IG::OpenPopup("Export Failed");
		}
		
		// Show popup messages
		if (Zgine::IG::BeginPopupModal("Export Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			Zgine::IG::Text("Configuration exported successfully!");
			Zgine::IG::Text("File: %s", filename.str().c_str());
			Zgine::IG::Separator();
			if (Zgine::IG::Button("OK"))
			{
				Zgine::IG::CloseCurrentPopup();
			}
			Zgine::IG::EndPopup();
		}
		
		if (Zgine::IG::BeginPopupModal("Export Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			Zgine::IG::Text("Failed to export configuration!");
			Zgine::IG::Text("Please check file permissions.");
			Zgine::IG::Separator();
			if (Zgine::IG::Button("OK"))
			{
				Zgine::IG::CloseCurrentPopup();
			}
			Zgine::IG::EndPopup();
		}
	}

	void UIManager::Export3DConfiguration(const struct Test3DConfig& config)
	{
		// Create a timestamp for the filename
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto tm = *std::localtime(&time_t);
		
		// Create filename with timestamp
		std::stringstream filename;
		filename << "3D_Config_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".json";
		
		// Create JSON content
		std::stringstream json;
		json << "{\n";
		json << "  \"3D_Rendering_Configuration\": {\n";
		json << "    \"render_options\": {\n";
		json << "      \"showCubes\": " << (config.showCubes ? "true" : "false") << ",\n";
		json << "      \"showSpheres\": " << (config.showSpheres ? "true" : "false") << ",\n";
		json << "      \"showPlanes\": " << (config.showPlanes ? "true" : "false") << ",\n";
		json << "      \"showEnvironment\": " << (config.showEnvironment ? "true" : "false") << ",\n";
		json << "      \"animateObjects\": " << (config.animateObjects ? "true" : "false") << ",\n";
		json << "      \"wireframeMode\": " << (config.wireframeMode ? "true" : "false") << "\n";
		json << "    },\n";
		json << "    \"lighting_settings\": {\n";
		json << "      \"lightIntensity\": " << config.lightIntensity << ",\n";
		json << "      \"lightPosition\": [\n";
		json << "        " << config.lightPosition.x << ",\n";
		json << "        " << config.lightPosition.y << ",\n";
		json << "        " << config.lightPosition.z << "\n";
		json << "      ],\n";
		json << "      \"lightColor\": [\n";
		json << "        " << config.lightColor.x << ",\n";
		json << "        " << config.lightColor.y << ",\n";
		json << "        " << config.lightColor.z << "\n";
		json << "      ]\n";
		json << "    },\n";
		json << "    \"camera_settings\": {\n";
		json << "      \"cameraSpeed\": " << config.cameraSpeed << ",\n";
		json << "      \"rotationSpeed\": " << config.rotationSpeed << ",\n";
		json << "      \"cameraPosition\": [\n";
		json << "        " << config.cameraPosition.x << ",\n";
		json << "        " << config.cameraPosition.y << ",\n";
		json << "        " << config.cameraPosition.z << "\n";
		json << "      ],\n";
		json << "      \"cameraRotation\": [\n";
		json << "        " << config.cameraRotation.x << ",\n";
		json << "        " << config.cameraRotation.y << ",\n";
		json << "        " << config.cameraRotation.z << "\n";
		json << "      ]\n";
		json << "    }\n";
		json << "  },\n";
		json << "  \"export_info\": {\n";
		json << "    \"timestamp\": \"" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\",\n";
		json << "    \"engine_version\": \"Zgine 1.0.0\",\n";
		json << "    \"active_scene\": \"" << (m_Test3DModule ? m_Test3DModule->GetActiveScene() : "Unknown") << "\"\n";
		json << "  }\n";
		json << "}\n";
		
		// Write to file
		std::ofstream file(filename.str());
		if (file.is_open())
		{
			file << json.str();
			file.close();
			ZG_CORE_INFO("3D Configuration exported to: {}", filename.str());
			
			// Show success message in ImGui
			Zgine::IG::OpenPopup("3D Export Success");
		}
		else
		{
			ZG_CORE_ERROR("Failed to export 3D configuration to: {}", filename.str());
			Zgine::IG::OpenPopup("3D Export Failed");
		}
		
		// Show popup messages
		if (Zgine::IG::BeginPopupModal("3D Export Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			Zgine::IG::Text("3D Configuration exported successfully!");
			Zgine::IG::Text("File: %s", filename.str().c_str());
			Zgine::IG::Separator();
			if (Zgine::IG::Button("OK"))
			{
				Zgine::IG::CloseCurrentPopup();
			}
			Zgine::IG::EndPopup();
		}
		
		if (Zgine::IG::BeginPopupModal("3D Export Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			Zgine::IG::Text("Failed to export 3D configuration!");
			Zgine::IG::Text("Please check file permissions and try again.");
			Zgine::IG::Separator();
			if (Zgine::IG::Button("OK"))
			{
				Zgine::IG::CloseCurrentPopup();
			}
			Zgine::IG::EndPopup();
		}
	}

}
