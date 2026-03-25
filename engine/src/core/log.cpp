#include "log.h"

#include "build_config.h"
#include "types.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ngin::log {
namespace {
ref<spdlog::logger> s_core_logger;
} // namespace

void init() {
    if (s_core_logger) {
        return;
    }

    s_core_logger = spdlog::stdout_color_mt("NGIN");
    s_core_logger->set_pattern("[%T] [%^%l%$] %v");
#if NGIN_DEBUG
    s_core_logger->set_level(spdlog::level::trace);
#else
    s_core_logger->set_level(spdlog::level::info);
#endif
}

void shutdown() {
    s_core_logger.reset();
    spdlog::shutdown();
}

void write(level log_level, std::string_view message) {
    if (!s_core_logger) {
        return;
    }

    switch (log_level) {
        case level::trace:
            s_core_logger->trace(message);
            break;
        case level::info:
            s_core_logger->info(message);
            break;
        case level::warn:
            s_core_logger->warn(message);
            break;
        case level::error:
            s_core_logger->error(message);
            break;
        case level::critical:
            s_core_logger->critical(message);
            break;
    }
}
} // namespace ngin::log
