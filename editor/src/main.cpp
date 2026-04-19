#include "editor.h"
#include "project_launcher_window.h"

#include "core/log.h"
#include "scene/entity.h"

#include <QApplication>
#include <QDialog>
#include <QMetaType>
#include <QSurfaceFormat>

Q_DECLARE_METATYPE(ngin::Entity)

int main(int argc, char* argv[]) {
	ngin::log::initialize();
	NGIN_INFO("Initializing Log system");

    qRegisterMetaType<ngin::Entity>("ngin::Entity");

    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    ProjectLauncherWindow launcher;
    if (launcher.exec() != QDialog::Accepted) {
        NGIN_INFO("Shutting Log system down");
        ngin::log::release();
        return 0;
    }

    Editor editor;
    editor.run(launcher.selected_project_path());

	NGIN_INFO("Shutting Log system down");
	ngin::log::release();

    return 0;
}
