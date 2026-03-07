#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Math/Math.h>

// Jolt 命名空间别名
using namespace JPH;

namespace Zgine {

// 自定义分配器和错误回调
namespace {
    void* JoltAllocate(size_t size) {
        return malloc(size);
    }

    void JoltFree(void* ptr) {
        free(ptr);
    }

    static constexpr ObjectLayer LAYER_NON_MOVING = 0;
    static constexpr ObjectLayer LAYER_MOVING = 1;
    static constexpr ObjectLayer LAYER_COUNT = 2;

    namespace BroadPhaseLayers {
        static constexpr BroadPhaseLayer NON_MOVING(0);
        static constexpr BroadPhaseLayer MOVING(1);
        static constexpr uint32 COUNT = 2;
    }

    class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface {
    public:
        BPLayerInterfaceImpl() {
            m_ObjectToBroadPhase[LAYER_NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            m_ObjectToBroadPhase[LAYER_MOVING] = BroadPhaseLayers::MOVING;
        }

        uint GetNumBroadPhaseLayers() const override {
            return BroadPhaseLayers::COUNT;
        }

        BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
            JPH_ASSERT(inLayer < LAYER_COUNT);
            return m_ObjectToBroadPhase[inLayer];
        }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
            if (inLayer == BroadPhaseLayers::NON_MOVING) {
                return "NON_MOVING";
            }
            if (inLayer == BroadPhaseLayers::MOVING) {
                return "MOVING";
            }
            return "UNKNOWN";
        }
#endif

    private:
        BroadPhaseLayer m_ObjectToBroadPhase[LAYER_COUNT];
    };

    class ObjectVsBroadPhaseLayerFilterImpl final : public ObjectVsBroadPhaseLayerFilter {
    public:
        bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override {
            switch (inLayer1) {
                case LAYER_NON_MOVING:
                    return inLayer2 == BroadPhaseLayers::MOVING;
                case LAYER_MOVING:
                    return true;
                default:
                    return false;
            }
        }
    };

    class ObjectLayerPairFilterImpl final : public ObjectLayerPairFilter {
    public:
        bool ShouldCollide(ObjectLayer inLayer1, ObjectLayer inLayer2) const override {
            if (inLayer1 == LAYER_NON_MOVING && inLayer2 == LAYER_NON_MOVING) {
                return false;
            }
            return true;
        }
    };

    BPLayerInterfaceImpl s_BroadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl s_ObjectVsBroadPhaseLayerFilter;
    ObjectLayerPairFilterImpl s_ObjectLayerPairFilter;
}

PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::~PhysicsSystem() {
    Shutdown();
}

void PhysicsSystem::Initialize() {
    if (m_Initialized) {
        return;
    }

    // 注册 Jolt 类型
    RegisterDefaultAllocator();

    // 创建工厂
    Factory::sInstance = new Factory();
    RegisterTypes();

    // 创建临时分配器
    m_TempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

    // 创建任务系统
    m_JobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    // 创建物理系统
    m_PhysicsSystem = std::make_unique<JPH::PhysicsSystem>();
    const uint cMaxBodies = 1024;
    const uint cNumBodyMutexes = 0;
    const uint cMaxBodyPairs = 1024;
    const uint cMaxContactConstraints = 1024;
    m_PhysicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                          s_BroadPhaseLayerInterface, s_ObjectVsBroadPhaseLayerFilter, s_ObjectLayerPairFilter);

    // 获取 BodyInterface
    m_BodyInterface = &m_PhysicsSystem->GetBodyInterface();

    // 设置重力
    m_PhysicsSystem->SetGravity(Vec3(0.0f, -9.81f, 0.0f));

    m_Initialized = true;
    ZGINE_CORE_INFO("Physics System Initialized");
}

void PhysicsSystem::Shutdown() {
    if (!m_Initialized) {
        return;
    }

    OnSceneStop();

    m_BodyInterface = nullptr;
    m_PhysicsSystem.reset();
    m_JobSystem.reset();
    m_TempAllocator.reset();

    // 清理工厂
    if (Factory::sInstance) {
        delete Factory::sInstance;
        Factory::sInstance = nullptr;
    }

    m_Initialized = false;
    ZGINE_CORE_INFO("Physics System Shutdown");
}

void PhysicsSystem::OnSceneStart(World* World) {
    if (!m_Initialized) {
        Initialize();
    }

    m_World = World;

    // 遍历所有实体，创建物理体
    if (World) {
        auto& registry = World->GetRegistry();
        auto view = registry.view<RigidbodyComponent, TransformComponent, BoxColliderComponent>();
        for (auto entity : view) {
            CreateBody(Entity(entity, World));
        }
    }

    ZGINE_CORE_INFO("Physics System: World started");
}

void PhysicsSystem::OnSceneStop() {
    if (!m_Initialized || !m_BodyInterface || !m_World) {
        return;
    }

    // 移除所有物理体
    auto& registry = m_World->GetRegistry();
    auto view = registry.view<RigidbodyComponent>();
    for (auto entity : view) {
        DestroyBody(Entity(entity, m_World));
    }

    m_World = nullptr;
    ZGINE_CORE_INFO("Physics System: World stopped");
}

void PhysicsSystem::Step(float deltaTime) {
    if (!m_Initialized || !m_PhysicsSystem) {
        return;
    }

    // 更新物理系统
    const int cCollisionSteps = 1;
    const float cDeltaTime = deltaTime;
    m_PhysicsSystem->Update(cDeltaTime, cCollisionSteps, m_TempAllocator.get(), m_JobSystem.get());
}

// ISystem interface implementations
void PhysicsSystem::Update(World* World, float deltaTime) {
    // Variable timestep update (called every frame)
    // For physics, we typically use FixedUpdate instead
    ZGINE_UNUSED(World);
    ZGINE_UNUSED(deltaTime);
}

void PhysicsSystem::FixedUpdate(World* World, float fixedDeltaTime) {
    // Fixed timestep update for physics simulation
    if (!m_Initialized || !m_World) {
        return;
    }

    Step(fixedDeltaTime);
    SyncPhysicsToECS(World);
}


void PhysicsSystem::CreateBody(Entity entity) {
    if (!m_Initialized || !m_BodyInterface || !entity.HasComponent<RigidbodyComponent>()) {
        return;
    }

    auto& rigidBody = entity.GetComponent<RigidbodyComponent>();
    auto& transform = entity.GetComponent<TransformComponent>();
    auto& collider = entity.GetComponent<BoxColliderComponent>();

    // 创建形状
    Vec3 halfExtent(
        collider.Size.x * transform.Scale.x * 0.5f,
        collider.Size.y * transform.Scale.y * 0.5f,
        collider.Size.z * transform.Scale.z * 0.5f
    );
    BoxShapeSettings boxShapeSettings(halfExtent);
    ShapeSettings::ShapeResult shapeResult = boxShapeSettings.Create();
    RefConst<Shape> shape = shapeResult.Get();

    // 创建体设置
    Vec3 position(
        transform.Translation.x + collider.Offset.x,
        transform.Translation.y + collider.Offset.y,
        transform.Translation.z + collider.Offset.z
    );

    Quat rotation = Quat::sIdentity(); // 简化：暂时不考虑旋转

    EMotionType motionType = EMotionType::Static;
    if (rigidBody.Type == RigidbodyType::Dynamic) {
        motionType = EMotionType::Dynamic;
    } else if (rigidBody.Type == RigidbodyType::Kinematic) {
        motionType = EMotionType::Kinematic;
    }

    ObjectLayer layer = (motionType == EMotionType::Static) ? LAYER_NON_MOVING : LAYER_MOVING;
    BodyCreationSettings bodySettings(shape, position, rotation, motionType, layer);
    bodySettings.mMassPropertiesOverride.mMass = rigidBody.Mass;
    bodySettings.mFriction = rigidBody.Friction;
    bodySettings.mRestitution = rigidBody.Restitution;

    // Create body
    Body* body = m_BodyInterface->CreateBody(bodySettings);
    if (body) {
        BodyID bodyID = body->GetID();
        m_BodyInterface->AddBody(bodyID, EActivation::Activate);
        // Store as type-safe handle
        rigidBody.RuntimeBody.Set(reinterpret_cast<void*>(
            static_cast<uintptr_t>(bodyID.GetIndexAndSequenceNumber())));
        ZGINE_CORE_TRACE("Created physics body for entity");
    }
}

void PhysicsSystem::DestroyBody(Entity entity) {
    if (!m_Initialized || !m_BodyInterface || !entity.HasComponent<RigidbodyComponent>()) {
        return;
    }

    auto& rigidBody = entity.GetComponent<RigidbodyComponent>();
    if (rigidBody.RuntimeBody.IsValid()) {
        uint32 id = static_cast<uint32>(reinterpret_cast<uintptr_t>(rigidBody.RuntimeBody.Get()));
        BodyID bodyID(id);
        m_BodyInterface->RemoveBody(bodyID);
        m_BodyInterface->DestroyBody(bodyID);
        rigidBody.RuntimeBody.Reset();
    }
}

void PhysicsSystem::SyncPhysicsToECS(World* World) {
    if (!m_Initialized || !m_BodyInterface || !World) {
        return;
    }

    // 遍历所有有 RigidBodyComponent 的实体
    auto& registry = World->GetRegistry();
    auto view = registry.view<RigidbodyComponent, TransformComponent>();

    for (auto entity : view) {
        auto& rigidBody = registry.get<RigidbodyComponent>(entity);
        auto& transform = registry.get<TransformComponent>(entity);

        if (rigidBody.RuntimeBody.IsValid() && rigidBody.Type == RigidbodyType::Dynamic) {
            uint32 id = static_cast<uint32>(reinterpret_cast<uintptr_t>(rigidBody.RuntimeBody.Get()));
            BodyID bodyID(id);

            BodyLockRead lock(m_PhysicsSystem->GetBodyLockInterface(), bodyID);
            if (lock.Succeeded()) {
                const Body& body = lock.GetBody();
                Vec3 position = body.GetPosition();
                Quat rotation = body.GetRotation();

                // 更新 TransformComponent
                transform.Translation = Math::Vector3(position.GetX(), position.GetY(), position.GetZ());

                // 将 Jolt 四元数转换为欧拉角（简化实现）
                // Convert Jolt Quat to Euler angles (ZXY order, degrees)
                float qw = rotation.GetW(), qx = rotation.GetX(), qy = rotation.GetY(), qz = rotation.GetZ();
                float sinX =  2.f * (qw * qx + qy * qz);
                float cosX =  1.f - 2.f * (qx * qx + qy * qy);
                float sinY =  2.f * (qw * qy - qz * qx);
                float sinZ =  2.f * (qw * qz + qx * qy);
                float cosZ =  1.f - 2.f * (qy * qy + qz * qz);
                using namespace Math;
                transform.Rotation = Math::Vector3(
                    RadToDeg(std::atan2(sinX, cosX)),
                    RadToDeg(std::asin(Math::Clamp(sinY, -1.f, 1.f))),
                    RadToDeg(std::atan2(sinZ, cosZ))
                );
            }
        }
    }
}

void PhysicsSystem::UpdateBodyTransform(Entity entity) {
    if (!m_Initialized || !m_BodyInterface || !entity.HasComponent<RigidbodyComponent>() || !entity.HasComponent<TransformComponent>()) {
        return;
    }

    auto& rigidBody = entity.GetComponent<RigidbodyComponent>();
    auto& transform = entity.GetComponent<TransformComponent>();

    if (!rigidBody.RuntimeBody.IsValid()) {
        CreateBody(entity);
        return;
    }

    uint32 id = static_cast<uint32>(reinterpret_cast<uintptr_t>(rigidBody.RuntimeBody.Get()));
    BodyID bodyID(id);

    Vec3 position(transform.Translation.x, transform.Translation.y, transform.Translation.z);
    // Convert Euler angles (degrees) to Jolt Quat
    float rx = Math::DegToRad(transform.Rotation.x);
    float ry = Math::DegToRad(transform.Rotation.y);
    float rz = Math::DegToRad(transform.Rotation.z);
    float cy = std::cos(rz * 0.5f), sy = std::sin(rz * 0.5f);
    float cp = std::cos(ry * 0.5f), sp = std::sin(ry * 0.5f);
    float cr = std::cos(rx * 0.5f), sr = std::sin(rx * 0.5f);
    Quat rotation(
        sr * cp * cy - cr * sp * sy,  // x
        cr * sp * cy + sr * cp * sy,  // y
        cr * cp * sy - sr * sp * cy,  // z
        cr * cp * cy + sr * sp * sy   // w
    );

    m_BodyInterface->SetPositionAndRotation(bodyID, position, rotation, EActivation::Activate);
}

}

