#pragma once

#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace ngin::log {
enum class level {
    trace,
    info,
    warn,
    error,
    critical
};

void init();
void shutdown();

void write(level log_level, std::string_view message);

template <typename... Args>
void trace(std::format_string<Args...> fmt, Args&&... args) {
    write(level::trace, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void info(std::format_string<Args...> fmt, Args&&... args) {
    write(level::info, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void warn(std::format_string<Args...> fmt, Args&&... args) {
    write(level::warn, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void error(std::format_string<Args...> fmt, Args&&... args) {
    write(level::error, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void critical(std::format_string<Args...> fmt, Args&&... args) {
    write(level::critical, std::format(fmt, std::forward<Args>(args)...));
}
} // namespace ngin::log

#define NGIN_TRACE(...) ::ngin::log::trace(__VA_ARGS__)
#define NGIN_INFO(...) ::ngin::log::info(__VA_ARGS__)
#define NGIN_WARN(...) ::ngin::log::warn(__VA_ARGS__)
#define NGIN_ERROR(...) ::ngin::log::error(__VA_ARGS__)
#define NGIN_CRITICAL(...) ::ngin::log::critical(__VA_ARGS__)
