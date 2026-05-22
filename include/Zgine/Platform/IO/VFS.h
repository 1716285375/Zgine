#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace Zgine {

/**
 * @brief Virtual File System abstraction layer.
 *
 * Provides unified file access across real directories and mounted ZIP archives
 * (backed by PhysicsFS). Allows transparent asset loading from both the
 * filesystem and packed release archives.
 *
 * Lives in Platform/IO; VFS initialization and mount points are
 * OS-specific operations.
 */
class VFS {
public:
    // ----- Lifecycle -----

    /*
        Purpose : Initialize VFS; must be called before any file operations.
        Args    : argv0 - path to the executable (required by PhysicsFS).
        Return  : true on success.
    */
    [[nodiscard]] static bool Initialize(const char* argv0);

    /*
        Purpose : Shut down VFS and release all resources.
    */
    static void Shutdown();

    /*
        Purpose : Query whether VFS has been successfully initialized.
    */
    [[nodiscard]] static bool IsInitialized();

    // ----- Mount Points -----

    [[nodiscard]] static bool Mount(std::string_view newDir,
                                    const char* mountPoint = nullptr,
                                    bool appendToPath = true);
    static bool Unmount(std::string_view oldDir);
    [[nodiscard]] static std::vector<std::string> GetSearchPath();

    // ----- File I/O -----

    [[nodiscard]] static std::string          ReadFileText (std::string_view filename);
    [[nodiscard]] static std::vector<uint8_t> ReadFileBytes(std::string_view filename);
    static bool WriteFileText (std::string_view filename, std::string_view content);
    static bool WriteFileBytes(std::string_view filename, const std::vector<uint8_t>& data);

    // ----- Query -----

    [[nodiscard]] static bool                 Exists          (std::string_view filename);
    [[nodiscard]] static bool                 IsDirectory     (std::string_view filename);
    [[nodiscard]] static int64_t              GetFileSize     (std::string_view filename);
    [[nodiscard]] static int64_t              GetModTime      (std::string_view filename);
    [[nodiscard]] static std::vector<std::string> EnumerateFiles(std::string_view dir);

    // ----- Write Directory -----

    static bool SetWriteDir(std::string_view newDir);
    [[nodiscard]] static std::string GetWriteDir();
    static bool MakeDir(std::string_view dirName);

    // ----- Info -----

    [[nodiscard]] static std::vector<std::string> SupportedArchiveTypes();
    [[nodiscard]] static std::string              GetLastError();

private:
    static bool s_Initialized;
};

} // namespace Zgine
