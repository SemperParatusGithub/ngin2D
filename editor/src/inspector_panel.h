#pragma once

#include <QWidget>

class Editor;
class QLabel;
class QLineEdit;

class InspectorPanel final : public QWidget {
public:
    explicit InspectorPanel(Editor* editor, QWidget* parent = nullptr);

    void refresh();

private:
    Editor* m_editor = nullptr;
    QLabel* m_id_value = nullptr;
    QLineEdit* m_name_edit = nullptr;
};
