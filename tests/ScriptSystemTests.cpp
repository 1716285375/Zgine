#include <gtest/gtest.h>

#include <Zgine/Physics/PhysicsSystem.h>
#include <Zgine/Platform/IO/VFS.h>
#include <Zgine/Scripting/ScriptSystem.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/World.h>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace {

class ScriptSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
        m_Root = std::filesystem::temp_directory_path() /
            ("zgine-script-system-test-" + std::to_string(unique));

        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
        ASSERT_TRUE(std::filesystem::create_directories(m_Root, ec));
        ASSERT_FALSE(ec);

        if (Zgine::VFS::IsInitialized()) {
            Zgine::VFS::Shutdown();
        }
        ASSERT_TRUE(Zgine::VFS::Initialize("ZgineScriptSystemTests"));
        ASSERT_TRUE(Zgine::VFS::Mount(m_Root.string(), nullptr, true));
    }

    void TearDown() override {
        if (Zgine::VFS::IsInitialized()) {
            Zgine::VFS::Shutdown();
        }

        std::error_code ec;
        std::filesystem::remove_all(m_Root, ec);
    }

    void WriteScript(const std::filesystem::path& relativePath, std::string_view content) const {
        const std::filesystem::path path = m_Root / relativePath;
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        file << content;
    }

private:
    std::filesystem::path m_Root;
};

} // namespace

TEST_F(ScriptSystemTest, ScriptInstancesUseIsolatedGlobals) {
    WriteScript("first.lua", R"(
offset = 1.0

function OnStart(entity)
    setPosition(entity, offset, 0.0, 0.0)
end

function OnUpdate(entity, dt)
    setPosition(entity, offset + dt, 0.0, 0.0)
end

function OnDestroy(entity)
    setPosition(entity, -offset, 0.0, 0.0)
end
)");

    WriteScript("second.lua", R"(
offset = 10.0

function OnStart(entity)
    setPosition(entity, offset, 0.0, 0.0)
end

function OnUpdate(entity, dt)
    setPosition(entity, offset + dt, 0.0, 0.0)
end

function OnDestroy(entity)
    setPosition(entity, -offset, 0.0, 0.0)
end
)");

    Zgine::World world;
    Zgine::Entity first = world.CreateEntity("First");
    Zgine::Entity second = world.CreateEntity("Second");
    first.AddComponent<Zgine::ScriptComponent>("first.lua");
    second.AddComponent<Zgine::ScriptComponent>("second.lua");

    Zgine::ScriptSystem scripts;
    scripts.Initialize();
    scripts.OnSceneStart(&world);

    ASSERT_TRUE(first.GetComponent<Zgine::ScriptComponent>().IsInitialized);
    ASSERT_TRUE(second.GetComponent<Zgine::ScriptComponent>().IsInitialized);
    EXPECT_FLOAT_EQ(first.GetComponent<Zgine::TransformComponent>().Translation.x, 1.0f);
    EXPECT_FLOAT_EQ(second.GetComponent<Zgine::TransformComponent>().Translation.x, 10.0f);

    scripts.Update(&world, 0.25f);

    EXPECT_FLOAT_EQ(first.GetComponent<Zgine::TransformComponent>().Translation.x, 1.25f);
    EXPECT_FLOAT_EQ(second.GetComponent<Zgine::TransformComponent>().Translation.x, 10.25f);

    scripts.OnSceneStop();

    EXPECT_FALSE(first.GetComponent<Zgine::ScriptComponent>().IsInitialized);
    EXPECT_FALSE(second.GetComponent<Zgine::ScriptComponent>().IsInitialized);
    EXPECT_FLOAT_EQ(first.GetComponent<Zgine::TransformComponent>().Translation.x, -1.0f);
    EXPECT_FLOAT_EQ(second.GetComponent<Zgine::TransformComponent>().Translation.x, -10.0f);
}

TEST_F(ScriptSystemTest, PhysicsBindingsDriveRuntimeBodies) {
    WriteScript("physics.lua", R"(
function OnStart(entity)
    setVelocity(entity, 2.0, 3.0, 4.0)
    applyForce(entity, 60.0, 0.0, 0.0)
end
)");

    Zgine::World world;
    Zgine::Entity entity = world.CreateEntity("PhysicsBody");
    entity.AddComponent<Zgine::RigidbodyComponent>();
    entity.AddComponent<Zgine::BoxColliderComponent>();
    entity.AddComponent<Zgine::ScriptComponent>("physics.lua");

    Zgine::PhysicsSystem physics;
    Zgine::ScriptSystem scripts;
    physics.Initialize();
    scripts.Initialize();
    scripts.SetPhysicsSystem(&physics);

    physics.OnSceneStart(&world);
    scripts.OnSceneStart(&world);

    ASSERT_TRUE(entity.GetComponent<Zgine::ScriptComponent>().IsInitialized);
    ASSERT_TRUE(entity.GetComponent<Zgine::RigidbodyComponent>().RuntimeBody.IsValid());

    Zgine::Math::Vector3 velocity = physics.GetLinearVelocity(entity);
    EXPECT_FLOAT_EQ(velocity.x, 2.0f);
    EXPECT_FLOAT_EQ(velocity.y, 3.0f);
    EXPECT_FLOAT_EQ(velocity.z, 4.0f);

    physics.Step(1.0f / 60.0f);
    velocity = physics.GetLinearVelocity(entity);
    EXPECT_GT(velocity.x, 2.0f);

    scripts.OnSceneStop();
    physics.OnSceneStop();
    scripts.Shutdown();
    physics.Shutdown();
}
