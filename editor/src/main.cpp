#include "editor.h"

#include "core/log.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[]) {
	ngin::log::init();
	NGIN_INFO("Initializing Log system");

    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    Editor editor;
    editor.run();

	NGIN_INFO("Shutting Log system down");
	ngin::log::shutdown();

    return 0;
}
