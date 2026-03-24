#include "window.h"

#include "core/log.h"

#include <GLFW/glfw3.h>
#include <string>

namespace ngin {

Window::Window(u32 width, u32 height, std::string_view title) : m_window_handle(nullptr) {
    if (!glfwInit()) {
        NGIN_ERROR("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
}

Window::~Window() {
    if (m_window_handle) {
        glfwDestroyWindow(m_window_handle);
        m_window_handle = nullptr;
        glfwTerminate();
    }
}

void Window::poll_events() const {
    glfwPollEvents();
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
