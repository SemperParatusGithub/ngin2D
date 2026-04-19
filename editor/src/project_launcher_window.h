#pragma once

#include <QDialog>

#include <filesystem>

class QListWidget;
class QListWidgetItem;

class ProjectLauncherWindow final : public QDialog {
    Q_OBJECT

public:
    explicit ProjectLauncherWindow(QWidget* parent = nullptr);

    std::filesystem::path selected_project_path() const { return m_selected_path; }

private slots:
    void on_open_project();
    void on_new_project();
    void on_recent_item_double_clicked(QListWidgetItem* item);

private:
    void build_ui();
    void refresh_recent_list();
    void remember_recent(const std::filesystem::path& path);
    void accept_project(const std::filesystem::path& path);
    bool try_load_project_file(const std::filesystem::path& path);

    std::filesystem::path m_selected_path;
    QListWidget* m_recent_list = nullptr;
};
