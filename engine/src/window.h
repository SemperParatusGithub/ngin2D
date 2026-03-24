#pragma once

#include "core/types.h"

#include <string_view>

struct GLFWwindow;

namespace ngin {

class Window {
public:
    Window(u32 width, u32 height, std::string_view title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void poll_events() const;
    void swap_buffers() const;

    bool should_close() const;
    void set_should_close(bool value) const;

    GLFWwindow* native_handle() const;
    bool valid() const;

private:
    GLFWwindow* m_window_handle;
};

} // namespace ngin
