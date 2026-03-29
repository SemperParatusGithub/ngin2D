#include "editor.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Editor editor;
    editor.run();
    return 0;
}
