#pragma once

#include "Zgine/Layer.h"
#include "imgui.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Sandbox {

	// Forward declarations
	class RenderManager;
	class SceneManager;
	class SettingsManager;
	class Test2DModule;
	class Test3DModule;

	// UI Theme management
	enum class UITheme
	{
		Dark,
		Light,
		Classic,
		Custom
	};

	// UI Layout management
	struct UILayout
	{
		std::string name;
		std::vector<std::string> windowOrder;
		std::unordered_map<std::string, ImVec2> windowPositions;
		std::unordered_map<std::string, ImVec2> windowSizes;
		bool isDefault = false;
	};

	class UIManager
	{
	public:
		UIManager();
		~UIManager();

		void OnAttach();
		void OnUpdate(float ts);
		void OnImGuiRender();
		void OnEvent(Zgine::Event& e);

		// Window management
		void RegisterWindow(const std::string& name, std::function<void()> renderFunc, bool defaultVisible = false);
		void ShowWindow(const std::string& name, bool show = true);
		bool IsWindowVisible(const std::string& name) const;
		void ToggleWindow(const std::string& name);
		void HideAllWindows();
		void ShowAllWindows();

		// Menu management
		void RenderMainMenu();
		void AddMenuCategory(const std::string& category, std::function<void()> renderFunc);
		void AddMenuItem(const std::string& category, const std::string& name, std::function<void()> callback);

		// Theme management
		void SetTheme(UITheme theme);
		UITheme GetCurrentTheme() const { return m_CurrentTheme; }
		void ApplyCustomTheme();

		// Layout management
		void SaveCurrentLayout(const std::string& name);
		void LoadLayout(const std::string& name);
		void DeleteLayout(const std::string& name);
		std::vector<std::string> GetAvailableLayouts() const;

		// Settings
		void SetShowMainMenu(bool show) { m_ShowMainMenu = show; }
		bool IsMainMenuVisible() const { return m_ShowMainMenu; }
		void SetShowStatusBar(bool show) { m_ShowStatusBar = show; }
		bool IsStatusBarVisible() const { return m_ShowStatusBar; }

		// Docking
		void EnableDocking(bool enable) { m_EnableDocking = enable; }
		bool IsDockingEnabled() const { return m_EnableDocking; }

		// Performance
		void ShowPerformanceOverlay(bool show) { m_ShowPerformanceOverlay = show; }
		bool IsPerformanceOverlayVisible() const { return m_ShowPerformanceOverlay; }

		// Manager references (set by MainControlLayer)
		void SetRenderManager(RenderManager* renderManager) { m_RenderManager = renderManager; }
		void SetSceneManager(SceneManager* sceneManager) { m_SceneManager = sceneManager; }
		void SetSettingsManager(SettingsManager* settingsManager) { m_SettingsManager = settingsManager; }
		void SetTest2DModule(Test2DModule* test2DModule) { m_Test2DModule = test2DModule; }
		void SetTest3DModule(Test3DModule* test3DModule) { m_Test3DModule = test3DModule; }

	private:
		struct WindowInfo
		{
			std::function<void()> renderFunction;
			bool isVisible = false;
			bool* visibilityPtr = nullptr;
			bool isRegistered = false;
		};

		struct MenuCategory
		{
			std::string name;
			std::function<void()> renderFunction;
			std::vector<std::pair<std::string, std::function<void()>>> items;
		};

		// Core UI state
		std::unordered_map<std::string, WindowInfo> m_Windows;
		std::unordered_map<std::string, MenuCategory> m_MenuCategories;
		std::unordered_map<std::string, UILayout> m_Layouts;

		// UI Settings
		bool m_ShowMainMenu = true;
		bool m_ShowStatusBar = true;
		bool m_EnableDocking = true;
		bool m_ShowPerformanceOverlay = false;

		// Built-in windows
		bool m_ShowDemoWindow = false;
		bool m_ShowMetricsWindow = false;
		bool m_ShowStyleEditor = false;
		bool m_ShowAboutWindow = false;
		bool m_ShowLayoutManager = false;

		// Theme
		UITheme m_CurrentTheme = UITheme::Dark;

		// Manager references
		RenderManager* m_RenderManager = nullptr;
		SceneManager* m_SceneManager = nullptr;
		SettingsManager* m_SettingsManager = nullptr;
		Test2DModule* m_Test2DModule = nullptr;
		Test3DModule* m_Test3DModule = nullptr;

		// Internal methods
		void RenderMainMenuBar();
		void RenderStatusBar();
		void RenderPerformanceOverlay();
		void RenderLayoutManager();
		void RenderDemoWindow();
		void RenderMetricsWindow();
		void RenderStyleEditor();
		void RenderAboutWindow();
		void RenderEngineInfo();
		void RenderTestWindows();
		void RenderTestModuleWindows();
		void Render2DTestWindow();
		void Render3DTestWindow();

		// Theme application
		void ApplyDarkTheme();
		void ApplyLightTheme();
		void ApplyClassicTheme();

		// Layout management
		void SaveWindowStates();
		void RestoreWindowStates(const UILayout& layout);
		void CreateDefaultLayouts();
		
		// Configuration export/import
		void Export2DConfiguration(const struct Test2DConfig& config);
	};
}
