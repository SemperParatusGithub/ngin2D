#pragma once

#include "build_config.h"

#include <cstdio>
#include <cstdlib>

#if NGIN_ENABLE_ASSERTS
    #define NGIN_ASSERT(check)                                                                  \
        do {                                                                                    \
            if (!(check)) {                                                                     \
                std::fprintf(stderr, "[NGIN_ASSERT] %s:%d: '%s' failed.\n", __FILE__, __LINE__, #check); \
                NGIN_DEBUGBREAK();                                                              \
                std::abort();                                                                   \
            }                                                                                   \
        } while (false)

    #define NGIN_ASSERT_MSG(check, message)                                                     \
        do {                                                                                    \
            if (!(check)) {                                                                     \
                std::fprintf(stderr, "[NGIN_ASSERT] %s:%d: '%s' failed. %s\n", __FILE__, __LINE__, #check, message); \
                NGIN_DEBUGBREAK();                                                              \
                std::abort();                                                                   \
            }                                                                                   \
        } while (false)
#else
    #define NGIN_ASSERT(check) ((void)0)
    #define NGIN_ASSERT_MSG(check, message) ((void)0)
#endif
