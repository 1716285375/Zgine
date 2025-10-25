#pragma once

#include "Zgine/ImGui/ImGuiLayer.h"
#include "Zgine/Timestep.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Resources/AssetLoader.h"

namespace Sandbox {

class ResourceTestLayer : public Zgine::ImGuiLayer
{
public:
    ResourceTestLayer();
    virtual ~ResourceTestLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Zgine::Timestep ts) override;
    virtual void OnEvent(Zgine::Event& e) override;

protected:
    virtual void RenderCustomUI() override;

private:
    void RenderResourceManagerUI();
    void RenderTextureManagerUI();
    void RenderShaderManagerUI();
    void RenderModelManagerUI();
    void RenderAudioManagerUI();
    void RenderAssetLoaderUI();
    void RenderStatisticsUI();
    
    void LoadTestResources();
    void UnloadTestResources();
    void ReloadTestResources();

    // Asset Loader
    Zgine::Resources::AssetLoader m_AssetLoader;

    // UI State
    bool m_ShowResourceManager = true;
    bool m_ShowTextureManager = true;
    bool m_ShowShaderManager = true;
    bool m_ShowModelManager = true;
    bool m_ShowAudioManager = true;
    bool m_ShowAssetLoader = true;
    bool m_ShowStatistics = true;
    
    // Test Resources
    std::vector<std::string> m_TestTexturePaths;
    std::vector<std::string> m_TestShaderPaths;
    std::vector<std::string> m_TestModelPaths;
    std::vector<std::string> m_TestAudioPaths;
    
    // Audio Test
    uint32_t m_CurrentPlayID = 0;
    float m_AudioVolume = 1.0f;
    bool m_AudioLoop = false;
    
    // Async Loading Test
    std::vector<uint32_t> m_ActiveTasks;
    std::vector<std::string> m_LoadingResults;
    
    // Statistics
    float m_LastUpdateTime = 0.0f;
    std::string m_LastStats;
};

} // namespace Sandbox
