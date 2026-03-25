#pragma once

// Platform detection macros:
// - NGIN_PLATFORM_WINDOWS
// - NGIN_PLATFORM_MACOS
// - NGIN_PLATFORM_LINUX
// - NGIN_PLATFORM_IOS
// - NGIN_PLATFORM_ANDROID

#if defined(_WIN32) || defined(_WIN64)
    #define NGIN_PLATFORM_WINDOWS 1
    #define NGIN_PLATFORM_NAME "Windows"
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define NGIN_PLATFORM_IOS 1
        #define NGIN_PLATFORM_NAME "iOS"
    #else
        #define NGIN_PLATFORM_MACOS 1
        #define NGIN_PLATFORM_NAME "macOS"
    #endif
#elif defined(__ANDROID__)
    #define NGIN_PLATFORM_ANDROID 1
    #define NGIN_PLATFORM_NAME "Android"
#elif defined(__linux__)
    #define NGIN_PLATFORM_LINUX 1
    #define NGIN_PLATFORM_NAME "Linux"
#else
    #error "NGIN: Unsupported platform."
#endif

namespace ngin {
inline constexpr const char* platform_name() {
    return NGIN_PLATFORM_NAME;
}
} // namespace ngin
