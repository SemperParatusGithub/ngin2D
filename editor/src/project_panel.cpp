#include "project_panel.h"

#include "editor.h"

#include "project/project.h"

#include <QAbstractItemView>
#include <QAction>
#include <QFont>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QSignalBlocker>
#include <QString>
#include <QVBoxLayout>

#include <filesystem>
#include <optional>

namespace {

constexpr auto k_dirty_marker = "* ";
constexpr auto k_scene_extension = ".scene";

QString format_item_label(const QString& rel_path, bool dirty) {
    if (dirty) {
        return QLatin1String(k_dirty_marker) + rel_path;
    }
    return rel_path;
}

// Strip filesystem-hostile characters and surrounding whitespace; auto-append
// the `.scene` extension if the user didn't type one. Returns an empty string
// if nothing sensible remains.
QString sanitize_scene_leaf(const QString& user_input) {
    QString s = user_input.trimmed();
    const QString illegal = QStringLiteral(R"(<>:"/\|?*)");
    for (QChar c : illegal) {
        s.remove(c);
    }
    while (s.endsWith(QLatin1Char('.')) || s.endsWith(QLatin1Char(' '))) {
        s.chop(1);
    }
    if (s.isEmpty()) {
        return {};
    }
    if (!s.endsWith(QLatin1String(k_scene_extension), Qt::CaseInsensitive)) {
        s += QLatin1String(k_scene_extension);
    }
    return s;
}

} // namespace

ProjectPanel::ProjectPanel(Editor* editor, QWidget* parent) : QWidget(parent), m_editor(editor) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    auto* title = new QLabel(QStringLiteral("Project"), this);
    layout->addWidget(title);

    m_name_label = new QLabel(this);
    QFont name_font = m_name_label->font();
    name_font.setBold(true);
    m_name_label->setFont(name_font);
    m_name_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(m_name_label);

    m_root_label = new QLabel(this);
    m_root_label->setWordWrap(true);
    m_root_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_root_label->setStyleSheet(QStringLiteral("color: palette(mid);"));
    layout->addWidget(m_root_label);

    m_scenes_caption = new QLabel(this);
    m_scenes_caption->setWordWrap(true);
    layout->addWidget(m_scenes_caption);

    m_scenes_list = new QListWidget(this);
    m_scenes_list->setFocusPolicy(Qt::NoFocus);
    m_scenes_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scenes_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_scenes_list->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_scenes_list, 1);

    auto* buttons = new QHBoxLayout();
    m_new_scene_button = new QPushButton(QStringLiteral("New Scene"), this);
    m_refresh_button = new QPushButton(QStringLiteral("Refresh"), this);
    buttons->addWidget(m_new_scene_button);
    buttons->addWidget(m_refresh_button);
    layout->addLayout(buttons);

    QObject::connect(m_new_scene_button, &QPushButton::clicked, this, [this]() {
        on_new_scene_clicked();
    });
    QObject::connect(m_refresh_button, &QPushButton::clicked, this, &ProjectPanel::refresh);
    QObject::connect(m_scenes_list, &QListWidget::customContextMenuRequested, this,
        [this](const QPoint& pos) { on_context_menu_requested(pos); });
    QObject::connect(m_scenes_list, &QListWidget::itemDoubleClicked, this,
        [this](QListWidgetItem* item) { on_item_double_clicked(item); });
    QObject::connect(m_editor, &Editor::project_scenes_changed, this, &ProjectPanel::refresh);
    QObject::connect(m_editor, &Editor::scene_contents_changed, this, &ProjectPanel::update_dirty_indicator);

    refresh();
}

void ProjectPanel::refresh() {
    const ngin::Project& project = m_editor->get_project();

    m_name_label->setText(QString::fromStdString(project.get_project_name()));

    const std::filesystem::path root = project.get_root_directory();
    if (root.empty()) {
        m_root_label->setText(tr("(no location)"));
    } else {
        m_root_label->setText(QString::fromStdString(root.generic_string()));
    }

    const std::filesystem::path scenes_dir = project.get_scenes_directory();
    const std::filesystem::path relative_scenes = project.get_relative_scenes_directory();
    if (relative_scenes.empty()) {
        m_scenes_caption->setText(tr("Scenes: (scenes folder not configured)"));
    } else {
        m_scenes_caption->setText(
            tr("Scenes (./%1):").arg(QString::fromStdString(relative_scenes.generic_string())));
    }

    const QSignalBlocker blocker(m_scenes_list);
    m_scenes_list->clear();

    const auto scene_paths = project.enumerate_scenes();
    const auto& current = m_editor->get_current_scene_path();
    const bool dirty = m_editor->is_scene_dirty();

    for (const std::filesystem::path& rel : scene_paths) {
        const QString rel_qs = QString::fromStdString(rel.generic_string());
        const std::filesystem::path abs = scenes_dir / rel;
        const bool is_current = current.has_value() && *current == abs;
        auto* item = new QListWidgetItem(format_item_label(rel_qs, is_current && dirty), m_scenes_list);
        item->setToolTip(QString::fromStdString(abs.generic_string()));
        // Stash the relative (pre-decoration) path. Both the dirty indicator
        // refresh and rename/load wiring read it via UserRole.
        item->setData(Qt::UserRole, rel_qs);
    }

    if (scene_paths.empty()) {
        auto* empty = new QListWidgetItem(tr("(no scenes)"), m_scenes_list);
        empty->setFlags(Qt::NoItemFlags);
    }
}

void ProjectPanel::update_dirty_indicator() {
    const ngin::Project& project = m_editor->get_project();
    const std::filesystem::path scenes_dir = project.get_scenes_directory();
    const auto& current = m_editor->get_current_scene_path();
    const bool dirty = m_editor->is_scene_dirty();

    const QSignalBlocker blocker(m_scenes_list);
    for (int i = 0; i < m_scenes_list->count(); ++i) {
        QListWidgetItem* item = m_scenes_list->item(i);
        const QVariant data = item->data(Qt::UserRole);
        if (!data.isValid() || data.toString().isEmpty()) {
            // Placeholder row, e.g. "(no scenes)".
            continue;
        }
        const QString rel_qs = data.toString();
        const std::filesystem::path abs = scenes_dir / std::filesystem::path{ rel_qs.toStdString() };
        const bool is_current = current.has_value() && *current == abs;
        item->setText(format_item_label(rel_qs, is_current && dirty));
    }
}

void ProjectPanel::on_new_scene_clicked() {
    bool ok = false;
    const QString raw = QInputDialog::getText(
        this, tr("New scene"), tr("Scene name:"),
        QLineEdit::Normal, QStringLiteral("NewScene"), &ok);
    if (!ok) {
        return;
    }
    const QString leaf = sanitize_scene_leaf(raw);
    if (leaf.isEmpty()) {
        QMessageBox::warning(this, tr("New scene"), tr("Please enter a valid scene name."));
        return;
    }

    const std::filesystem::path rel{ leaf.toStdString() };
    if (const auto err = m_editor->create_scene(rel)) {
        QMessageBox::warning(this, tr("New scene"), *err);
    }
    // Success path: project_scenes_changed triggers a full refresh, which
    // also re-applies the dirty indicator (the new scene starts clean).
}

void ProjectPanel::on_context_menu_requested(const QPoint& pos) {
    QListWidgetItem* item = m_scenes_list->itemAt(pos);
    if (!item) {
        return;
    }
    const QVariant data = item->data(Qt::UserRole);
    if (!data.isValid() || data.toString().isEmpty()) {
        return;
    }

    QMenu menu(this);
    QAction* rename_action = menu.addAction(tr("Rename…"));
    QAction* chosen = menu.exec(m_scenes_list->mapToGlobal(pos));
    if (chosen == rename_action) {
        rename_item(item);
    }
}

void ProjectPanel::rename_item(QListWidgetItem* item) {
    const QString old_rel_qs = item->data(Qt::UserRole).toString();
    if (old_rel_qs.isEmpty()) {
        return;
    }
    const std::filesystem::path old_rel{ old_rel_qs.toStdString() };

    // Preserve the subfolder portion of the path; only prompt for the leaf
    // stem. This keeps scope contained to "rename a file in place" — moving
    // between folders is deliberately out of scope for this first version.
    const QString old_leaf = QString::fromStdString(old_rel.filename().string());
    const QString old_stem = QString::fromStdString(old_rel.stem().string());

    bool ok = false;
    const QString raw = QInputDialog::getText(
        this, tr("Rename scene"), tr("New name for %1:").arg(old_leaf),
        QLineEdit::Normal, old_stem, &ok);
    if (!ok) {
        return;
    }
    const QString new_leaf = sanitize_scene_leaf(raw);
    if (new_leaf.isEmpty()) {
        QMessageBox::warning(this, tr("Rename scene"), tr("Please enter a valid scene name."));
        return;
    }

    std::filesystem::path new_rel = old_rel.parent_path() / new_leaf.toStdString();
    if (new_rel == old_rel) {
        return;
    }

    if (const auto err = m_editor->rename_scene(old_rel, new_rel)) {
        QMessageBox::warning(this, tr("Rename scene"), *err);
    }
}

void ProjectPanel::on_item_double_clicked(QListWidgetItem* item) {
    if (!item) {
        return;
    }
    const QVariant data = item->data(Qt::UserRole);
    if (!data.isValid()) {
        return;
    }
    const QString rel_qs = data.toString();
    if (rel_qs.isEmpty()) {
        // Placeholder row ("(no scenes)") — non-interactive.
        return;
    }

    const ngin::Project& project = m_editor->get_project();
    const std::filesystem::path scenes_dir = project.get_scenes_directory();
    const std::filesystem::path abs_path = scenes_dir / std::filesystem::path{ rel_qs.toStdString() };

    // Double-clicking the already-active scene is a no-op. Reloading from
    // disk (and blowing away unsaved edits) would be a footgun without an
    // explicit "Revert" action.
    const auto& current = m_editor->get_current_scene_path();
    if (current.has_value() && *current == abs_path) {
        return;
    }

    if (!confirm_discard_dirty()) {
        return;
    }

    if (const auto err = m_editor->load_scene(abs_path)) {
        QMessageBox::warning(this, tr("Open scene"), *err);
    }
}

bool ProjectPanel::confirm_discard_dirty() {
    if (!m_editor->is_scene_dirty() || !m_editor->get_current_scene_path().has_value()) {
        return true;
    }

    const QMessageBox::StandardButton choice = QMessageBox::question(
        this,
        tr("Unsaved changes"),
        tr("The current scene has unsaved changes. Save before switching?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (choice == QMessageBox::Cancel) {
        return false;
    }
    if (choice == QMessageBox::Save) {
        if (const auto err = m_editor->save_current_scene()) {
            QMessageBox::warning(this, tr("Save scene"), *err);
            return false;
        }
    }
    // QMessageBox::Discard falls through — the caller will proceed and the
    // in-memory changes will be overwritten by the next load.
    return true;
}
