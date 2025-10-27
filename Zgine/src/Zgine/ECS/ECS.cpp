#include "zgpch.h" // Precompiled header
#include "ECS.h"
#include <entt/entt.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Zgine {
namespace ECS {

// Transform component implementation
glm::mat4 Transform::GetMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotation = glm::eulerAngleXYZ(
        glm::radians(this->rotation.x),
        glm::radians(this->rotation.y),
        glm::radians(this->rotation.z)
    );
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
    
    return translation * rotation * scale;
}

// Internal implementation using EnTT
class ECSManager::RegistryImpl {
public:
    entt::registry registry;
};

class ECSManager::MovementSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Position, Velocity>();
        
        for (auto enttEntity : view) {
            auto& pos = view.get<Position>(enttEntity);
            auto& vel = view.get<Velocity>(enttEntity);
            
            pos.position += vel.velocity * deltaTime;
        }
    }
};

class ECSManager::RenderSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        // 渲染系统通常不在这里更新，而是由渲染器调用
        // 这里可以做一些渲染相关的预处理
    }
};

class ECSManager::AnimationSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Animation, Transform>();
        
        for (auto enttEntity : view) {
            auto& anim = view.get<Animation>(enttEntity);
            auto& transform = view.get<Transform>(enttEntity);
            
            if (!anim.playing) continue;
            
            anim.currentTime += deltaTime * anim.speed;
            
            if (anim.currentTime >= anim.duration) {
                if (anim.loop) {
                    anim.currentTime = 0.0f;
                } else {
                    anim.playing = false;
                    anim.currentTime = anim.duration;
                }
            }
            
            // 插值计算当前值
            if (!anim.keyTimes.empty() && !anim.keyValues.empty()) {
                float normalizedTime = anim.currentTime / anim.duration;
                
                // 简单的线性插值
                if (anim.keyTimes.size() == 2 && anim.keyValues.size() == 2) {
                    float t = (normalizedTime - anim.keyTimes[0]) / (anim.keyTimes[1] - anim.keyTimes[0]);
                    t = std::max(0.0f, std::min(1.0f, t));
                    
                    glm::vec4 currentValue = glm::mix(anim.keyValues[0], anim.keyValues[1], t);
                    
                    // 根据动画类型应用到Transform
                    switch (anim.type) {
                        case Animation::Position:
                            transform.position = glm::vec3(currentValue);
                            break;
                        case Animation::Scale:
                            transform.scale = glm::vec3(currentValue);
                            break;
                        case Animation::Color:
                            // 颜色动画需要Sprite组件
                            if (registry.all_of<Sprite>(enttEntity)) {
                                auto& sprite = registry.get<Sprite>(enttEntity);
                                sprite.color = currentValue;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
};

class ECSManager::PhysicsSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Physics, Transform>();
        
        for (auto enttEntity : view) {
            auto& physics = view.get<Physics>(enttEntity);
            auto& transform = view.get<Transform>(enttEntity);
            
            if (physics.isStatic) continue;
            
            // 应用重力
            if (physics.affectedByGravity) {
                physics.velocity.y -= 9.81f * deltaTime; // 重力加速度
            }
            
            // 更新速度
            physics.velocity += physics.acceleration * deltaTime;
            
            // 应用摩擦力
            physics.velocity *= (1.0f - physics.friction * deltaTime);
            
            // 更新位置
            transform.position += physics.velocity * deltaTime;
            
            // 重置加速度
            physics.acceleration = glm::vec3(0.0f);
        }
    }
};

class ECSManager::AudioSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Audio>();
        
        for (auto enttEntity : view) {
            auto& audio = view.get<Audio>(enttEntity);
            
            // 音频系统更新逻辑
            // 这里可以处理音频的播放状态、音量控制等
        }
    }
};

class ECSManager::HealthSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Health>();
        
        for (auto enttEntity : view) {
            auto& health = view.get<Health>(enttEntity);
            
            // 健康系统更新逻辑
            // 这里可以处理生命值恢复、死亡检测等
        }
    }
};

// ECSManager Implementation
ECSManager::ECSManager() 
    : m_RegistryImpl(std::make_unique<RegistryImpl>())
    , m_MovementSystem(std::make_unique<MovementSystem>())
    , m_RenderSystem(std::make_unique<RenderSystem>())
    , m_AnimationSystem(std::make_unique<AnimationSystem>())
    , m_PhysicsSystem(std::make_unique<PhysicsSystem>())
    , m_AudioSystem(std::make_unique<AudioSystem>())
    , m_HealthSystem(std::make_unique<HealthSystem>())
{
}

ECSManager::~ECSManager() = default;

Entity ECSManager::CreateEntity() {
    auto enttEntity = m_RegistryImpl->registry.create();
    EntityID id = m_NextEntityID++;
    
    // 为实体添加EntityID组件
    m_RegistryImpl->registry.emplace<EntityID>(enttEntity, id);
    
    // 更新实体计数（性能优化）
    m_EntityCount++;
    
    return Entity(id, this);
}

void ECSManager::DestroyEntity(Entity entity) {
    if (entity.IsValid()) {
        DestroyEntity(entity.GetID());
    }
}

void ECSManager::DestroyEntity(EntityID entityID) {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    bool found = false;
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            m_RegistryImpl->registry.destroy(enttEntity);
            found = true;
            break;
        }
    }
    
    // 更新实体计数（性能优化）
    if (found && m_EntityCount > 0) {
        m_EntityCount--;
    }
}

void ECSManager::Update(float deltaTime) {
    // 更新各个系统
    m_MovementSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_AnimationSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_PhysicsSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_AudioSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_HealthSystem->Update(m_RegistryImpl->registry, deltaTime);
}

// Entity Implementation
template<typename Component>
void Entity::AddComponent(const Component& component) {
    if (IsValid()) {
        m_Manager->AddComponent(*this, component);
    }
}

template<typename Component>
Component& Entity::GetComponent() {
    return m_Manager->GetComponent<Component>(*this);
}

template<typename Component>
const Component& Entity::GetComponent() const {
    return m_Manager->GetComponent<Component>(*this);
}

template<typename Component>
bool Entity::HasComponent() const {
    return m_Manager->HasComponent<Component>(*this);
}

template<typename Component>
void Entity::RemoveComponent() {
    if (IsValid()) {
        m_Manager->RemoveComponent<Component>(*this);
    }
}

// ECSManager Template Methods
template<typename Component>
void ECSManager::AddComponent(Entity entity, const Component& component) {
    if (entity.IsValid()) {
        AddComponent(entity.GetID(), component);
    }
}

template<typename Component>
void ECSManager::AddComponent(EntityID entityID, const Component& component) {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            m_RegistryImpl->registry.emplace_or_replace<Component>(enttEntity, component);
            break;
        }
    }
}

template<typename Component>
Component& ECSManager::GetComponent(Entity entity) {
    return GetComponent<Component>(entity.GetID());
}

template<typename Component>
Component& ECSManager::GetComponent(EntityID entityID) {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            if (!m_RegistryImpl->registry.all_of<Component>(enttEntity)) {
                throw std::runtime_error("Entity does not have the requested component");
            }
            return m_RegistryImpl->registry.get<Component>(enttEntity);
        }
    }
    throw std::runtime_error("Entity not found");
}

template<typename Component>
const Component& ECSManager::GetComponent(Entity entity) const {
    return GetComponent<Component>(entity.GetID());
}

template<typename Component>
const Component& ECSManager::GetComponent(EntityID entityID) const {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            if (!m_RegistryImpl->registry.all_of<Component>(enttEntity)) {
                throw std::runtime_error("Entity does not have the requested component");
            }
            return m_RegistryImpl->registry.get<Component>(enttEntity);
        }
    }
    throw std::runtime_error("Entity not found");
}

template<typename Component>
bool ECSManager::HasComponent(Entity entity) const {
    return HasComponent<Component>(entity.GetID());
}

template<typename Component>
bool ECSManager::HasComponent(EntityID entityID) const {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            return m_RegistryImpl->registry.all_of<Component>(enttEntity);
        }
    }
    return false;
}

template<typename Component>
void ECSManager::RemoveComponent(Entity entity) {
    if (entity.IsValid()) {
        RemoveComponent<Component>(entity.GetID());
    }
}

template<typename Component>
void ECSManager::RemoveComponent(EntityID entityID) {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            m_RegistryImpl->registry.remove<Component>(enttEntity);
            break;
        }
    }
}

template<typename Component>
std::vector<Entity> ECSManager::GetEntitiesWithComponent() {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<EntityID, Component>();
    
    for (auto enttEntity : view) {
        EntityID id = view.get<EntityID>(enttEntity);
        entities.emplace_back(id, this);
    }
    
    return entities;
}

template<typename Component>
std::vector<Entity> ECSManager::GetEntitiesWithComponent() const {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<EntityID, Component>();
    
    for (auto enttEntity : view) {
        EntityID id = view.get<EntityID>(enttEntity);
        entities.emplace_back(id, const_cast<ECSManager*>(this));
    }
    
    return entities;
}

template<typename... Components>
std::vector<Entity> ECSManager::GetEntitiesWith() {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<EntityID, Components...>();
    
    for (auto enttEntity : view) {
        EntityID id = view.get<EntityID>(enttEntity);
        entities.emplace_back(id, this);
    }
    
    return entities;
}

std::vector<Entity> ECSManager::GetEntitiesWithTag(const std::string& tag) {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<EntityID, Tag>();
    
    for (auto enttEntity : view) {
        EntityID id = view.get<EntityID>(enttEntity);
        const auto& tagComponent = view.get<Tag>(enttEntity);
        
        if (tagComponent.HasTag(tag)) {
            entities.emplace_back(id, this);
        }
    }
    
    return entities;
}

std::vector<Entity> ECSManager::GetEntitiesWithName(const std::string& name) {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<EntityID, Tag>();
    
    for (auto enttEntity : view) {
        EntityID id = view.get<EntityID>(enttEntity);
        const auto& tagComponent = view.get<Tag>(enttEntity);
        
        if (tagComponent.name == name) {
            entities.emplace_back(id, this);
        }
    }
    
    return entities;
}

size_t ECSManager::GetEntityCount() const {
    // 返回维护的实体计数（性能优化：O(1)而不是O(n)）
    return m_EntityCount;
}

size_t ECSManager::GetComponentCount() const {
    size_t total = 0;
    // 遍历所有组件类型并累加
    total += m_RegistryImpl->registry.storage<Position>().size();
    total += m_RegistryImpl->registry.storage<Velocity>().size();
    total += m_RegistryImpl->registry.storage<Renderable>().size();
    total += m_RegistryImpl->registry.storage<Transform>().size();
    total += m_RegistryImpl->registry.storage<Sprite>().size();
    total += m_RegistryImpl->registry.storage<Animation>().size();
    total += m_RegistryImpl->registry.storage<Physics>().size();
    total += m_RegistryImpl->registry.storage<Audio>().size();
    total += m_RegistryImpl->registry.storage<Health>().size();
    total += m_RegistryImpl->registry.storage<Tag>().size();
    return total;
}

size_t ECSManager::GetComponentCount(const std::string& componentName) const {
    if (componentName == "Position") return m_RegistryImpl->registry.storage<Position>().size();
    if (componentName == "Velocity") return m_RegistryImpl->registry.storage<Velocity>().size();
    if (componentName == "Renderable") return m_RegistryImpl->registry.storage<Renderable>().size();
    if (componentName == "Transform") return m_RegistryImpl->registry.storage<Transform>().size();
    if (componentName == "Sprite") return m_RegistryImpl->registry.storage<Sprite>().size();
    if (componentName == "Animation") return m_RegistryImpl->registry.storage<Animation>().size();
    if (componentName == "Physics") return m_RegistryImpl->registry.storage<Physics>().size();
    if (componentName == "Audio") return m_RegistryImpl->registry.storage<Audio>().size();
    if (componentName == "Health") return m_RegistryImpl->registry.storage<Health>().size();
    if (componentName == "Tag") return m_RegistryImpl->registry.storage<Tag>().size();
    return 0;
}

// Helper method to find EnTT entity by EntityID
ECSManager::EnTTEntity ECSManager::FindEnTTEntity(EntityID entityID) const {
    EnTTEntity result;
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            result.found = true;
            result.enttEntity = &enttEntity;
            break;
        }
    }
    return result;
}

// Explicit template instantiations for all components
#define INSTANTIATE_COMPONENT_TEMPLATES(ComponentType) \
    template void Entity::AddComponent<ComponentType>(const ComponentType&); \
    template ComponentType& Entity::GetComponent<ComponentType>(); \
    template const ComponentType& Entity::GetComponent<ComponentType>() const; \
    template bool Entity::HasComponent<ComponentType>() const; \
    template void Entity::RemoveComponent<ComponentType>(); \
    template void ECSManager::AddComponent<ComponentType>(Entity, const ComponentType&); \
    template void ECSManager::AddComponent<ComponentType>(EntityID, const ComponentType&); \
    template ComponentType& ECSManager::GetComponent<ComponentType>(Entity); \
    template ComponentType& ECSManager::GetComponent<ComponentType>(EntityID); \
    template const ComponentType& ECSManager::GetComponent<ComponentType>(Entity) const; \
    template const ComponentType& ECSManager::GetComponent<ComponentType>(EntityID) const; \
    template bool ECSManager::HasComponent<ComponentType>(Entity) const; \
    template bool ECSManager::HasComponent<ComponentType>(EntityID) const; \
    template void ECSManager::RemoveComponent<ComponentType>(Entity); \
    template void ECSManager::RemoveComponent<ComponentType>(EntityID); \
    template std::vector<Entity> ECSManager::GetEntitiesWithComponent<ComponentType>(); \
    template std::vector<Entity> ECSManager::GetEntitiesWithComponent<ComponentType>() const;

INSTANTIATE_COMPONENT_TEMPLATES(Position)
INSTANTIATE_COMPONENT_TEMPLATES(Velocity)
INSTANTIATE_COMPONENT_TEMPLATES(Renderable)
INSTANTIATE_COMPONENT_TEMPLATES(Transform)
INSTANTIATE_COMPONENT_TEMPLATES(Sprite)
INSTANTIATE_COMPONENT_TEMPLATES(Animation)
INSTANTIATE_COMPONENT_TEMPLATES(Physics)
INSTANTIATE_COMPONENT_TEMPLATES(Audio)
INSTANTIATE_COMPONENT_TEMPLATES(Health)
INSTANTIATE_COMPONENT_TEMPLATES(Tag)

// Multi-component query instantiations
template std::vector<Entity> ECSManager::GetEntitiesWith<Position, Velocity>();
template std::vector<Entity> ECSManager::GetEntitiesWith<Transform, Sprite>();
template std::vector<Entity> ECSManager::GetEntitiesWith<Physics, Transform>();
template std::vector<Entity> ECSManager::GetEntitiesWith<Animation, Transform>();
template std::vector<Entity> ECSManager::GetEntitiesWith<Health, Tag>();

} // namespace ECS
} // namespace Zgine
