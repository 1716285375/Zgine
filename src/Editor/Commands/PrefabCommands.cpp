#include <Zgine/Editor/Commands/PrefabCommands.h>

#include <Zgine/Core/Log/Log.h>
#include <Zgine/Resources/Prefab/PrefabSerializer.h>
#include <Zgine/World/Core/World.h>

#include <fstream>
#include <sstream>
#include <system_error>
#include <utility>

namespace Zgine {

namespace {

std::string ReadTextFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

bool WriteTextFile(const std::filesystem::path& path, std::string_view contents) {
    std::error_code ec;
    const std::filesystem::path parent = path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent, ec);
        if (ec) {
            return false;
        }
    }

    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        return false;
    }

    out.write(contents.data(), static_cast<std::streamsize>(contents.size()));
    return static_cast<bool>(out);
}

} // namespace

CreatePrefabFromEntityCommand::CreatePrefabFromEntityCommand(Entity sourceRoot, std::filesystem::path targetPath)
    : m_SourceRoot(sourceRoot)
    , m_TargetPath(std::move(targetPath))
{
}

bool CreatePrefabFromEntityCommand::Execute() {
    if (m_TargetPath.empty()) {
        ZGINE_CORE_ERROR("CreatePrefabFromEntityCommand: target path is empty");
        return false;
    }

    if (!m_HasCapturedPrefab && !CapturePrefabData()) {
        return false;
    }

    if (!m_HasCapturedPreviousFile && !CapturePreviousFileState()) {
        return false;
    }

    if (!WritePrefabFile()) {
        ZGINE_CORE_ERROR("CreatePrefabFromEntityCommand: failed to write {}", m_TargetPath.string());
        return false;
    }

    m_Executed = true;
    return true;
}

bool CreatePrefabFromEntityCommand::Undo() {
    if (!m_Executed || m_TargetPath.empty()) {
        return false;
    }

    if (m_HadPreviousFile) {
        if (!WriteTextFile(m_TargetPath, m_PreviousContents)) {
            return false;
        }
    } else {
        std::error_code ec;
        std::filesystem::remove(m_TargetPath, ec);
        if (ec) {
            return false;
        }
    }

    m_Executed = false;
    return true;
}

std::string CreatePrefabFromEntityCommand::GetName() const {
    return "Create Prefab " + m_TargetPath.filename().string();
}

bool CreatePrefabFromEntityCommand::CapturePrefabData() {
    if (!m_SourceRoot || !m_SourceRoot.IsValid()) {
        ZGINE_CORE_ERROR("CreatePrefabFromEntityCommand: source entity is invalid");
        return false;
    }

    auto prefab = PrefabSerializer::CreateFromEntityHierarchy(m_SourceRoot);
    if (!prefab.has_value()) {
        ZGINE_CORE_ERROR("CreatePrefabFromEntityCommand: failed to create prefab data");
        return false;
    }

    m_PrefabContents = PrefabSerializer::SerializeToString(*prefab);
    m_HasCapturedPrefab = true;
    return true;
}

bool CreatePrefabFromEntityCommand::CapturePreviousFileState() {
    std::error_code ec;
    m_HadPreviousFile = std::filesystem::exists(m_TargetPath, ec);
    if (ec) {
        return false;
    }

    if (m_HadPreviousFile) {
        m_PreviousContents = ReadTextFile(m_TargetPath);
        const auto previousSize = std::filesystem::file_size(m_TargetPath, ec);
        if (ec) {
            return false;
        }
        if (m_PreviousContents.empty() && previousSize > 0) {
            return false;
        }
    }

    m_HasCapturedPreviousFile = true;
    return true;
}

bool CreatePrefabFromEntityCommand::WritePrefabFile() const {
    return WriteTextFile(m_TargetPath, m_PrefabContents);
}

InstantiatePrefabCommand::InstantiatePrefabCommand(World* world, std::filesystem::path prefabPath)
    : m_World(world)
    , m_PrefabPath(std::move(prefabPath))
{
}

InstantiatePrefabCommand::InstantiatePrefabCommand(World* world, PrefabAsset prefab)
    : m_World(world)
    , m_Prefab(std::move(prefab))
    , m_HasPrefab(m_Prefab.IsValid())
{
}

bool InstantiatePrefabCommand::Execute() {
    if (!m_World) {
        ZGINE_CORE_ERROR("InstantiatePrefabCommand: World is null");
        return false;
    }

    if (!EnsurePrefabLoaded()) {
        return false;
    }

    m_InstanceRoot = PrefabSerializer::Instantiate(m_Prefab, *m_World);
    if (!m_InstanceRoot) {
        ZGINE_CORE_ERROR("InstantiatePrefabCommand: failed to instantiate prefab");
        return false;
    }

    return true;
}

bool InstantiatePrefabCommand::Undo() {
    if (!m_World || !m_InstanceRoot) {
        return false;
    }

    if (m_World->IsEntityValid(m_InstanceRoot)) {
        m_World->DestroyEntity(m_InstanceRoot);
    }
    m_InstanceRoot = {};
    return true;
}

std::string InstantiatePrefabCommand::GetName() const {
    if (!m_Prefab.Name.empty()) {
        return "Instantiate Prefab " + m_Prefab.Name;
    }
    return "Instantiate Prefab " + m_PrefabPath.filename().string();
}

bool InstantiatePrefabCommand::EnsurePrefabLoaded() {
    if (m_HasPrefab && m_Prefab.IsValid()) {
        return true;
    }

    if (m_PrefabPath.empty()) {
        ZGINE_CORE_ERROR("InstantiatePrefabCommand: prefab path is empty");
        return false;
    }

    auto prefab = PrefabSerializer::LoadFromFile(m_PrefabPath);
    if (!prefab.has_value()) {
        ZGINE_CORE_ERROR("InstantiatePrefabCommand: failed to load {}", m_PrefabPath.string());
        return false;
    }

    m_Prefab = std::move(*prefab);
    m_HasPrefab = true;
    return true;
}

} // namespace Zgine
