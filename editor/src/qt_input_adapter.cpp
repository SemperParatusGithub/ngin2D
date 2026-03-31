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
    if (qt_key >= 32 && qt_key <= 126) {
        return static_cast<KeyCode>(qt_key);
    }

    switch (qt_key) {
    case Qt::Key_Escape:
        return KeyCode::escape;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return KeyCode::tab;
    case Qt::Key_Backspace:
        return KeyCode::backspace;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return KeyCode::enter;
    case Qt::Key_Insert:
        return KeyCode::insert;
    case Qt::Key_Delete:
        return KeyCode::del;
    case Qt::Key_Pause:
        return KeyCode::pause;
    case Qt::Key_Print:
        return KeyCode::print_screen;
    case Qt::Key_Home:
        return KeyCode::home;
    case Qt::Key_End:
        return KeyCode::end;
    case Qt::Key_PageUp:
        return KeyCode::page_up;
    case Qt::Key_PageDown:
        return KeyCode::page_down;
    case Qt::Key_Left:
        return KeyCode::left;
    case Qt::Key_Right:
        return KeyCode::right;
    case Qt::Key_Up:
        return KeyCode::up;
    case Qt::Key_Down:
        return KeyCode::down;
    case Qt::Key_Shift:
        return KeyCode::left_shift;
    case Qt::Key_Control:
        return KeyCode::left_control;
    case Qt::Key_Alt:
        return KeyCode::left_alt;
    case Qt::Key_Meta:
        return KeyCode::left_super;
    case Qt::Key_CapsLock:
        return KeyCode::caps_lock;
    case Qt::Key_NumLock:
        return KeyCode::num_lock;
    case Qt::Key_ScrollLock:
        return KeyCode::scroll_lock;
    case Qt::Key_F1:
        return KeyCode::f1;
    case Qt::Key_F2:
        return KeyCode::f2;
    case Qt::Key_F3:
        return KeyCode::f3;
    case Qt::Key_F4:
        return KeyCode::f4;
    case Qt::Key_F5:
        return KeyCode::f5;
    case Qt::Key_F6:
        return KeyCode::f6;
    case Qt::Key_F7:
        return KeyCode::f7;
    case Qt::Key_F8:
        return KeyCode::f8;
    case Qt::Key_F9:
        return KeyCode::f9;
    case Qt::Key_F10:
        return KeyCode::f10;
    case Qt::Key_F11:
        return KeyCode::f11;
    case Qt::Key_F12:
        return KeyCode::f12;
    case Qt::Key_Space:
        return KeyCode::space;
    default:
        return KeyCode::unknown;
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
