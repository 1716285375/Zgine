#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace Zgine {

/**
 * @brief High-level file utilities using VFS backend
 *
 * Simplified file I/O interface that delegates to the VFS abstraction layer.
 * Supports loading files from both filesystem and mounted archives.
 */
class File {
public:
    // Read entire file as text (UTF-8)
    static std::string ReadFile(const std::string& filepath);

    // Write text to file (UTF-8)
    static bool WriteFile(const std::string& filepath, const std::string& content);

    // Read binary file
    static std::vector<uint8_t> ReadBinaryFile(const std::string& filepath);

    // Write binary file
    static bool WriteBinaryFile(const std::string& filepath, const std::vector<uint8_t>& data);

    // Check if file exists
    static bool Exists(const std::string& filepath);
};

}
