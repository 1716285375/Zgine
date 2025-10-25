#include "sandbox_pch.h"
#include "ResourceTestLayer.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/ImGui/ImGuiWrapper.h"

namespace Sandbox {

    ResourceTestLayer::ResourceTestLayer()
        : ImGuiLayer()
        , m_AudioManager(nullptr)
        , m_AudioVolume(0.5f)
        , m_AudioLoop(false)
        , m_CurrentPlayID(0)
    {
        ZG_CORE_INFO("Resource Test Layer created");
    }

    void ResourceTestLayer::OnAttach()
    {
        ZG_CORE_INFO("Resource Test Layer attached");
        
        // 初始化AudioManager
        m_AudioManager = new Zgine::Resources::Audio::AudioManager();
        if (!m_AudioManager->Initialize()) {
            ZG_CORE_ERROR("Failed to initialize AudioManager");
            delete m_AudioManager;
            m_AudioManager = nullptr;
        }
        
        // 设置测试资源路径 - 使用绝对路径
        m_TestAudioPaths = {
            "C:/C/Zgine/assets/audio/test.wav",
            "C:/C/Zgine/assets/audio/background.mp3"
        };
    }

    void ResourceTestLayer::OnDetach()
    {
        ZG_CORE_INFO("Resource Test Layer detached");
        if (m_AudioManager) {
            m_AudioManager->Shutdown();
            delete m_AudioManager;
            m_AudioManager = nullptr;
        }
    }

    void ResourceTestLayer::OnUpdate(Zgine::Timestep ts)
    {
        // 更新AudioManager
        if (m_AudioManager) {
            m_AudioManager->Update(ts.GetSeconds());
        }
    }

    void ResourceTestLayer::OnEvent(Zgine::Event& e)
    {
        // Handle events if necessary
    }

    void ResourceTestLayer::RenderCustomUI()
    {
        if (Zgine::IG::Begin("Audio Management Test", &m_ShowResourceManager))
        {
            RenderAudioManagerUI();
        }
        Zgine::IG::End();
    }

    void ResourceTestLayer::RenderAudioManagerUI()
    {
        Zgine::IG::Text("Audio Manager Test");
        Zgine::IG::Separator();
        
        if (!m_AudioManager) {
            Zgine::IG::Text("AudioManager not initialized!");
            return;
        }
        
        // 音频播放控制
        if (Zgine::IG::CollapsingHeader("Audio Playback"))
        {
            Zgine::IG::SliderFloat("Volume", &m_AudioVolume, 0.0f, 1.0f);
            Zgine::IG::Checkbox("Loop", &m_AudioLoop);
            
        if (Zgine::IG::Button("Play Test Audio"))
        {
            uint32_t playID = m_AudioManager->PlayAudio("assets/audio/test.wav", m_AudioLoop, m_AudioVolume);
            if (playID != 0) {
                m_CurrentPlayID = playID;
                ZG_CORE_INFO("Audio playback started with ID: {}", playID);
            } else {
                ZG_CORE_ERROR("Failed to start audio playback");
            }
        }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Pause Audio"))
            {
                if (m_CurrentPlayID != 0) {
                    m_AudioManager->PauseAudio(m_CurrentPlayID);
                    ZG_CORE_INFO("Paused audio with ID: {}", m_CurrentPlayID);
                }
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Resume Audio"))
            {
                if (m_CurrentPlayID != 0) {
                    m_AudioManager->ResumeAudio(m_CurrentPlayID);
                    ZG_CORE_INFO("Resumed audio with ID: {}", m_CurrentPlayID);
                }
            }
            
            if (Zgine::IG::Button("Stop Audio"))
            {
                if (m_CurrentPlayID != 0) {
                    m_AudioManager->StopAudio(m_CurrentPlayID);
                    ZG_CORE_INFO("Stopped audio with ID: {}", m_CurrentPlayID);
                    m_CurrentPlayID = 0;
                }
            }
            
            Zgine::IG::SameLine();
            if (Zgine::IG::Button("Stop All Audio"))
            {
                m_AudioManager->StopAllAudio();
                m_CurrentPlayID = 0;
                ZG_CORE_INFO("Stopped all audio");
            }
        }
        
        // 音频文件测试
        if (Zgine::IG::CollapsingHeader("Audio Files"))
        {
            for (size_t i = 0; i < m_TestAudioPaths.size(); ++i) {
                Zgine::IG::Text("File %d: %s", i + 1, m_TestAudioPaths[i].c_str());
                Zgine::IG::SameLine();
                
                std::string playButton = "Play " + std::to_string(i + 1);
                if (Zgine::IG::Button(playButton.c_str())) {
                    uint32_t playID = m_AudioManager->PlayAudio(m_TestAudioPaths[i], false, m_AudioVolume);
                    if (playID != 0) {
                        ZG_CORE_INFO("Playing audio file %d with ID: {}", i + 1, playID);
                    }
                }
            }
        }
    }

} // namespace Sandbox