#include "sandbox_pch.h"
#include "SettingsManager.h"
#include "Zgine/Logging/Log.h"
#include <fstream>
#include <sstream>

namespace Sandbox {

	SettingsManager::SettingsManager()
	{
		ZG_CORE_INFO("SettingsManager created");
		LoadSettings(); // Load settings on creation
	}

	SettingsManager::~SettingsManager()
	{
		SaveSettings(); // Save settings on destruction
		ZG_CORE_INFO("SettingsManager destroyed");
	}

	void SettingsManager::OnAttach()
	{
		ZG_CORE_INFO("SettingsManager attached");
	}

	void SettingsManager::OnUpdate(float ts)
	{
		// Update settings logic if needed
	}

	void SettingsManager::OnImGuiRender()
	{
		RenderSettingsWindow();
	}

	void SettingsManager::SaveSettings()
	{
		std::ofstream file("settings.json");
		if (file.is_open())
		{
			file << "{\n";
			file << "  \"advancedRendering\": {\n";
			file << "    \"enabled\": " << (m_AdvancedRendering.enabled ? "true" : "false") << ",\n";
			file << "    \"currentPreset\": " << m_AdvancedRendering.currentPreset << ",\n";
			file << "    \"hdrExposure\": " << m_AdvancedRendering.hdrExposure << ",\n";
			file << "    \"hdrGamma\": " << m_AdvancedRendering.hdrGamma << ",\n";
			file << "    \"bloomIntensity\": " << m_AdvancedRendering.bloomIntensity << ",\n";
			file << "    \"bloomThreshold\": " << m_AdvancedRendering.bloomThreshold << ",\n";
			file << "    \"vignetteStrength\": " << m_AdvancedRendering.vignetteStrength << ",\n";
			file << "    \"vignetteRadius\": " << m_AdvancedRendering.vignetteRadius << ",\n";
			file << "    \"chromaticAberrationStrength\": " << m_AdvancedRendering.chromaticAberrationStrength << ",\n";
			file << "    \"filmGrainStrength\": " << m_AdvancedRendering.filmGrainStrength << ",\n";
			file << "    \"shadowMapResolution\": " << m_AdvancedRendering.shadowMapResolution << ",\n";
			file << "    \"softShadowsEnabled\": " << (m_AdvancedRendering.softShadowsEnabled ? "true" : "false") << ",\n";
			file << "    \"cascadedShadowsEnabled\": " << (m_AdvancedRendering.cascadedShadowsEnabled ? "true" : "false") << ",\n";
			file << "    \"shadowBias\": " << m_AdvancedRendering.shadowBias << ",\n";
			file << "    \"shadowNormalBias\": " << m_AdvancedRendering.shadowNormalBias << "\n";
			file << "  }\n";
			file << "}\n";
			file.close();
			ZG_CORE_INFO("Settings saved to settings.json");
		}
		else
		{
			ZG_CORE_ERROR("Failed to save settings to settings.json");
		}
	}

	void SettingsManager::LoadSettings()
	{
		std::ifstream file("settings.json");
		if (file.is_open())
		{
			// Simple JSON parsing (in a real implementation, use a proper JSON library)
			std::string line;
			while (std::getline(file, line))
			{
				// Basic parsing logic would go here
				// For now, we'll just use default values
			}
			file.close();
			ZG_CORE_INFO("Settings loaded from settings.json");
		}
		else
		{
			ZG_CORE_INFO("No settings file found, using defaults");
		}
	}

	void SettingsManager::ResetToDefaults()
	{
		m_AdvancedRendering = AdvancedRenderingSettings();
		ZG_CORE_INFO("Settings reset to defaults");
	}

	void SettingsManager::RegisterSettingsCallback(const std::string& setting, std::function<void()> callback)
	{
		m_SettingsCallbacks[setting] = callback;
	}

	void SettingsManager::TriggerSettingsCallback(const std::string& setting)
	{
		auto it = m_SettingsCallbacks.find(setting);
		if (it != m_SettingsCallbacks.end())
		{
			it->second();
		}
	}

	void SettingsManager::RenderSettingsWindow()
	{
		if (ImGui::Begin("Settings"))
		{
			if (ImGui::BeginTabBar("SettingsTabs"))
			{
				if (ImGui::BeginTabItem("Graphics"))
				{
					RenderGraphicsSettingsWindow();
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Advanced Rendering"))
				{
					RenderAdvancedRenderingWindow();
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Performance"))
				{
					RenderPerformanceSettingsWindow();
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Input"))
				{
					RenderInputSettingsWindow();
					ImGui::EndTabItem();
				}
				
				ImGui::EndTabBar();
			}
			
			ImGui::Separator();
			if (ImGui::Button("Save Settings"))
			{
				SaveSettings();
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Settings"))
			{
				LoadSettings();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset to Defaults"))
			{
				ResetToDefaults();
			}
		}
		ImGui::End();
	}

	void SettingsManager::RenderAdvancedRenderingWindow()
	{
		ImGui::Text("Advanced Rendering Settings");
		ImGui::Separator();
		
		ImGui::Checkbox("Enable Advanced Rendering", &m_AdvancedRendering.enabled);
		
		if (m_AdvancedRendering.enabled)
		{
			const char* presets[] = { "Low", "Medium", "High", "Ultra", "Cinematic", "Performance", "Quality" };
			if (ImGui::Combo("Rendering Preset", &m_AdvancedRendering.currentPreset, presets, 7))
			{
				TriggerSettingsCallback("rendering_preset");
			}
			
			ImGui::Separator();
			ImGui::Text("HDR & Tone Mapping");
			ImGui::SliderFloat("Exposure", &m_AdvancedRendering.hdrExposure, 0.1f, 5.0f);
			ImGui::SliderFloat("Gamma", &m_AdvancedRendering.hdrGamma, 1.0f, 3.0f);
			
			ImGui::Separator();
			ImGui::Text("Post-Processing Effects");
			ImGui::SliderFloat("Bloom Intensity", &m_AdvancedRendering.bloomIntensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Bloom Threshold", &m_AdvancedRendering.bloomThreshold, 0.0f, 2.0f);
			ImGui::SliderFloat("Vignette Strength", &m_AdvancedRendering.vignetteStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("Vignette Radius", &m_AdvancedRendering.vignetteRadius, 0.0f, 1.0f);
			ImGui::SliderFloat("Chromatic Aberration", &m_AdvancedRendering.chromaticAberrationStrength, 0.0f, 0.1f);
			ImGui::SliderFloat("Film Grain", &m_AdvancedRendering.filmGrainStrength, 0.0f, 1.0f);
			
			ImGui::Separator();
			ImGui::Text("Shadow Mapping");
			const char* resolutions[] = { "512x512", "1024x1024", "2048x2048", "4096x4096" };
			ImGui::Combo("Shadow Map Resolution", &m_AdvancedRendering.shadowMapResolution, resolutions, 4);
			ImGui::Checkbox("Soft Shadows", &m_AdvancedRendering.softShadowsEnabled);
			ImGui::Checkbox("Cascaded Shadows", &m_AdvancedRendering.cascadedShadowsEnabled);
			ImGui::SliderFloat("Shadow Bias", &m_AdvancedRendering.shadowBias, 0.0f, 0.1f);
			ImGui::SliderFloat("Shadow Normal Bias", &m_AdvancedRendering.shadowNormalBias, 0.0f, 0.1f);
		}
	}

	void SettingsManager::RenderPerformanceSettingsWindow()
	{
		ImGui::Text("Performance Settings");
		ImGui::Separator();
		
		ImGui::Text("Performance monitoring and optimization settings will go here.");
		ImGui::Text("This includes:");
		ImGui::BulletText("Frame rate limiting");
		ImGui::BulletText("VSync settings");
		ImGui::BulletText("LOD settings");
		ImGui::BulletText("Culling settings");
		ImGui::BulletText("Memory management");
	}

	void SettingsManager::RenderGraphicsSettingsWindow()
	{
		ImGui::Text("Graphics Settings");
		ImGui::Separator();
		
		ImGui::Text("Basic graphics settings will go here.");
		ImGui::Text("This includes:");
		ImGui::BulletText("Resolution settings");
		ImGui::BulletText("Fullscreen mode");
		ImGui::BulletText("Anti-aliasing");
		ImGui::BulletText("Texture quality");
		ImGui::BulletText("Shader quality");
	}

	void SettingsManager::RenderInputSettingsWindow()
	{
		ImGui::Text("Input Settings");
		ImGui::Separator();
		
		ImGui::Text("Input configuration will go here.");
		ImGui::Text("This includes:");
		ImGui::BulletText("Key bindings");
		ImGui::BulletText("Mouse sensitivity");
		ImGui::BulletText("Controller settings");
		ImGui::BulletText("Input device selection");
	}

}