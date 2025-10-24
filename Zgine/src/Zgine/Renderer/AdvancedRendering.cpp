#include "zgpch.h"
#include "AdvancedRendering.h"
#include "RenderCommand.h"
#include <glad/glad.h>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace Zgine {

	// AdvancedRenderingManager Implementation
	bool AdvancedRenderingManager::s_Initialized = false;
	bool AdvancedRenderingManager::s_DebugEnabled = false;
	AdvancedRenderingConfig AdvancedRenderingManager::s_Config;
	AdvancedRenderStats AdvancedRenderingManager::s_Stats;

	void AdvancedRenderingManager::Init()
	{
		if (s_Initialized)
			return;

		InitializeSystems();
		LoadDefaultPresets();
		
		s_Initialized = true;
		ZG_CORE_INFO("Advanced rendering manager initialized");
	}

	void AdvancedRenderingManager::Shutdown()
	{
		if (!s_Initialized)
			return;

		ShutdownSystems();
		
		s_Initialized = false;
		ZG_CORE_INFO("Advanced rendering manager shutdown");
	}

	void AdvancedRenderingManager::BeginFrame()
	{
		if (!s_Initialized)
			return;

		// Reset frame statistics
		s_Stats.totalDrawCalls = 0;
		s_Stats.shadowDrawCalls = 0;
		s_Stats.postProcessDrawCalls = 0;
		s_Stats.culledObjects = 0;
		s_Stats.visibleObjects = 0;
	}

	void AdvancedRenderingManager::EndFrame()
	{
		if (!s_Initialized)
			return;

		// Update frame statistics
		UpdateStats(0.016f); // Assume 60 FPS
	}

	void AdvancedRenderingManager::BeginScene(const PerspectiveCamera& camera)
	{
		if (!s_Initialized)
			return;

		// Begin HDR rendering
		if (s_Config.enableHDR)
		{
			HDRSystem::BeginScene();
		}

		// Begin post-processing
		if (s_Config.enablePostProcessing)
		{
			PostProcessingManager::BeginScene();
		}

		// Begin shadow mapping
		if (s_Config.enableShadowMapping)
		{
			ShadowMappingSystem::BeginScene();
		}
	}

	void AdvancedRenderingManager::BeginScene(const OrthographicCamera& camera)
	{
		if (!s_Initialized)
			return;

		// Begin HDR rendering
		if (s_Config.enableHDR)
		{
			HDRSystem::BeginScene();
		}

		// Begin post-processing
		if (s_Config.enablePostProcessing)
		{
			PostProcessingManager::BeginScene();
		}
	}

	void AdvancedRenderingManager::EndScene()
	{
		if (!s_Initialized)
			return;

		// End shadow mapping
		if (s_Config.enableShadowMapping)
		{
			ShadowMappingSystem::EndScene();
		}

		// End post-processing
		if (s_Config.enablePostProcessing)
		{
			PostProcessingManager::EndScene();
		}

		// End HDR rendering
		if (s_Config.enableHDR)
		{
			HDRSystem::EndScene();
		}
	}

	void AdvancedRenderingManager::Render()
	{
		if (!s_Initialized)
			return;

		// Render HDR tone mapping
		if (s_Config.enableHDR)
		{
			HDRSystem::Render();
		}

		// Render post-processing effects
		if (s_Config.enablePostProcessing)
		{
			PostProcessingManager::Render();
		}

		// Render shadow maps
		if (s_Config.enableShadowMapping)
		{
			ShadowMappingSystem::RenderShadows();
		}
	}

	void AdvancedRenderingManager::RenderDebug()
	{
		if (!s_Initialized || !s_DebugEnabled)
			return;

		// Render shadow map debug
		if (s_Config.enableShadowMapping)
		{
			ShadowMappingSystem::RenderDebug();
		}
	}

	void AdvancedRenderingManager::SetConfig(const AdvancedRenderingConfig& config)
	{
		s_Config = config;

		// Apply configuration to subsystems
		if (s_Config.enableHDR)
		{
			HDRSystem::SetExposure(config.exposure);
			HDRSystem::SetGamma(config.gamma);
			HDRSystem::SetToneMapping(config.toneMapping);
			HDRSystem::SetAutoExposure(config.enableAutoExposure);
		}

		if (s_Config.enableShadowMapping)
		{
			ShadowMapConfig shadowConfig;
			shadowConfig.resolution = config.shadowMapResolution;
			shadowConfig.enableSoftShadows = config.enableSoftShadows;
			shadowConfig.enableCascadedShadows = config.enableCascadedShadows;
			shadowConfig.cascadeCount = config.cascadeCount;
			ShadowMappingSystem::SetConfig(shadowConfig);
		}

		if (s_Config.enablePostProcessing)
		{
			// Configure post-processing effects
			if (config.enableBloom)
				PostProcessingManager::EnableEffect(PostEffectType::Bloom);
			if (config.enableBlur)
				PostProcessingManager::EnableEffect(PostEffectType::Blur);
			if (config.enableVignette)
				PostProcessingManager::EnableEffect(PostEffectType::Vignette);
			if (config.enableChromaticAberration)
				PostProcessingManager::EnableEffect(PostEffectType::ChromaticAberration);
			if (config.enableFilmGrain)
				PostProcessingManager::EnableEffect(PostEffectType::FilmGrain);
			if (config.enableMotionBlur)
				PostProcessingManager::EnableEffect(PostEffectType::MotionBlur);
			if (config.enableDepthOfField)
				PostProcessingManager::EnableEffect(PostEffectType::DepthOfField);
			if (config.enableSSAO)
				PostProcessingManager::EnableEffect(PostEffectType::SSAO);
			if (config.enableFXAA)
				PostProcessingManager::EnableEffect(PostEffectType::FXAA);
			if (config.enableSMAA)
				PostProcessingManager::EnableEffect(PostEffectType::SMAA);
			if (config.enableTAA)
				PostProcessingManager::EnableEffect(PostEffectType::TAA);
		}
	}

	void AdvancedRenderingManager::ResetStats()
	{
		s_Stats = AdvancedRenderStats();
	}

	void AdvancedRenderingManager::EnableFeature(const std::string& featureName, bool enabled)
	{
		if (featureName == "HDR")
			s_Config.enableHDR = enabled;
		else if (featureName == "PostProcessing")
			s_Config.enablePostProcessing = enabled;
		else if (featureName == "ShadowMapping")
			s_Config.enableShadowMapping = enabled;
		else if (featureName == "Bloom")
			s_Config.enableBloom = enabled;
		else if (featureName == "Blur")
			s_Config.enableBlur = enabled;
		else if (featureName == "Vignette")
			s_Config.enableVignette = enabled;
		else if (featureName == "ChromaticAberration")
			s_Config.enableChromaticAberration = enabled;
		else if (featureName == "FilmGrain")
			s_Config.enableFilmGrain = enabled;
		else if (featureName == "MotionBlur")
			s_Config.enableMotionBlur = enabled;
		else if (featureName == "DepthOfField")
			s_Config.enableDepthOfField = enabled;
		else if (featureName == "SSAO")
			s_Config.enableSSAO = enabled;
		else if (featureName == "FXAA")
			s_Config.enableFXAA = enabled;
		else if (featureName == "SMAA")
			s_Config.enableSMAA = enabled;
		else if (featureName == "TAA")
			s_Config.enableTAA = enabled;
		else if (featureName == "SoftShadows")
			s_Config.enableSoftShadows = enabled;
		else if (featureName == "CascadedShadows")
			s_Config.enableCascadedShadows = enabled;
		else if (featureName == "AutoExposure")
			s_Config.enableAutoExposure = enabled;
		else if (featureName == "FrustumCulling")
			s_Config.enableFrustumCulling = enabled;
		else if (featureName == "OcclusionCulling")
			s_Config.enableOcclusionCulling = enabled;
		else if (featureName == "LOD")
			s_Config.enableLOD = enabled;
		else if (featureName == "Instancing")
			s_Config.enableInstancing = enabled;
		else if (featureName == "GPUCulling")
			s_Config.enableGPUCulling = enabled;
	}

	bool AdvancedRenderingManager::IsFeatureEnabled(const std::string& featureName)
	{
		if (featureName == "HDR")
			return s_Config.enableHDR;
		else if (featureName == "PostProcessing")
			return s_Config.enablePostProcessing;
		else if (featureName == "ShadowMapping")
			return s_Config.enableShadowMapping;
		else if (featureName == "Bloom")
			return s_Config.enableBloom;
		else if (featureName == "Blur")
			return s_Config.enableBlur;
		else if (featureName == "Vignette")
			return s_Config.enableVignette;
		else if (featureName == "ChromaticAberration")
			return s_Config.enableChromaticAberration;
		else if (featureName == "FilmGrain")
			return s_Config.enableFilmGrain;
		else if (featureName == "MotionBlur")
			return s_Config.enableMotionBlur;
		else if (featureName == "DepthOfField")
			return s_Config.enableDepthOfField;
		else if (featureName == "SSAO")
			return s_Config.enableSSAO;
		else if (featureName == "FXAA")
			return s_Config.enableFXAA;
		else if (featureName == "SMAA")
			return s_Config.enableSMAA;
		else if (featureName == "TAA")
			return s_Config.enableTAA;
		else if (featureName == "SoftShadows")
			return s_Config.enableSoftShadows;
		else if (featureName == "CascadedShadows")
			return s_Config.enableCascadedShadows;
		else if (featureName == "AutoExposure")
			return s_Config.enableAutoExposure;
		else if (featureName == "FrustumCulling")
			return s_Config.enableFrustumCulling;
		else if (featureName == "OcclusionCulling")
			return s_Config.enableOcclusionCulling;
		else if (featureName == "LOD")
			return s_Config.enableLOD;
		else if (featureName == "Instancing")
			return s_Config.enableInstancing;
		else if (featureName == "GPUCulling")
			return s_Config.enableGPUCulling;
		
		return false;
	}

	void AdvancedRenderingManager::LoadPreset(const std::string& presetName)
	{
		AdvancedRenderingConfig preset = RenderingPresets::GetPreset(presetName);
		SetConfig(preset);
		ZG_CORE_INFO("Loaded rendering preset: {0}", presetName);
	}

	void AdvancedRenderingManager::SavePreset(const std::string& presetName)
	{
		// Save preset to file
		std::ofstream file("presets/" + presetName + ".json");
		if (file.is_open())
		{
			// Simple JSON-like format
			file << "{\n";
			file << "  \"enableHDR\": " << (s_Config.enableHDR ? "true" : "false") << ",\n";
			file << "  \"enablePostProcessing\": " << (s_Config.enablePostProcessing ? "true" : "false") << ",\n";
			file << "  \"enableShadowMapping\": " << (s_Config.enableShadowMapping ? "true" : "false") << ",\n";
			file << "  \"enableBloom\": " << (s_Config.enableBloom ? "true" : "false") << ",\n";
			file << "  \"exposure\": " << s_Config.exposure << ",\n";
			file << "  \"gamma\": " << s_Config.gamma << "\n";
			file << "}\n";
			file.close();
			ZG_CORE_INFO("Saved rendering preset: {0}", presetName);
		}
	}

	std::vector<std::string> AdvancedRenderingManager::GetAvailablePresets()
	{
		return RenderingPresets::GetPresetNames();
	}

	void AdvancedRenderingManager::InitializeSystems()
	{
		// Initialize HDR system
		if (s_Config.enableHDR)
		{
			HDRSystem::Init();
		}

		// Initialize post-processing system
		if (s_Config.enablePostProcessing)
		{
			PostProcessingManager::Init();
		}

		// Initialize shadow mapping system
		if (s_Config.enableShadowMapping)
		{
			ShadowMappingSystem::Init();
		}
	}

	void AdvancedRenderingManager::ShutdownSystems()
	{
		ShadowMappingSystem::Shutdown();
		PostProcessingManager::Shutdown();
		HDRSystem::Shutdown();
	}

	void AdvancedRenderingManager::UpdateStats(float deltaTime)
	{
		s_Stats.frameTime = deltaTime;
		s_Stats.averageFPS = 1.0f / deltaTime;
		
		// Update min/max FPS
		if (s_Stats.minFPS == 0.0f || s_Stats.averageFPS < s_Stats.minFPS)
			s_Stats.minFPS = s_Stats.averageFPS;
		if (s_Stats.averageFPS > s_Stats.maxFPS)
			s_Stats.maxFPS = s_Stats.averageFPS;
		
		// Update memory usage
		s_Stats.textureMemory = RenderingUtils::GetGPUMemoryUsage();
		s_Stats.totalMemory = s_Stats.textureMemory + s_Stats.bufferMemory + s_Stats.shaderMemory;
	}

	void AdvancedRenderingManager::LoadDefaultPresets()
	{
		// Default presets are already defined in RenderingPresets
	}

	// RenderingPresets Implementation
	AdvancedRenderingConfig RenderingPresets::GetLowPreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = false;
		config.enablePostProcessing = false;
		config.enableShadowMapping = false;
		config.enableBloom = false;
		config.enableBlur = false;
		config.enableVignette = false;
		config.enableChromaticAberration = false;
		config.enableFilmGrain = false;
		config.enableMotionBlur = false;
		config.enableDepthOfField = false;
		config.enableSSAO = false;
		config.enableFXAA = false;
		config.enableSMAA = false;
		config.enableTAA = false;
		config.enableSoftShadows = false;
		config.enableCascadedShadows = false;
		config.enableAutoExposure = false;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = false;
		config.enableLOD = false;
		config.enableInstancing = false;
		config.enableGPUCulling = false;
		config.shadowMapResolution = ShadowMapResolution::Low;
		config.exposure = 1.0f;
		config.gamma = 2.2f;
		config.toneMapping = ToneMappingOperator::Linear;
		config.msaaSamples = 1;
		config.enableAnisotropicFiltering = false;
		config.anisotropicLevel = 1.0f;
		config.enableTextureCompression = false;
		config.enableMipmaps = false;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetMediumPreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = true;
		config.enablePostProcessing = true;
		config.enableShadowMapping = true;
		config.enableBloom = false;
		config.enableBlur = false;
		config.enableVignette = false;
		config.enableChromaticAberration = false;
		config.enableFilmGrain = false;
		config.enableMotionBlur = false;
		config.enableDepthOfField = false;
		config.enableSSAO = false;
		config.enableFXAA = true;
		config.enableSMAA = false;
		config.enableTAA = false;
		config.enableSoftShadows = true;
		config.enableCascadedShadows = false;
		config.enableAutoExposure = true;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = false;
		config.enableLOD = true;
		config.enableInstancing = true;
		config.enableGPUCulling = false;
		config.shadowMapResolution = ShadowMapResolution::Medium;
		config.exposure = 1.0f;
		config.gamma = 2.2f;
		config.toneMapping = ToneMappingOperator::Reinhard;
		config.msaaSamples = 2;
		config.enableAnisotropicFiltering = true;
		config.anisotropicLevel = 4.0f;
		config.enableTextureCompression = true;
		config.enableMipmaps = true;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetHighPreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = true;
		config.enablePostProcessing = true;
		config.enableShadowMapping = true;
		config.enableBloom = true;
		config.enableBlur = false;
		config.enableVignette = true;
		config.enableChromaticAberration = false;
		config.enableFilmGrain = false;
		config.enableMotionBlur = false;
		config.enableDepthOfField = false;
		config.enableSSAO = false;
		config.enableFXAA = true;
		config.enableSMAA = false;
		config.enableTAA = false;
		config.enableSoftShadows = true;
		config.enableCascadedShadows = true;
		config.enableAutoExposure = true;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = false;
		config.enableLOD = true;
		config.enableInstancing = true;
		config.enableGPUCulling = false;
		config.shadowMapResolution = ShadowMapResolution::High;
		config.exposure = 1.0f;
		config.gamma = 2.2f;
		config.toneMapping = ToneMappingOperator::ACES;
		config.msaaSamples = 4;
		config.enableAnisotropicFiltering = true;
		config.anisotropicLevel = 8.0f;
		config.enableTextureCompression = true;
		config.enableMipmaps = true;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetUltraPreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = true;
		config.enablePostProcessing = true;
		config.enableShadowMapping = true;
		config.enableBloom = true;
		config.enableBlur = true;
		config.enableVignette = true;
		config.enableChromaticAberration = true;
		config.enableFilmGrain = true;
		config.enableMotionBlur = true;
		config.enableDepthOfField = true;
		config.enableSSAO = true;
		config.enableFXAA = false;
		config.enableSMAA = true;
		config.enableTAA = true;
		config.enableSoftShadows = true;
		config.enableCascadedShadows = true;
		config.enableAutoExposure = true;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = true;
		config.enableLOD = true;
		config.enableInstancing = true;
		config.enableGPUCulling = true;
		config.shadowMapResolution = ShadowMapResolution::Ultra;
		config.exposure = 1.0f;
		config.gamma = 2.2f;
		config.toneMapping = ToneMappingOperator::ACES;
		config.msaaSamples = 8;
		config.enableAnisotropicFiltering = true;
		config.anisotropicLevel = 16.0f;
		config.enableTextureCompression = true;
		config.enableMipmaps = true;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetCinematicPreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = true;
		config.enablePostProcessing = true;
		config.enableShadowMapping = true;
		config.enableBloom = true;
		config.enableBlur = true;
		config.enableVignette = true;
		config.enableChromaticAberration = true;
		config.enableFilmGrain = true;
		config.enableMotionBlur = true;
		config.enableDepthOfField = true;
		config.enableSSAO = true;
		config.enableFXAA = false;
		config.enableSMAA = true;
		config.enableTAA = true;
		config.enableSoftShadows = true;
		config.enableCascadedShadows = true;
		config.enableAutoExposure = true;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = true;
		config.enableLOD = true;
		config.enableInstancing = true;
		config.enableGPUCulling = true;
		config.shadowMapResolution = ShadowMapResolution::Ultra;
		config.exposure = 1.2f;
		config.gamma = 2.4f;
		config.toneMapping = ToneMappingOperator::ACES;
		config.msaaSamples = 8;
		config.enableAnisotropicFiltering = true;
		config.anisotropicLevel = 16.0f;
		config.enableTextureCompression = true;
		config.enableMipmaps = true;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetPerformancePreset()
	{
		AdvancedRenderingConfig config;
		config.enableHDR = false;
		config.enablePostProcessing = false;
		config.enableShadowMapping = false;
		config.enableBloom = false;
		config.enableBlur = false;
		config.enableVignette = false;
		config.enableChromaticAberration = false;
		config.enableFilmGrain = false;
		config.enableMotionBlur = false;
		config.enableDepthOfField = false;
		config.enableSSAO = false;
		config.enableFXAA = false;
		config.enableSMAA = false;
		config.enableTAA = false;
		config.enableSoftShadows = false;
		config.enableCascadedShadows = false;
		config.enableAutoExposure = false;
		config.enableFrustumCulling = true;
		config.enableOcclusionCulling = false;
		config.enableLOD = true;
		config.enableInstancing = true;
		config.enableGPUCulling = false;
		config.shadowMapResolution = ShadowMapResolution::Low;
		config.exposure = 1.0f;
		config.gamma = 2.2f;
		config.toneMapping = ToneMappingOperator::Linear;
		config.msaaSamples = 1;
		config.enableAnisotropicFiltering = false;
		config.anisotropicLevel = 1.0f;
		config.enableTextureCompression = true;
		config.enableMipmaps = true;
		return config;
	}

	AdvancedRenderingConfig RenderingPresets::GetQualityPreset()
	{
		return GetUltraPreset();
	}

	std::vector<std::string> RenderingPresets::GetPresetNames()
	{
		return {
			"Low",
			"Medium", 
			"High",
			"Ultra",
			"Cinematic",
			"Performance",
			"Quality"
		};
	}

	AdvancedRenderingConfig RenderingPresets::GetPreset(const std::string& name)
	{
		if (name == "Low")
			return GetLowPreset();
		else if (name == "Medium")
			return GetMediumPreset();
		else if (name == "High")
			return GetHighPreset();
		else if (name == "Ultra")
			return GetUltraPreset();
		else if (name == "Cinematic")
			return GetCinematicPreset();
		else if (name == "Performance")
			return GetPerformancePreset();
		else if (name == "Quality")
			return GetQualityPreset();
		else
			return GetMediumPreset(); // Default
	}

	// RenderingUtils Implementation
	std::unordered_map<std::string, float> RenderingUtils::s_Timers;
	std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> RenderingUtils::s_StartTimes;

	void RenderingUtils::StartTimer(const std::string& name)
	{
		s_StartTimes[name] = std::chrono::high_resolution_clock::now();
	}

	void RenderingUtils::EndTimer(const std::string& name)
	{
		auto it = s_StartTimes.find(name);
		if (it != s_StartTimes.end())
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
			s_Timers[name] = duration.count() / 1000.0f; // Convert to milliseconds
			s_StartTimes.erase(it);
		}
	}

	float RenderingUtils::GetTimer(const std::string& name)
	{
		auto it = s_Timers.find(name);
		return (it != s_Timers.end()) ? it->second : 0.0f;
	}

	size_t RenderingUtils::GetGPUMemoryUsage()
	{
		// This would require OpenGL extensions to query GPU memory
		// For now, return a placeholder value
		return 0;
	}

	size_t RenderingUtils::GetCPUMemoryUsage()
	{
		// This would require platform-specific memory queries
		// For now, return a placeholder value
		return 0;
	}

	void RenderingUtils::LogMemoryUsage()
	{
		size_t gpuMemory = GetGPUMemoryUsage();
		size_t cpuMemory = GetCPUMemoryUsage();
		
		ZG_CORE_INFO("Memory Usage - GPU: {0} MB, CPU: {1} MB", gpuMemory / (1024 * 1024), cpuMemory / (1024 * 1024));
	}

	float RenderingUtils::CalculateQualityScore(const AdvancedRenderStats& stats)
	{
		float score = 0.0f;
		
		// FPS score (higher is better)
		if (stats.averageFPS > 60.0f)
			score += 30.0f;
		else if (stats.averageFPS > 30.0f)
			score += 20.0f;
		else if (stats.averageFPS > 15.0f)
			score += 10.0f;
		
		// Draw call efficiency (lower is better)
		if (stats.totalDrawCalls < 100)
			score += 25.0f;
		else if (stats.totalDrawCalls < 500)
			score += 15.0f;
		else if (stats.totalDrawCalls < 1000)
			score += 5.0f;
		
		// Memory efficiency (lower is better)
		if (stats.totalMemory < 100 * 1024 * 1024) // 100 MB
			score += 25.0f;
		else if (stats.totalMemory < 500 * 1024 * 1024) // 500 MB
			score += 15.0f;
		else if (stats.totalMemory < 1024 * 1024 * 1024) // 1 GB
			score += 5.0f;
		
		// Frame stability (lower frame drops is better)
		if (stats.frameDrops < 1.0f)
			score += 20.0f;
		else if (stats.frameDrops < 5.0f)
			score += 10.0f;
		else if (stats.frameDrops < 10.0f)
			score += 5.0f;
		
		return glm::clamp(score, 0.0f, 100.0f);
	}

	std::string RenderingUtils::GetQualityRecommendation(const AdvancedRenderStats& stats)
	{
		float score = CalculateQualityScore(stats);
		
		if (score >= 80.0f)
			return "Excellent performance! You can enable more advanced features.";
		else if (score >= 60.0f)
			return "Good performance. Consider enabling some advanced features.";
		else if (score >= 40.0f)
			return "Average performance. Consider reducing quality settings.";
		else if (score >= 20.0f)
			return "Poor performance. Reduce quality settings significantly.";
		else
			return "Very poor performance. Use low quality preset.";
	}

	bool RenderingUtils::IsFeatureSupported(const std::string& featureName)
	{
		// This would require OpenGL extension queries
		// For now, return true for all features
		return true;
	}

	std::vector<std::string> RenderingUtils::GetSupportedFeatures()
	{
		return {
			"HDR",
			"PostProcessing",
			"ShadowMapping",
			"Bloom",
			"Blur",
			"Vignette",
			"ChromaticAberration",
			"FilmGrain",
			"MotionBlur",
			"DepthOfField",
			"SSAO",
			"FXAA",
			"SMAA",
			"TAA",
			"SoftShadows",
			"CascadedShadows",
			"AutoExposure",
			"FrustumCulling",
			"OcclusionCulling",
			"LOD",
			"Instancing",
			"GPUCulling"
		};
	}

	std::vector<std::string> RenderingUtils::GetOptimizationSuggestions(const AdvancedRenderStats& stats)
	{
		std::vector<std::string> suggestions;
		
		if (stats.averageFPS < 30.0f)
		{
			suggestions.push_back("Reduce shadow map resolution");
			suggestions.push_back("Disable post-processing effects");
			suggestions.push_back("Enable LOD system");
			suggestions.push_back("Reduce texture quality");
		}
		
		if (stats.totalDrawCalls > 1000)
		{
			suggestions.push_back("Enable instancing");
			suggestions.push_back("Enable frustum culling");
			suggestions.push_back("Reduce object count");
		}
		
		if (stats.totalMemory > 1024 * 1024 * 1024) // 1 GB
		{
			suggestions.push_back("Enable texture compression");
			suggestions.push_back("Reduce texture resolution");
			suggestions.push_back("Enable mipmaps");
		}
		
		if (stats.frameDrops > 5.0f)
		{
			suggestions.push_back("Enable VSync");
			suggestions.push_back("Reduce frame rate target");
			suggestions.push_back("Enable adaptive quality");
		}
		
		return suggestions;
	}

	void RenderingUtils::ApplyOptimizationSuggestion(const std::string& suggestion)
	{
		if (suggestion == "Reduce shadow map resolution")
		{
			AdvancedRenderingManager::SetConfig(RenderingPresets::GetMediumPreset());
		}
		else if (suggestion == "Disable post-processing effects")
		{
			AdvancedRenderingManager::EnableFeature("PostProcessing", false);
		}
		else if (suggestion == "Enable LOD system")
		{
			AdvancedRenderingManager::EnableFeature("LOD", true);
		}
		else if (suggestion == "Enable instancing")
		{
			AdvancedRenderingManager::EnableFeature("Instancing", true);
		}
		else if (suggestion == "Enable frustum culling")
		{
			AdvancedRenderingManager::EnableFeature("FrustumCulling", true);
		}
		// Add more optimization suggestions as needed
	}

}
