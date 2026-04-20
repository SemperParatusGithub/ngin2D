#pragma once

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QLabel;

class NewProjectDialog final : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget* parent = nullptr);

    [[nodiscard]] QString created_project_file_path() const { return m_result_path; }

private slots:
    void on_browse_parent_folder();
    void update_path_previews();
    void on_create();

private:
    [[nodiscard]] QString sanitized_folder_name(const QString& name) const;
    [[nodiscard]] QString project_folder_path() const;
    [[nodiscard]] QString project_file_path() const;
    [[nodiscard]] QString relative_asset_path_string() const;

    QString m_parent_dir;
    QString m_result_path;

    QLineEdit* m_name_edit = nullptr;
    QLabel* m_folder_preview = nullptr;
    QLabel* m_file_preview = nullptr;
    QCheckBox* m_use_default_assets = nullptr;
    QLineEdit* m_asset_path_edit = nullptr;
};
