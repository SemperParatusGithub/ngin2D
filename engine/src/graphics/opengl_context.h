#pragma once

#include "core/types.h"

struct GLFWwindow;

class QOpenGLWidget;

namespace ngin {

using GladLoadProc = void* (*)(const char* name);

class OpenGLContext {
public:
    OpenGLContext() = default;

    /// GLFW path: makes context current, initializes GLAD + callbacks, sets viewport.
    bool create_from_glfw(GLFWwindow* native_window_handle);

    /// Qt path: `surface` must have/activate a current GL context; initializes GLAD + callbacks.
    bool create_from_qt(QOpenGLWidget* surface);

    void set_vsync(bool enabled);

    bool is_external() const { return m_is_external; }

private:
    bool init_external(GladLoadProc get_proc_address, bool set_viewport_from_context = true);

    GLFWwindow* m_window_handle = nullptr;
    bool m_is_external = false;
};

} // namespace ngin
