#pragma once

#include <QFileDialog>
#include <QString>

#include <filesystem>
#include <optional>

class QWidget;

namespace ngin::editor {

/// Options for @ref open_file_dialog and @ref save_file_dialog.
struct FileDialogOptions {
    /// Window title. Default: "Open File" / "Save File".
    QString title;
    /// Initial directory. Empty uses the platform default (often last-used or home).
    QString directory;
    /// Filter string, e.g. `"JSON (*.json);;All files (*.*)"`. Empty uses `"All files (*.*)"`.
    QString name_filter;
    /// Save only: suggested file name (relative to @a directory unless absolute).
    QString default_file_name;
    /// Save only: if the chosen path has no extension, append `.<default_suffix>` (without dot).
    QString default_suffix;
    QFileDialog::Options options = QFileDialog::Options();
};

/// Modal open-file dialog. Call from the GUI thread only.
[[nodiscard]] std::optional<std::filesystem::path> open_file_dialog(
    QWidget* parent = nullptr,
    const FileDialogOptions& opts = FileDialogOptions{}
);

/// Modal save-file dialog. Call from the GUI thread only.
[[nodiscard]] std::optional<std::filesystem::path> save_file_dialog(
    QWidget* parent = nullptr,
    const FileDialogOptions& opts = FileDialogOptions{}
);

} // namespace ngin::editor
