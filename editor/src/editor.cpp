#include "editor.h"

#include "engine_viewport.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

void Editor::run() {
    QMainWindow window;
    window.setWindowTitle(QStringLiteral("ngin2D Editor"));
    window.resize(1280, 720);

    QWidget* central = new QWidget(&window);
    QHBoxLayout* outer = new QHBoxLayout(central);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, central);
    outer->addWidget(splitter, 1);

    auto* side = new QFrame(splitter);
    side->setObjectName(QStringLiteral("editorSidePanel"));
    side->setFrameShape(QFrame::NoFrame);
    side->setMinimumWidth(180);
    side->setMaximumWidth(320);
    side->setStyleSheet(QStringLiteral(
        "QFrame#editorSidePanel { background-color: #2b2d35; border-right: 1px solid #3d4049; }"
        "QLabel#sideTitle { color: #e8e9ef; font-size: 13px; font-weight: 600; }"
        "QListWidget { background: transparent; border: none; color: #c4c6cf; font-size: 13px; outline: 0; }"
        "QListWidget::item { padding: 8px 10px; border-radius: 6px; }"
        "QListWidget::item:hover { background-color: #3a3d47; }"
        "QListWidget::item:selected { background-color: #3d5a99; color: #ffffff; }"));

    QVBoxLayout* sideLay = new QVBoxLayout(side);
    sideLay->setContentsMargins(12, 14, 10, 12);
    sideLay->setSpacing(10);

    QLabel* sideTitle = new QLabel(QStringLiteral("Menu"));
    sideTitle->setObjectName(QStringLiteral("sideTitle"));
    sideLay->addWidget(sideTitle);

    QListWidget* menuList = new QListWidget(side);
    menuList->setFocusPolicy(Qt::NoFocus);
    menuList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    menuList->addItems({
        QStringLiteral("File"),
        QStringLiteral("Edit"),
        QStringLiteral("View"),
        QStringLiteral("Scene"),
        QStringLiteral("Build"),
        QStringLiteral("Help"),
    });
    menuList->setCurrentRow(2);
    sideLay->addWidget(menuList, 1);

    auto* viewport = new EngineViewport(splitter);
    viewport->setFocusPolicy(Qt::StrongFocus);
    viewport->setMinimumSize(480, 360);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitter->addWidget(side);
    splitter->addWidget(viewport);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({220, 1040});

    window.setCentralWidget(central);
    window.show();
    viewport->setFocus(Qt::OtherFocusReason);

    QApplication::exec();
}
