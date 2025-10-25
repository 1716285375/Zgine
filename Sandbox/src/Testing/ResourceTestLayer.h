#pragma once

#include "Zgine/ImGui/ImGuiLayer.h"
#include "Zgine/Timestep.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Resources/Audio/AudioManager.h"

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
    void RenderAudioManagerUI();

    // Audio Manager
    Zgine::Resources::Audio::AudioManager* m_AudioManager;

    // UI State
    bool m_ShowResourceManager = true;
    
    // Audio Test Variables
    float m_AudioVolume = 0.5f;
    bool m_AudioLoop = false;
    uint32_t m_CurrentPlayID = 0;
    
    // Test Resources
    std::vector<std::string> m_TestAudioPaths;
};

} // namespace Sandbox