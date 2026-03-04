#include <Zgine/Core/OS/VFS.h>
#include <Zgine/Core/Log/Log.h>
#include <physfs.h>

namespace Zgine {

// Helper function to normalize paths for PhysicsFS
// PhysicsFS requires forward slashes, Windows uses backslashes
static std::string NormalizePath(const std::string& path) {
    std::string normalized = path;
    for (char& c : normalized) {
        if (c == '\\') {
            c = '/';
        }
    }
    return normalized;
}

// Normalize path for READ operations (strip assets/ prefix since it's mounted at /)
static std::string NormalizeReadPath(const std::string& path) {
    std::string normalized = NormalizePath(path);

    // Remove "assets/" prefix if present (already mounted at root)
    if (normalized.size() >= 7 && normalized.substr(0, 7) == "assets/") {
        normalized = normalized.substr(7);
    }

    return normalized;
}

// Normalize path for WRITE operations (keep assets/ prefix for proper directory structure)
static std::string NormalizeWritePath(const std::string& path) {
    return NormalizePath(path);
}

bool VFS::s_Initialized = false;

bool VFS::Initialize(const char* argv0) {
    if (s_Initialized) {
        ZGINE_CORE_WARN("VFS already initialized");
        return true;
    }

    if (!PHYSFS_init(argv0)) {
        ZGINE_CORE_ERROR("Failed to initialize PhysicsFS: {}", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return false;
    }

    s_Initialized = true;
    ZGINE_CORE_INFO("VFS initialized successfully");
    return true;
}

void VFS::Shutdown() {
    if (!s_Initialized) {
        return;
    }

    PHYSFS_deinit();
    s_Initialized = false;
    ZGINE_CORE_INFO("VFS shutdown");
}

bool VFS::IsInitialized() {
    return s_Initialized;
}

// ===== Mount Points =====

bool VFS::Mount(const std::string& newDir, const char* mountPoint, bool appendToPath) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    std::string normalizedDir = NormalizePath(newDir);

    if (!PHYSFS_mount(normalizedDir.c_str(), mountPoint, appendToPath ? 1 : 0)) {
        ZGINE_CORE_ERROR("Failed to mount '{}': {}", normalizedDir, GetLastError());
        return false;
    }

    ZGINE_CORE_INFO("VFS mounted: {} -> {}", normalizedDir, mountPoint ? mountPoint : "/");
    return true;
}

bool VFS::Unmount(const std::string& oldDir) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    if (!PHYSFS_unmount(oldDir.c_str())) {
        ZGINE_CORE_ERROR("Failed to unmount '{}': {}", oldDir, GetLastError());
        return false;
    }

    ZGINE_CORE_INFO("VFS unmounted: {}", oldDir);
    return true;
}

std::vector<std::string> VFS::GetSearchPath() {
    std::vector<std::string> paths;

    if (!s_Initialized) {
        return paths;
    }

    char** searchPath = PHYSFS_getSearchPath();
    for (char** i = searchPath; *i != nullptr; i++) {
        paths.push_back(*i);
    }
    PHYSFS_freeList(searchPath);

    return paths;
}

// ===== File Operations =====

std::string VFS::ReadFileText(const std::string& filename) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return "";
    }

    std::string normalizedPath = NormalizeReadPath(filename);

    PHYSFS_File* file = PHYSFS_openRead(normalizedPath.c_str());
    if (!file) {
        ZGINE_CORE_ERROR("Failed to open file '{}': {}", normalizedPath, GetLastError());
        return "";
    }

    PHYSFS_sint64 fileSize = PHYSFS_fileLength(file);
    if (fileSize < 0) {
        ZGINE_CORE_ERROR("Failed to get file size for '{}': {}", normalizedPath, GetLastError());
        PHYSFS_close(file);
        return "";
    }

    std::string content;
    content.resize(static_cast<size_t>(fileSize));

    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(file, &content[0], static_cast<PHYSFS_uint64>(fileSize));
    PHYSFS_close(file);

    if (bytesRead != fileSize) {
        ZGINE_CORE_ERROR("Failed to read file '{}': {}", normalizedPath, GetLastError());
        return "";
    }

    return content;
}

std::vector<uint8_t> VFS::ReadFileBytes(const std::string& filename) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return {};
    }

    std::string normalizedPath = NormalizeReadPath(filename);

    PHYSFS_File* file = PHYSFS_openRead(normalizedPath.c_str());
    if (!file) {
        ZGINE_CORE_ERROR("Failed to open file '{}': {}", normalizedPath, GetLastError());
        return {};
    }

    PHYSFS_sint64 fileSize = PHYSFS_fileLength(file);
    if (fileSize < 0) {
        ZGINE_CORE_ERROR("Failed to get file size for '{}': {}", normalizedPath, GetLastError());
        PHYSFS_close(file);
        return {};
    }

    std::vector<uint8_t> data;
    data.resize(static_cast<size_t>(fileSize));

    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(file, data.data(), static_cast<PHYSFS_uint64>(fileSize));
    PHYSFS_close(file);

    if (bytesRead != fileSize) {
        ZGINE_CORE_ERROR("Failed to read file '{}': {}", normalizedPath, GetLastError());
        return {};
    }

    return data;
}

bool VFS::WriteFileText(const std::string& filename, const std::string& content) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    std::string normalizedPath = NormalizeWritePath(filename);

    // Ensure parent directories exist
    size_t lastSlash = normalizedPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string dirPath = normalizedPath.substr(0, lastSlash);
        if (!dirPath.empty()) {
            MakeDir(dirPath);
        }
    }

    PHYSFS_File* file = PHYSFS_openWrite(normalizedPath.c_str());
    if (!file) {
        ZGINE_CORE_ERROR("Failed to open file for writing '{}': {}", normalizedPath, GetLastError());
        return false;
    }

    PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(file, content.c_str(), content.size());
    PHYSFS_close(file);

    if (bytesWritten != static_cast<PHYSFS_sint64>(content.size())) {
        ZGINE_CORE_ERROR("Failed to write file '{}': {}", normalizedPath, GetLastError());
        return false;
    }

    return true;
}

bool VFS::WriteFileBytes(const std::string& filename, const std::vector<uint8_t>& data) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    std::string normalizedPath = NormalizePath(filename);

    // Ensure parent directories exist
    size_t lastSlash = normalizedPath.find_last_of('/');
    if (lastSlash != std::string::npos) {
        std::string dirPath = normalizedPath.substr(0, lastSlash);
        if (!dirPath.empty()) {
            MakeDir(dirPath);
        }
    }

    PHYSFS_File* file = PHYSFS_openWrite(normalizedPath.c_str());
    if (!file) {
        ZGINE_CORE_ERROR("Failed to open file for writing '{}': {}", normalizedPath, GetLastError());
        return false;
    }

    PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(file, data.data(), data.size());
    PHYSFS_close(file);

    if (bytesWritten != static_cast<PHYSFS_sint64>(data.size())) {
        ZGINE_CORE_ERROR("Failed to write file '{}': {}", normalizedPath, GetLastError());
        return false;
    }

    return true;
}

// ===== File Information =====

bool VFS::Exists(const std::string& filename) {
    if (!s_Initialized) {
        return false;
    }

    std::string normalizedPath = NormalizePath(filename);
    return PHYSFS_exists(normalizedPath.c_str()) != 0;
}

bool VFS::IsDirectory(const std::string& filename) {
    if (!s_Initialized) {
        return false;
    }

    std::string normalizedPath = NormalizeReadPath(filename);

    PHYSFS_Stat stat;
    if (!PHYSFS_stat(normalizedPath.c_str(), &stat)) {
        return false;
    }

    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

int64_t VFS::GetFileSize(const std::string& filename) {
    if (!s_Initialized) {
        return -1;
    }

    std::string normalizedPath = NormalizePath(filename);

    PHYSFS_Stat stat;
    if (!PHYSFS_stat(normalizedPath.c_str(), &stat)) {
        return -1;
    }

    return static_cast<int64_t>(stat.filesize);
}

int64_t VFS::GetModTime(const std::string& filename) {
    if (!s_Initialized) {
        return -1;
    }

    std::string normalizedPath = NormalizePath(filename);

    PHYSFS_Stat stat;
    if (!PHYSFS_stat(normalizedPath.c_str(), &stat)) {
        return -1;
    }

    return static_cast<int64_t>(stat.modtime);
}

// ===== Directory Enumeration =====

std::vector<std::string> VFS::EnumerateFiles(const std::string& dir) {
    std::vector<std::string> files;

    if (!s_Initialized) {
        return files;
    }

    std::string normalizedPath = NormalizeReadPath(dir);
    char** rc = PHYSFS_enumerateFiles(normalizedPath.c_str());
    if (rc == nullptr) {
        return files;
    }

    for (char** i = rc; *i != nullptr; i++) {
        files.push_back(*i);
    }

    PHYSFS_freeList(rc);
    return files;
}

// ===== Write Directory =====

bool VFS::SetWriteDir(const std::string& newDir) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    std::string normalizedDir = NormalizePath(newDir);

    if (!PHYSFS_setWriteDir(normalizedDir.c_str())) {
        ZGINE_CORE_ERROR("Failed to set write directory '{}': {}", normalizedDir, GetLastError());
        return false;
    }

    ZGINE_CORE_INFO("VFS write directory set to: {}", normalizedDir);
    return true;
}

std::string VFS::GetWriteDir() {
    if (!s_Initialized) {
        return "";
    }

    const char* writeDir = PHYSFS_getWriteDir();
    return writeDir ? std::string(writeDir) : "";
}

bool VFS::MakeDir(const std::string& dirName) {
    if (!s_Initialized) {
        ZGINE_CORE_ERROR("VFS not initialized");
        return false;
    }

    std::string normalizedPath = NormalizePath(dirName);

    // Recursively create parent directories
    size_t pos = 0;
    while ((pos = normalizedPath.find('/', pos + 1)) != std::string::npos) {
        std::string parentDir = normalizedPath.substr(0, pos);
        if (!parentDir.empty() && !IsDirectory(parentDir)) {
            if (!PHYSFS_mkdir(parentDir.c_str())) {
                // Ignore errors for intermediate directories that might already exist
            }
        }
    }

    if (!PHYSFS_mkdir(normalizedPath.c_str())) {
        // Only report error if the final directory creation failed
        PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
        // Ignore "file exists" errors
        if (errorCode != PHYSFS_ERR_OK && !IsDirectory(normalizedPath)) {
            ZGINE_CORE_ERROR("Failed to create directory '{}': {}", normalizedPath, GetLastError());
            return false;
        }
    }

    return true;
}

// ===== Archive Support =====

std::vector<std::string> VFS::SupportedArchiveTypes() {
    std::vector<std::string> types;

    if (!s_Initialized) {
        return types;
    }

    const PHYSFS_ArchiveInfo** info = PHYSFS_supportedArchiveTypes();
    for (const PHYSFS_ArchiveInfo** i = info; *i != nullptr; i++) {
        types.push_back((*i)->extension);
    }

    return types;
}

std::string VFS::GetLastError() {
    PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
    const char* errorMsg = PHYSFS_getErrorByCode(errorCode);
    return errorMsg ? std::string(errorMsg) : "Unknown error";
}

}
