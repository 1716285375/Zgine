#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace Zgine {

/**
 * @brief High-level file utilities (platform-specific I/O via VFS backend).
 *
 * Sits in Platform/IO because file system access is inherently OS-dependent.
 * Delegates to VFS for transparent archive/directory support.
 */
class File {
public:
    /*
        Purpose : Read entire file as UTF-8 text.
        Return  : File contents, or empty string on failure.
    */
    [[nodiscard]] static std::string ReadFile(std::string_view filepath);

    /*
        Purpose : Write UTF-8 text to a file.
        Return  : true on success.
    */
    static bool WriteFile(std::string_view filepath, std::string_view content);

    /*
        Purpose : Read entire file as raw bytes.
        Return  : Byte vector, or empty vector on failure.
    */
    [[nodiscard]] static std::vector<uint8_t> ReadBinaryFile(std::string_view filepath);

    /*
        Purpose : Write raw bytes to a file.
        Return  : true on success.
    */
    static bool WriteBinaryFile(std::string_view filepath, const std::vector<uint8_t>& data);

    /*
        Purpose : Check whether a file exists.
    */
    [[nodiscard]] static bool Exists(std::string_view filepath);
};

} // namespace Zgine
