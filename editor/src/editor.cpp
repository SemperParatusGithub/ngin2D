#include "editor.h"

#include "engine_viewport.h"
#include "hierarchy_panel.h"
#include "inspector_panel.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMainWindow>
#include <QShortcut>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

Editor::Editor(QObject* parent) : QObject(parent) {}

void Editor::run() {
    QMainWindow window;
    window.setWindowTitle(QStringLiteral("ngin2D Editor"));
    window.resize(1280, 720);

    auto* central = new QWidget(&window);
    auto* outer = new QHBoxLayout(central);
    outer->setContentsMargins(6, 6, 6, 6);
    outer->setSpacing(0);

    auto* side = new QWidget(central);
    side->setFixedWidth(280);

    auto* sideLay = new QVBoxLayout(side);
    sideLay->setContentsMargins(6, 6, 6, 6);
    sideLay->setSpacing(6);

    m_hierarchy_panel = new HierarchyPanel(this, side);
    m_inspector_panel = new InspectorPanel(this, side);

    sideLay->addWidget(m_hierarchy_panel, 1);
    sideLay->addWidget(m_inspector_panel, 0);

    auto* viewport = new EngineViewport(central);
    viewport->setFocusPolicy(Qt::ClickFocus);
    viewport->setMinimumSize(320, 240);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    outer->addWidget(side);
    outer->addWidget(viewport, 1);

    QObject::connect(this, &Editor::selection_changed, m_hierarchy_panel, &HierarchyPanel::refresh);
    QObject::connect(this, &Editor::selection_changed, m_inspector_panel, &InspectorPanel::refresh);

    QObject::connect(this, &Editor::scene_contents_changed, m_hierarchy_panel, &HierarchyPanel::refresh);
    QObject::connect(this, &Editor::scene_contents_changed, m_inspector_panel, &InspectorPanel::refresh);

    auto* delete_shortcut = new QShortcut(QKeySequence::Delete, &window);
    delete_shortcut->setContext(Qt::WindowShortcut);
    QObject::connect(delete_shortcut, &QShortcut::activated, this, [this]() {
        delete_entity(m_selected_entity);
    });

    emit scene_contents_changed();
    emit selection_changed();

    window.setCentralWidget(central);
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
