#pragma once

#include <QColor>

class QApplication;

namespace ngin::editor {

/// Fusion + dark palette + QSS for the neo-dashboard look (nav rail, cards, frameless chrome, etc.).
void applyDashboardTheme(QApplication* app = nullptr);

/// Parse #RRGGBB strings for custom paint code (charts, etc.).
QColor dashboardHexColor(const char* hex);

} // namespace ngin::editor
