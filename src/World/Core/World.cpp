#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/EntityManager.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include "WorldRegistryAccess.h"
#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>

namespace Zgine {

namespace {
    template <typename T>
    void CopyComponentIfExists(Entity dst, Entity src) {
        if (!src.HasComponent<T>()) {
            return;
        }
        if constexpr (std::is_empty_v<T>) {
            if (!dst.HasComponent<T>()) {
                dst.AddComponent<T>();
            }
        } else {
            if (dst.HasComponent<T>()) {
                dst.GetComponent<T>() = src.GetComponent<T>();
            } else {
                dst.AddComponent<T>(src.GetComponent<T>());
            }
        }
    }

    bool IsDescendant(const World* world, EntityHandle child, EntityHandle possibleParent) {
        if (!world || !possibleParent) {
            return false;
        }

        World* mutableWorld = const_cast<World*>(world);
        if (!world->IsEntityValid(Entity(child, mutableWorld)) ||
            !world->IsEntityValid(Entity(possibleParent, mutableWorld))) {
            return false;
        }

        const auto& registry = Internal::GetRegistry(*world);
        auto* rel = registry.try_get<RelationshipComponent>(Internal::ToEnTT(possibleParent));
        if (!rel) {
            return false;
        }

        for (const auto& descendant : rel->Children) {
            if (descendant == child) {
                return true;
            }
            if (IsDescendant(world, child, descendant)) {
                return true;
            }
        }
        return false;
    }

    void ResetRuntimeOnlyComponentState(Entity entity) {
        if (entity.HasComponent<CameraComponent>()) {
            auto& camera = entity.GetComponent<CameraComponent>();
            if (camera.Camera) {
                camera.Camera = std::make_shared<Camera>(*camera.Camera);
            }
        }
        if (entity.HasComponent<RigidbodyComponent>()) {
            entity.GetComponent<RigidbodyComponent>().RuntimeBody.Reset();
        }
        if (entity.HasComponent<AudioSourceComponent>()) {
            auto& audio = entity.GetComponent<AudioSourceComponent>();
            audio.RuntimeSourcePtr = nullptr;
            audio.IsPlaying = false;
        }
        if (entity.HasComponent<ScriptComponent>()) {
            entity.GetComponent<ScriptComponent>().IsInitialized = false;
        }
        if (entity.HasComponent<PBRMaterialComponent>()) {
            auto& material = entity.GetComponent<PBRMaterialComponent>();
            material.AlbedoTexture.reset();
            material.NormalTexture.reset();
            material.MetallicTexture.reset();
            material.RoughnessTexture.reset();
            material.AOTexture.reset();
        }
    }

    void CopyCloneComponents(Entity dst, Entity src) {
        CopyComponentIfExists<IDComponent>(dst, src);
        CopyComponentIfExists<TagComponent>(dst, src);
        CopyComponentIfExists<TransformComponent>(dst, src);
        CopyComponentIfExists<CameraComponent>(dst, src);
        CopyComponentIfExists<PrimitiveComponent>(dst, src);
        CopyComponentIfExists<SpriteRendererComponent>(dst, src);
        CopyComponentIfExists<ColorComponent>(dst, src);
        CopyComponentIfExists<MeshComponent>(dst, src);
        CopyComponentIfExists<RigidbodyComponent>(dst, src);
        CopyComponentIfExists<BoxColliderComponent>(dst, src);
        CopyComponentIfExists<CircleColliderComponent>(dst, src);
        CopyComponentIfExists<AudioSourceComponent>(dst, src);
        CopyComponentIfExists<AudioListenerComponent>(dst, src);
        CopyComponentIfExists<ScriptComponent>(dst, src);
        CopyComponentIfExists<PBRMaterialComponent>(dst, src);
        CopyComponentIfExists<DirectionalLightComponent>(dst, src);
        CopyComponentIfExists<PointLightComponent>(dst, src);
        CopyComponentIfExists<SpotLightComponent>(dst, src);

        ResetRuntimeOnlyComponentState(dst);
    }

    Entity CloneEntityHierarchy(Entity src, World& dstWorld, Entity dstParent = Entity()) {
        if (!src) {
            return Entity();
        }

        const std::string name = src.HasComponent<TagComponent>()
            ? src.GetComponent<TagComponent>().Tag
            : std::string("Entity");
        Entity dst = dstWorld.CreateEntity(name);
        CopyCloneComponents(dst, src);

        if (dstParent) {
            dstWorld.SetParent(dst, dstParent);
        }

        if (src.HasComponent<RelationshipComponent>()) {
            const auto& rel = src.GetComponent<RelationshipComponent>();
            for (const auto& child : rel.Children) {
                CloneEntityHierarchy(Entity(child, src.GetWorld()), dstWorld, dst);
            }
        }

        return dst;
    }
}

World::World()
    : m_Storage(std::make_unique<Storage>())
    , m_EntityManager(std::make_unique<EntityManager>(*this))
{
}

World::~World() {
    // Release ECS objects while World services are still alive and the order is explicit.
    Clear();
}

Entity World::CreateEntity(const std::string& name) {
    EntityHandle handle = m_EntityManager->Create(name);
    return Entity(handle, this);
}

Entity World::CreateEntity(const std::string& name, Entity parent) {
    Entity entity = CreateEntity(name);
    if (parent) {
        SetParent(entity, parent);
    }
    return entity;
}

void World::DestroyEntity(Entity entity) {
    if (!entity) {
        return;
    }

    EntityHandle handle = entity.GetHandle();
    m_EntityManager->Destroy(handle);
}

void World::Clear() {
    m_EntityManager->Clear();
}

std::vector<Entity> World::GetAllEntities() const {
    std::vector<Entity> result;
    const auto& registry = Internal::GetRegistry(*this);
    result.reserve(GetEntityCount());
    auto view = registry.view<IDComponent>();
    for (auto entity : view) {
        result.emplace_back(Internal::FromEnTT(entity), const_cast<World*>(this));
    }
    return result;
}

std::vector<Entity> World::GetRootEntities() const {
    std::vector<Entity> result;
    const auto& registry = Internal::GetRegistry(*this);
    auto view = registry.view<TagComponent, RelationshipComponent>();
    for (auto entity : view) {
        const auto& rel = view.get<RelationshipComponent>(entity);
        if (!rel.Parent) {
            result.emplace_back(Internal::FromEnTT(entity), const_cast<World*>(this));
        }
    }
    return result;
}

std::vector<Entity> World::GetChildren(Entity entity) const {
    std::vector<Entity> result;
    if (!entity || !entity.HasComponent<RelationshipComponent>()) {
        return result;
    }

    const auto& rel = entity.GetComponent<RelationshipComponent>();
    result.reserve(rel.Children.size());
    for (const auto& child : rel.Children) {
        result.emplace_back(child, const_cast<World*>(this));
    }
    return result;
}

size_t World::GetEntityCount() const {
    return m_EntityManager->GetEntityCount();
}

bool World::IsEntityValid(Entity entity) const {
    if (!entity) {
        return false;
    }
    return Internal::GetRegistry(*this).valid(Internal::ToEnTT(entity.GetHandle()));
}

void World::SetParent(Entity child, Entity parent) {
    if (!child || child == parent) {
        return;
    }

    if (!IsEntityValid(child)) {
        return;
    }

    auto& registry = Internal::GetRegistry(*this);
    EntityHandle childHandle = child.GetHandle();
    EntityHandle parentHandle = parent ? parent.GetHandle() : EntityHandle();
    entt::entity childEntity = Internal::ToEnTT(childHandle);
    entt::entity parentEntity = Internal::ToEnTT(parentHandle);

    if (parent && IsDescendant(this, parentHandle, childHandle)) {
        ZGINE_CORE_WARN("World::SetParent failed: cannot parent entity to its descendant.");
        return;
    }

    if (!registry.all_of<RelationshipComponent>(childEntity)) {
        registry.emplace<RelationshipComponent>(childEntity);
    }
    auto& childRel = registry.get<RelationshipComponent>(childEntity);
    if (childRel.Parent && IsEntityValid(Entity(childRel.Parent, this))) {
        auto& oldParentRel = registry.get<RelationshipComponent>(Internal::ToEnTT(childRel.Parent));
        oldParentRel.Children.erase(
            std::remove(oldParentRel.Children.begin(), oldParentRel.Children.end(), childHandle),
            oldParentRel.Children.end());
    }

    if (parent) {
        if (!registry.all_of<RelationshipComponent>(parentEntity)) {
            registry.emplace<RelationshipComponent>(parentEntity);
        }
        auto& parentRel = registry.get<RelationshipComponent>(parentEntity);
        if (std::find(parentRel.Children.begin(), parentRel.Children.end(), childHandle) == parentRel.Children.end()) {
            parentRel.Children.push_back(childHandle);
        }
        childRel.Parent = parentHandle;
    } else {
        childRel.Parent = EntityHandle();
    }
}

void World::ClearParent(Entity child) {
    SetParent(child, Entity());
}

Entity World::DuplicateEntity(Entity source) {
    if (!source) {
        return Entity();
    }

    if (!IsEntityValid(source)) {
        return Entity();
    }

    const std::string baseName = source.HasComponent<TagComponent>()
        ? source.GetComponent<TagComponent>().Tag
        : std::string("Entity");
    Entity copy = CreateEntity(baseName + " Copy");

    CopyComponentIfExists<TagComponent>(copy, source);
    if (copy.HasComponent<TagComponent>()) {
        copy.GetComponent<TagComponent>().Tag = baseName + " Copy";
    }
    CopyComponentIfExists<TransformComponent>(copy, source);
    CopyComponentIfExists<CameraComponent>(copy, source);
    CopyComponentIfExists<PrimitiveComponent>(copy, source);
    CopyComponentIfExists<ColorComponent>(copy, source);
    CopyComponentIfExists<MeshComponent>(copy, source);
    CopyComponentIfExists<RigidbodyComponent>(copy, source);
    CopyComponentIfExists<BoxColliderComponent>(copy, source);
    CopyComponentIfExists<AudioSourceComponent>(copy, source);
    CopyComponentIfExists<AudioListenerComponent>(copy, source);
    CopyComponentIfExists<ScriptComponent>(copy, source);
    CopyComponentIfExists<PBRMaterialComponent>(copy, source);
    CopyComponentIfExists<DirectionalLightComponent>(copy, source);
    CopyComponentIfExists<PointLightComponent>(copy, source);
    CopyComponentIfExists<SpotLightComponent>(copy, source);
    // CopyComponentIfExists<SkyboxComponent>(copy, source); // TODO: Define SkyboxComponent

    if (copy.HasComponent<RigidbodyComponent>()) {
        copy.GetComponent<RigidbodyComponent>().RuntimeBody.Reset();
    }
    if (copy.HasComponent<AudioSourceComponent>()) {
        auto& audio = copy.GetComponent<AudioSourceComponent>();
        audio.RuntimeSourcePtr = nullptr;
        audio.IsPlaying = false;
    }
    if (copy.HasComponent<ScriptComponent>()) {
        copy.GetComponent<ScriptComponent>().IsInitialized = false;
    }

    if (source.HasComponent<RelationshipComponent>()) {
        auto& rel = source.GetComponent<RelationshipComponent>();
        if (rel.Parent) {
            SetParent(copy, Entity(rel.Parent, this));
        }
        for (const auto& child : rel.Children) {
            Entity childEntity(child, this);
            Entity childCopy = DuplicateEntity(childEntity);
            SetParent(childCopy, copy);
        }
    }

    return copy;
}

std::unique_ptr<World> World::CloneForRuntime() const {
    auto clone = std::make_unique<World>();
    for (Entity root : GetRootEntities()) {
        CloneEntityHierarchy(root, *clone);
    }
    return clone;
}

void World::OnUpdate(float deltaTime) {
    m_SystemManager.UpdateAll(this, deltaTime);
}

void World::OnRender() {
    // Rendering is handled by RenderSystem in SystemManager
    // This method is kept for explicit render calls if needed
}

template<typename T>
T& World::AddComponentFromValue(EntityHandle handle, T&& component) {
    auto& registry = Internal::GetRegistry(*this);
    entt::entity entity = Internal::ToEnTT(handle);
    if constexpr (std::is_empty_v<T>) {
        registry.emplace<T>(entity);
        return registry.get<T>(entity);
    } else {
        return registry.emplace<T>(entity, std::move(component));
    }
}

template<typename T>
T& World::GetComponent(EntityHandle handle) {
    return Internal::GetRegistry(*this).get<T>(Internal::ToEnTT(handle));
}

template<typename T>
const T& World::GetComponent(EntityHandle handle) const {
    return Internal::GetRegistry(*this).get<T>(Internal::ToEnTT(handle));
}

template<typename T>
bool World::HasComponent(EntityHandle handle) const {
    return Internal::GetRegistry(*this).all_of<T>(Internal::ToEnTT(handle));
}

template<typename T>
void World::RemoveComponent(EntityHandle handle) {
    Internal::GetRegistry(*this).remove<T>(Internal::ToEnTT(handle));
}

#define ZGINE_INSTANTIATE_COMPONENT_ACCESS(ComponentType) \
    template ComponentType& World::AddComponentFromValue<ComponentType>(EntityHandle, ComponentType&&); \
    template ComponentType& World::GetComponent<ComponentType>(EntityHandle); \
    template const ComponentType& World::GetComponent<ComponentType>(EntityHandle) const; \
    template bool World::HasComponent<ComponentType>(EntityHandle) const; \
    template void World::RemoveComponent<ComponentType>(EntityHandle)

ZGINE_INSTANTIATE_COMPONENT_ACCESS(IDComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(TagComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(TransformComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(RelationshipComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(CameraComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(PrimitiveComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(SpriteRendererComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(ColorComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(MeshComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(PBRMaterialComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(DirectionalLightComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(PointLightComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(SpotLightComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(RigidbodyComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(BoxColliderComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(CircleColliderComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(AudioSourceComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(AudioListenerComponent);
ZGINE_INSTANTIATE_COMPONENT_ACCESS(ScriptComponent);

#undef ZGINE_INSTANTIATE_COMPONENT_ACCESS

}
