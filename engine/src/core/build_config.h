#pragma once

#include "../platform_detection.h"

// Build configuration macros.
#if defined(NDEBUG)
    #define NGIN_DEBUG 0
    #define NGIN_RELEASE 1
#else
    #define NGIN_DEBUG 1
    #define NGIN_RELEASE 0
#endif

#if NGIN_DEBUG
    #define NGIN_ENABLE_ASSERTS 1
#else
    #define NGIN_ENABLE_ASSERTS 0
#endif

// Compiler detection macros.
#if defined(_MSC_VER)
    #define NGIN_COMPILER_MSVC 1
#elif defined(__clang__)
    #define NGIN_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define NGIN_COMPILER_GCC 1
#else
    #error "NGIN: Unsupported compiler."
#endif

// Debug break helper.
#if defined(_MSC_VER)
    #define NGIN_DEBUGBREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define NGIN_DEBUGBREAK() __builtin_trap()
#else
    #define NGIN_DEBUGBREAK() ((void)0)
#endif
