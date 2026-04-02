#pragma once

#include <QWidget>

class Editor;
class QListWidget;
class QPushButton;

class HierarchyPanel final : public QWidget {
public:
    explicit HierarchyPanel(Editor* editor, QWidget* parent = nullptr);

    void refresh();

private:
    void on_new_entity_clicked();
    void on_current_item_changed();

private:
    Editor* m_editor = nullptr;
    QListWidget* m_entity_list = nullptr;
    QPushButton* m_new_entity_button = nullptr;
};
