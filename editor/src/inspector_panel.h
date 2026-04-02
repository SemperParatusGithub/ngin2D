#pragma once

#include <QWidget>

class Editor;
class QLabel;
class QLineEdit;
class QMenu;
class QAction;
class QToolButton;
class QDoubleSpinBox;
class QPushButton;

class InspectorPanel final : public QWidget {
public:
    explicit InspectorPanel(Editor* editor, QWidget* parent = nullptr);

    void refresh();

private:
    void update_component_menu();
    void push_transform_from_spins();

    Editor* m_editor = nullptr;
    QLabel* m_id_value = nullptr;
    QLineEdit* m_name_edit = nullptr;
    QToolButton* m_add_component_button = nullptr;
    QMenu* m_component_menu = nullptr;
    QAction* m_add_sprite_action = nullptr;
    QAction* m_add_transform_action = nullptr;
    QAction* m_remove_sprite_action = nullptr;
    QAction* m_remove_transform_action = nullptr;

    QWidget* m_transform_section = nullptr;
    QDoubleSpinBox* m_pos_x = nullptr;
    QDoubleSpinBox* m_pos_y = nullptr;
    QDoubleSpinBox* m_pos_z = nullptr;
    QDoubleSpinBox* m_rot_x = nullptr;
    QDoubleSpinBox* m_rot_y = nullptr;
    QDoubleSpinBox* m_rot_z = nullptr;
    QDoubleSpinBox* m_scale_x = nullptr;
    QDoubleSpinBox* m_scale_y = nullptr;
    QDoubleSpinBox* m_scale_z = nullptr;

    QWidget* m_sprite_section = nullptr;
    QPushButton* m_sprite_color_button = nullptr;
};
