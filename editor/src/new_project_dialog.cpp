#include "new_project_dialog.h"

#include "project/project.h"
#include "project/project_serializer.h"
#include "qt_path_utils.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

NewProjectDialog::NewProjectDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("New project"));
    setModal(true);
    resize(560, 420);
    setMinimumWidth(480);

    m_parent_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (m_parent_dir.isEmpty()) {
        m_parent_dir = QDir::homePath();
    }

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    auto* name_label = new QLabel(tr("Project name"), this);

    m_name_edit = new QLineEdit(this);
    m_name_edit->setText(tr("Untitled Project"));
    m_name_edit->setPlaceholderText(tr("Untitled Project"));

    auto* loc_label = new QLabel(tr("Location"), this);

    auto* loc_row = new QHBoxLayout();
    m_folder_preview = new QLabel(this);
    m_folder_preview->setWordWrap(true);
    m_folder_preview->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto* browse_btn = new QPushButton(tr("Browse…"), this);
    loc_row->addWidget(m_folder_preview, 1);
    loc_row->addWidget(browse_btn, 0, Qt::AlignTop);

    auto* file_label = new QLabel(tr("Project file"), this);

    m_file_preview = new QLabel(this);
    m_file_preview->setWordWrap(true);
    m_file_preview->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto* asset_label = new QLabel(tr("Assets"), this);

    m_use_default_assets = new QCheckBox(tr("Use default asset folder (%1)").arg(QStringLiteral("assets")), this);
    m_use_default_assets->setChecked(true);

    m_asset_path_edit = new QLineEdit(this);
    m_asset_path_edit->setText(QStringLiteral("assets"));
    m_asset_path_edit->setEnabled(false);
    m_asset_path_edit->setPlaceholderText(tr("Relative path from project folder, e.g. assets"));

    auto* buttons = new QHBoxLayout();
    buttons->addStretch(1);
    auto* cancel_btn = new QPushButton(tr("Cancel"), this);
    auto* create_btn = new QPushButton(tr("Create"), this);
    create_btn->setDefault(true);
    buttons->addWidget(cancel_btn);
    buttons->addWidget(create_btn);

    root->addWidget(name_label);
    root->addWidget(m_name_edit);
    root->addWidget(loc_label);
    root->addLayout(loc_row);
    root->addWidget(file_label);
    root->addWidget(m_file_preview);
    root->addWidget(asset_label);
    root->addWidget(m_use_default_assets);
    root->addWidget(m_asset_path_edit);
    root->addStretch(1);
    root->addLayout(buttons);

    connect(m_name_edit, &QLineEdit::textChanged, this, &NewProjectDialog::update_path_previews);
    connect(browse_btn, &QPushButton::clicked, this, &NewProjectDialog::on_browse_parent_folder);
    connect(m_use_default_assets, &QCheckBox::toggled, this, [this](bool checked) {
        m_asset_path_edit->setEnabled(!checked);
        if (checked) {
            m_asset_path_edit->setText(QStringLiteral("assets"));
        }
    });
    connect(cancel_btn, &QPushButton::clicked, this, &QDialog::reject);
    connect(create_btn, &QPushButton::clicked, this, &NewProjectDialog::on_create);

    update_path_previews();
}

QString NewProjectDialog::sanitized_folder_name(const QString& name) const {
    QString s = name.trimmed();
    const QString illegal = QStringLiteral(R"(<>:"/\|?*)");
    for (QChar c : illegal) {
        s.remove(c);
    }
    while (s.endsWith(QLatin1Char('.')) || s.endsWith(QLatin1Char(' '))) {
        s.chop(1);
    }
    if (s.isEmpty()) {
        s = tr("Untitled Project");
    }
    return s;
}

QString NewProjectDialog::project_folder_path() const {
    const QString folder_name = sanitized_folder_name(m_name_edit->text());
    return QDir(m_parent_dir).filePath(folder_name);
}

QString NewProjectDialog::project_file_path() const {
    const QString folder_name = sanitized_folder_name(m_name_edit->text());
    return QDir(project_folder_path()).filePath(folder_name + QStringLiteral(".nginproject"));
}

QString NewProjectDialog::relative_asset_path_string() const {
    if (m_use_default_assets->isChecked()) {
        return QStringLiteral("assets");
    }
    return m_asset_path_edit->text().trimmed();
}

void NewProjectDialog::update_path_previews() {
    const QString folder = QDir::toNativeSeparators(project_folder_path());
    const QString file = QDir::toNativeSeparators(project_file_path());
    m_folder_preview->setText(folder);
    m_file_preview->setText(file);
}

void NewProjectDialog::on_browse_parent_folder() {
    const QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Choose folder for project"),
        m_parent_dir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }
    m_parent_dir = dir;
    update_path_previews();
}

void NewProjectDialog::on_create() {
    const QString display_name = m_name_edit->text().trimmed();
    if (display_name.isEmpty()) {
        QMessageBox::warning(this, tr("New project"), tr("Please enter a project name."));
        return;
    }

    const QString rel_assets = relative_asset_path_string();
    if (!m_use_default_assets->isChecked() && rel_assets.isEmpty()) {
        QMessageBox::warning(
            this,
            tr("New project"),
            tr("Enter a relative asset path from the project folder, or enable \"Use default asset folder\"."));
        return;
    }

    const QString folder_path = project_folder_path();
    const QString file_path = project_file_path();

    if (QFileInfo::exists(file_path)) {
        const auto answer =
            QMessageBox::question(this, tr("Project file exists"), tr("Overwrite existing file?\n%1").arg(file_path));
        if (answer != QMessageBox::Yes) {


            return;
        }
    }

    QDir root_dir;
    if (!root_dir.mkpath(folder_path)) {
        QMessageBox::warning(this, tr("New project"), tr("Could not create the project folder:\n%1").arg(folder_path));
        return;
    }

    ngin::Project project(display_name.toStdString());
    project.set_root_directory(ngin::editor::qstring_to_path(folder_path));
    project.set_relative_asset_directory(ngin::editor::qstring_to_path(rel_assets));
    project.set_relative_default_scene(std::filesystem::path());

    if (!ngin::ProjectSerializer::serialize(project, ngin::editor::qstring_to_path(file_path))) {
        QMessageBox::warning(this, tr("New project"), tr("Failed to write the project file."));
        return;
    }

    m_result_path = file_path;
    accept();
}
