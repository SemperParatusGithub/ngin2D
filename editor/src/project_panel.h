#pragma once

#include <QWidget>

class Editor;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPoint;
class QPushButton;

class ProjectPanel final : public QWidget {
public:
    explicit ProjectPanel(Editor* editor, QWidget* parent = nullptr);

    void refresh();
    void update_dirty_indicator();

private:
    void on_new_scene_clicked();
    void on_item_double_clicked(QListWidgetItem* item);
    void on_context_menu_requested(const QPoint& pos);
    void rename_item(QListWidgetItem* item);

    // Returns true if the caller should proceed with a destructive action
    // that would replace `m_scene` (e.g. loading another scene). Handles the
    // Save/Discard/Cancel prompt when the current scene is dirty.
    bool confirm_discard_dirty();

    Editor* m_editor = nullptr;
    QLabel* m_name_label = nullptr;
    QLabel* m_root_label = nullptr;
    QLabel* m_scenes_caption = nullptr;
    QListWidget* m_scenes_list = nullptr;
    QPushButton* m_new_scene_button = nullptr;
    QPushButton* m_refresh_button = nullptr;
};
