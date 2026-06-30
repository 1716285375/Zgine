#include <gtest/gtest.h>

#include <Zgine/Editor/Core/AssetSelectionContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Events/AssetEvents.h>
#include <Zgine/Resources/Core/AssetDatabase.h>

using namespace Zgine;

namespace {

AssetRecord MakeRecord() {
    AssetRecord record;
    record.Handle = AssetHandle::New();
    record.Type = AssetType::Texture;
    record.SourcePath = "C:/Project/assets/Textures/albedo.png";
    record.RelativePath = "Textures/albedo.png";
    record.IsDirectory = false;
    record.HasMetadata = true;
    return record;
}

} // namespace

TEST(AssetSelectionContextTest, InitialStateIsEmpty) {
    AssetSelectionContext selection;

    EXPECT_FALSE(selection.HasSelection());
    EXPECT_FALSE(selection.GetSelection().IsValid());
}

TEST(AssetSelectionContextTest, SelectCopiesAssetRecordData) {
    AssetSelectionContext selection;
    const AssetRecord record = MakeRecord();

    selection.Select(record);

    ASSERT_TRUE(selection.HasSelection());
    const AssetSelection& selected = selection.GetSelection();
    EXPECT_EQ(selected.Handle, record.Handle);
    EXPECT_EQ(selected.Type, AssetType::Texture);
    EXPECT_EQ(selected.SourcePath, record.SourcePath);
    EXPECT_EQ(selected.RelativePath, record.RelativePath);
    EXPECT_FALSE(selected.IsDirectory);
    EXPECT_TRUE(selected.HasMetadata);
}

TEST(AssetSelectionContextTest, ClearRemovesSelection) {
    AssetSelectionContext selection;
    selection.Select(MakeRecord());

    selection.Clear();

    EXPECT_FALSE(selection.HasSelection());
    EXPECT_FALSE(selection.GetSelection().IsValid());
}

TEST(AssetSelectionContextTest, PublishesSelectionEvents) {
    EditorEventBus eventBus;
    AssetSelectionContext selection(&eventBus);

    bool selectedEventReceived = false;
    bool clearedEventReceived = false;

    eventBus.Subscribe<AssetSelectedEvent>([&](AssetSelectedEvent& event) {
        selectedEventReceived = true;
        EXPECT_EQ(event.GetAssetType(), AssetType::Texture);
        EXPECT_EQ(event.GetRelativePath().generic_string(), "Textures/albedo.png");
    });

    eventBus.Subscribe<AssetSelectionClearedEvent>([&](AssetSelectionClearedEvent&) {
        clearedEventReceived = true;
    });

    selection.Select(MakeRecord());
    selection.Clear();

    EXPECT_TRUE(selectedEventReceived);
    EXPECT_TRUE(clearedEventReceived);
}
