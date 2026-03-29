#include "editor.h"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

Editor::Editor() = default;
Editor::~Editor() = default;

void Editor::run() {
    QMainWindow window;
    window.setWindowTitle(QStringLiteral("ngin2D Editor"));
    window.resize(640, 400);

    auto* central = new QWidget;
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(24, 24, 24, 24);

    layout->addStretch(1);
    auto* label = new QLabel(QStringLiteral("ngin2D editor"));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    layout->addStretch(1);
    window.setCentralWidget(central);
    window.show();
    QApplication::exec();
}
