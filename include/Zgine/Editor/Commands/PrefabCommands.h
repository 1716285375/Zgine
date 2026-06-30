#pragma once

#include <Zgine/Editor/Commands/IEditorCommand.h>
#include <Zgine/Resources/Prefab/PrefabAsset.h>
#include <Zgine/World/Core/Entity.h>

#include <filesystem>
#include <string>

namespace Zgine {

class World;

class CreatePrefabFromEntityCommand : public IEditorCommand {
public:
    CreatePrefabFromEntityCommand(Entity sourceRoot, std::filesystem::path targetPath);
    ~CreatePrefabFromEntityCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    [[nodiscard]] std::string GetName() const override;

    [[nodiscard]] const std::filesystem::path& GetTargetPath() const { return m_TargetPath; }

private:
    [[nodiscard]] bool CapturePrefabData();
    [[nodiscard]] bool CapturePreviousFileState();
    [[nodiscard]] bool WritePrefabFile() const;

    Entity m_SourceRoot;
    std::filesystem::path m_TargetPath;
    std::string m_PrefabContents;
    std::string m_PreviousContents;
    bool m_HasCapturedPrefab = false;
    bool m_HasCapturedPreviousFile = false;
    bool m_HadPreviousFile = false;
    bool m_Executed = false;
};

class InstantiatePrefabCommand : public IEditorCommand {
public:
    InstantiatePrefabCommand(World* world, std::filesystem::path prefabPath);
    InstantiatePrefabCommand(World* world, PrefabAsset prefab);
    ~InstantiatePrefabCommand() override = default;

    bool Execute() override;
    bool Undo() override;
    [[nodiscard]] std::string GetName() const override;

    [[nodiscard]] Entity GetInstantiatedRoot() const { return m_InstanceRoot; }

private:
    [[nodiscard]] bool EnsurePrefabLoaded();

    World* m_World = nullptr;
    std::filesystem::path m_PrefabPath;
    PrefabAsset m_Prefab;
    Entity m_InstanceRoot;
    bool m_HasPrefab = false;
};

} // namespace Zgine
