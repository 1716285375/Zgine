#include <gtest/gtest.h>
#include <Zgine/Editor/Commands/EditorCommand.h>
#include <Zgine/Editor/Commands/EditorCommandHistory.h>
#include <Zgine/Editor/Commands/EntityCommands.h>
#include <Zgine/Editor/Commands/TransformCommands.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Components/Components.h>

using namespace Zgine;

// ============================================================================
// Mock Command for Testing
// ============================================================================

class MockCommand : public EditorCommand {
public:
    MockCommand(int* counter) : m_Counter(counter) {}

    bool Execute() override {
        (*m_Counter)++;
        m_Executed = true;
        return true;
    }

    bool Undo() override {
        (*m_Counter)--;
        m_Executed = false;
        return true;
    }

    std::string GetName() const override {
        return "MockCommand";
    }

    bool m_Executed = false;

private:
    int* m_Counter;
};

class MergeableCommand : public EditorCommand {
public:
    MergeableCommand(int value) : m_Value(value) {}

    bool Execute() override { m_Executed = true; return true; }
    bool Undo() override { m_Executed = false; return true; }
    std::string GetName() const override { return "MergeableCommand"; }

    bool CanMergeWith(const EditorCommand* other) const override {
        return dynamic_cast<const MergeableCommand*>(other) != nullptr;
    }

    void MergeWith(const EditorCommand* other) override {
        if (auto* cmd = dynamic_cast<const MergeableCommand*>(other)) {
            m_Value += cmd->m_Value;
        }
    }

    int GetValue() const { return m_Value; }

private:
    int m_Value;
    bool m_Executed = false;
};

// ============================================================================
// EditorCommandHistory Tests
// ============================================================================

class EditorCommandHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_History = std::make_unique<EditorCommandHistory>();
        m_Counter = 0;
    }

    void TearDown() override {
        m_History.reset();
    }

    std::unique_ptr<EditorCommandHistory> m_History;
    int m_Counter;
};

TEST_F(EditorCommandHistoryTest, ExecuteCommand) {
    auto cmd = std::make_unique<MockCommand>(&m_Counter);

    EXPECT_EQ(m_Counter, 0);
    m_History->Execute(std::move(cmd));
    EXPECT_EQ(m_Counter, 1);
}

TEST_F(EditorCommandHistoryTest, UndoCommand) {
    auto cmd = std::make_unique<MockCommand>(&m_Counter);

    m_History->Execute(std::move(cmd));
    EXPECT_EQ(m_Counter, 1);
    EXPECT_TRUE(m_History->CanUndo());

    m_History->Undo();
    EXPECT_EQ(m_Counter, 0);
    EXPECT_FALSE(m_History->CanUndo());
}

TEST_F(EditorCommandHistoryTest, RedoCommand) {
    auto cmd = std::make_unique<MockCommand>(&m_Counter);

    m_History->Execute(std::move(cmd));
    m_History->Undo();

    EXPECT_TRUE(m_History->CanRedo());
    m_History->Redo();
    EXPECT_EQ(m_Counter, 1);
    EXPECT_FALSE(m_History->CanRedo());
}

TEST_F(EditorCommandHistoryTest, MultipleUndoRedo) {
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));

    EXPECT_EQ(m_Counter, 3);
    EXPECT_EQ(m_History->GetHistorySize(), 3);

    m_History->Undo();
    m_History->Undo();
    EXPECT_EQ(m_Counter, 1);
    EXPECT_EQ(m_History->GetHistorySize(), 1);

    m_History->Redo();
    EXPECT_EQ(m_Counter, 2);
}

TEST_F(EditorCommandHistoryTest, RedoStackClearedOnNewCommand) {
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));

    m_History->Undo();
    EXPECT_TRUE(m_History->CanRedo());

    // Execute new command should clear redo stack
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    EXPECT_FALSE(m_History->CanRedo());
}

TEST_F(EditorCommandHistoryTest, Clear) {
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));

    m_History->Clear();

    EXPECT_FALSE(m_History->CanUndo());
    EXPECT_FALSE(m_History->CanRedo());
    EXPECT_FALSE(m_History->CanUndo());
    EXPECT_FALSE(m_History->CanRedo());
    EXPECT_EQ(m_History->GetHistorySize(), 0);
}

TEST_F(EditorCommandHistoryTest, MaxHistorySize) {
    m_History->SetMaxHistorySize(5);

    // Add more than max
    for (int i = 0; i < 10; ++i) {
        m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    }

    // Should only keep last 5
    EXPECT_LE(m_History->GetHistorySize(), 5);
}

// ============================================================================
// Command Merging Tests
// ============================================================================

TEST_F(EditorCommandHistoryTest, CommandMerging) {
    m_History->Execute(std::make_unique<MergeableCommand>(10));
    EXPECT_EQ(m_History->GetHistorySize(), 1);

    // This should merge with previous
    m_History->Execute(std::make_unique<MergeableCommand>(20));
    EXPECT_EQ(m_History->GetHistorySize(), 1);

    // Verify merged value
    // Note: This is a simplified test - in real implementation,
    // you'd need to verify the actual merged command state
}

// ============================================================================
// Transaction Tests
// ============================================================================

TEST_F(EditorCommandHistoryTest, Transaction) {
    m_History->BeginTransaction("Batch Operation");

    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));

    m_History->EndTransaction();

    EXPECT_EQ(m_Counter, 3);

    // Undo should undo entire transaction
    m_History->Undo();
    EXPECT_EQ(m_Counter, 0);

    // Redo should redo entire transaction
    m_History->Redo();
    EXPECT_EQ(m_Counter, 3);
}

TEST_F(EditorCommandHistoryTest, NestedTransactionsNotAllowed) {
    m_History->BeginTransaction("Outer");

    // Nested transaction should be ignored (warning logged)
    m_History->BeginTransaction("Inner");

    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));

    // Only end outer transaction (inner was ignored)
    m_History->EndTransaction();
}

// ============================================================================
// EntityCommands Tests
// ============================================================================

class EntityCommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_World = new World();
        m_History = std::make_unique<EditorCommandHistory>();
    }

    void TearDown() override {
        delete m_World;
        m_History.reset();
    }

    World* m_World;
    std::unique_ptr<EditorCommandHistory> m_History;
};

TEST_F(EntityCommandsTest, CreateEntityCommand) {
    auto cmd = std::make_unique<CreateEntityCommand>(
        m_World, "TestEntity", PrimitiveType::Cube);

    size_t initialCount = m_World->GetRegistry().storage<entt::entity>().size();

    m_History->Execute(std::move(cmd));

    EXPECT_EQ(m_World->GetRegistry().storage<entt::entity>().size(), initialCount + 1);
}

TEST_F(EntityCommandsTest, CreateEntityCommandUndo) {
    auto cmd = std::make_unique<CreateEntityCommand>(
        m_World, "TestEntity", PrimitiveType::Cube);

    size_t initialCount = m_World->GetRegistry().storage<entt::entity>().size();

    m_History->Execute(std::move(cmd));
    EXPECT_EQ(m_World->GetRegistry().storage<entt::entity>().size(), initialCount + 1);

    m_History->Undo();
    EXPECT_EQ(m_World->GetRegistry().storage<entt::entity>().size(), initialCount);
}

TEST_F(EntityCommandsTest, DeleteEntityCommand) {
    // Create an entity first
    Entity entity = m_World->CreateEntity("ToDelete");
    size_t countAfterCreate = m_World->GetRegistry().storage<entt::entity>().size();

    auto cmd = std::make_unique<DeleteEntityCommand>(m_World, entity);
    m_History->Execute(std::move(cmd));

    EXPECT_EQ(m_World->GetRegistry().storage<entt::entity>().size(), countAfterCreate - 1);
}

TEST_F(EntityCommandsTest, DeleteEntityCommandUndo) {
    Entity entity = m_World->CreateEntity("ToDelete");
    size_t countAfterCreate = m_World->GetRegistry().storage<entt::entity>().size();

    auto cmd = std::make_unique<DeleteEntityCommand>(m_World, entity);
    m_History->Execute(std::move(cmd));
    m_History->Undo();

    // Should restore entity
    EXPECT_EQ(m_World->GetRegistry().storage<entt::entity>().size(), countAfterCreate);
}

// ============================================================================
// TransformCommands Tests
// ============================================================================

TEST_F(EntityCommandsTest, TransformEntityCommand) {
    Entity entity = m_World->CreateEntity("Transformable");
    // Entity already has TransformComponent by default

    Math::Vector3 oldPos = entity.GetComponent<TransformComponent>().Translation;
    Math::Vector3 newPos(10.0f, 20.0f, 30.0f);
    Math::Vector3 rot(0.0f);
    Math::Vector3 scale(1.0f);

    auto cmd = std::make_unique<TransformEntityCommand>(entity, newPos, rot, scale);
    m_History->Execute(std::move(cmd));

    EXPECT_EQ(entity.GetComponent<TransformComponent>().Translation, newPos);
}

TEST_F(EntityCommandsTest, TransformCommandUndo) {
    Entity entity = m_World->CreateEntity("Transformable");
    // Entity already has TransformComponent by default

    Math::Vector3 oldPos = entity.GetComponent<TransformComponent>().Translation;
    Math::Vector3 newPos(10.0f, 20.0f, 30.0f);
    Math::Vector3 rot(0.0f);
    Math::Vector3 scale(1.0f);

    auto cmd = std::make_unique<TransformEntityCommand>(entity, newPos, rot, scale);
    m_History->Execute(std::move(cmd));
    m_History->Undo();

    EXPECT_EQ(entity.GetComponent<TransformComponent>().Translation, oldPos);
}

TEST_F(EntityCommandsTest, TransformCommandMerging) {
    Entity entity = m_World->CreateEntity("Transformable");
    // Entity already has TransformComponent by default

    // Execute multiple transform commands
    m_History->Execute(std::make_unique<TransformEntityCommand>(
        entity, Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector3(0.0f), Math::Vector3(1.0f)));
    m_History->Execute(std::make_unique<TransformEntityCommand>(
        entity, Math::Vector3(2.0f, 0.0f, 0.0f), Math::Vector3(0.0f), Math::Vector3(1.0f)));
    m_History->Execute(std::make_unique<TransformEntityCommand>(
        entity, Math::Vector3(3.0f, 0.0f, 0.0f), Math::Vector3(0.0f), Math::Vector3(1.0f)));

    // Should have merged into single command
    EXPECT_EQ(m_History->GetHistorySize(), 1);

    // Undo should go back to original position
    m_History->Undo();
    EXPECT_EQ(entity.GetComponent<TransformComponent>().Translation,
              Math::Vector3(0.0f, 0.0f, 0.0f));
}

// ============================================================================
// History Change Listener Tests
// ============================================================================

TEST_F(EditorCommandHistoryTest, HistoryChangeCallback) {
    bool callbackInvoked = false;

    m_History->SetHistoryChangedCallback([&]() {
        callbackInvoked = true;
    });

    m_History->Execute(std::make_unique<MockCommand>(&m_Counter));
    EXPECT_TRUE(callbackInvoked);

    callbackInvoked = false;
    m_History->Undo();
    EXPECT_TRUE(callbackInvoked);

    callbackInvoked = false;
    m_History->Redo();
    EXPECT_TRUE(callbackInvoked);
}
