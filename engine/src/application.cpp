#include "application.h"

#include "core/log.h"
#include "platform_detection.h"

#include <GLFW/glfw3.h>

namespace ngin {
static Application* s_application_instance = nullptr;

Application::Application() : m_running(false), m_native_window_handle(nullptr) {
    ngin::log::init();
    NGIN_INFO("Starting application on platform: {}", ngin::platform_name());
    s_application_instance = this;
}

Application::~Application() {
    s_application_instance = nullptr;
    ngin::log::shutdown();
}

void Application::run() {
    m_running = true;
    on_create();

    time_stamp previous_time = static_cast<time_stamp>(glfwGetTime());
    while (m_running) {
        const time_stamp current_time = static_cast<time_stamp>(glfwGetTime());
        const time_stamp delta_time = current_time - previous_time;
        previous_time = current_time;

        on_update(delta_time);
    }

    on_destroy();
}

Application* Application::get_instance() {
    return s_application_instance;
}
} // namespace ngin