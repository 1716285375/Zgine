#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Zgine {

/**
 * @brief Virtual File System abstraction layer
 *
 * Provides unified file access across directories, ZIP archives, and other
 * archive formats supported by PhysicsFS. This allows transparent loading
 * of assets from both the filesystem and packed archives.
 */
class VFS {
public:
    // ===== Initialization =====

    /**
     * @brief Initialize the VFS system
     * @param argv0 Path to the executable (needed for PhysicsFS initialization)
     * @return true on success, false on failure
     */
    static bool Initialize(const char* argv0);

    /**
     * @brief Shutdown the VFS system
     * Call this before application exit to clean up resources
     */
    static void Shutdown();

    /**
     * @brief Check if VFS is initialized
     * @return true if initialized, false otherwise
     */
    static bool IsInitialized();

    // ===== Mount Points =====

    /**
     * @brief Mount a directory or archive to the search path
     * @param newDir Path to directory or archive file
     * @param mountPoint Virtual mount point (nullptr for root "/")
     * @param appendToPath If true, append to search path; if false, prepend
     * @return true on success, false on failure
     */
    static bool Mount(const std::string& newDir, const char* mountPoint = nullptr, bool appendToPath = true);

    /**
     * @brief Unmount a previously mounted directory or archive
     * @param oldDir Path that was previously mounted
     * @return true on success, false on failure
     */
    static bool Unmount(const std::string& oldDir);

    /**
     * @brief Get the current search path
     * @return Vector of mounted paths in search order
     */
    static std::vector<std::string> GetSearchPath();

    // ===== File Operations =====

    /**
     * @brief Read entire file as text (UTF-8)
     * @param filename Virtual file path
     * @return File contents as string, or empty string on failure
     */
    static std::string ReadFileText(const std::string& filename);

    /**
     * @brief Read entire file as binary data
     * @param filename Virtual file path
     * @return File contents as byte vector, or empty vector on failure
     */
    static std::vector<uint8_t> ReadFileBytes(const std::string& filename);

    /**
     * @brief Write text to a file
     * @param filename Virtual file path
     * @param content Text content to write
     * @return true on success, false on failure
     */
    static bool WriteFileText(const std::string& filename, const std::string& content);

    /**
     * @brief Write binary data to a file
     * @param filename Virtual file path
     * @param data Binary data to write
     * @return true on success, false on failure
     */
    static bool WriteFileBytes(const std::string& filename, const std::vector<uint8_t>& data);

    // ===== File Information =====

    /**
     * @brief Check if a file exists
     * @param filename Virtual file path
     * @return true if file exists, false otherwise
     */
    static bool Exists(const std::string& filename);

    /**
     * @brief Check if path is a directory
     * @param filename Virtual path
     * @return true if directory, false otherwise
     */
    static bool IsDirectory(const std::string& filename);

    /**
     * @brief Get file size in bytes
     * @param filename Virtual file path
     * @return File size, or -1 on error
     */
    static int64_t GetFileSize(const std::string& filename);

    /**
     * @brief Get file modification time
     * @param filename Virtual file path
     * @return Unix timestamp of last modification, or -1 on error
     */
    static int64_t GetModTime(const std::string& filename);

    // ===== Directory Enumeration =====

    /**
     * @brief List files in a directory
     * @param dir Virtual directory path
     * @return Vector of filenames (not full paths)
     */
    static std::vector<std::string> EnumerateFiles(const std::string& dir);

    // ===== Write Directory =====

    /**
     * @brief Set the write directory
     * Only files in this directory can be written
     * @param newDir Physical path to write directory
     * @return true on success, false on failure
     */
    static bool SetWriteDir(const std::string& newDir);

    /**
     * @brief Get the current write directory
     * @return Write directory path, or empty string if not set
     */
    static std::string GetWriteDir();

    /**
     * @brief Create a directory in the write directory
     * @param dirName Directory name (can include subdirectories with '/')
     * @return true on success, false on failure
     */
    static bool MakeDir(const std::string& dirName);

    // ===== Archive Support =====

    /**
     * @brief Get list of supported archive types
     * @return Vector of archive extensions (e.g., "ZIP", "7Z")
     */
    static std::vector<std::string> SupportedArchiveTypes();

    /**
     * @brief Get the last error message
     * @return Error message string
     */
    static std::string GetLastError();

private:
    static bool s_Initialized;
};

}
