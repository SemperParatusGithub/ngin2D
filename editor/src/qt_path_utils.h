#pragma once

#include <QString>

#include <filesystem>
#include <string>

namespace ngin::editor {

#if defined(_WIN32)
[[nodiscard]] inline std::filesystem::path qstring_to_path(const QString& q) {
    return std::filesystem::path(q.toStdWString());
}

[[nodiscard]] inline QString path_to_qstring(const std::filesystem::path& p) {
    return QString::fromStdWString(p.wstring());
}
#else
[[nodiscard]] inline std::filesystem::path qstring_to_path(const QString& q) {
    return std::filesystem::path(std::string(q.toUtf8().constData()));
}

[[nodiscard]] inline QString path_to_qstring(const std::filesystem::path& p) {
    const std::string& n = p.native();
    return QString::fromUtf8(n.data(), static_cast<int>(n.size()));
}
#endif

} // namespace ngin::editor
