#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <string>

namespace Zgine {

enum class FileStatus {
    Created,
    Modified,
    Removed
};

class FileWatcher {
public:
    using Callback = std::function<void(const std::filesystem::path&, FileStatus)>;

    void Watch(const std::filesystem::path& path);
    void Unwatch(const std::filesystem::path& path);
    void Clear();

    void SetCallback(Callback callback);
    void Poll();

private:
    struct FileRecord {
        std::filesystem::file_time_type LastWriteTime{};
        bool Exists = false;
    };

    std::unordered_map<std::string, FileRecord> m_Records;
    Callback m_Callback;
};

}
