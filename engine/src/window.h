#pragma once

#include "core/types.h"
#include "event.h"
#include "event_queue.h"

#include <optional>
#include <span>
#include <string_view>

struct GLFWwindow;

namespace ngin {

class Window {
public:
    /// `event_queue` must outlive the window; GLFW callbacks push into it.
    Window(u32 width, u32 height, std::string_view title, EventQueue& event_queue);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    std::span<const Event> poll_events();
    std::optional<Event> poll_event();
    std::span<const Event> event_buffer() const;
    void clear_event_buffer();
    void swap_buffers() const;

    bool should_close() const;
    void set_should_close(bool value) const;

    GLFWwindow* native_handle() const;
    bool valid() const;

private:
    void set_event_callbacks();
    static Window* from_glfw(GLFWwindow* glfw_window);
    static void push_glfw_event(GLFWwindow* glfw_window, Event event);
    void push_event(Event event);

    GLFWwindow* m_window_handle = nullptr;
    EventQueue* m_event_queue = nullptr;
};

} // namespace ngin
