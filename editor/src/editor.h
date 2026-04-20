#pragma once

#include <QObject>
#include <QString>

#include <filesystem>
#include <optional>

#include "project/project.h"
#include "scene/entity.h"
#include "scene/scene.h"

class HierarchyPanel;
class InspectorPanel;
class ProjectPanel;

class Editor : public QObject {
    Q_OBJECT

public:
    explicit Editor(QObject* parent = nullptr);

    void run(const std::filesystem::path& project_file_path);

    const ngin::Project& get_project() const;

    ngin::Scene& get_scene();
    const ngin::Scene& get_scene() const;

    void set_selected_entity(ngin::Entity entity);
    ngin::Entity get_selected_entity() const { return m_selected_entity; }

    ngin::Entity create_entity();
    void delete_entity(ngin::Entity entity);

    // Current scene identity + dirty state. When `m_current_scene_path` is
    // empty, `m_scene` is an unbound scratch scene (initial state at startup
    // before any scene has been adopted).
    const std::optional<std::filesystem::path>& get_current_scene_path() const { return m_current_scene_path; }
    bool is_scene_dirty() const { return m_scene_dirty; }

    // Create an empty `.scene` file at `<scenes_dir>/<relative_file_path>` and
    // adopt it as the current scene (resets `m_scene`, clears the dirty flag).
    // `relative_file_path` should include the `.scene` extension and be a
    // simple leaf name for now (no subfolders). Returns a user-facing error
    // message on failure; std::nullopt on success.
    std::optional<QString> create_scene(const std::filesystem::path& relative_file_path);

    // Rename `<scenes_dir>/<relative_old>` to `<scenes_dir>/<relative_new>`.
    // If the renamed file is the current scene, updates the stored current
    // path. Returns a user-facing error message on failure.
    std::optional<QString> rename_scene(
        const std::filesystem::path& relative_old,
        const std::filesystem::path& relative_new);

    // Serialize `m_scene` over its current file on disk and clear the dirty
    // flag. Returns an error message if no scene is bound or if serialization
    // fails.
    std::optional<QString> save_current_scene();

    // Persist every scene owned by the project. Today only the active scene
    // lives in memory, so this is effectively `save_current_scene` when a
    // scene is bound (scenes on disk that aren't loaded are already
    // up-to-date). Returns an error message if saving the current scene
    // fails. With no scene bound this is a no-op and returns std::nullopt.
    std::optional<QString> save_all_scenes();

    // Serialize the project file back to `m_project_file_path` and then
    // persist every scene via `save_all_scenes`. Returns the first
    // user-facing error encountered (project file write error takes
    // precedence over scene save errors) or std::nullopt on success.
    std::optional<QString> save_project();

    // Deserialize the scene at `absolute_path` into `m_scene` and adopt it as
    // the current scene. Any prior in-memory scene state (including selection)
    // is discarded. Clears the dirty flag on success. On deserialize failure,
    // leaves the editor with an empty scratch scene and no bound path.
    std::optional<QString> load_scene(const std::filesystem::path& absolute_path);

    void notify_scene_contents_changed();

signals:
    void selection_changed();
    void scene_contents_changed();
    void project_scenes_changed();

private:
    ngin::Project m_project;
    std::filesystem::path m_project_file_path;
    ngin::Scene m_scene;
    ngin::Entity m_selected_entity;
    std::optional<std::filesystem::path> m_current_scene_path;
    bool m_scene_dirty = false;
    ProjectPanel* m_project_panel = nullptr;
    HierarchyPanel* m_hierarchy_panel = nullptr;
    InspectorPanel* m_inspector_panel = nullptr;
};
