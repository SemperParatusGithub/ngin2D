#include "opengl_context.h"

#include "core/log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ngin {

OpenGLContext::OpenGLContext(GLFWwindow* native_window_handle) : m_window_handle(native_window_handle) {}

bool OpenGLContext::init() {
    if (!m_window_handle) {
        NGIN_ERROR("OpenGLContext init failed: native window handle is null.");
        return false;
    }

    glfwMakeContextCurrent(m_window_handle);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        NGIN_ERROR("Failed to initialize GLAD");
        return false;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(m_window_handle, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);
    return true;
}

void OpenGLContext::set_vsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

} // namespace ngin
