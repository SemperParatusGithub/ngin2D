#pragma once

#include <QObject>

#include <filesystem>

#include "project/project.h"
#include "scene/entity.h"
#include "scene/scene.h"

class HierarchyPanel;
class InspectorPanel;

class Editor : public QObject {
    Q_OBJECT

public:
    explicit Editor(QObject* parent = nullptr);

    void run(const std::filesystem::path& project_file_path);

    ngin::Scene& get_scene();
    const ngin::Scene& get_scene() const;

    void set_selected_entity(ngin::Entity entity);
    ngin::Entity get_selected_entity() const { return m_selected_entity; }

    ngin::Entity create_entity();
    void delete_entity(ngin::Entity entity);

    void notify_scene_contents_changed();

signals:
    void selection_changed();
    void scene_contents_changed();

private:
    ngin::Project m_project;
    std::filesystem::path m_project_file_path;
    ngin::Scene m_scene;
    ngin::Entity m_selected_entity;
    HierarchyPanel* m_hierarchy_panel = nullptr;
    InspectorPanel* m_inspector_panel = nullptr;
};
