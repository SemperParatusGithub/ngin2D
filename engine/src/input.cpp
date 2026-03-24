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

    GLFWwindow* window = static_cast<GLFWwindow*>(app->get_native_window_handle());
    NGIN_ASSERT_MSG(window != nullptr, "Input queried before native window handle is set (window/context not ready).");
    if (!window) {
        return false;
    }

    const i32 state = glfwGetKey(window, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

} // namespace ngin
