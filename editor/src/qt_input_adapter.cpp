#include "qt_input_adapter.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>

namespace ngin::editor {

ngin::KeyModifiers key_modifiers_from_qt(QEvent* event) {
    auto* ke = dynamic_cast<QKeyEvent*>(event);
    const Qt::KeyboardModifiers m = ke ? ke->modifiers() : Qt::NoModifier;
    return ngin::KeyModifiers{
        .shift = (m & Qt::ShiftModifier) != Qt::NoModifier,
        .control = (m & Qt::ControlModifier) != Qt::NoModifier,
        .alt = (m & Qt::AltModifier) != Qt::NoModifier,
        .super = (m & Qt::MetaModifier) != Qt::NoModifier,
        .caps_lock = false,
        .num_lock = false,
    };
}

ngin::KeyCode qt_key_to_ngin(int qt_key) {
    using KC = ngin::KeyCode;

    if (qt_key >= 32 && qt_key <= 126) {
        return static_cast<KC>(qt_key);
    }

    switch (qt_key) {
    case Qt::Key_Escape:
        return KC::escape;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return KC::tab;
    case Qt::Key_Backspace:
        return KC::backspace;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return KC::enter;
    case Qt::Key_Insert:
        return KC::insert;
    case Qt::Key_Delete:
        return KC::del;
    case Qt::Key_Pause:
        return KC::pause;
    case Qt::Key_Print:
        return KC::print_screen;
    case Qt::Key_Home:
        return KC::home;
    case Qt::Key_End:
        return KC::end;
    case Qt::Key_PageUp:
        return KC::page_up;
    case Qt::Key_PageDown:
        return KC::page_down;
    case Qt::Key_Left:
        return KC::left;
    case Qt::Key_Right:
        return KC::right;
    case Qt::Key_Up:
        return KC::up;
    case Qt::Key_Down:
        return KC::down;
    case Qt::Key_Shift:
        return KC::left_shift;
    case Qt::Key_Control:
        return KC::left_control;
    case Qt::Key_Alt:
        return KC::left_alt;
    case Qt::Key_Meta:
        return KC::left_super;
    case Qt::Key_CapsLock:
        return KC::caps_lock;
    case Qt::Key_NumLock:
        return KC::num_lock;
    case Qt::Key_ScrollLock:
        return KC::scroll_lock;
    case Qt::Key_F1:
        return KC::f1;
    case Qt::Key_F2:
        return KC::f2;
    case Qt::Key_F3:
        return KC::f3;
    case Qt::Key_F4:
        return KC::f4;
    case Qt::Key_F5:
        return KC::f5;
    case Qt::Key_F6:
        return KC::f6;
    case Qt::Key_F7:
        return KC::f7;
    case Qt::Key_F8:
        return KC::f8;
    case Qt::Key_F9:
        return KC::f9;
    case Qt::Key_F10:
        return KC::f10;
    case Qt::Key_F11:
        return KC::f11;
    case Qt::Key_F12:
        return KC::f12;
    case Qt::Key_Space:
        return KC::space;
    default:
        return KC::unknown;
    }
}

std::optional<ngin::Event> event_from_qt_key_press(QKeyEvent* event) {
    if (!event) {
        return std::nullopt;
    }
    const ngin::KeyCode key = qt_key_to_ngin(event->key());
    if (key == ngin::KeyCode::unknown) {
        return std::nullopt;
    }
    const ngin::KeyModifiers mods = key_modifiers_from_qt(event);
    const int scancode = static_cast<int>(event->nativeScanCode());

    if (event->isAutoRepeat()) {
        return ngin::Event{
            ngin::EventType::key_repeated,
            ngin::KeyRepeated{
                .key_code = key,
                .scancode = scancode,
                .modifiers = mods,
            }
        };
    }
    return ngin::Event{
        ngin::EventType::key_pressed,
        ngin::KeyPressed{
            .key_code = key,
            .scancode = scancode,
            .modifiers = mods,
        }
    };
}

std::optional<ngin::Event> event_from_qt_key_release(QKeyEvent* event) {
    if (!event || event->isAutoRepeat()) {
        return std::nullopt;
    }
    const ngin::KeyCode key = qt_key_to_ngin(event->key());
    if (key == ngin::KeyCode::unknown) {
        return std::nullopt;
    }
    return ngin::Event{
        ngin::EventType::key_released,
        ngin::KeyReleased{
            .key_code = key,
            .scancode = static_cast<int>(event->nativeScanCode()),
            .modifiers = key_modifiers_from_qt(event),
        }
    };
}

std::optional<ngin::Event> event_from_qt_wheel(QWheelEvent* event) {
    if (!event) {
        return std::nullopt;
    }
    const QPoint ad = event->angleDelta();
    return ngin::Event{
        ngin::EventType::mouse_scroll,
        ngin::MouseScroll{
            .x_offset = static_cast<ngin::f32>(ad.x()) / 120.0f,
            .y_offset = static_cast<ngin::f32>(ad.y()) / 120.0f,
        }
    };
}

} // namespace ngin::editor
