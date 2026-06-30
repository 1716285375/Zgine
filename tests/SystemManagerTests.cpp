#include <gtest/gtest.h>
#include <Zgine/World/Systems/SystemManager.h>
#include <Zgine/World/Core/World.h>

#include <string>
#include <utility>
#include <vector>

namespace {

class OrderedTestSystem final : public Zgine::ISystem {
public:
    OrderedTestSystem(std::vector<std::string>* order, std::string name, int priority)
        : m_Order(order), m_Name(std::move(name)), m_Priority(priority) {}

    void Initialize() override {}
    void Shutdown() override {}

    void OnSceneStart(Zgine::World* world) override {
        (void)world;
        m_Order->push_back("start:" + m_Name);
    }

    void OnSceneStop() override {
        m_Order->push_back("stop:" + m_Name);
    }

    void Update(Zgine::World* world, float deltaTime) override {
        (void)world;
        (void)deltaTime;
        m_Order->push_back(m_Name);
    }

    const char* GetName() const override {
        return m_Name.c_str();
    }

    int GetPriority() const override {
        return m_Priority;
    }

private:
    std::vector<std::string>* m_Order = nullptr;
    std::string m_Name;
    int m_Priority = 100;
};

} // namespace

TEST(SystemManagerTest, InterleavesOwnedAndExternalSystemsByPriority) {
    Zgine::World world;
    Zgine::SystemManager manager;
    std::vector<std::string> order;

    auto external = std::make_unique<OrderedTestSystem>(&order, "external-early", 10);
    manager.RegisterSystem<OrderedTestSystem>(&order, "owned-late", 50);
    manager.RegisterExternalSystem(external.get());

    manager.UpdateAll(&world, 0.016f);

    ASSERT_EQ(order.size(), 2);
    EXPECT_EQ(order[0], "external-early");
    EXPECT_EQ(order[1], "owned-late");
}

TEST(SystemManagerTest, PreservesRegistrationOrderForEqualPriorityAcrossOwnershipModes) {
    Zgine::World world;
    Zgine::SystemManager manager;
    std::vector<std::string> order;

    auto external = std::make_unique<OrderedTestSystem>(&order, "external-first", 20);
    manager.RegisterExternalSystem(external.get());
    manager.RegisterSystem<OrderedTestSystem>(&order, "owned-second", 20);

    manager.UpdateAll(&world, 0.016f);

    ASSERT_EQ(order.size(), 2);
    EXPECT_EQ(order[0], "external-first");
    EXPECT_EQ(order[1], "owned-second");
}

TEST(SystemManagerTest, StartsSceneByPriorityAndStopsInReverseOrder) {
    Zgine::World world;
    Zgine::SystemManager manager;
    std::vector<std::string> order;

    auto external = std::make_unique<OrderedTestSystem>(&order, "external-early", 10);
    manager.RegisterSystem<OrderedTestSystem>(&order, "owned-late", 50);
    manager.RegisterExternalSystem(external.get());

    manager.StartScene(&world);

    ASSERT_EQ(order.size(), 2);
    EXPECT_EQ(order[0], "start:external-early");
    EXPECT_EQ(order[1], "start:owned-late");
    EXPECT_TRUE(manager.IsSceneRunning());
    EXPECT_EQ(manager.GetActiveScene(), &world);

    manager.StartScene(&world);
    EXPECT_EQ(order.size(), 2);

    manager.StopScene();

    ASSERT_EQ(order.size(), 4);
    EXPECT_EQ(order[2], "stop:owned-late");
    EXPECT_EQ(order[3], "stop:external-early");
    EXPECT_FALSE(manager.IsSceneRunning());
    EXPECT_EQ(manager.GetActiveScene(), nullptr);
}
