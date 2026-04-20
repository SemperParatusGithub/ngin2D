#include "core/filesystem.h"

#include "core/platform_detection.h"

#include <cstddef>
#include <cstdint>

#if NGIN_PLATFORM_WINDOWS
#include <windows.h>
#include <string>
#elif NGIN_PLATFORM_LINUX
#include <unistd.h>
#include <vector>
#elif NGIN_PLATFORM_MACOS || (defined(NGIN_PLATFORM_IOS) && NGIN_PLATFORM_IOS)
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#include <vector>
#endif

namespace ngin {

std::filesystem::path Filesystem::get_executable_dir() {
#if NGIN_PLATFORM_WINDOWS
    std::wstring buf(32768, L'\0');
    const DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (n == 0) {
        return {};
    }
    buf.resize(static_cast<std::size_t>(n));
    return std::filesystem::path(buf).parent_path();
#elif NGIN_PLATFORM_LINUX
    std::string path(4096, '\0');
    ssize_t len = readlink("/proc/self/exe", path.data(), path.size());
    while (len >= static_cast<ssize_t>(path.size()) - 1) {
        path.resize(path.size() * 2);
        len = readlink("/proc/self/exe", path.data(), path.size());
    }
    if (len <= 0) {
        return {};
    }
    path.resize(static_cast<std::size_t>(len));
    return std::filesystem::path(path).parent_path();
#elif NGIN_PLATFORM_MACOS || (defined(NGIN_PLATFORM_IOS) && NGIN_PLATFORM_IOS)
    std::uint32_t bufsize = 0;
    _NSGetExecutablePath(nullptr, &bufsize);
    std::vector<char> buf(bufsize);
    if (_NSGetExecutablePath(buf.data(), &bufsize) != 0) {
        return {};
    }
    char resolved[PATH_MAX];
    if (realpath(buf.data(), resolved) != nullptr) {
        return std::filesystem::path(resolved).parent_path();
    }
    return std::filesystem::path(buf.data()).parent_path();
#else
    return {};
#endif
}

} // namespace ngin
