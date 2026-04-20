#include "editor.h"

#include "engine_viewport.h"
#include "hierarchy_panel.h"
#include "inspector_panel.h"
#include "project_panel.h"

#include "core/log.h"
#include "project/project_serializer.h"
#include "scene/scene_serializer.h"
#include "transform.h"
#include "scene/components/components.h"

#include <system_error>

#include <QApplication>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

Editor::Editor(QObject* parent) : QObject(parent) {}

void Editor::run(const std::filesystem::path& project_file_path) {
    m_project_file_path = project_file_path;
    if (!ngin::ProjectSerializer::deserialize(m_project, project_file_path)) {
        QMessageBox::critical(nullptr, tr("Editor"), tr("Could not load the project file."));
        return;
    }

    QMainWindow window;
    window.setWindowTitle(
        tr("%1 - %2").arg(QStringLiteral("ngin2D Editor"), QString::fromStdString(m_project.get_project_name())));
    window.resize(1280, 720);

    auto* central = new QWidget(&window);
    auto* outer = new QHBoxLayout(central);
    outer->setContentsMargins(6, 6, 6, 6);
    outer->setSpacing(0);

    constexpr int k_side_panel_width = 335;

    auto* left_side = new QWidget(central);
    left_side->setFixedWidth(k_side_panel_width);

    auto* left_lay = new QVBoxLayout(left_side);
    left_lay->setContentsMargins(6, 6, 6, 6);
    left_lay->setSpacing(6);

    m_hierarchy_panel = new HierarchyPanel(this, left_side);
    m_inspector_panel = new InspectorPanel(this, left_side);

    left_lay->addWidget(m_hierarchy_panel, 1);
    left_lay->addWidget(m_inspector_panel, 1);

    auto* viewport = new EngineViewport(this, central);
    viewport->setFocusPolicy(Qt::ClickFocus);
    viewport->setMinimumSize(320, 240);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* right_side = new QWidget(central);
    right_side->setFixedWidth(k_side_panel_width);

    auto* right_lay = new QVBoxLayout(right_side);
    right_lay->setContentsMargins(6, 6, 6, 6);
    right_lay->setSpacing(6);

    m_project_panel = new ProjectPanel(this, right_side);
    right_lay->addWidget(m_project_panel, 1);

    outer->addWidget(left_side);
    outer->addWidget(viewport, 1);
    outer->addWidget(right_side);

    QObject::connect(this, &Editor::selection_changed, m_hierarchy_panel, &HierarchyPanel::refresh);
    QObject::connect(this, &Editor::selection_changed, m_inspector_panel, &InspectorPanel::refresh);

    QObject::connect(this, &Editor::scene_contents_changed, m_hierarchy_panel, &HierarchyPanel::refresh);
    QObject::connect(this, &Editor::scene_contents_changed, m_inspector_panel, &InspectorPanel::refresh);

    QObject::connect(this, &Editor::scene_contents_changed, viewport, [viewport]() {
        viewport->update();
    });

    auto* delete_shortcut = new QShortcut(QKeySequence::Delete, &window);
    delete_shortcut->setContext(Qt::WindowShortcut);
    QObject::connect(delete_shortcut, &QShortcut::activated, this, [this]() {
        delete_entity(m_selected_entity);
    });

    // Keep the window title in sync with the active scene and its dirty flag.
    // Listens on scene_contents_changed (fired on load, save, rename, and any
    // scene mutation) so both scene identity and dirty state keep it current.
    const auto update_title = [this, window_ptr = &window]() {
        QString title = QString::fromStdString(m_project.get_project_name());
        if (title.isEmpty()) {
            title = tr("Untitled project");
        }
        title.prepend(QStringLiteral("ngin2D Editor - "));
        if (m_current_scene_path) {
            title += QStringLiteral(" — %1").arg(
                QString::fromStdString(m_current_scene_path->filename().string()));
            if (m_scene_dirty) {
                title += QStringLiteral(" *");
            }
        }
        window_ptr->setWindowTitle(title);
    };
    QObject::connect(this, &Editor::scene_contents_changed, &window, update_title);
    update_title();

    // Auto-load the configured default scene if one is set and present on disk.
    // If nothing loads here, the editor starts with an empty unbound scratch
    // scene; the user creates or opens a scene via the Project panel.
    if (!m_project.get_realitve_default_scene().empty()) {
        const std::filesystem::path default_abs = m_project.get_default_scene();
        std::error_code ec;
        if (std::filesystem::exists(default_abs, ec) && !ec) {
            if (const auto err = load_scene(default_abs)) {
                NGIN_WARN("Failed to auto-load default scene '{}': {}",
                    default_abs.generic_string(), err->toStdString());
            }
        }
    }

    // Initial panel kick for the "no scene loaded" case. If load_scene ran
    // above it has already fired these, but a second emission is cheap.
    emit scene_contents_changed();
    emit selection_changed();

    window.setCentralWidget(central);

    QMenu* const file_menu = window.menuBar()->addMenu(tr("&File"));

    QAction* const save_project_action = file_menu->addAction(tr("Save &Project"));
    save_project_action->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S));
    QObject::connect(save_project_action, &QAction::triggered, &window, [this, &window]() {
        if (const auto err = save_project()) {
            QMessageBox::warning(&window, tr("Save Project"), *err);
        }
    });

    QAction* const save_scene_action = file_menu->addAction(tr("&Save Current Scene"));
    save_scene_action->setShortcut(QKeySequence::Save);
    QObject::connect(save_scene_action, &QAction::triggered, &window, [this, &window]() {
        if (const auto err = save_current_scene()) {
            QMessageBox::warning(&window, tr("Save Scene"), *err);
        }
    });

    QAction* const save_all_scenes_action = file_menu->addAction(tr("Save &All Scenes"));
    save_all_scenes_action->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    QObject::connect(save_all_scenes_action, &QAction::triggered, &window, [this, &window]() {
        if (const auto err = save_all_scenes()) {
            QMessageBox::warning(&window, tr("Save All Scenes"), *err);
        }
    });

    window.show();

    QApplication::exec();
}

const ngin::Project& Editor::get_project() const {
    return m_project;
}

ngin::Scene& Editor::get_scene() {
    return m_scene;
}

const ngin::Scene& Editor::get_scene() const {
    return m_scene;
}

void Editor::set_selected_entity(ngin::Entity entity) {
	if (m_selected_entity == entity) {
		return;
	}
	m_selected_entity = entity;
	emit selection_changed();
}

ngin::Entity Editor::create_entity() {
    const ngin::Entity created = m_scene.create_entity();
    set_selected_entity(created);
    notify_scene_contents_changed();
    return created;
}

void Editor::delete_entity(ngin::Entity entity) {
    if (!entity) {
        return;
    }

    const bool was_selected = (m_selected_entity == entity);
    m_scene.destroy_entity(entity);

    if (was_selected) {
        m_selected_entity = {};
    }

    notify_scene_contents_changed();
    if (was_selected) {
        emit selection_changed();
    }
}

void Editor::notify_scene_contents_changed() {
    // Single point of truth for user-driven scene mutations: flip the dirty
    // flag and notify listeners. Paths that reset or adopt the scene (e.g.
    // `create_scene`) deliberately bypass this and manage the dirty flag
    // themselves.
    m_scene_dirty = true;
    emit scene_contents_changed();
}

std::optional<QString> Editor::create_scene(const std::filesystem::path& relative_file_path) {
    if (relative_file_path.empty()) {
        return tr("Scene file name is empty.");
    }
    if (m_project.get_relative_scenes_directory().empty()) {
        return tr("The project has no scenes folder configured.");
    }

    const std::filesystem::path scenes_dir = m_project.get_scenes_directory();
    const std::filesystem::path abs_path = scenes_dir / relative_file_path;

    std::error_code ec;
    std::filesystem::create_directories(abs_path.parent_path(), ec);
    if (ec) {
        return tr("Could not create scenes folder: %1").arg(QString::fromStdString(ec.message()));
    }

    if (std::filesystem::exists(abs_path, ec) && !ec) {
        return tr("A scene with that name already exists.");
    }

    // Adopt a clean empty scene. Any prior unsaved in-memory state (including
    // the selection) is discarded here; a "save before switching" prompt
    // belongs next to the Save wiring once it lands.
    m_scene.clear();
    m_selected_entity = {};

    if (!ngin::SceneSerializer::serialize(m_scene, abs_path)) {
        return tr("Failed to write scene file: %1").arg(QString::fromStdString(abs_path.generic_string()));
    }

    m_current_scene_path = abs_path;
    m_scene_dirty = false;

    emit project_scenes_changed();
    emit scene_contents_changed();
    emit selection_changed();

    return std::nullopt;
}

std::optional<QString> Editor::rename_scene(
    const std::filesystem::path& relative_old,
    const std::filesystem::path& relative_new) {
    if (relative_old.empty() || relative_new.empty()) {
        return tr("Scene file name is empty.");
    }
    if (relative_old == relative_new) {
        return std::nullopt;
    }
    if (m_project.get_relative_scenes_directory().empty()) {
        return tr("The project has no scenes folder configured.");
    }

    const std::filesystem::path scenes_dir = m_project.get_scenes_directory();
    const std::filesystem::path old_abs = scenes_dir / relative_old;
    const std::filesystem::path new_abs = scenes_dir / relative_new;

    std::error_code ec;
    if (!std::filesystem::exists(old_abs, ec) || ec) {
        return tr("Source scene no longer exists on disk.");
    }
    if (std::filesystem::exists(new_abs, ec) && !ec) {
        return tr("A scene with that name already exists.");
    }

    std::filesystem::rename(old_abs, new_abs, ec);
    if (ec) {
        return tr("Rename failed: %1").arg(QString::fromStdString(ec.message()));
    }

    const bool renamed_current = m_current_scene_path && *m_current_scene_path == old_abs;
    if (renamed_current) {
        m_current_scene_path = new_abs;
    }

    emit project_scenes_changed();
    if (renamed_current) {
        // Window title + dirty indicator reference the current scene path;
        // re-emit so those listeners pick up the new name.
        emit scene_contents_changed();
    }
    return std::nullopt;
}

std::optional<QString> Editor::save_current_scene() {
    if (!m_current_scene_path) {
        return tr("No scene is currently loaded.");
    }

    if (!ngin::SceneSerializer::serialize(m_scene, *m_current_scene_path)) {
        return tr("Failed to write scene file: %1")
            .arg(QString::fromStdString(m_current_scene_path->generic_string()));
    }

    const bool was_dirty = m_scene_dirty;
    m_scene_dirty = false;
    if (was_dirty) {
        // Only notify if state changed; this prevents spurious full refreshes.
        emit scene_contents_changed();
    }
    return std::nullopt;
}

std::optional<QString> Editor::save_all_scenes() {
    // The editor only keeps a single scene resident in memory, so the only
    // scene that can possibly have unsaved changes is the bound one. Scenes
    // sitting on disk are authoritative until they're loaded and edited.
    if (!m_current_scene_path) {
        return std::nullopt;
    }
    return save_current_scene();
}

std::optional<QString> Editor::save_project() {
    if (m_project_file_path.empty()) {
        return tr("No project file path is set.");
    }

    if (!ngin::ProjectSerializer::serialize(m_project, m_project_file_path)) {
        return tr("Failed to write project file: %1")
            .arg(QString::fromStdString(m_project_file_path.generic_string()));
    }

    // Saving the project implies persisting any scene edits as well — users
    // expect "Save Project" to leave the whole project in a clean state on
    // disk, not just the `.nginproj` file.
    return save_all_scenes();
}

std::optional<QString> Editor::load_scene(const std::filesystem::path& absolute_path) {
    if (absolute_path.empty()) {
        return tr("Scene file path is empty.");
    }

    std::error_code ec;
    if (!std::filesystem::exists(absolute_path, ec) || ec) {
        return tr("Scene file does not exist: %1")
            .arg(QString::fromStdString(absolute_path.generic_string()));
    }

    // Reset before deserializing so the scene is in a known-empty state even
    // if deserialize bails out partway through.
    m_scene.clear();
    m_selected_entity = {};

    if (!ngin::SceneSerializer::deserialize(m_scene, absolute_path)) {
        m_current_scene_path.reset();
        m_scene_dirty = false;
        emit scene_contents_changed();
        emit selection_changed();
        return tr("Failed to load scene: %1")
            .arg(QString::fromStdString(absolute_path.generic_string()));
    }

    m_current_scene_path = absolute_path;
    m_scene_dirty = false;

    emit scene_contents_changed();
    emit selection_changed();
    return std::nullopt;
}
