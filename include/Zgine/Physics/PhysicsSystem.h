#pragma once

#include <Zgine/World/Systems/ISystem.h>
#include <memory>

namespace Zgine {

class World;

class PhysicsSystem : public ISystem {
public:
    PhysicsSystem();
    ~PhysicsSystem() override;

    // ISystem interface implementation
    void Initialize() override;
    void Shutdown() override;
    void Update(World* World, float deltaTime) override;
    void FixedUpdate(World* World, float fixedDeltaTime) override;
    const char* GetName() const override { return "PhysicsSystem"; }
    int GetPriority() const override { return 10; }  // Physics runs early

    void OnSceneStart(World* World) override;
    void OnSceneStop() override;
    void Step(float deltaTime);

    // 创建物理体
    void CreateBody(class Entity entity);
    void DestroyBody(class Entity entity);

    // 同步物理世界和 ECS 变换
    void SyncPhysicsToECS(World* World);
    void UpdateBodyTransform(class Entity entity);

private:
    struct Impl;

    std::unique_ptr<Impl> m_Impl;

    bool m_Initialized = false;
    World* m_World = nullptr;
};

}

