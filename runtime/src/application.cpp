#include "application.h"

#include <iostream>

Application::Application() = default;
Application::~Application() = default;

void Application::run() {
    std::cout << "Hello from Application!\n";
}
