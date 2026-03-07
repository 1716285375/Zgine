#include <Zgine/Platform/IO/VFS.h>
#include <Zgine/Core/Log/Log.h>
#include <physfs.h>

namespace Zgine {

// Helper: normalize path separators for PhysicsFS (forward slashes required)
static std::string NormalizePath(std::string_view path) {
    std::string normalized(path);
    for (char& c : normalized) {
        if (c == '\\') c = '/';
    }
    return normalized;
}

// Normalize for READ (strip leading "assets/" since it's mounted at /)
static std::string NormalizeReadPath(std::string_view path) {
    std::string normalized = NormalizePath(path);
    if (normalized.size() >= 7 && normalized.substr(0, 7) == "assets/")
        normalized = normalized.substr(7);
    return normalized;
}

static std::string NormalizeWritePath(std::string_view path) {
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
    if (!s_Initialized) return;
    PHYSFS_deinit();
    s_Initialized = false;
    ZGINE_CORE_INFO("VFS shutdown");
}

bool VFS::IsInitialized() { return s_Initialized; }

// ===== Mount Points =====

bool VFS::Mount(std::string_view newDir, const char* mountPoint, bool appendToPath) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string dir = NormalizePath(newDir);
    if (!PHYSFS_mount(dir.c_str(), mountPoint, appendToPath ? 1 : 0)) {
        ZGINE_CORE_ERROR("Failed to mount '{}': {}", dir, GetLastError());
        return false;
    }
    ZGINE_CORE_INFO("VFS mounted: {} -> {}", dir, mountPoint ? mountPoint : "/");
    return true;
}

bool VFS::Unmount(std::string_view oldDir) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string dir(oldDir);
    if (!PHYSFS_unmount(dir.c_str())) {
        ZGINE_CORE_ERROR("Failed to unmount '{}': {}", dir, GetLastError());
        return false;
    }
    ZGINE_CORE_INFO("VFS unmounted: {}", dir);
    return true;
}

std::vector<std::string> VFS::GetSearchPath() {
    std::vector<std::string> paths;
    if (!s_Initialized) return paths;
    char** sp = PHYSFS_getSearchPath();
    for (char** i = sp; *i != nullptr; i++) paths.push_back(*i);
    PHYSFS_freeList(sp);
    return paths;
}

// ===== File Operations =====

std::string VFS::ReadFileText(std::string_view filename) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return ""; }
    std::string path = NormalizeReadPath(filename);
    PHYSFS_File* file = PHYSFS_openRead(path.c_str());
    if (!file) { ZGINE_CORE_ERROR("Failed to open file '{}': {}", path, GetLastError()); return ""; }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (size < 0) { ZGINE_CORE_ERROR("Bad file size '{}': {}", path, GetLastError()); PHYSFS_close(file); return ""; }
    std::string content(static_cast<size_t>(size), '\0');
    PHYSFS_sint64 read = PHYSFS_readBytes(file, content.data(), static_cast<PHYSFS_uint64>(size));
    PHYSFS_close(file);
    if (read != size) { ZGINE_CORE_ERROR("Read error '{}': {}", path, GetLastError()); return ""; }
    return content;
}

std::vector<uint8_t> VFS::ReadFileBytes(std::string_view filename) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return {}; }
    std::string path = NormalizeReadPath(filename);
    PHYSFS_File* file = PHYSFS_openRead(path.c_str());
    if (!file) { ZGINE_CORE_ERROR("Failed to open file '{}': {}", path, GetLastError()); return {}; }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (size < 0) { PHYSFS_close(file); return {}; }
    std::vector<uint8_t> data(static_cast<size_t>(size));
    PHYSFS_sint64 read = PHYSFS_readBytes(file, data.data(), static_cast<PHYSFS_uint64>(size));
    PHYSFS_close(file);
    if (read != size) { ZGINE_CORE_ERROR("Read error '{}': {}", path, GetLastError()); return {}; }
    return data;
}

bool VFS::WriteFileText(std::string_view filename, std::string_view content) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string path = NormalizeWritePath(filename);
    // Ensure parent dirs
    size_t slash = path.find_last_of('/');
    if (slash != std::string::npos) { std::string dir = path.substr(0, slash); if (!dir.empty()) MakeDir(dir); }
    PHYSFS_File* file = PHYSFS_openWrite(path.c_str());
    if (!file) { ZGINE_CORE_ERROR("Failed to open for write '{}': {}", path, GetLastError()); return false; }
    PHYSFS_sint64 written = PHYSFS_writeBytes(file, content.data(), content.size());
    PHYSFS_close(file);
    if (written != static_cast<PHYSFS_sint64>(content.size())) {
        ZGINE_CORE_ERROR("Write error '{}': {}", path, GetLastError()); return false;
    }
    return true;
}

bool VFS::WriteFileBytes(std::string_view filename, const std::vector<uint8_t>& data) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string path = NormalizePath(filename);
    size_t slash = path.find_last_of('/');
    if (slash != std::string::npos) { std::string dir = path.substr(0, slash); if (!dir.empty()) MakeDir(dir); }
    PHYSFS_File* file = PHYSFS_openWrite(path.c_str());
    if (!file) { ZGINE_CORE_ERROR("Failed to open for write '{}': {}", path, GetLastError()); return false; }
    PHYSFS_sint64 written = PHYSFS_writeBytes(file, data.data(), data.size());
    PHYSFS_close(file);
    if (written != static_cast<PHYSFS_sint64>(data.size())) {
        ZGINE_CORE_ERROR("Write error '{}': {}", path, GetLastError()); return false;
    }
    return true;
}

// ===== File Information =====

bool VFS::Exists(std::string_view filename) {
    if (!s_Initialized) return false;
    std::string path = NormalizePath(filename);
    return PHYSFS_exists(path.c_str()) != 0;
}

bool VFS::IsDirectory(std::string_view filename) {
    if (!s_Initialized) return false;
    std::string path = NormalizeReadPath(filename);
    PHYSFS_Stat stat;
    return PHYSFS_stat(path.c_str(), &stat) && stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

int64_t VFS::GetFileSize(std::string_view filename) {
    if (!s_Initialized) return -1;
    std::string path = NormalizePath(filename);
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(path.c_str(), &stat)) return -1;
    return static_cast<int64_t>(stat.filesize);
}

int64_t VFS::GetModTime(std::string_view filename) {
    if (!s_Initialized) return -1;
    std::string path = NormalizePath(filename);
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(path.c_str(), &stat)) return -1;
    return static_cast<int64_t>(stat.modtime);
}

// ===== Directory Enumeration =====

std::vector<std::string> VFS::EnumerateFiles(std::string_view dir) {
    std::vector<std::string> files;
    if (!s_Initialized) return files;
    std::string path = NormalizeReadPath(dir);
    char** rc = PHYSFS_enumerateFiles(path.c_str());
    if (!rc) return files;
    for (char** i = rc; *i != nullptr; i++) files.push_back(*i);
    PHYSFS_freeList(rc);
    return files;
}

// ===== Write Directory =====

bool VFS::SetWriteDir(std::string_view newDir) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string dir = NormalizePath(newDir);
    if (!PHYSFS_setWriteDir(dir.c_str())) {
        ZGINE_CORE_ERROR("Failed to set write dir '{}': {}", dir, GetLastError()); return false;
    }
    ZGINE_CORE_INFO("VFS write dir: {}", dir);
    return true;
}

std::string VFS::GetWriteDir() {
    if (!s_Initialized) return "";
    const char* d = PHYSFS_getWriteDir();
    return d ? std::string(d) : "";
}

bool VFS::MakeDir(std::string_view dirName) {
    if (!s_Initialized) { ZGINE_CORE_ERROR("VFS not initialized"); return false; }
    std::string path = NormalizePath(dirName);
    // Recursively create parent directories
    size_t pos = 0;
    while ((pos = path.find('/', pos + 1)) != std::string::npos) {
        std::string parent = path.substr(0, pos);
        if (!parent.empty() && !IsDirectory(parent))
            PHYSFS_mkdir(parent.c_str());
    }
    if (!PHYSFS_mkdir(path.c_str())) {
        if (PHYSFS_getLastErrorCode() != PHYSFS_ERR_OK && !IsDirectory(path)) {
            ZGINE_CORE_ERROR("Failed to create dir '{}': {}", path, GetLastError());
            return false;
        }
    }
    return true;
}

// ===== Archive Support =====

std::vector<std::string> VFS::SupportedArchiveTypes() {
    std::vector<std::string> types;
    if (!s_Initialized) return types;
    const PHYSFS_ArchiveInfo** info = PHYSFS_supportedArchiveTypes();
    for (const PHYSFS_ArchiveInfo** i = info; *i != nullptr; i++)
        types.push_back((*i)->extension);
    return types;
}

std::string VFS::GetLastError() {
    const char* msg = PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    return msg ? std::string(msg) : "Unknown error";
}

} // namespace Zgine
