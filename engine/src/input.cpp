#include "input.h"

#include "core/assert.h"

#include <GLFW/glfw3.h>

namespace ngin {

namespace {
    static GLFWwindow* s_current_window_handle = nullptr;
}

void Input::initialize(GLFWwindow* window_handle) {
    s_current_window_handle = window_handle;
}
void Input::release() {
    s_current_window_handle = nullptr;
}

bool Input::is_key_pressed(KeyCode key) {
    return is_key_pressed(static_cast<i32>(key));
}

bool Input::is_key_pressed(i32 key) {
    NGIN_ASSERT_MSG(s_current_window_handle != nullptr, "Input queried before native window handle is set (window/context not ready).");
    if (!s_current_window_handle) {
        return false;
    }

    const i32 state = glfwGetKey(s_current_window_handle, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

std::pair<f32, f32> Input::mouse_position() {
    NGIN_ASSERT_MSG(s_current_window_handle != nullptr, "Mouse queried before native window handle is set (window/context not ready).");
    if (!s_current_window_handle) {
        return {0.0f, 0.0f};
    }

    f64 x = 0.0;
    f64 y = 0.0;
    glfwGetCursorPos(s_current_window_handle, &x, &y);
    return {static_cast<f32>(x), static_cast<f32>(y)};
}

f32 Input::mouse_x() {
    return mouse_position().first;
}

f32 Input::mouse_y() {
    return mouse_position().second;
}

} // namespace ngin
