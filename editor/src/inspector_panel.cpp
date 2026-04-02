#include "inspector_panel.h"

#include "editor.h"

#include "scene/components/id_component.h"
#include "scene/components/tag_component.h"

#include <QLabel>
#include <QLineEdit>
#include <QSignalBlocker>
#include <QVBoxLayout>

InspectorPanel::InspectorPanel(Editor* editor, QWidget* parent) : QWidget(parent), m_editor(editor) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto* title = new QLabel(QStringLiteral("Inspector"), this);
    layout->addWidget(title);

    auto* id_label = new QLabel(QStringLiteral("ID"), this);
    layout->addWidget(id_label);

    m_id_value = new QLabel(QStringLiteral("-"), this);
    layout->addWidget(m_id_value);

    auto* name_label = new QLabel(QStringLiteral("Tag"), this);
    layout->addWidget(name_label);

    m_name_edit = new QLineEdit(this);
    m_name_edit->setPlaceholderText(QStringLiteral("Entity name"));
    m_name_edit->setEnabled(false);
    layout->addWidget(m_name_edit);

    QObject::connect(m_name_edit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity) {
            return;
        }
        entity.get<ngin::TagComponent>().tag = text.toStdString();
        m_editor->notify_scene_contents_changed();
    });

    QObject::connect(m_name_edit, &QLineEdit::returnPressed, this, [this]() {
        if (m_name_edit) {
            m_name_edit->clearFocus();
        }
    });
}

void InspectorPanel::refresh() {
    const ngin::Entity entity = m_editor->get_selected_entity();

    if (m_name_edit) {
        const QSignalBlocker blocker(m_name_edit);
        m_name_edit->setEnabled(static_cast<bool>(entity));
        m_name_edit->setText(entity
            ? QString::fromStdString(entity.get<ngin::TagComponent>().tag)
            : QString{});
    }

    if (m_id_value) {
        m_id_value->setText(entity
            ? QString::number(entity.get<ngin::IDComponent>().id)
            : QStringLiteral("-"));
    }
}
