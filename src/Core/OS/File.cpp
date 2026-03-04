#include <Zgine/Core/OS/File.h>
#include <Zgine/Core/OS/VFS.h>
#include <Zgine/Core/Log/Log.h>

namespace Zgine {

std::string File::ReadFile(const std::string& filepath) {
    std::string content = VFS::ReadFileText(filepath);

    if (content.empty() && VFS::Exists(filepath)) {
        // File exists but is empty or failed to read
        ZGINE_CORE_WARN("File '{}' exists but is empty or could not be read", filepath);
    } else if (!content.empty()) {
        ZGINE_CORE_INFO("Successfully loaded file: {}", filepath);
    } else {
        // File doesn't exist
        ZGINE_CORE_ERROR("Failed to open file: {} (file not found in VFS)", filepath);
    }

    return content;
}

bool File::WriteFile(const std::string& filepath, const std::string& content) {
    bool success = VFS::WriteFileText(filepath, content);

    if (success) {
        ZGINE_CORE_INFO("Successfully wrote file: {}", filepath);
    } else {
        ZGINE_CORE_ERROR("Failed to write file: {}", filepath);
    }

    return success;
}

std::vector<uint8_t> File::ReadBinaryFile(const std::string& filepath) {
    std::vector<uint8_t> data = VFS::ReadFileBytes(filepath);

    if (data.empty() && VFS::Exists(filepath)) {
        ZGINE_CORE_WARN("File '{}' exists but is empty or could not be read", filepath);
    } else if (!data.empty()) {
        ZGINE_CORE_INFO("Successfully loaded binary file: {} ({} bytes)", filepath, data.size());
    } else {
        ZGINE_CORE_ERROR("Failed to open binary file: {} (file not found in VFS)", filepath);
    }

    return data;
}

bool File::WriteBinaryFile(const std::string& filepath, const std::vector<uint8_t>& data) {
    bool success = VFS::WriteFileBytes(filepath, data);

    if (success) {
        ZGINE_CORE_INFO("Successfully wrote binary file: {} ({} bytes)", filepath, data.size());
    } else {
        ZGINE_CORE_ERROR("Failed to write binary file: {}", filepath);
    }

    return success;
}

bool File::Exists(const std::string& filepath) {
    return VFS::Exists(filepath);
}

}
