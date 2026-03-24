#include "input.h"

#include "application.h"
#include "core/assert.h"

#include <GLFW/glfw3.h>

namespace ngin {

bool Input::is_key_pressed(KeyCode key) {
    return is_key_pressed(static_cast<i32>(key));
}

bool Input::is_key_pressed(i32 key) {
    Application* app = Application::get_instance();
    NGIN_ASSERT_MSG(app != nullptr, "Input queried without an active Application instance.");
    if (!app) {
        return false;
    }

    GLFWwindow* window = app->get_native_window_handle();
    NGIN_ASSERT_MSG(window != nullptr, "Input queried before native window handle is set (window/context not ready).");
    if (!window) {
        return false;
    }

    const i32 state = glfwGetKey(window, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

std::pair<f32, f32> Input::mouse_position() {
    Application* app = Application::get_instance();
    NGIN_ASSERT_MSG(app != nullptr, "Mouse queried without an active Application instance.");
    if (!app) {
        return {0.0f, 0.0f};
    }

    GLFWwindow* window = app->get_native_window_handle();
    NGIN_ASSERT_MSG(window != nullptr, "Mouse queried before native window handle is set (window/context not ready).");
    if (!window) {
        return {0.0f, 0.0f};
    }

    f64 x = 0.0;
    f64 y = 0.0;
    glfwGetCursorPos(window, &x, &y);
    return {static_cast<f32>(x), static_cast<f32>(y)};
}

f32 Input::mouse_x() {
    return mouse_position().first;
}

f32 Input::mouse_y() {
    return mouse_position().second;
}

} // namespace ngin
