#include <Zgine/Platform/IO/File.h>
#include <Zgine/Platform/IO/VFS.h>
#include <Zgine/Core/Log/Log.h>
#include <filesystem>

namespace Zgine {

std::string File::ReadFile(std::string_view filepath) {
    std::string content = VFS::ReadFileText(filepath);

    if (content.empty() && VFS::Exists(filepath)) {
        ZGINE_CORE_WARN("File '{}' exists but is empty or could not be read", filepath);
    } else if (!content.empty()) {
        ZGINE_CORE_INFO("Successfully loaded file: {}", filepath);
    } else {
        ZGINE_CORE_ERROR("Failed to open file: {} (file not found in VFS)", filepath);
    }

    return content;
}

bool File::WriteFile(std::string_view filepath, std::string_view content) {
    bool success = VFS::WriteFileText(filepath, content);

    if (success) {
        ZGINE_CORE_INFO("Successfully wrote file: {}", filepath);
    } else {
        ZGINE_CORE_ERROR("Failed to write file: {}", filepath);
    }

    return success;
}

std::vector<uint8_t> File::ReadBinaryFile(std::string_view filepath) {
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

bool File::WriteBinaryFile(std::string_view filepath, const std::vector<uint8_t>& data) {
    bool success = VFS::WriteFileBytes(filepath, data);

    if (success) {
        ZGINE_CORE_INFO("Successfully wrote binary file: {} ({} bytes)", filepath, data.size());
    } else {
        ZGINE_CORE_ERROR("Failed to write binary file: {}", filepath);
    }

    return success;
}

bool File::Exists(std::string_view filepath) {
    return std::filesystem::exists(filepath);
}

}
