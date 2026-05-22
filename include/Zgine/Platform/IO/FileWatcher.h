#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <string>

namespace Zgine {

enum class FileStatus { Created, Modified, Removed };

/**
 * @brief Polls a set of watched paths and fires a callback on filesystem changes.
 *
 * Lives in Platform/IO because filesystem notifications are OS-dependent.
 * Uses std::filesystem for cross-platform stat queries.
 */
class FileWatcher {
public:
    using Callback = std::function<void(const std::filesystem::path&, FileStatus)>;

    /*
        Purpose : Begin watching a path for changes.
    */
    void Watch(const std::filesystem::path& path);

    /*
        Purpose : Stop watching a previously registered path.
    */
    void Unwatch(const std::filesystem::path& path);

    /*
        Purpose : Clear all watched paths.
    */
    void Clear();

    void SetCallback(Callback callback);

    /*
        Purpose : Poll all watched paths and fire callback for any changes.
        Notes   : Call once per frame from the main loop.
    */
    void Poll();

private:
    struct FileRecord {
        std::filesystem::file_time_type LastWriteTime{};
        bool Exists = false;
    };

    std::unordered_map<std::string, FileRecord> m_Records;
    Callback m_Callback;
};

} // namespace Zgine
