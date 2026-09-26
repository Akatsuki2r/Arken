/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "arkenshell.h"
#include "arkenstyle.h"
#include "arkentheme.h"

#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

namespace ArkenShell {

namespace {
// Shorthand: hex form of a token color for style-sheet generation.
QString hex(const QColor &color)
{
    return color.name();
}
} // namespace

QString shellStyleSheet()
{
    const ArkenTheme *t = ArkenTheme::instance();
    const QString appBg = hex(t->appBackground());
    const QString surface = hex(t->surface());
    const QString elevated = hex(t->surfaceElevated());
    const QString border = hex(t->border());
    const QString divider = hex(t->divider());
    const QString primary = hex(t->textPrimary());
    const QString secondary = hex(t->textSecondary());
    const QString muted = hex(t->textMuted());
    const QString selection = hex(t->selection());
    const QString accent = hex(t->accent());
    const QString pressedElevated = hex(t->pressedColor(t->surfaceElevated()));
    const QString hoverSelection = hex(t->hoverColor(t->selection()));
    const int xs = t->spaceXs();
    const int sm = t->spaceSm();
    const int md = t->spaceMd();
    const int radius = t->radiusSm();

    QString sheet;
    sheet += QStringLiteral("QMenuBar { background: %1; spacing: %2px; padding: 2px %3px; border: none; border-bottom: 1px solid %4; }\n")
                 .arg(appBg)
                 .arg(xs)
                 .arg(sm)
                 .arg(divider);
    sheet += QStringLiteral("QMenuBar::item { background: transparent; color: %1; padding: %2px %3px; margin: 1px 2px; border-radius: %4px; }\n")
                 .arg(secondary)
                 .arg(xs)
                 .arg(md)
                 .arg(radius);
    sheet += QStringLiteral("QMenuBar::item:selected { background: %1; color: %2; }\n").arg(elevated, primary);
    sheet += QStringLiteral("QMenuBar::item:pressed { background: %1; color: %2; }\n").arg(surface, primary);
    sheet += QStringLiteral("QMenu { background: %1; border: 1px solid %2; padding: %3px; }\n").arg(elevated, border).arg(xs);
    sheet += QStringLiteral("QMenu::item { color: %1; background: transparent; padding: %2px %3px; border-radius: %4px; }\n")
                 .arg(primary)
                 .arg(xs)
                 .arg(md)
                 .arg(radius);
    sheet += QStringLiteral("QMenu::item:disabled { color: %1; }\n").arg(muted);
    sheet += QStringLiteral("QMenu::separator { height: 1px; background: %1; margin: %2px %3px; }\n").arg(divider).arg(xs).arg(sm);
    sheet += QStringLiteral("QToolBar { background: %1; border: none; border-bottom: 1px solid %2; spacing: %3px; padding: %4px %3px; }\n")
                 .arg(appBg)
                 .arg(divider)
                 .arg(xs)
                 .arg(xs);
    sheet += QStringLiteral("QToolBar::separator { background: %1; width: 1px; margin: %2px %3px; }\n").arg(divider).arg(sm).arg(xs);
    sheet += QStringLiteral("QToolBar QToolButton { background: transparent; border: none; border-radius: %1px; padding: %2px; color: %3; }\n")
                 .arg(radius)
                 .arg(xs)
                 .arg(primary);
    sheet += QStringLiteral("QToolBar QToolButton:hover { background: %1; }\n").arg(elevated);
    sheet += QStringLiteral("QToolBar QToolButton:pressed { background: %1; }\n").arg(pressedElevated);
    sheet += QStringLiteral("QToolBar QToolButton:checked { background: %1; color: %2; }\n").arg(selection, primary);
    sheet += QStringLiteral("QToolBar QToolButton:checked:hover { background: %1; }\n").arg(hoverSelection);
    sheet += QStringLiteral("QMenu::item:selected { background: %1; color: %2; }\n").arg(accent, primary);
    sheet += QStringLiteral("QToolBar QToolButton:disabled { color: %1; }\n").arg(muted);
    sheet += QStringLiteral("QStatusBar { background: %1; border-top: 1px solid %2; color: %3; }\n").arg(appBg, divider, secondary);
    sheet += QStringLiteral("QStatusBar::item { border: none; }\n");
    sheet += QStringLiteral("QToolTip { background: %1; color: %2; border: 1px solid %3; padding: %4px; }\n")
                 .arg(elevated, primary, border)
                 .arg(xs);
    return sheet;
}

void styleMainWindow(QMainWindow *window)
{
    if (window == nullptr) {
        return;
    }
    ArkenStyle::polishPanel(window);
    const ArkenTheme *t = ArkenTheme::instance();
    if (window->menuBar() != nullptr) {
        window->menuBar()->setFont(t->bodyFont());
    }
    if (window->statusBar() != nullptr) {
        window->statusBar()->setFont(t->captionFont());
    }
}

void styleToolBar(QToolBar *bar)
{
    if (bar == nullptr) {
        return;
    }
    bar->setFont(ArkenTheme::instance()->bodyFont());
    if (bar->layout() != nullptr) {
        bar->layout()->setSpacing(ArkenTheme::instance()->spaceXs());
    }
}

} // namespace ArkenShell
