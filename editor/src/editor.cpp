#include "editor.h"

#include "engine_viewport.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

void Editor::run() {
    QMainWindow window;
    window.setWindowTitle(QStringLiteral("ngin2D Editor"));
    window.resize(1280, 720);

    QWidget* central = new QWidget(&window);
    central->setStyleSheet(QStringLiteral(
        "QWidget#editorRoot { background-color: #1a1c24; }"
        "QFrame#editorSidePanel { background-color: #1e212b; border-right: 1px solid #32374a; }"
        "QLabel#sideTitle { color: #e8e9ef; font-size: 13px; font-weight: 600; }"
        "QListWidget { background: transparent; border: none; color: #c4c6cf; font-size: 13px; outline: 0; }"
        "QListWidget::item { padding: 8px 10px; border-radius: 6px; }"
        "QListWidget::item:hover { background-color: #2a2f42; }"
        "QListWidget::item:selected { background-color: #5b7fff; color: #ffffff; }"
        "QFrame#viewportCard { background-color: #252836; border: none; border-radius: 18px; }"));
    central->setObjectName(QStringLiteral("editorRoot"));

    QHBoxLayout* outer = new QHBoxLayout(central);
    outer->setContentsMargins(2, 2, 2, 2);
    outer->setSpacing(0);

    auto* side = new QFrame(central);
    side->setObjectName(QStringLiteral("editorSidePanel"));
    side->setFrameShape(QFrame::NoFrame);
    side->setFixedWidth(230);

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

    auto* workspace = new QWidget(central);
    QVBoxLayout* workspaceLay = new QVBoxLayout(workspace);
    workspaceLay->setContentsMargins(10, 10, 10, 10);
    workspaceLay->setSpacing(0);

    auto* viewportCard = new QFrame(workspace);
    viewportCard->setObjectName(QStringLiteral("viewportCard"));
    viewportCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* viewportCardLay = new QVBoxLayout(viewportCard);
    viewportCardLay->setContentsMargins(2, 2, 2, 2);
    viewportCardLay->setSpacing(0);

    auto* viewport = new EngineViewport(viewportCard);
    viewport->setFocusPolicy(Qt::StrongFocus);
    viewport->setMinimumSize(480, 360);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewport->setStyleSheet(QStringLiteral("background-color: #0f172a; border-radius: 16px;"));
    viewportCardLay->addWidget(viewport);

    workspaceLay->addWidget(viewportCard, 1);
    outer->addWidget(side);
    outer->addWidget(workspace, 1);

    window.setCentralWidget(central);
    window.show();
    viewport->setFocus(Qt::OtherFocusReason);

    QApplication::exec();
}
