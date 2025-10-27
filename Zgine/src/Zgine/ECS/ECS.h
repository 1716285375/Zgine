#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
#include <stdexcept>

namespace Zgine {
namespace ECS {

// Forward declarations
class ECSManager;
class Entity;

// Entity ID type - 封装EnTT的entity类型
using EntityID = uint32_t;
constexpr EntityID INVALID_ENTITY = 0;

// Basic ECS Components
struct Position {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
};

struct Velocity {
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
};

struct Renderable {
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    float scale = 1.0f;
};

// Transform Component - 包含位置、旋转、缩放
struct Transform {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // Euler angles in degrees
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    
    // 计算变换矩阵
    glm::mat4 GetMatrix() const;
};

// Sprite Component - 2D精灵渲染
struct Sprite {
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec2 size = {1.0f, 1.0f};
    glm::vec2 texCoordOffset = {0.0f, 0.0f};
    glm::vec2 texCoordScale = {1.0f, 1.0f};
    uint32_t textureID = 0; // 纹理ID
    bool visible = true;
};

// Animation Component - 动画控制
struct Animation {
    float currentTime = 0.0f;
    float duration = 1.0f;
    bool loop = true;
    bool playing = false;
    float speed = 1.0f;
    
    // 动画类型枚举
    enum Type {
        Position,
        Rotation,
        Scale,
        Color,
        SpriteFrame
    } type = Position;
    
    // 关键帧数据
    std::vector<float> keyTimes;
    std::vector<glm::vec4> keyValues; // 可以存储位置、颜色等
};

// Physics Component - 物理属性
struct Physics {
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};
    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.8f; // 弹性系数
    bool isStatic = false;
    bool affectedByGravity = true;
};

// Audio Component - 音频播放
struct Audio {
    uint32_t soundID = 0;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool loop = false;
    bool playing = false;
    bool spatial = false; // 3D空间音频
    float maxDistance = 100.0f;
};

// Health Component - 生命值
struct Health {
    float current = 100.0f;
    float maximum = 100.0f;
    bool isAlive = true;
    
    void TakeDamage(float damage) {
        current = std::max(0.0f, current - damage);
        isAlive = current > 0.0f;
    }
    
    void Heal(float amount) {
        current = std::min(maximum, current + amount);
        isAlive = current > 0.0f;
    }
};

// Tag Component - 实体标签
struct Tag {
    std::string name;
    std::vector<std::string> tags;
    
    bool HasTag(const std::string& tag) const {
        return std::find(tags.begin(), tags.end(), tag) != tags.end();
    }
    
    void AddTag(const std::string& tag) {
        if (!HasTag(tag)) {
            tags.push_back(tag);
        }
    }
    
    void RemoveTag(const std::string& tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    }
};

// Entity wrapper class - 封装EnTT entity
class Entity {
public:
    Entity() : m_ID(INVALID_ENTITY), m_Manager(nullptr) {}
    Entity(EntityID id, ECSManager* manager) : m_ID(id), m_Manager(manager) {}
    
    bool IsValid() const { return m_ID != INVALID_ENTITY && m_Manager != nullptr; }
    EntityID GetID() const { return m_ID; }
    
    // Component management
    template<typename Component>
    void AddComponent(const Component& component);
    
    template<typename Component>
    Component& GetComponent();
    
    template<typename Component>
    const Component& GetComponent() const;
    
    template<typename Component>
    bool HasComponent() const;
    
    template<typename Component>
    void RemoveComponent();
    
    // Operators
    bool operator==(const Entity& other) const { return m_ID == other.m_ID; }
    bool operator!=(const Entity& other) const { return m_ID != other.m_ID; }
    
private:
    EntityID m_ID;
    ECSManager* m_Manager;
};

// ECS Manager class - 封装EnTT registry
class ECSManager {
public:
    ECSManager();
    ~ECSManager();
    
    // Entity management
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    void DestroyEntity(EntityID entityID);
    
    // Component management
    template<typename Component>
    void AddComponent(Entity entity, const Component& component);
    
    template<typename Component>
    void AddComponent(EntityID entityID, const Component& component);
    
    template<typename Component>
    Component& GetComponent(Entity entity);
    
    template<typename Component>
    Component& GetComponent(EntityID entityID);
    
    template<typename Component>
    const Component& GetComponent(Entity entity) const;
    
    template<typename Component>
    const Component& GetComponent(EntityID entityID) const;
    
    template<typename Component>
    bool HasComponent(Entity entity) const;
    
    template<typename Component>
    bool HasComponent(EntityID entityID) const;
    
    template<typename Component>
    void RemoveComponent(Entity entity);
    
    template<typename Component>
    void RemoveComponent(EntityID entityID);
    
    // System updates
    void Update(float deltaTime);
    
    // Entity queries
    template<typename... Components>
    std::vector<Entity> GetEntitiesWith();
    
    template<typename Component>
    std::vector<Entity> GetEntitiesWithComponent();
    
    template<typename Component>
    std::vector<Entity> GetEntitiesWithComponent() const;
    
    // Tag queries
    std::vector<Entity> GetEntitiesWithTag(const std::string& tag);
    std::vector<Entity> GetEntitiesWithName(const std::string& name);
    
    // Statistics
    size_t GetEntityCount() const;
    size_t GetComponentCount() const;
    size_t GetComponentCount(const std::string& componentName) const;
    
private:
    // Forward declaration of EnTT registry
    class RegistryImpl;
    std::unique_ptr<RegistryImpl> m_RegistryImpl;
    
    // ECS Systems
    class MovementSystem;
    class RenderSystem;
    class AnimationSystem;
    class PhysicsSystem;
    class AudioSystem;
    class HealthSystem;
    
    std::unique_ptr<MovementSystem> m_MovementSystem;
    std::unique_ptr<RenderSystem> m_RenderSystem;
    std::unique_ptr<AnimationSystem> m_AnimationSystem;
    std::unique_ptr<PhysicsSystem> m_PhysicsSystem;
    std::unique_ptr<AudioSystem> m_AudioSystem;
    std::unique_ptr<HealthSystem> m_HealthSystem;
    
    // Entity ID counter
    EntityID m_NextEntityID = 1;
    
    // Performance optimization: Maintain entity count
    size_t m_EntityCount = 0;
    
    // Helper method to find EnTT entity by EntityID
    struct EnTTEntity {
        bool found = false;
        void* enttEntity = nullptr;
    };
    EnTTEntity FindEnTTEntity(EntityID entityID) const;
};

} // namespace ECS
} // namespace Zgine

// 包含序列化功能
#include "ECSSerialization.h"