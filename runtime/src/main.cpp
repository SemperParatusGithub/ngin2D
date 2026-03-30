#include "runtime_application.h"

int main() {
    ngin::log::initialize();

    RuntimeApplication* app = new RuntimeApplication();
    app->run();
    delete app;

    ngin::log::release();

    return 0;
}
