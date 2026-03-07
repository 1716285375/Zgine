#include <Zgine/Audio/AudioSystem.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/World/Camera/Camera.h>
#include <miniaudio.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Zgine {

AudioSystem::AudioSystem() {
}

AudioSystem::~AudioSystem() {
    Shutdown();
}

void AudioSystem::Initialize() {
    if (m_Initialized) {
        return;
    }

    // 创建 miniaudio 引擎
    ma_engine* engine = new ma_engine();
    ma_engine_config config = ma_engine_config_init();
    config.listenerCount = 1; // 支持一个监听器

    ma_result result = ma_engine_init(&config, engine);
    if (result != MA_SUCCESS) {
        ZGINE_CORE_ERROR("Failed to initialize miniaudio engine: {}", static_cast<int>(result));
        delete engine;
        return;
    }

    m_Engine = engine;
    m_Initialized = true;
    ZGINE_CORE_INFO("Audio System Initialized");
}

void AudioSystem::Shutdown() {
    if (!m_Initialized) {
        return;
    }

    OnSceneStop();

    // 关闭引擎
    if (m_Engine) {
        ma_engine* engine = static_cast<ma_engine*>(m_Engine);
        ma_engine_uninit(engine);
        delete engine;
        m_Engine = nullptr;
    }

    m_Initialized = false;
    ZGINE_CORE_INFO("Audio System Shutdown");
}

void AudioSystem::OnSceneStart(World* World) {
    if (!m_Initialized) {
        Initialize();
    }

    m_World = World;

    // 遍历所有有 AudioSourceComponent 的实体，创建音频源
    if (World) {
        auto& registry = World->GetRegistry();
        auto view = registry.view<AudioSourceComponent>();

        for (auto entity : view) {
            CreateAudioSource(Entity(entity, World));
        }
    }

    ZGINE_CORE_INFO("Audio System: World started");
}

void AudioSystem::OnSceneStop() {
    if (!m_Initialized || !m_World) {
        return;
    }

    // 销毁所有音频源
    auto& registry = m_World->GetRegistry();
    auto view = registry.view<AudioSourceComponent>();

    for (auto entity : view) {
        DestroyAudioSource(Entity(entity, m_World));
    }

    m_World = nullptr;
    ZGINE_CORE_INFO("Audio System: World stopped");
}

void AudioSystem::Update(World* World, float deltaTime) {
    if (!m_Initialized || !m_Engine) {
        return;
    }

    ZGINE_UNUSED(deltaTime);

    // 更新监听器位置和方向
    UpdateListener(World);

    // 更新所有音频源的位置（3D 空间音频）
    if (World) {
        auto& registry = World->GetRegistry();
        auto view = registry.view<AudioSourceComponent, TransformComponent>();

        for (auto entity : view) {
            UpdateAudioSourcePosition(Entity(entity, World));
        }
    }
}

void AudioSystem::CreateAudioSource(Entity entity) {
    if (!m_Initialized || !m_Engine || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    // 如果已经创建过，先销毁
    if (audioSource.RuntimeSourcePtr) {
        DestroyAudioSource(entity);
    }

    // 加载音频文件
    if (audioSource.FilePath.empty()) {
        ZGINE_CORE_WARN("AudioSource has no file path");
        return;
    }

    // 创建声音对象
    ma_sound* sound = new ma_sound();
    ma_engine* engine = static_cast<ma_engine*>(m_Engine);
    ma_result result = ma_sound_init_from_file(engine, audioSource.FilePath.c_str(),
                                                MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, sound);

    if (result != MA_SUCCESS) {
        ZGINE_CORE_ERROR("Failed to create sound from file: {} (error: {})",
                         audioSource.FilePath, static_cast<int>(result));
        delete sound;
        return;
    }

    // 配置声音属性
    ma_sound_set_volume(sound, audioSource.Volume);
    ma_sound_set_pitch(sound, audioSource.Pitch);
    ma_sound_set_looping(sound, audioSource.IsLooping ? MA_TRUE : MA_FALSE);

    // 设置 3D 空间音频
    if (audioSource.Spatialized) {
        ma_sound_set_spatialization_enabled(sound, MA_TRUE);
        ma_sound_set_min_distance(sound, audioSource.MinDistance);
        ma_sound_set_max_distance(sound, audioSource.MaxDistance);
    }

    // 更新位置
    UpdateAudioSourcePosition(entity);

    audioSource.RuntimeSourcePtr = sound;

    // 如果应该自动播放
    if (audioSource.IsPlaying) {
        PlayAudioSource(entity);
    }

    ZGINE_CORE_TRACE("Created audio source for entity: {}", audioSource.FilePath);
}

void AudioSystem::DestroyAudioSource(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    if (audioSource.RuntimeSourcePtr) {
        ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);

        // 停止播放
        ma_sound_stop(sound);

        // 销毁声音对象
        ma_sound_uninit(sound);
        delete sound;

        audioSource.RuntimeSourcePtr = nullptr;
    }
}

void AudioSystem::PlayAudioSource(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    if (audioSource.RuntimeSourcePtr) {
        ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);
        ma_result result = ma_sound_start(sound);

        if (result == MA_SUCCESS) {
            audioSource.IsPlaying = true;
        }
    }
}

void AudioSystem::StopAudioSource(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    if (audioSource.RuntimeSourcePtr) {
        ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);
        ma_sound_stop(sound);
        audioSource.IsPlaying = false;
    }
}

void AudioSystem::PauseAudioSource(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    if (audioSource.RuntimeSourcePtr) {
        ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);
        ma_sound_stop(sound);
        audioSource.IsPlaying = false;
    }
}

void AudioSystem::ApplyAudioSourceSettings(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();
    if (!audioSource.RuntimeSourcePtr) {
        return;
    }

    ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);
    ma_sound_set_volume(sound, audioSource.Volume);
    ma_sound_set_pitch(sound, audioSource.Pitch);
    ma_sound_set_looping(sound, audioSource.IsLooping ? MA_TRUE : MA_FALSE);
    ma_sound_set_spatialization_enabled(sound, audioSource.Spatialized ? MA_TRUE : MA_FALSE);

    if (audioSource.Spatialized) {
        ma_sound_set_min_distance(sound, audioSource.MinDistance);
        ma_sound_set_max_distance(sound, audioSource.MaxDistance);
    }

    if (audioSource.IsPlaying) {
        if (!ma_sound_is_playing(sound)) {
            ma_sound_start(sound);
        }
    } else if (ma_sound_is_playing(sound)) {
        ma_sound_stop(sound);
    }
}

void AudioSystem::RecreateAudioSource(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>()) {
        return;
    }

    DestroyAudioSource(entity);
    CreateAudioSource(entity);
}

void AudioSystem::UpdateAudioSourcePosition(Entity entity) {
    if (!m_Initialized || !entity.HasComponent<AudioSourceComponent>() ||
        !entity.HasComponent<TransformComponent>()) {
        return;
    }

    auto& audioSource = entity.GetComponent<AudioSourceComponent>();

    if (!audioSource.RuntimeSourcePtr || !audioSource.Spatialized) {
        return;
    }

    auto& transform = entity.GetComponent<TransformComponent>();
    ma_sound* sound = static_cast<ma_sound*>(audioSource.RuntimeSourcePtr);

    // 设置 3D 位置
    ma_sound_set_position(sound,
        transform.Translation.x,
        transform.Translation.y,
        transform.Translation.z);
}

void AudioSystem::UpdateListener(World* World) {
    if (!m_Initialized || !m_Engine || !World) {
        return;
    }

    // 查找主摄像机（带 AudioListenerComponent）
    auto& registry = World->GetRegistry();
    auto listenerView = registry.view<AudioListenerComponent, CameraComponent, TransformComponent>();

    if (listenerView.begin() == listenerView.end()) {
        // 如果没有找到监听器，尝试使用主摄像机
        auto cameraView = registry.view<CameraComponent, TransformComponent>();
        for (auto entity : cameraView) {
            auto& camera = cameraView.get<CameraComponent>(entity);
            if (camera.Primary) {
                auto& transform = cameraView.get<TransformComponent>(entity);

                // 计算前方向量（从旋转）
                glm::vec3 forward = glm::normalize(glm::vec3(
                    glm::cos(glm::radians(transform.Rotation.y)) * glm::cos(glm::radians(transform.Rotation.x)),
                    glm::sin(glm::radians(transform.Rotation.x)),
                    glm::sin(glm::radians(transform.Rotation.y)) * glm::cos(glm::radians(transform.Rotation.x))
                ));

                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    ma_engine* engine = static_cast<ma_engine*>(m_Engine);
    ma_engine_listener_set_position(engine, 0,
        transform.Translation.x, transform.Translation.y, transform.Translation.z);
    ma_engine_listener_set_direction(engine, 0,
        forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(engine, 0,
        up.x, up.y, up.z);
    return;
            }
        }
        return;
    }

    // 使用第一个监听器
    auto entity = *listenerView.begin();
    auto& transform = listenerView.get<TransformComponent>(entity);

    // 计算前方向量
    glm::vec3 forward = glm::normalize(glm::vec3(
        glm::cos(glm::radians(transform.Rotation.y)) * glm::cos(glm::radians(transform.Rotation.x)),
        glm::sin(glm::radians(transform.Rotation.x)),
        glm::sin(glm::radians(transform.Rotation.y)) * glm::cos(glm::radians(transform.Rotation.x))
    ));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    ma_engine* engine = static_cast<ma_engine*>(m_Engine);
    ma_engine_listener_set_position(engine, 0,
        transform.Translation.x, transform.Translation.y, transform.Translation.z);
    ma_engine_listener_set_direction(engine, 0,
        forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(engine, 0,
        up.x, up.y, up.z);
}

}

