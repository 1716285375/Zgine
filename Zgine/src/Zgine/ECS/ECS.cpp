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
                    }
                }
            }
        }
    }
};

class ECSManager::PhysicsSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        const glm::vec3 gravity = {0.0f, -9.81f, 0.0f};
        
        auto view = registry.view<Physics, Transform>();
        
        for (auto enttEntity : view) {
            auto& physics = view.get<Physics>(enttEntity);
            auto& transform = view.get<Transform>(enttEntity);
            
            if (physics.isStatic) continue;
            
            // 应用重力
            if (physics.affectedByGravity) {
                physics.acceleration += gravity;
            }
            
            // 更新速度
            physics.velocity += physics.acceleration * deltaTime;
            
            // 应用摩擦力
            physics.velocity *= (1.0f - physics.friction * deltaTime);
            
            // 更新位置
            transform.position += physics.velocity * deltaTime;
            
            // 重置加速度
            physics.acceleration = {0.0f, 0.0f, 0.0f};
        }
    }
};

class ECSManager::AudioSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Audio, Transform>();
        
        for (auto enttEntity : view) {
            auto& audio = view.get<Audio>(enttEntity);
            
            if (audio.playing) {
                // 这里应该调用音频引擎播放声音
                // 对于空间音频，需要根据Transform计算3D位置
                if (audio.spatial) {
                    auto& transform = view.get<Transform>(enttEntity);
                    // 设置3D音频位置
                }
            }
        }
    }
};

class ECSManager::HealthSystem {
public:
    void Update(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Health>();
        
        for (auto enttEntity : view) {
            auto& health = view.get<Health>(enttEntity);
            
            // 检查生命值状态
            if (health.current <= 0.0f && health.isAlive) {
                health.isAlive = false;
                // 可以触发死亡事件
            }
        }
    }
};

// ECSManager implementation
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
    EntityID id = m_NextEntityID++;
    m_RegistryImpl->registry.emplace<EntityID>(m_RegistryImpl->registry.create(), id);
    return Entity(id, this);
}

void ECSManager::DestroyEntity(Entity entity) {
    if (entity.IsValid()) {
        DestroyEntity(entity.GetID());
    }
}

void ECSManager::DestroyEntity(EntityID entityID) {
    auto view = m_RegistryImpl->registry.view<EntityID>();
    for (auto enttEntity : view) {
        if (view.get<EntityID>(enttEntity) == entityID) {
            m_RegistryImpl->registry.destroy(enttEntity);
            break;
        }
    }
}

void ECSManager::Update(float deltaTime) {
    m_MovementSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_AnimationSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_PhysicsSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_AudioSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_HealthSystem->Update(m_RegistryImpl->registry, deltaTime);
    m_RenderSystem->Update(m_RegistryImpl->registry, deltaTime);
}

size_t ECSManager::GetEntityCount() const {
    return m_RegistryImpl->registry.storage<EntityID>().size();
}

size_t ECSManager::GetComponentCount() const {
    return m_RegistryImpl->registry.storage<Position>().size() + 
           m_RegistryImpl->registry.storage<Velocity>().size() + 
           m_RegistryImpl->registry.storage<Renderable>().size() +
           m_RegistryImpl->registry.storage<Transform>().size() +
           m_RegistryImpl->registry.storage<Sprite>().size() +
           m_RegistryImpl->registry.storage<Animation>().size() +
           m_RegistryImpl->registry.storage<Physics>().size() +
           m_RegistryImpl->registry.storage<Audio>().size() +
           m_RegistryImpl->registry.storage<Health>().size() +
           m_RegistryImpl->registry.storage<Tag>().size();
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

// Entity query implementations
template<typename... Components>
std::vector<Entity> ECSManager::GetEntitiesWith() {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<Components...>();
    
    for (auto enttEntity : view) {
        // 查找对应的EntityID
        if (m_RegistryImpl->registry.all_of<EntityID>(enttEntity)) {
            EntityID id = m_RegistryImpl->registry.get<EntityID>(enttEntity);
            entities.emplace_back(id, this);
        }
    }
    
    return entities;
}

template<typename Component>
std::vector<Entity> ECSManager::GetEntitiesWithComponent() {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<Component>();
    
    for (auto enttEntity : view) {
        if (m_RegistryImpl->registry.all_of<EntityID>(enttEntity)) {
            EntityID id = m_RegistryImpl->registry.get<EntityID>(enttEntity);
            entities.emplace_back(id, this);
        }
    }
    
    return entities;
}

template<typename Component>
std::vector<Entity> ECSManager::GetEntitiesWithComponent() const {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<Component>();
    
    for (auto enttEntity : view) {
        if (m_RegistryImpl->registry.all_of<EntityID>(enttEntity)) {
            EntityID id = m_RegistryImpl->registry.get<EntityID>(enttEntity);
            entities.emplace_back(id, const_cast<ECSManager*>(this));
        }
    }
    
    return entities;
}

std::vector<Entity> ECSManager::GetEntitiesWithTag(const std::string& tag) {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<Tag>();
    
    for (auto enttEntity : view) {
        auto& tagComponent = view.get<Tag>(enttEntity);
        if (tagComponent.HasTag(tag)) {
            if (m_RegistryImpl->registry.all_of<EntityID>(enttEntity)) {
                EntityID id = m_RegistryImpl->registry.get<EntityID>(enttEntity);
                entities.emplace_back(id, this);
            }
        }
    }
    
    return entities;
}

std::vector<Entity> ECSManager::GetEntitiesWithName(const std::string& name) {
    std::vector<Entity> entities;
    auto view = m_RegistryImpl->registry.view<Tag>();
    
    for (auto enttEntity : view) {
        auto& tagComponent = view.get<Tag>(enttEntity);
        if (tagComponent.name == name) {
            if (m_RegistryImpl->registry.all_of<EntityID>(enttEntity)) {
                EntityID id = m_RegistryImpl->registry.get<EntityID>(enttEntity);
                entities.emplace_back(id, this);
            }
        }
    }
    
    return entities;
}

// Entity template implementations
template<typename Component>
void Entity::AddComponent(const Component& component) {
    if (m_Manager && IsValid()) {
        m_Manager->AddComponent(*this, component);
    }
}

template<typename Component>
Component& Entity::GetComponent() {
    if (m_Manager && IsValid()) {
        return m_Manager->GetComponent<Component>(*this);
    }
    static Component dummy;
    return dummy;
}

template<typename Component>
const Component& Entity::GetComponent() const {
    if (m_Manager && IsValid()) {
        return m_Manager->GetComponent<Component>(*this);
    }
    static Component dummy;
    return dummy;
}

template<typename Component>
bool Entity::HasComponent() const {
    if (m_Manager && IsValid()) {
        return m_Manager->HasComponent<Component>(*this);
    }
    return false;
}

template<typename Component>
void Entity::RemoveComponent() {
    if (m_Manager && IsValid()) {
        m_Manager->RemoveComponent<Component>(*this);
    }
}

// ECSManager template implementations
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
            m_RegistryImpl->registry.emplace<Component>(enttEntity, component);
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
            return m_RegistryImpl->registry.get<Component>(enttEntity);
        }
    }
    static Component dummy;
    return dummy; // Fallback for invalid entity
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
            return m_RegistryImpl->registry.get<Component>(enttEntity);
        }
    }
    static Component dummy;
    return dummy; // Fallback for invalid entity
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
    return m_RegistryImpl->registry.size();
}

size_t ECSManager::GetComponentCount() const {
    size_t total = 0;
    // 遍历所有组件类型并累加
    total += m_RegistryImpl->registry.size<Position>();
    total += m_RegistryImpl->registry.size<Velocity>();
    total += m_RegistryImpl->registry.size<Renderable>();
    total += m_RegistryImpl->registry.size<Transform>();
    total += m_RegistryImpl->registry.size<Sprite>();
    total += m_RegistryImpl->registry.size<Animation>();
    total += m_RegistryImpl->registry.size<Physics>();
    total += m_RegistryImpl->registry.size<Audio>();
    total += m_RegistryImpl->registry.size<Health>();
    total += m_RegistryImpl->registry.size<Tag>();
    return total;
}

size_t ECSManager::GetComponentCount(const std::string& componentName) const {
    if (componentName == "Position") return m_RegistryImpl->registry.size<Position>();
    if (componentName == "Velocity") return m_RegistryImpl->registry.size<Velocity>();
    if (componentName == "Renderable") return m_RegistryImpl->registry.size<Renderable>();
    if (componentName == "Transform") return m_RegistryImpl->registry.size<Transform>();
    if (componentName == "Sprite") return m_RegistryImpl->registry.size<Sprite>();
    if (componentName == "Animation") return m_RegistryImpl->registry.size<Animation>();
    if (componentName == "Physics") return m_RegistryImpl->registry.size<Physics>();
    if (componentName == "Audio") return m_RegistryImpl->registry.size<Audio>();
    if (componentName == "Health") return m_RegistryImpl->registry.size<Health>();
    if (componentName == "Tag") return m_RegistryImpl->registry.size<Tag>();
    return 0;
}
