#include "inspector_panel.h"

#include "editor.h"

#include "scene/components/id_component.h"
#include "scene/components/sprite_component.h"
#include "scene/components/tag_component.h"
#include "scene/components/transform_component.h"

#include <glm/glm.hpp>

#include <algorithm>

#include <QColor>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSignalBlocker>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

constexpr double kSpinRange = 1'000'000.0;

void configure_vec_spin(QDoubleSpinBox* s, double value) {
    s->setDecimals(3);
    s->setRange(-kSpinRange, kSpinRange);
    s->setSingleStep(0.1);
    s->setValue(value);
    s->setFixedWidth(72);
}

void configure_rot_spin(QDoubleSpinBox* s, double degrees) {
    s->setDecimals(2);
    s->setRange(-360.0, 360.0);
    s->setSingleStep(1.0);
    s->setSuffix(QStringLiteral(" \u00B0"));
    s->setValue(degrees);
    s->setFixedWidth(72);
}

QHBoxLayout* make_xyz_row(
    QWidget* parent,
    const QString& rowLabel,
    QDoubleSpinBox** outX,
    QDoubleSpinBox** outY,
    QDoubleSpinBox** outZ,
    void (*configure)(QDoubleSpinBox*, double),
    double defX,
    double defY,
    double defZ
) {
    auto* row = new QHBoxLayout();
    row->setSpacing(6);

    auto* name = new QLabel(rowLabel, parent);
    name->setMinimumWidth(54);
    row->addWidget(name);

    const QString axes[] = {QStringLiteral("X"), QStringLiteral("Y"), QStringLiteral("Z")};
    QDoubleSpinBox* spins[3] = {nullptr, nullptr, nullptr};
    const double defs[] = {defX, defY, defZ};

    for (int i = 0; i < 3; ++i) {
        row->addWidget(new QLabel(axes[i], parent));
        spins[i] = new QDoubleSpinBox(parent);
        configure(spins[i], defs[i]);
        row->addWidget(spins[i]);
    }

    row->addStretch(1);

    *outX = spins[0];
    *outY = spins[1];
    *outZ = spins[2];
    return row;
}

QColor vec4_to_qcolor(const glm::vec4& v) {
    const auto c = [](float x) { return std::clamp(x, 0.f, 1.f); };
    return QColor::fromRgbF(c(v.r), c(v.g), c(v.b), c(v.a));
}

void apply_sprite_color_swatch(QPushButton* btn, const glm::vec4& color) {
    const QColor qc = vec4_to_qcolor(color);
    btn->setStyleSheet(
        QStringLiteral("QPushButton { background-color: %1; }").arg(qc.name(QColor::HexArgb)));
    btn->setToolTip(QStringLiteral("Sprite tint (click to pick)"));
}

} // namespace

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

    auto* components_row = new QHBoxLayout();
    components_row->setSpacing(8);
    auto* components_label = new QLabel(QStringLiteral("Components"), this);
    components_row->addWidget(components_label);
    components_row->addStretch(1);

    m_add_component_button = new QToolButton(this);
    m_add_component_button->setText(QStringLiteral("+"));
    m_add_component_button->setToolTip(QStringLiteral("Add or remove components"));
    m_add_component_button->setPopupMode(QToolButton::InstantPopup);
    m_add_component_button->setEnabled(false);
    m_add_component_button->setFixedSize(28, 28);

    m_component_menu = new QMenu(m_add_component_button);
    m_add_sprite_action = m_component_menu->addAction(QStringLiteral("Add sprite component"));
    m_add_transform_action = m_component_menu->addAction(QStringLiteral("Add transform component"));
    m_component_menu->addSeparator();
    m_remove_sprite_action = m_component_menu->addAction(QStringLiteral("Remove sprite component"));
    m_remove_transform_action = m_component_menu->addAction(QStringLiteral("Remove transform component"));
    m_add_component_button->setMenu(m_component_menu);

    QObject::connect(m_component_menu, &QMenu::aboutToShow, this, &InspectorPanel::update_component_menu);

    QObject::connect(m_add_sprite_action, &QAction::triggered, this, [this]() {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity || entity.has<ngin::SpriteComponent>()) {
            return;
        }
        entity.emplace<ngin::SpriteComponent>();
        m_editor->notify_scene_contents_changed();
    });

    QObject::connect(m_add_transform_action, &QAction::triggered, this, [this]() {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity || entity.has<ngin::TransformComponent>()) {
            return;
        }
        entity.emplace<ngin::TransformComponent>();
        m_editor->notify_scene_contents_changed();
    });

    QObject::connect(m_remove_sprite_action, &QAction::triggered, this, [this]() {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity || !entity.has<ngin::SpriteComponent>()) {
            return;
        }
        entity.remove<ngin::SpriteComponent>();
        m_editor->notify_scene_contents_changed();
    });

    QObject::connect(m_remove_transform_action, &QAction::triggered, this, [this]() {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity || !entity.has<ngin::TransformComponent>()) {
            return;
        }
        entity.remove<ngin::TransformComponent>();
        m_editor->notify_scene_contents_changed();
    });

    components_row->addWidget(m_add_component_button);
    layout->addLayout(components_row);

    m_transform_section = new QWidget(this);
    auto* tLay = new QVBoxLayout(m_transform_section);
    tLay->setContentsMargins(0, 4, 0, 0);
    tLay->setSpacing(6);

    auto* transTitle = new QLabel(QStringLiteral("Transform"), m_transform_section);
    tLay->addWidget(transTitle);

    tLay->addLayout(make_xyz_row(
        m_transform_section,
        QStringLiteral("Position"),
        &m_pos_x,
        &m_pos_y,
        &m_pos_z,
        configure_vec_spin,
        0.0,
        0.0,
        0.0
    ));
    tLay->addLayout(make_xyz_row(
        m_transform_section,
        QStringLiteral("Rotation"),
        &m_rot_x,
        &m_rot_y,
        &m_rot_z,
        configure_rot_spin,
        0.0,
        0.0,
        0.0
    ));
    tLay->addLayout(make_xyz_row(
        m_transform_section,
        QStringLiteral("Scale"),
        &m_scale_x,
        &m_scale_y,
        &m_scale_z,
        configure_vec_spin,
        1.0,
        1.0,
        1.0
    ));

    layout->addWidget(m_transform_section);

    m_sprite_section = new QWidget(this);
    auto* sLay = new QVBoxLayout(m_sprite_section);
    sLay->setContentsMargins(0, 4, 0, 0);
    sLay->setSpacing(6);

    auto* sprite_title = new QLabel(QStringLiteral("Sprite"), m_sprite_section);
    sLay->addWidget(sprite_title);

    auto* color_row = new QHBoxLayout();
    color_row->setSpacing(8);
    color_row->addWidget(new QLabel(QStringLiteral("Color"), m_sprite_section));
    m_sprite_color_button = new QPushButton(m_sprite_section);
    m_sprite_color_button->setMinimumWidth(120);
    apply_sprite_color_swatch(m_sprite_color_button, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    color_row->addWidget(m_sprite_color_button, 1);
    sLay->addLayout(color_row);

    layout->addWidget(m_sprite_section);

    QObject::connect(m_sprite_color_button, &QPushButton::clicked, this, [this]() {
        if (!m_editor) {
            return;
        }
        ngin::Entity entity = m_editor->get_selected_entity();
        if (!entity || !entity.has<ngin::SpriteComponent>()) {
            return;
        }

        glm::vec4& col = entity.get<ngin::SpriteComponent>().color;
        const QColor current = vec4_to_qcolor(col);
        const QColor chosen = QColorDialog::getColor(
            current,
            this,
            QStringLiteral("Sprite color"),
            QColorDialog::ShowAlphaChannel
        );
        if (!chosen.isValid()) {
            return;
        }

        col = glm::vec4(chosen.redF(), chosen.greenF(), chosen.blueF(), chosen.alphaF());
        apply_sprite_color_swatch(m_sprite_color_button, col);
        m_editor->notify_scene_contents_changed();
    });

    auto connect_spin = [this](QDoubleSpinBox* s) {
        QObject::connect(s, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
            push_transform_from_spins();
        });
    };
    connect_spin(m_pos_x);
    connect_spin(m_pos_y);
    connect_spin(m_pos_z);
    connect_spin(m_rot_x);
    connect_spin(m_rot_y);
    connect_spin(m_rot_z);
    connect_spin(m_scale_x);
    connect_spin(m_scale_y);
    connect_spin(m_scale_z);

    layout->addStretch(1);

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

void InspectorPanel::push_transform_from_spins() {
    if (!m_editor) {
        return;
    }
    ngin::Entity entity = m_editor->get_selected_entity();
    if (!entity || !entity.has<ngin::TransformComponent>()) {
        return;
    }

    ngin::Transform& tr = entity.get<ngin::TransformComponent>().transform;
    tr.set_position(glm::vec3(
        static_cast<float>(m_pos_x->value()),
        static_cast<float>(m_pos_y->value()),
        static_cast<float>(m_pos_z->value())
    ));
    tr.set_rotation(glm::vec3(
        glm::radians(static_cast<float>(m_rot_x->value())),
        glm::radians(static_cast<float>(m_rot_y->value())),
        glm::radians(static_cast<float>(m_rot_z->value()))
    ));
    tr.set_scale(glm::vec3(
        static_cast<float>(m_scale_x->value()),
        static_cast<float>(m_scale_y->value()),
        static_cast<float>(m_scale_z->value())
    ));

    m_editor->notify_scene_contents_changed();
}

void InspectorPanel::update_component_menu() {
    const ngin::Entity entity = m_editor ? m_editor->get_selected_entity() : ngin::Entity{};
    const bool ok = static_cast<bool>(entity);

    m_add_sprite_action->setEnabled(ok && !entity.has<ngin::SpriteComponent>());
    m_add_transform_action->setEnabled(ok && !entity.has<ngin::TransformComponent>());
    m_remove_sprite_action->setEnabled(ok && entity.has<ngin::SpriteComponent>());
    m_remove_transform_action->setEnabled(ok && entity.has<ngin::TransformComponent>());
}

void InspectorPanel::refresh() {
    const ngin::Entity entity = m_editor ? m_editor->get_selected_entity() : ngin::Entity{};

    if (m_name_edit) {
        const QSignalBlocker blocker(m_name_edit);
        m_name_edit->setEnabled(static_cast<bool>(entity));
        m_name_edit->setText(entity
            ? QString::fromStdString(entity.get<ngin::TagComponent>().tag)
            : QString{});
    }

    if (m_add_component_button) {
        m_add_component_button->setEnabled(static_cast<bool>(entity));
    }

    if (m_id_value) {
        m_id_value->setText(entity
            ? QString::number(entity.get<ngin::IDComponent>().id)
            : QStringLiteral("-"));
    }

    const bool has_transform = entity && entity.has<ngin::TransformComponent>();
    if (m_transform_section) {
        m_transform_section->setVisible(has_transform);
    }

    if (has_transform) {
        const ngin::Transform& tr = entity.get<ngin::TransformComponent>().transform;
        const glm::vec3 p = tr.get_position();
        const glm::vec3 r = tr.get_rotation();
        const glm::vec3 s = tr.get_scale();

        const QSignalBlocker bp_x(m_pos_x);
        const QSignalBlocker bp_y(m_pos_y);
        const QSignalBlocker bp_z(m_pos_z);
        const QSignalBlocker br_x(m_rot_x);
        const QSignalBlocker br_y(m_rot_y);
        const QSignalBlocker br_z(m_rot_z);
        const QSignalBlocker bs_x(m_scale_x);
        const QSignalBlocker bs_y(m_scale_y);
        const QSignalBlocker bs_z(m_scale_z);

        m_pos_x->setValue(static_cast<double>(p.x));
        m_pos_y->setValue(static_cast<double>(p.y));
        m_pos_z->setValue(static_cast<double>(p.z));

        m_rot_x->setValue(static_cast<double>(glm::degrees(r.x)));
        m_rot_y->setValue(static_cast<double>(glm::degrees(r.y)));
        m_rot_z->setValue(static_cast<double>(glm::degrees(r.z)));

        m_scale_x->setValue(static_cast<double>(s.x));
        m_scale_y->setValue(static_cast<double>(s.y));
        m_scale_z->setValue(static_cast<double>(s.z));
    }

    const bool has_sprite = entity && entity.has<ngin::SpriteComponent>();
    if (m_sprite_section) {
        m_sprite_section->setVisible(has_sprite);
    }
    if (has_sprite && m_sprite_color_button) {
        apply_sprite_color_swatch(m_sprite_color_button, entity.get<ngin::SpriteComponent>().color);
    }
}
