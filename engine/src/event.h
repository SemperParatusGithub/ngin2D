#pragma once

#include "core/types.h"
#include "core/assert.h"
#include "key_codes.h"

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ngin {

enum class EventType {
    none = 0,

    window_close,
    window_resize,
    window_move,
    window_refresh,
    window_focus,
    window_iconify,
    window_maximize,
    window_framebuffer_resize,
    window_content_scale,

    key_pressed,
    key_released,
    key_repeated,
    character,
    character_mods,

    mouse_button_pressed,
    mouse_button_released,
    mouse_move,
    mouse_enter,
    mouse_scroll,

    file_drop
};

struct KeyModifiers {
    bool shift = false;
    bool control = false;
    bool alt = false;
    bool super = false;
    bool caps_lock = false;
    bool num_lock = false;
};

struct WindowCloseEvent {};
struct WindowResizeEvent {
    i32 width = 0;
    i32 height = 0;
};
struct WindowMoveEvent {
    i32 x = 0;
    i32 y = 0;
};
struct WindowRefreshEvent {};
struct WindowFocusEvent {
    bool focused = false;
};
struct WindowIconifyEvent {
    bool iconified = false;
};
struct WindowMaximizeEvent {
    bool maximized = false;
};
struct WindowFramebufferResizeEvent {
    i32 width = 0;
    i32 height = 0;
};
struct WindowContentScaleEvent {
    f32 x_scale = 0.0f;
    f32 y_scale = 0.0f;
};

struct KeyPressedEvent {
    KeyCode key_code = KeyCode::unknown;
    i32 scancode = 0;
    KeyModifiers modifiers = {};
};
struct KeyReleasedEvent {
    KeyCode key_code = KeyCode::unknown;
    i32 scancode = 0;
    KeyModifiers modifiers = {};
};
struct KeyRepeatedEvent {
    KeyCode key_code = KeyCode::unknown;
    i32 scancode = 0;
    KeyModifiers modifiers = {};
};
struct CharacterEvent {
    u32 codepoint = 0;
};
struct CharacterModsEvent {
    u32 codepoint = 0;
    KeyModifiers modifiers = {};
};

struct MouseButtonPressedEvent {
    i32 button = 0;
    KeyModifiers modifiers = {};
};
struct MouseButtonReleasedEvent {
    i32 button = 0;
    KeyModifiers modifiers = {};
};
struct MouseMoveEvent {
    f32 x = 0.0f;
    f32 y = 0.0f;
};
struct MouseEnterEvent {
    bool entered = false;
};
struct MouseScrollEvent {
    f32 x_offset = 0.0f;
    f32 y_offset = 0.0f;
};

struct FileDropEvent {
    std::vector<std::string> paths;
};

using EventData = std::variant<
    std::monostate,
    WindowCloseEvent,
    WindowResizeEvent,
    WindowMoveEvent,
    WindowRefreshEvent,
    WindowFocusEvent,
    WindowIconifyEvent,
    WindowMaximizeEvent,
    WindowFramebufferResizeEvent,
    WindowContentScaleEvent,
    KeyPressedEvent,
    KeyReleasedEvent,
    KeyRepeatedEvent,
    CharacterEvent,
    CharacterModsEvent,
    MouseButtonPressedEvent,
    MouseButtonReleasedEvent,
    MouseMoveEvent,
    MouseEnterEvent,
    MouseScrollEvent,
    FileDropEvent>;

using WindowClose = WindowCloseEvent;
using WindowResize = WindowResizeEvent;
using WindowMove = WindowMoveEvent;
using WindowRefresh = WindowRefreshEvent;
using WindowFocus = WindowFocusEvent;
using WindowIconify = WindowIconifyEvent;
using WindowMaximize = WindowMaximizeEvent;
using WindowFramebufferResize = WindowFramebufferResizeEvent;
using WindowContentScale = WindowContentScaleEvent;
using KeyPressed = KeyPressedEvent;
using KeyReleased = KeyReleasedEvent;
using KeyRepeated = KeyRepeatedEvent;
using CharacterInput = CharacterEvent;
using CharacterWithModifiers = CharacterModsEvent;
using MouseButtonPressed = MouseButtonPressedEvent;
using MouseButtonReleased = MouseButtonReleasedEvent;
using MouseMove = MouseMoveEvent;
using MouseEnter = MouseEnterEvent;
using MouseScroll = MouseScrollEvent;
using FileDrop = FileDropEvent;

template <typename T>
class EventMatch {
public:
    constexpr EventMatch() = default;
    constexpr explicit EventMatch(T* value) : m_value(value) {}

    constexpr explicit operator bool() const {
        return m_value != nullptr;
    }

    constexpr T& operator*() const {
        NGIN_ASSERT_MSG(m_value != nullptr, "Attempted to dereference an empty EventMatch.");
        return *m_value;
    }

    constexpr T* operator->() const {
        NGIN_ASSERT_MSG(m_value != nullptr, "Attempted to access an empty EventMatch.");
        return m_value;
    }

private:
    T* m_value = nullptr;
};

struct Event {
    EventType type = EventType::none;
    EventData data = std::monostate{};

    Event() = default;

    template <typename T>
    Event(EventType type, T&& payload) : type(type), data(std::forward<T>(payload)) {}

    template <typename T>
    bool is_type() const {
        return std::holds_alternative<T>(data);
    }

    template <typename T>
    EventMatch<T> get_if() {
        return EventMatch<T>(std::get_if<T>(&data));
    }

    template <typename T>
    EventMatch<const T> get_if() const {
        return EventMatch<const T>(std::get_if<T>(&data));
    }
};

} // namespace ngin
