#include "application.h"

#include <iostream>

namespace ngin {
static Application* s_application_instance = nullptr;

Application::Application() : m_running(false), m_window(nullptr) {
    std::cout << "Application::Application()" << std::endl;
    s_application_instance = this;
}

Application::~Application() {
    std::cout << "Application::~Application()" << std::endl;
    s_application_instance = nullptr;
}

void Application::run() {
    std::cout << "Application::run()" << std::endl;

    m_running = true;
    on_create();

    while (m_running) {
        Event e {};
        on_event(e);
        on_update(0.0f);
    }

    on_destroy();
}

Application* Application::get_instance() {
    return s_application_instance;
}
} // namespace ngin