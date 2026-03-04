#include <gtest/gtest.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Scene/Core/Scene.h>

using namespace Zgine;

// ============================================================================
// SelectionContext Tests
// ============================================================================

class SelectionContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_Scene = new Scene();
        m_Selection = std::make_unique<SelectionContext>();

        // Create test entities
        m_Entity1 = m_Scene->CreateEntity("Entity1");
        m_Entity2 = m_Scene->CreateEntity("Entity2");
        m_Entity3 = m_Scene->CreateEntity("Entity3");
    }

    void TearDown() override {
        m_Selection.reset();
        delete m_Scene;
    }

    Scene* m_Scene;
    std::unique_ptr<SelectionContext> m_Selection;
    Entity m_Entity1, m_Entity2, m_Entity3;
};

TEST_F(SelectionContextTest, InitialState) {
    EXPECT_TRUE(m_Selection->GetSelection().empty());
    EXPECT_FALSE(m_Selection->GetPrimary());
    EXPECT_TRUE(m_Selection->GetSelection().empty());
}

TEST_F(SelectionContextTest, SelectSingleEntity) {
    m_Selection->SetSelection({m_Entity1}, m_Entity1);

    EXPECT_FALSE(m_Selection->GetSelection().empty());
    EXPECT_EQ(m_Selection->GetPrimary(), m_Entity1);
    EXPECT_EQ(m_Selection->GetSelection().size(), 1);
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity1));
}

TEST_F(SelectionContextTest, DeselectEntity) {
    m_Selection->SetSelection({m_Entity1}, m_Entity1);
    m_Selection->Remove(m_Entity1);

    EXPECT_TRUE(m_Selection->GetSelection().empty());
    EXPECT_FALSE(m_Selection->IsSelected(m_Entity1));
}

TEST_F(SelectionContextTest, SelectMultiple) {
    m_Selection->SetSelection({m_Entity1}, m_Entity1);
    m_Selection->Add(m_Entity2);  // Don't clear previous

    EXPECT_EQ(m_Selection->GetSelection().size(), 2);
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity1));
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity2));
}

TEST_F(SelectionContextTest, SetSelectionReplacesPrevious) {
    m_Selection->SetSelection({m_Entity1}, m_Entity1);
    m_Selection->SetSelection({m_Entity2, m_Entity3}, m_Entity2);

    EXPECT_FALSE(m_Selection->IsSelected(m_Entity1));
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity2));
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity3));
    EXPECT_EQ(m_Selection->GetPrimary(), m_Entity2);
}

// Toggle test removed as Toggle method no longer exists in SelectionContext

TEST_F(SelectionContextTest, ClearSelection) {
    m_Selection->SetSelection({m_Entity1}, m_Entity1);
    m_Selection->Add(m_Entity2);

    m_Selection->Clear();

    EXPECT_TRUE(m_Selection->GetSelection().empty());
    EXPECT_TRUE(m_Selection->GetSelection().empty());
}

TEST_F(SelectionContextTest, RemoveFromSelection) {
    m_Selection->SetSelection({m_Entity1, m_Entity2, m_Entity3}, m_Entity1);

    m_Selection->Remove(m_Entity2);

    EXPECT_EQ(m_Selection->GetSelection().size(), 2);
    EXPECT_FALSE(m_Selection->IsSelected(m_Entity2));
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity1));
    EXPECT_TRUE(m_Selection->IsSelected(m_Entity3));
}

TEST_F(SelectionContextTest, RemovePrimaryUpdates) {
    m_Selection->SetSelection({m_Entity1, m_Entity2}, m_Entity1);

    m_Selection->Remove(m_Entity1);  // Remove primary

    // Primary should update to another selected entity or null
    Entity newPrimary = m_Selection->GetPrimary();
    EXPECT_TRUE(!newPrimary || newPrimary == m_Entity2);
}

// SelectionChangedCallback and MultipleCallbacks tests removed as SelectionContext
// no longer handles events directly. Events are handled by EditorEventBus.
