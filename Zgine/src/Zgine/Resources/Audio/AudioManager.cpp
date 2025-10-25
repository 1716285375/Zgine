#include "zgpch.h"
#include "AudioManager.h"
#include "MiniaudioBackend.h"
#include "../Core/ResourceBackendRegistry.h"
#include <glm/glm.hpp>

namespace Zgine {
namespace Resources {
namespace Audio {

    AudioManager::AudioManager() 
        : m_ResourceManager(nullptr)
        , m_AudioBackend(nullptr)
        , m_MasterVolume(1.0f)
        , m_BackendName("Miniaudio") {
        ZG_CORE_INFO("AudioManager created");
    }

    AudioManager::~AudioManager() {
        Shutdown();
    }

    bool AudioManager::Initialize() {
        ZG_CORE_INFO("Initializing AudioManager...");

        // 首先注册音频后端到ResourceBackendRegistry
        if (!RegisterAudioBackend()) {
            ZG_CORE_ERROR("Failed to register audio backend");
            return false;
        }

        // 然后创建资源管理器（此时ResourceBackendRegistry中已有后端）
        m_ResourceManager = new ResourceManager();
        if (!m_ResourceManager->Initialize()) {
            ZG_CORE_ERROR("Failed to initialize ResourceManager");
            return false;
        }

        // 从ResourceManager获取音频后端实例
        m_AudioBackend = std::static_pointer_cast<IAudioBackend>(
            m_ResourceManager->GetBackend("Miniaudio"));

        ZG_CORE_INFO("AudioManager initialized successfully");
        return true;
    }

    void AudioManager::Shutdown() {
        ZG_CORE_INFO("Shutting down AudioManager...");

        StopAllAudio();

        if (m_AudioBackend) {
            m_AudioBackend->Shutdown();
            m_AudioBackend = nullptr;
        }

        if (m_ResourceManager) {
            m_ResourceManager->Shutdown();
            delete m_ResourceManager;
            m_ResourceManager = nullptr;
        }

        ZG_CORE_INFO("AudioManager shutdown complete");
    }

    bool AudioManager::RegisterAudioBackend() {
        ZG_CORE_INFO("Registering audio backend: {}", m_BackendName);
        
        // 注册MiniaudioBackend工厂函数到ResourceBackendRegistry
        Core::ResourceBackendRegistry::GetInstance().RegisterBackend("Miniaudio", 
            []() -> Core::ResourceBackendRef { 
                return CreateRef<MiniaudioBackend>(); 
            }, 100);
        
        ZG_CORE_INFO("Audio backend registered successfully");
        return true;
    }

    void AudioManager::Update(float deltaTime) {
        if (m_ResourceManager) {
            m_ResourceManager->Update(deltaTime);
        }

        if (m_AudioBackend) {
            // 调用MiniaudioBackend的Update方法来清理已完成的播放
            auto miniaudioBackend = std::static_pointer_cast<MiniaudioBackend>(m_AudioBackend);
            if (miniaudioBackend) {
                miniaudioBackend->Update(deltaTime);
            }
        }
    }

    uint32_t AudioManager::PlayAudio(const std::string& path, bool loop, float volume) {
        if (!m_AudioBackend) {
            ZG_CORE_ERROR("Audio backend is not initialized");
            return 0;
        }

        // 检查资源是否已缓存
        ResourceRef resource = m_ResourceManager->GetResource(path);
        if (!resource) {
            // 如果未缓存，则加载资源
            resource = m_ResourceManager->LoadSync(path, ResourceType::Audio);
            if (!resource) {
                ZG_CORE_ERROR("Failed to load audio resource: {}", path);
                return 0;
            }
        }

        // 播放音频
        float finalVolume = volume * m_MasterVolume;
        uint32_t playID = m_AudioBackend->PlayAudio(resource, loop, finalVolume);
        
        if (playID != 0) {
            ZG_CORE_INFO("Playing audio: {} (ID: {}, Loop: {}, Volume: {})", path, playID, loop, finalVolume);
        } else {
            ZG_CORE_ERROR("Failed to play audio: {}", path);
        }

        return playID;
    }

    void AudioManager::StopAudio(uint32_t playID) {
        if (!m_AudioBackend) {
            ZG_CORE_WARN("Audio backend is not initialized");
            return;
        }

        m_AudioBackend->StopAudio(playID);
        ZG_CORE_INFO("Stopped audio with ID: {}", playID);
    }

    void AudioManager::PauseAudio(uint32_t playID) {
        if (!m_AudioBackend) {
            ZG_CORE_WARN("Audio backend is not initialized");
            return;
        }

        m_AudioBackend->PauseAudio(playID);
        ZG_CORE_INFO("Paused audio with ID: {}", playID);
    }

    void AudioManager::ResumeAudio(uint32_t playID) {
        if (!m_AudioBackend) {
            ZG_CORE_WARN("Audio backend is not initialized");
            return;
        }

        m_AudioBackend->ResumeAudio(playID);
        ZG_CORE_INFO("Resumed audio with ID: {}", playID);
    }

    void AudioManager::SetAudioVolume(uint32_t playID, float volume) {
        if (!m_AudioBackend) {
            ZG_CORE_WARN("Audio backend is not initialized");
            return;
        }

        float finalVolume = volume * m_MasterVolume;
        m_AudioBackend->SetAudioVolume(playID, finalVolume);
        ZG_CORE_INFO("Set volume for audio ID {} to {}", playID, finalVolume);
    }

    void AudioManager::StopAllAudio() {
        if (!m_AudioBackend) {
            ZG_CORE_WARN("Audio backend is not initialized");
            return;
        }

        m_AudioBackend->StopAllAudio();
        ZG_CORE_INFO("Stopped all audio playback");
    }

    std::vector<uint32_t> AudioManager::GetActivePlaybacks() const {
        if (!m_AudioBackend) {
            return {};
        }

        return m_AudioBackend->GetActivePlaybacks();
    }

    void AudioManager::SetMasterVolume(float volume) {
        m_MasterVolume = glm::clamp(volume, 0.0f, 1.0f);
        ZG_CORE_INFO("Set master volume to {}", m_MasterVolume);
    }

    float AudioManager::GetMasterVolume() const {
        return m_MasterVolume;
    }

    AudioBackendRef AudioManager::GetAudioBackend() const {
        return m_AudioBackend;
    }

    ResourceManager* AudioManager::GetResourceManager() const {
        return m_ResourceManager;
    }

    std::string AudioManager::GetStatistics() const {
        std::ostringstream stats;
        stats << "AudioManager Statistics:\n";
        stats << "  Backend: " << m_BackendName << "\n";
        stats << "  Master Volume: " << m_MasterVolume << "\n";
        
        if (m_ResourceManager) {
            stats << m_ResourceManager->GetStatistics();
        }
        
        if (m_AudioBackend) {
            auto backendStats = m_AudioBackend->GetStatistics();
            stats << "  Backend Statistics:\n";
            stats << "    Total Resources: " << backendStats.totalResources << "\n";
            stats << "    Loaded Resources: " << backendStats.loadedResources << "\n";
            stats << "    Failed Resources: " << backendStats.failedResources << "\n";
        }
        
        return stats.str();
    }

} // namespace Audio
} // namespace Resources
} // namespace Zgine