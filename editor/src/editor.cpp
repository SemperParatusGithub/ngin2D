#include "editor.h"

#include "engine_viewport.h"
#include "file_dialog.h"
#include "hierarchy_panel.h"
#include "qt_path_utils.h"
#include "inspector_panel.h"

#include "core/log.h"
#include "project/project_serializer.h"
#include "transform.h"
#include "scene/components/components.h"

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

    auto* side = new QWidget(central);
    side->setFixedWidth(335);

    auto* sideLay = new QVBoxLayout(side);
    sideLay->setContentsMargins(6, 6, 6, 6);
    sideLay->setSpacing(6);

    m_hierarchy_panel = new HierarchyPanel(this, side);
    m_inspector_panel = new InspectorPanel(this, side);

    sideLay->addWidget(m_hierarchy_panel, 1);
    sideLay->addWidget(m_inspector_panel, 1);

    auto* viewport = new EngineViewport(this, central);
    viewport->setFocusPolicy(Qt::ClickFocus);
    viewport->setMinimumSize(320, 240);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    outer->addWidget(side);
    outer->addWidget(viewport, 1);

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

	auto test_ent_1 = m_scene.create_entity();
	auto test_ent_2 = m_scene.create_entity();
    ngin::Transform t;
    t.set_position({ 100.0f, 100.0f, 0.0f });
    t.set_scale({ 100.0f, 100.0f, 0.0f });
    test_ent_1.emplace<ngin::TransformComponent>(t);
	test_ent_1.emplace<ngin::SpriteComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	t.set_position({ 250.0f, 100.0f, 0.0f });
	test_ent_2.emplace<ngin::TransformComponent>(t);
	test_ent_2.emplace<ngin::SpriteComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

    emit scene_contents_changed();
    emit selection_changed();

    window.setCentralWidget(central);

    QMenu* const file_menu = window.menuBar()->addMenu(tr("&File"));
    QAction* const open_action = file_menu->addAction(tr("&Open..."));
    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, &QAction::triggered, &window, [&window]() {
        if (const auto path = ngin::editor::open_file_dialog(&window, {.title = tr("Open")})) {
            NGIN_INFO("File > Open (not implemented): {}", ngin::editor::path_to_qstring(*path).toStdString());
        }
    });

    QAction* const save_action = file_menu->addAction(tr("&Save..."));
    save_action->setShortcut(QKeySequence::Save);
    QObject::connect(save_action, &QAction::triggered, &window, [&window]() {
        if (const auto path = ngin::editor::save_file_dialog(
                &window,
                {
                    .title = tr("Save"),
                    .name_filter = QStringLiteral("Scene (*.scene);;All files (*.*)"),
                    .default_file_name = QStringLiteral("untitled.scene"),
                    .default_suffix = QStringLiteral("scene"),
                }
            )) {
            NGIN_INFO("File > Save (not implemented): {}", ngin::editor::path_to_qstring(*path).toStdString());
        }
    });

    window.show();

    QApplication::exec();
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
    emit scene_contents_changed();
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

    emit scene_contents_changed();
    if (was_selected) {
        emit selection_changed();
    }
}

void Editor::notify_scene_contents_changed() {
    emit scene_contents_changed();
}
