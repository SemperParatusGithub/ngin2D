#pragma once

#include <filesystem>

namespace ngin {

class Filesystem {
public:
    Filesystem() = delete;

    // Directory containing the running executable (no trailing separator).
    static std::filesystem::path get_executable_dir();
};

} // namespace ngin
