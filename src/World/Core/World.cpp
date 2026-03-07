#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/EntityManager.h>
#include <Zgine/Core/Log/Log.h>
#include <algorithm>

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

    bool IsDescendant(World* world, entt::entity child, entt::entity possibleParent) {
        if (!world || possibleParent == entt::null) {
            return false;
        }
        if (!world->IsEntityValid(Entity(child, const_cast<World*>(world))) ||
            !world->IsEntityValid(Entity(possibleParent, const_cast<World*>(world)))) {
            return false;
        }
        auto& registry = world->GetRegistry();
        auto* rel = registry.try_get<RelationshipComponent>(possibleParent);
        if (!rel) {
            return false;
        }
        for (auto descendant : rel->Children) {
            if (descendant == child) {
                return true;
            }
            if (IsDescendant(world, child, descendant)) {
                return true;
            }
        }
        return false;
    }
}

World::World()
    : m_EntityManager(std::make_unique<EntityManager>(m_Registry))
{
}

World::~World() {
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

size_t World::GetEntityCount() const {
    return m_EntityManager->GetEntityCount();
}

bool World::IsEntityValid(Entity entity) const {
    if (!entity) {
        return false;
    }
    return m_Registry.valid(static_cast<entt::entity>(entity));
}

void World::SetParent(Entity child, Entity parent) {
    if (!child || child == parent) {
        return;
    }

    if (!IsEntityValid(child)) {
        return;
    }
    auto& registry = m_Registry;

    if (parent && IsDescendant(this, static_cast<entt::entity>(parent), static_cast<entt::entity>(child))) {
        ZGINE_CORE_WARN("World::SetParent failed: cannot parent entity to its descendant.");
        return;
    }

    if (!registry.all_of<RelationshipComponent>(child)) {
        registry.emplace<RelationshipComponent>(child);
    }
    auto& childRel = registry.get<RelationshipComponent>(child);
    if (childRel.Parent != entt::null && IsEntityValid(Entity(childRel.Parent, this))) {
        auto& oldParentRel = registry.get<RelationshipComponent>(childRel.Parent);
        oldParentRel.Children.erase(
            std::remove(oldParentRel.Children.begin(), oldParentRel.Children.end(), static_cast<entt::entity>(child)),
            oldParentRel.Children.end());
    }

    if (parent) {
        if (!registry.all_of<RelationshipComponent>(parent)) {
            registry.emplace<RelationshipComponent>(parent);
        }
        auto& parentRel = registry.get<RelationshipComponent>(parent);
        if (std::find(parentRel.Children.begin(), parentRel.Children.end(), static_cast<entt::entity>(child)) == parentRel.Children.end()) {
            parentRel.Children.push_back(static_cast<entt::entity>(child));
        }
        childRel.Parent = static_cast<entt::entity>(parent);
    } else {
        childRel.Parent = entt::null;
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
        if (rel.Parent != entt::null) {
            SetParent(copy, Entity(rel.Parent, this));
        }
        for (auto child : rel.Children) {
            Entity childEntity(child, this);
            Entity childCopy = DuplicateEntity(childEntity);
            SetParent(childCopy, copy);
        }
    }

    return copy;
}

void World::OnUpdate(float deltaTime) {
    m_SystemManager.UpdateAll(this, deltaTime);
}

void World::OnRender() {
    // Rendering is handled by RenderSystem in SystemManager
    // This method is kept for explicit render calls if needed
}


}

