#include "application.h"

#include "core/log.h"
#include "platform_detection.h"

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

    while (m_running) {
        on_update(0.0f);
    }

    on_destroy();
}

Application* Application::get_instance() {
    return s_application_instance;
}
} // namespace ngin