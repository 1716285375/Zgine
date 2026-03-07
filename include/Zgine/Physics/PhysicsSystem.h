#pragma once

#include <Zgine/World/Systems/ISystem.h>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyLock.h>
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

    // Legacy interface (kept for compatibility)
    void OnSceneStart(World* World);
    void OnSceneStop();
    void Step(float deltaTime);

    // 创建物理体
    void CreateBody(class Entity entity);
    void DestroyBody(class Entity entity);

    // 同步物理世界和 ECS 变换
    void SyncPhysicsToECS(World* World);
    void UpdateBodyTransform(class Entity entity);

private:
    std::unique_ptr<JPH::TempAllocatorImpl> m_TempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_JobSystem;
    std::unique_ptr<JPH::PhysicsSystem> m_PhysicsSystem;
    JPH::BodyInterface* m_BodyInterface = nullptr;

    bool m_Initialized = false;
    World* m_World = nullptr;
};

}

