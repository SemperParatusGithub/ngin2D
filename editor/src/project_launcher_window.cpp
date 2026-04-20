#include "project_launcher_window.h"

#include "file_dialog.h"
#include "new_project_dialog.h"
#include "qt_path_utils.h"

#include "project/project.h"
#include "project/project_serializer.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

namespace {

QSettings launcher_settings() {
    return QSettings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("ngin2D"), QStringLiteral("editor"));
}

QStringList load_recent_paths() {
    QSettings s = launcher_settings();
    QStringList list = s.value(QStringLiteral("recent_projects")).toStringList();
    QStringList existing;
    existing.reserve(list.size());
    for (const QString& p : list) {
        if (QFileInfo::exists(p)) {
            existing.append(p);
        }
    }
    if (existing != list) {
        s.setValue(QStringLiteral("recent_projects"), existing);
    }
    return existing;
}

void save_recent_paths(const QStringList& paths) {
    QSettings s = launcher_settings();
    s.setValue(QStringLiteral("recent_projects"), paths);
}

void remember_recent_path(const QString& path) {
    QStringList list = load_recent_paths();
    list.removeAll(path);
    list.prepend(path);
    while (list.size() > 10) {
        list.removeLast();
    }
    save_recent_paths(list);
}

QString project_file_filter() {
    return QStringLiteral("ngin2D project (*.nginproject);;JSON (*.json);;All files (*.*)");
}

} // namespace

ProjectLauncherWindow::ProjectLauncherWindow(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("ngin2D - Project launcher"));
    setModal(true);
    resize(680, 520);
    setMinimumSize(420, 320);

    build_ui();
    refresh_recent_list();
}

void ProjectLauncherWindow::build_ui() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    auto* title = new QLabel(tr("ngin2D"), this);

    auto* subtitle = new QLabel(tr("Open a project or create a new one to start the editor."), this);
    subtitle->setWordWrap(true);

    auto* recent_label = new QLabel(tr("Recent projects"), this);

    m_recent_list = new QListWidget(this);
    m_recent_list->setMinimumHeight(160);

    auto* buttons = new QHBoxLayout();
    buttons->setSpacing(10);

    auto* open_btn = new QPushButton(tr("Open project"), this);
    auto* new_btn = new QPushButton(tr("New project"), this);
    auto* quit_btn = new QPushButton(tr("Quit"), this);

    buttons->addWidget(open_btn, 1);
    buttons->addWidget(new_btn, 1);
    buttons->addWidget(quit_btn);

    root->addWidget(title);
    root->addWidget(subtitle);
    root->addWidget(recent_label);
    root->addWidget(m_recent_list, 1);
    root->addLayout(buttons);

    connect(open_btn, &QPushButton::clicked, this, &ProjectLauncherWindow::on_open_project);
    connect(new_btn, &QPushButton::clicked, this, &ProjectLauncherWindow::on_new_project);
    connect(quit_btn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_recent_list, &QListWidget::itemDoubleClicked, this, &ProjectLauncherWindow::on_recent_item_double_clicked);
}

void ProjectLauncherWindow::refresh_recent_list() {
    m_recent_list->clear();
    const QStringList paths = load_recent_paths();
    if (paths.isEmpty()) {
        auto* placeholder = new QListWidgetItem(tr("No recent projects yet"));
        placeholder->setFlags(Qt::NoItemFlags);
        m_recent_list->addItem(placeholder);
        return;
    }
    for (const QString& p : paths) {
        auto* item = new QListWidgetItem(QFileInfo(p).fileName());
        item->setData(Qt::UserRole, p);
        item->setToolTip(p);
        m_recent_list->addItem(item);
    }
}

void ProjectLauncherWindow::remember_recent(const std::filesystem::path& path) {
    remember_recent_path(ngin::editor::path_to_qstring(path));
}

bool ProjectLauncherWindow::try_load_project_file(const std::filesystem::path& path) {
    ngin::Project test;
    if (!ngin::ProjectSerializer::deserialize(test, path)) {
        QMessageBox::warning(
            this,
            tr("Could not open project"),
            tr("The file could not be read or is not valid project data:\n%1").arg(ngin::editor::path_to_qstring(path)));
        return false;
    }
    return true;
}

void ProjectLauncherWindow::accept_project(const std::filesystem::path& path) {
    m_selected_path = path;
    remember_recent(path);
    accept();
}

void ProjectLauncherWindow::on_open_project() {
    const auto path = ngin::editor::open_file_dialog(this, {.title = tr("Open project"), .name_filter = project_file_filter()});
    if (!path.has_value()) {
        return;
    }
    if (!try_load_project_file(*path)) {
        return;
    }
    accept_project(*path);
}

void ProjectLauncherWindow::on_new_project() {
    NewProjectDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    accept_project(ngin::editor::qstring_to_path(dlg.created_project_file_path()));
}

void ProjectLauncherWindow::on_recent_item_double_clicked(QListWidgetItem* item) {
    if (item == nullptr) {
        return;
    }
    const QVariant data = item->data(Qt::UserRole);
    if (!data.canConvert<QString>()) {
        return;
    }
    const QString path = data.toString();
    if (path.isEmpty()) {
        return;
    }
    const std::filesystem::path fs_path = ngin::editor::qstring_to_path(path);
    if (!try_load_project_file(fs_path)) {
        refresh_recent_list();
        return;
    }
    accept_project(fs_path);
}
