#pragma once

#include <Zgine/Editor/Events/EditorEvent.h>
#include <filesystem>
#include <sstream>

namespace Zgine {

// Forward declaration
class World;

/**
 * @brief Event fired when an asset is dropped onto the viewport
 */
class AssetDroppedEvent : public EditorEvent {
public:
    AssetDroppedEvent(World* World, const std::filesystem::path& path)
        : m_World(World), m_AssetPath(path) {}

    World* GetScene() const noexcept { return m_World; }
    const std::filesystem::path& GetAssetPath() const noexcept { return m_AssetPath; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "AssetDroppedEvent [ID:" << GetEventID()
           << ", Asset:" << m_AssetPath.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(AssetDroppedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Asset)

private:
    World* m_World;
    std::filesystem::path m_AssetPath;
};

/**
 * @brief Event fired when an asset is imported
 */
class AssetImportedEvent : public EditorEvent {
public:
    explicit AssetImportedEvent(const std::filesystem::path& path)
        : m_AssetPath(path) {}

    const std::filesystem::path& GetAssetPath() const noexcept { return m_AssetPath; }

    std::string ToString() const override {
        std::stringstream ss;
        ss << "AssetImportedEvent [ID:" << GetEventID()
           << ", Asset:" << m_AssetPath.string() << "]";
        return ss.str();
    }

    ZGINE_EDITOR_EVENT_CLASS_TYPE(AssetImportedEvent)
    ZGINE_EDITOR_EVENT_CLASS_CATEGORY(Asset)

private:
    std::filesystem::path m_AssetPath;
};

} // namespace Zgine
