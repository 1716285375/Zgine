#include <Zgine/Core/OS/FileWatcher.h>
#include <system_error>

namespace Zgine {

namespace {
    std::string NormalizePath(const std::filesystem::path& path) {
        std::error_code ec;
        auto normalized = std::filesystem::weakly_canonical(path, ec);
        if (ec) {
            return path.lexically_normal().string();
        }
        return normalized.string();
    }
}

void FileWatcher::Watch(const std::filesystem::path& path) {
    std::error_code ec;
    std::string key = NormalizePath(path);
    FileRecord record;
    record.Exists = std::filesystem::exists(path, ec);
    if (record.Exists) {
        record.LastWriteTime = std::filesystem::last_write_time(path, ec);
    }
    m_Records[key] = record;
}

void FileWatcher::Unwatch(const std::filesystem::path& path) {
    std::string key = NormalizePath(path);
    m_Records.erase(key);
}

void FileWatcher::Clear() {
    m_Records.clear();
}

void FileWatcher::SetCallback(Callback callback) {
    m_Callback = std::move(callback);
}

void FileWatcher::Poll() {
    if (!m_Callback) {
        return;
    }

    for (auto& [key, record] : m_Records) {
        std::filesystem::path path(key);
        std::error_code ec;
        bool exists = std::filesystem::exists(path, ec);
        if (ec) {
            continue;
        }

        if (!record.Exists && exists) {
            record.Exists = true;
            record.LastWriteTime = std::filesystem::last_write_time(path, ec);
            m_Callback(path, FileStatus::Created);
            continue;
        }

        if (record.Exists && !exists) {
            record.Exists = false;
            m_Callback(path, FileStatus::Removed);
            continue;
        }

        if (!record.Exists) {
            continue;
        }

        auto currentWrite = std::filesystem::last_write_time(path, ec);
        if (ec) {
            continue;
        }

        if (currentWrite != record.LastWriteTime) {
            record.LastWriteTime = currentWrite;
            m_Callback(path, FileStatus::Modified);
        }
    }
}

}
