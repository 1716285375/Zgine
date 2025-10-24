#pragma once

#include "imgui.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace Sandbox {

	struct AdvancedRenderingSettings
	{
		bool enabled = false;
		int currentPreset = 3; // High
		float hdrExposure = 1.0f;
		float hdrGamma = 2.2f;
		float bloomIntensity = 1.0f;
		float bloomThreshold = 1.0f;
		float vignetteStrength = 0.5f;
		float vignetteRadius = 0.5f;
		float chromaticAberrationStrength = 0.02f;
		float filmGrainStrength = 0.1f;
		int shadowMapResolution = 1; // 1024x1024
		bool softShadowsEnabled = true;
		bool cascadedShadowsEnabled = false;
		float shadowBias = 0.005f;
		float shadowNormalBias = 0.01f;
	};

	class SettingsManager
	{
	public:
		SettingsManager();
		~SettingsManager();

		void OnAttach();
		void OnUpdate(float ts);
		void OnImGuiRender();

		// Settings access
		AdvancedRenderingSettings& GetAdvancedRenderingSettings() { return m_AdvancedRendering; }
		const AdvancedRenderingSettings& GetAdvancedRenderingSettings() const { return m_AdvancedRendering; }

		// Settings persistence
		void SaveSettings();
		void LoadSettings();
		void ResetToDefaults();

		// Settings callbacks
		void RegisterSettingsCallback(const std::string& setting, std::function<void()> callback);
		void TriggerSettingsCallback(const std::string& setting);

	private:
		AdvancedRenderingSettings m_AdvancedRendering;
		std::unordered_map<std::string, std::function<void()>> m_SettingsCallbacks;

		void RenderSettingsWindow();
		void RenderAdvancedRenderingWindow();
		void RenderPerformanceSettingsWindow();
		void RenderGraphicsSettingsWindow();
		void RenderInputSettingsWindow();
	};

}
