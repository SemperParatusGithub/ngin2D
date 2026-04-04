#include "file_dialog.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QWidget>

namespace ngin::editor {

namespace {

QWidget* effective_parent(QWidget* parent) {
    if (parent != nullptr) {
        return parent;
    }
    return QApplication::activeWindow();
}

QString effective_name_filter(const FileDialogOptions& opts) {
    if (opts.name_filter.isEmpty()) {
        return QStringLiteral("All files (*.*)");
    }
    return opts.name_filter;
}

QString save_dialog_start_path(const FileDialogOptions& opts) {
    const QString base_dir = opts.directory.isEmpty() ? QDir::homePath() : opts.directory;

    if (opts.default_file_name.isEmpty()) {
        return base_dir;
    }

    const QFileInfo name_info(opts.default_file_name);
    if (name_info.isAbsolute()) {
        return opts.default_file_name;
    }
    return QDir(base_dir).filePath(opts.default_file_name);
}

QString maybe_apply_default_suffix(const QString& path, const QString& default_suffix) {
    if (default_suffix.isEmpty()) {
        return path;
    }

    QString suffix = default_suffix;
    if (suffix.startsWith(QLatin1Char('.'))) {
        suffix = suffix.mid(1);
    }
    if (suffix.isEmpty()) {
        return path;
    }

    if (!QFileInfo(path).suffix().isEmpty()) {
        return path;
    }

    return path + QLatin1Char('.') + suffix;
}

} // namespace

std::optional<QString> open_file_dialog(QWidget* parent, const FileDialogOptions& opts) {
    QWidget* const p = effective_parent(parent);
    const QString caption =
        opts.title.isEmpty() ? QStringLiteral("Open File") : opts.title;
    const QString path = QFileDialog::getOpenFileName(
        p,
        caption,
        opts.directory,
        effective_name_filter(opts),
        nullptr,
        opts.options
    );
    if (path.isEmpty()) {
        return std::nullopt;
    }
    return path;
}

std::optional<QString> save_file_dialog(QWidget* parent, const FileDialogOptions& opts) {
    QWidget* const p = effective_parent(parent);
    const QString caption =
        opts.title.isEmpty() ? QStringLiteral("Save File") : opts.title;
    const QString start_path = save_dialog_start_path(opts);
    QString path = QFileDialog::getSaveFileName(
        p,
        caption,
        start_path,
        effective_name_filter(opts),
        nullptr,
        opts.options
    );
    if (path.isEmpty()) {
        return std::nullopt;
    }
    path = maybe_apply_default_suffix(path, opts.default_suffix);
    return path;
}

} // namespace ngin::editor
