#include "dashboard_theme.h"

#include <QApplication>
#include <QPalette>
#include <QString>
#include <QStyleFactory>

namespace ngin::editor {

namespace {

constexpr char kBg[] = "#1a1c24";
constexpr char kCard[] = "#252836";
constexpr char kBlue[] = "#5b7fff";
constexpr char kMuted[] = "#8b90a8";
constexpr char kText[] = "#f2f4ff";

} // namespace

QColor dashboardHexColor(const char* hex) {
    return QColor(QString::fromLatin1(hex));
}

void applyDashboardTheme(QApplication* app) {
    QApplication* a = app ? app : qApp;
    if (!a) {
        return;
    }

    a->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette palette;
    palette.setColor(QPalette::Window, dashboardHexColor(kBg));
    palette.setColor(QPalette::WindowText, dashboardHexColor(kText));
    palette.setColor(QPalette::Base, dashboardHexColor(kCard));
    palette.setColor(QPalette::AlternateBase, dashboardHexColor(kCard));
    palette.setColor(QPalette::Text, dashboardHexColor(kText));
    palette.setColor(QPalette::Button, dashboardHexColor(kCard));
    palette.setColor(QPalette::ButtonText, dashboardHexColor(kText));
    palette.setColor(QPalette::Highlight, dashboardHexColor(kBlue));
    palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#ffffff")));
    palette.setColor(QPalette::PlaceholderText, dashboardHexColor(kMuted));
    a->setPalette(palette);

    a->setStyleSheet(QStringLiteral(
        R"(
            QMainWindow, QWidget#centralRoot { background-color: #1a1c24; }
            QWidget#navRail { background-color: #16181f; border: none; }
            QWidget#navPanel { background-color: #1e212b; border: none; }
            QWidget#contentHost { background-color: #1a1c24; border: none; }

            QFrame#dashCard {
                background-color: #252836;
                border: none;
                border-radius: 18px;
            }
            QLabel#cardTitle {
                color: #8b90a8;
                font-size: 12px;
                font-weight: 600;
                letter-spacing: 0.04em;
            }
            QLabel#cardHeadline {
                color: #f2f4ff;
                font-size: 20px;
                font-weight: 700;
            }
            QLabel#cardSub { color: #8b90a8; font-size: 12px; }

            QLineEdit#titleSearch {
                background-color: #252836;
                border: 1px solid #32374a;
                border-radius: 20px;
                padding: 8px 16px 8px 36px;
                color: #f2f4ff;
                font-size: 13px;
                min-height: 20px;
                max-width: 280px;
            }
            QLineEdit#titleSearch:focus { border: 1px solid #5b7fff; }

            QPushButton#navPill {
                background-color: transparent;
                border: none;
                border-radius: 12px;
                color: #c5c9dc;
                font-size: 13px;
                font-weight: 500;
                padding: 10px 14px;
                text-align: left;
            }
            QPushButton#navPill:hover { background-color: #2a2f42; color: #ffffff; }
            QPushButton#navPill:checked {
                background-color: #5b7fff;
                color: #ffffff;
            }

            QPushButton#railBtn {
                background-color: transparent;
                border: none;
                border-radius: 10px;
                color: #8b90a8;
                font-size: 16px;
                min-width: 44px;
                min-height: 44px;
            }
            QPushButton#railBtn:hover { background-color: #252836; color: #f2f4ff; }
            QPushButton#railBtn:checked { background-color: #5b7fff; color: #ffffff; }

            QPushButton#winBtn {
                background-color: transparent;
                border: none;
                color: #c5c9dc;
                font-size: 14px;
                min-width: 44px;
                min-height: 36px;
                border-radius: 6px;
            }
            QPushButton#winBtn:hover { background-color: #32374a; color: #ffffff; }
            QPushButton#winClose:hover { background-color: #ff3d71; color: #ffffff; }

            QPushButton#ctaMagenta {
                background-color: #ff3d71;
                color: #ffffff;
                border: none;
                border-radius: 12px;
                padding: 10px 22px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton#ctaMagenta:hover { background-color: #ff5c8a; }

            QFrame#gradientBanner {
                border: none;
                border-radius: 18px;
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #5b7fff, stop:1 #8a4dff);
            }

            QLabel#titleDashboard {
                color: #f2f4ff;
                font-size: 18px;
                font-weight: 700;
                letter-spacing: -0.3px;
            }

            QLabel#avatarRing {
                background-color: #32374a;
                border: 2px solid #5b7fff;
                border-radius: 22px;
                color: #f2f4ff;
                font-weight: 700;
                font-size: 13px;
            }

            QMessageBox { background-color: #252836; }
            QMessageBox QLabel { color: #f2f4ff; font-size: 13px; min-width: 280px; }
            QMessageBox QPushButton {
                background-color: #5b7fff;
                color: #ffffff;
                border: none;
                border-radius: 8px;
                padding: 6px 18px;
                min-width: 72px;
            }
            QMessageBox QPushButton:hover { background-color: #7392ff; }
        )"));
}

} // namespace ngin::editor
