#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#ifndef ZGINE_SOURCE_DIR
#define ZGINE_SOURCE_DIR "."
#endif

namespace {

std::filesystem::path RootPath() {
    return std::filesystem::path{ZGINE_SOURCE_DIR};
}

std::string ReadText(const std::filesystem::path& path) {
    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void ExpectFileExists(const std::filesystem::path& relativePath) {
    const std::filesystem::path path = RootPath() / relativePath;
    EXPECT_TRUE(std::filesystem::is_regular_file(path)) << path.string();
}

} // namespace

TEST(DocumentationStructureTest, HasConstitutionAndCoreWorkflowDocs) {
    for (const std::filesystem::path& path : {
             "docs/constitution.md",
             "docs/engine-vision.md",
             "docs/engine-architecture.md",
             "docs/project-standards.md",
             "docs/changes/README.md",
         }) {
        ExpectFileExists(path);
    }

    const std::string constitution = ReadText(RootPath() / "docs/constitution.md");
    EXPECT_NE(constitution.find("Runtime"), std::string::npos);
    EXPECT_NE(constitution.find("OpenSpec"), std::string::npos);
    EXPECT_NE(constitution.find("Vulkan"), std::string::npos);
}

TEST(DocumentationStructureTest, HasRequiredModuleSpecs) {
    for (const std::filesystem::path& path : {
             "docs/specs/Core.md",
             "docs/specs/Renderer.md",
             "docs/specs/ECS.md",
             "docs/specs/Scene.md",
             "docs/specs/Asset.md",
             "docs/specs/Physics.md",
             "docs/specs/Audio.md",
             "docs/specs/Serialization.md",
             "docs/specs/Scripting.md",
             "docs/specs/Editor.md",
         }) {
        ExpectFileExists(path);
    }
}

TEST(DocumentationStructureTest, HasRequiredArchitectureDocs) {
    for (const std::filesystem::path& path : {
             "docs/architecture/runtime-editor-separation.md",
             "docs/architecture/module-boundaries.md",
             "docs/architecture/ecs-rules.md",
             "docs/architecture/renderer-rules.md",
             "docs/architecture/ai-readable-engine-design.md",
         }) {
        ExpectFileExists(path);
    }
}

TEST(DocumentationStructureTest, InitialOpenSpecChangesUseRequiredFiles) {
    const std::vector<std::string_view> changes{
        "define-runtime-editor-boundary",
        "add-asset-database",
        "add-editor-asset-browser",
        "add-editor-asset-selection-inspector",
        "add-prefab-system",
        "add-editor-inspector-workflow",
        "add-play-mode",
    };

    const std::vector<std::string_view> requiredFiles{
        "proposal.md",
        "requirements.md",
        "design.md",
        "tasks.md",
        "acceptance.md",
    };

    for (std::string_view change : changes) {
        for (std::string_view file : requiredFiles) {
            ExpectFileExists(std::filesystem::path{"docs/changes"} / std::string{change} / std::string{file});
        }
    }
}
