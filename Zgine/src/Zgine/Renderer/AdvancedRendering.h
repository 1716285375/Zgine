#pragma once

#include "zgpch.h"
#include "../Core.h"
#include "PostProcessing.h"
#include "ShadowMapping.h"
#include "HDRSystem.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>

namespace Zgine {

	/**
	 * @brief Advanced rendering features configuration
	 */
	struct AdvancedRenderingConfig
	{
		// Post-processing
		bool enablePostProcessing = true;
		bool enableBloom = false;
		bool enableBlur = false;
		bool enableVignette = false;
		bool enableChromaticAberration = false;
		bool enableFilmGrain = false;
		bool enableMotionBlur = false;
		bool enableDepthOfField = false;
		bool enableSSAO = false;
		bool enableFXAA = true;
		bool enableSMAA = false;
		bool enableTAA = false;

		// Shadow mapping
		bool enableShadowMapping = true;
		ShadowMapResolution shadowMapResolution = ShadowMapResolution::High;
		bool enableSoftShadows = true;
		bool enableCascadedShadows = true;
		int cascadeCount = 4;

		// HDR
		bool enableHDR = true;
		bool enableAutoExposure = true;
		ToneMappingOperator toneMapping = ToneMappingOperator::ACES;
		float exposure = 1.0f;
		float gamma = 2.2f;

		// Performance
		bool enableFrustumCulling = true;
		bool enableOcclusionCulling = false;
		bool enableLOD = true;
		bool enableInstancing = true;
		bool enableGPUCulling = false;

		// Quality settings
		int msaaSamples = 4;
		bool enableAnisotropicFiltering = true;
		float anisotropicLevel = 16.0f;
		bool enableTextureCompression = true;
		bool enableMipmaps = true;
	};

	/**
	 * @brief Rendering statistics
	 */
	struct AdvancedRenderStats
	{
		// Frame timing
		float frameTime = 0.0f;
		float renderTime = 0.0f;
		float postProcessTime = 0.0f;
		float shadowTime = 0.0f;
		float cullingTime = 0.0f;

		// Draw calls
		uint32_t totalDrawCalls = 0;
		uint32_t shadowDrawCalls = 0;
		uint32_t postProcessDrawCalls = 0;
		uint32_t culledObjects = 0;
		uint32_t visibleObjects = 0;

		// Memory usage
		size_t textureMemory = 0;
		size_t bufferMemory = 0;
		size_t shaderMemory = 0;
		size_t totalMemory = 0;

		// Quality metrics
		float averageFPS = 0.0f;
		float minFPS = 0.0f;
		float maxFPS = 0.0f;
		float frameDrops = 0.0f;
	};

	/**
	 * @brief Advanced rendering manager
	 */
	class AdvancedRenderingManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		// Scene rendering
		static void BeginScene(const PerspectiveCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// Rendering
		static void Render();
		static void RenderDebug();

		// Configuration
		static void SetConfig(const AdvancedRenderingConfig& config);
		static const AdvancedRenderingConfig& GetConfig() { return s_Config; }

		// Statistics
		static const AdvancedRenderStats& GetStats() { return s_Stats; }
		static void ResetStats();

		// Feature toggles
		static void EnableFeature(const std::string& featureName, bool enabled);
		static bool IsFeatureEnabled(const std::string& featureName);

		// Quality presets
		static void LoadPreset(const std::string& presetName);
		static void SavePreset(const std::string& presetName);
		static std::vector<std::string> GetAvailablePresets();

		// System access
		static PostProcessingPipeline* GetPostProcessing() { return PostProcessingManager::GetPipeline(); }
		static ShadowMapRenderer* GetShadowMapping() { return ShadowMappingSystem::GetRenderer(); }
		static HDRToneMappingRenderer* GetHDRSystem() { return HDRSystem::GetRenderer(); }

		// Debug
		static void SetDebugEnabled(bool enabled) { s_DebugEnabled = enabled; }
		static bool IsDebugEnabled() { return s_DebugEnabled; }

		static bool IsInitialized() { return s_Initialized; }

	private:
		static bool s_Initialized;
		static bool s_DebugEnabled;
		static AdvancedRenderingConfig s_Config;
		static AdvancedRenderStats s_Stats;

		static void InitializeSystems();
		static void ShutdownSystems();
		static void UpdateStats(float deltaTime);
		static void LoadDefaultPresets();
	};

	/**
	 * @brief Rendering presets
	 */
	class RenderingPresets
	{
	public:
		static AdvancedRenderingConfig GetLowPreset();
		static AdvancedRenderingConfig GetMediumPreset();
		static AdvancedRenderingConfig GetHighPreset();
		static AdvancedRenderingConfig GetUltraPreset();
		static AdvancedRenderingConfig GetCinematicPreset();
		static AdvancedRenderingConfig GetPerformancePreset();
		static AdvancedRenderingConfig GetQualityPreset();

		static std::vector<std::string> GetPresetNames();
		static AdvancedRenderingConfig GetPreset(const std::string& name);
	};

	/**
	 * @brief Rendering utilities
	 */
	class RenderingUtils
	{
	public:
		// Performance monitoring
		static void StartTimer(const std::string& name);
		static void EndTimer(const std::string& name);
		static float GetTimer(const std::string& name);

		// Memory management
		static size_t GetGPUMemoryUsage();
		static size_t GetCPUMemoryUsage();
		static void LogMemoryUsage();

		// Quality assessment
		static float CalculateQualityScore(const AdvancedRenderStats& stats);
		static std::string GetQualityRecommendation(const AdvancedRenderStats& stats);

		// Feature compatibility
		static bool IsFeatureSupported(const std::string& featureName);
		static std::vector<std::string> GetSupportedFeatures();

		// Optimization suggestions
		static std::vector<std::string> GetOptimizationSuggestions(const AdvancedRenderStats& stats);
		static void ApplyOptimizationSuggestion(const std::string& suggestion);
		
	private:
		static std::unordered_map<std::string, float> s_Timers;
		static std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> s_StartTimes;
	};

}
