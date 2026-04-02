#include "hierarchy_panel.h"

#include "editor.h"

#include "scene/components/components.h"

#include <QAbstractItemView>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVariant>
#include <QVBoxLayout>

HierarchyPanel::HierarchyPanel(Editor* editor, QWidget* parent) : QWidget(parent), m_editor(editor) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    auto* title = new QLabel(QStringLiteral("Hierarchy"), this);
    layout->addWidget(title);

    m_new_entity_button = new QPushButton(QStringLiteral("New Entity"), this);
    layout->addWidget(m_new_entity_button);

    m_entity_list = new QListWidget(this);
    m_entity_list->setFocusPolicy(Qt::NoFocus);
    m_entity_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_entity_list->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_entity_list, 1);

    QObject::connect(m_new_entity_button, &QPushButton::clicked, this, [this]() {
        on_new_entity_clicked();
    });
    QObject::connect(m_entity_list, &QListWidget::currentItemChanged, this, [this]() {
        on_current_item_changed();
    });
}

void HierarchyPanel::refresh() {
    const ngin::Entity selected = m_editor->get_selected_entity();

    const QSignalBlocker blocker(m_entity_list);
    m_entity_list->clear();

    auto view = m_editor->get_scene().get_registry().view<ngin::TagComponent, ngin::IDComponent>();

    for (const auto& [entity_handle, tag, id] : view.each()) {
        ngin::Entity entity{ entity_handle, &m_editor->get_scene() };
        auto* item = new QListWidgetItem(QString::fromStdString(tag.tag), m_entity_list);
        item->setData(Qt::UserRole, QVariant::fromValue(entity));

        if (selected && entity == selected) {
            m_entity_list->setCurrentItem(item);
        }
    }
}

void HierarchyPanel::on_new_entity_clicked() {
    m_editor->create_entity();
}

void HierarchyPanel::on_current_item_changed() {
    if (!m_entity_list->currentItem()) {
        m_editor->set_selected_entity({});
        return;
    }

    const QVariant data = m_entity_list->currentItem()->data(Qt::UserRole);
    m_editor->set_selected_entity(data.value<ngin::Entity>());
}
