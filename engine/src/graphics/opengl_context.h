#pragma once

struct GLFWwindow;

namespace ngin {

class OpenGLContext {
public:
    explicit OpenGLContext(GLFWwindow* native_window_handle);

    bool init();
    void set_vsync(bool enabled);

private:
    GLFWwindow* m_window_handle;
};

} // namespace ngin
