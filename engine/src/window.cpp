#include "window.h"

#include "core/build_config.h"
#include "core/log.h"

#include <GLFW/glfw3.h>
#include <string>

namespace ngin {
namespace {

KeyModifiers key_modifiers_from_glfw(i32 mods) {
    return KeyModifiers{
        .shift = (mods & GLFW_MOD_SHIFT) != 0,
        .control = (mods & GLFW_MOD_CONTROL) != 0,
        .alt = (mods & GLFW_MOD_ALT) != 0,
        .super = (mods & GLFW_MOD_SUPER) != 0,
        .caps_lock = (mods & GLFW_MOD_CAPS_LOCK) != 0,
        .num_lock = (mods & GLFW_MOD_NUM_LOCK) != 0
    };
}

} // namespace

Window* Window::from_glfw(GLFWwindow* glfw_window) {
    return static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
}

void Window::push_glfw_event(GLFWwindow* glfw_window, Event event) {
    Window* window = from_glfw(glfw_window);
    if (!window) {
        return;
    }
    window->push_event(std::move(event));
}

Window::Window(u32 width, u32 height, std::string_view title) : m_window_handle(nullptr) {
    if (!glfwInit()) {
        NGIN_ERROR("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if NGIN_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    const std::string title_owned{title};
    m_window_handle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title_owned.c_str(), nullptr, nullptr);
    if (!m_window_handle) {
        NGIN_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(m_window_handle, this);
    set_event_callbacks();
}

void Window::set_event_callbacks() {
    if (!m_window_handle) {
        return;
    }

    glfwSetWindowPosCallback(m_window_handle, [](GLFWwindow* window, int x, int y) {
        Window::push_glfw_event(window, Event{
            EventType::window_move,
            WindowMoveEvent{.x = x, .y = y}
        });
    });
    glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* window, int width_value, int height_value) {
        Window::push_glfw_event(window, Event{
            EventType::window_resize,
            WindowResizeEvent{.width = width_value, .height = height_value}
        });
    });
    glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* window) {
        Window::push_glfw_event(window, Event{EventType::window_close, WindowCloseEvent{}});
    });
    glfwSetWindowRefreshCallback(m_window_handle, [](GLFWwindow* window) {
        Window::push_glfw_event(window, Event{EventType::window_refresh, WindowRefreshEvent{}});
    });
    glfwSetWindowFocusCallback(m_window_handle, [](GLFWwindow* window, int focused) {
        Window::push_glfw_event(window, Event{
            EventType::window_focus,
            WindowFocusEvent{.focused = focused == GLFW_TRUE}
        });
    });
    glfwSetWindowIconifyCallback(m_window_handle, [](GLFWwindow* window, int iconified) {
        Window::push_glfw_event(window, Event{
            EventType::window_iconify,
            WindowIconifyEvent{.iconified = iconified == GLFW_TRUE}
        });
    });
    glfwSetWindowMaximizeCallback(m_window_handle, [](GLFWwindow* window, int maximized) {
        Window::push_glfw_event(window, Event{
            EventType::window_maximize,
            WindowMaximizeEvent{.maximized = maximized == GLFW_TRUE}
        });
    });
    glfwSetFramebufferSizeCallback(m_window_handle, [](GLFWwindow* window, int width_value, int height_value) {
        Window::push_glfw_event(window, Event{
            EventType::window_framebuffer_resize,
            WindowFramebufferResizeEvent{.width = width_value, .height = height_value}
        });
    });
    glfwSetWindowContentScaleCallback(m_window_handle, [](GLFWwindow* window, float x_scale, float y_scale) {
        Window::push_glfw_event(window, Event{
            EventType::window_content_scale,
            WindowContentScaleEvent{.x_scale = x_scale, .y_scale = y_scale}
        });
    });
    glfwSetKeyCallback(m_window_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        const KeyCode key_code = static_cast<KeyCode>(key);
        const KeyModifiers modifiers = key_modifiers_from_glfw(mods);
        switch (action) {
            case GLFW_PRESS:
                Window::push_glfw_event(window, Event{
                    EventType::key_pressed,
                    KeyPressed{
                        .key_code = key_code,
                        .scancode = scancode,
                        .modifiers = modifiers
                    }
                });
                break;
            case GLFW_REPEAT:
                Window::push_glfw_event(window, Event{
                    EventType::key_repeated,
                    KeyRepeated{
                        .key_code = key_code,
                        .scancode = scancode,
                        .modifiers = modifiers
                    }
                });
                break;
            case GLFW_RELEASE:
            default:
                Window::push_glfw_event(window, Event{
                    EventType::key_released,
                    KeyReleased{
                        .key_code = key_code,
                        .scancode = scancode,
                        .modifiers = modifiers
                    }
                });
                break;
        }
    });
    glfwSetCharCallback(m_window_handle, [](GLFWwindow* window, unsigned int codepoint) {
        Window::push_glfw_event(window, Event{
            EventType::character,
            CharacterEvent{.codepoint = static_cast<u32>(codepoint)}
        });
    });
    glfwSetCharModsCallback(m_window_handle, [](GLFWwindow* window, unsigned int codepoint, int mods) {
        Window::push_glfw_event(window, Event{
            EventType::character_mods,
            CharacterModsEvent{
                .codepoint = static_cast<u32>(codepoint),
                .modifiers = key_modifiers_from_glfw(mods)
            }
        });
    });
    glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* window, int button, int action, int mods) {
        const KeyModifiers modifiers = key_modifiers_from_glfw(mods);
        if (action == GLFW_PRESS) {
            Window::push_glfw_event(window, Event{
                EventType::mouse_button_pressed,
                MouseButtonPressed{
                    .button = button,
                    .modifiers = modifiers
                }
            });
            return;
        }

        Window::push_glfw_event(window, Event{
            EventType::mouse_button_released,
            MouseButtonReleased{
                .button = button,
                .modifiers = modifiers
            }
        });
    });
    glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* window, double x, double y) {
        Window::push_glfw_event(window, Event{
            EventType::mouse_move,
            MouseMoveEvent{.x = static_cast<f32>(x), .y = static_cast<f32>(y)}
        });
    });
    glfwSetCursorEnterCallback(m_window_handle, [](GLFWwindow* window, int entered) {
        Window::push_glfw_event(window, Event{
            EventType::mouse_enter,
            MouseEnterEvent{.entered = entered == GLFW_TRUE}
        });
    });
    glfwSetScrollCallback(m_window_handle, [](GLFWwindow* window, double x_offset, double y_offset) {
        Window::push_glfw_event(window, Event{
            EventType::mouse_scroll,
            MouseScrollEvent{
                .x_offset = static_cast<f32>(x_offset),
                .y_offset = static_cast<f32>(y_offset)
            }
        });
    });
    glfwSetDropCallback(m_window_handle, [](GLFWwindow* window, int path_count, const char** paths) {
        FileDropEvent drop_event{};
        if (path_count > 0 && paths != nullptr) {
            drop_event.paths.reserve(static_cast<std::size_t>(path_count));
            for (int i = 0; i < path_count; ++i) {
                drop_event.paths.emplace_back(paths[i] != nullptr ? paths[i] : "");
            }
        }

        Window::push_glfw_event(window, Event{EventType::file_drop, std::move(drop_event)});
    });
}

Window::~Window() {
    if (m_window_handle) {
        glfwDestroyWindow(m_window_handle);
        m_window_handle = nullptr;
        glfwTerminate();
    }
}

std::span<const Event> Window::poll_events() {
    m_event_buffer.clear();
    m_next_event_index = 0;
    glfwPollEvents();
    return event_buffer();
}

std::optional<Event> Window::poll_event() {
    if (m_next_event_index >= m_event_buffer.size()) {
        poll_events();
    }

    if (m_next_event_index >= m_event_buffer.size()) {
        return std::nullopt;
    }

    return m_event_buffer[m_next_event_index++];
}

std::span<const Event> Window::event_buffer() const {
    return std::span<const Event>(m_event_buffer.data(), m_event_buffer.size());
}

void Window::clear_event_buffer() {
    m_event_buffer.clear();
    m_next_event_index = 0;
}

void Window::push_event(Event event) {
    m_event_buffer.push_back(std::move(event));
}

void Window::swap_buffers() const {
    if (m_window_handle) {
        glfwSwapBuffers(m_window_handle);
    }
}

bool Window::should_close() const {
    if (m_window_handle) {
        return glfwWindowShouldClose(m_window_handle) == GLFW_TRUE;
    }
    return true;
}

void Window::set_should_close(bool value) const {
    if (m_window_handle) {
        glfwSetWindowShouldClose(m_window_handle, value ? GLFW_TRUE : GLFW_FALSE);
    }
}

GLFWwindow* Window::native_handle() const {
    return m_window_handle;
}

bool Window::valid() const {
    return m_window_handle != nullptr;
}

} // namespace ngin
