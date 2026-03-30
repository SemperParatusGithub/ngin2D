#pragma once

#include "event.h"

#include <optional>

class QKeyEvent;
class QWheelEvent;
class QEvent;

namespace ngin::editor {

ngin::KeyModifiers key_modifiers_from_qt(QEvent* event);

/// Maps a Qt key to `KeyCode` (GLFW-compatible values). Returns `unknown` if unmappable.
ngin::KeyCode qt_key_to_ngin(int qt_key);

std::optional<ngin::Event> event_from_qt_key_press(QKeyEvent* event);
std::optional<ngin::Event> event_from_qt_key_release(QKeyEvent* event);
std::optional<ngin::Event> event_from_qt_wheel(QWheelEvent* event);

} // namespace ngin::editor
