/*
    SPDX-FileCopyrightText: 2026 Arken contributors
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QString>

class QMainWindow;
class QToolBar;

/** @namespace ArkenShell
 *  @brief Reusable application-shell primitives for the ArkenV identity.
 *
 *  Covers the chrome around the workspace: main window surfaces, toolbars,
 *  menus, status bar and tool tips. Internal editing surfaces (timeline, Bin,
 *  Monitor, Effect Stack, Render) are separate future phases and must not be
 *  targeted here.
 *
 *  The stylesheet uses narrow widget selectors (QMenuBar, QMenu, QToolBar and
 *  toolbar buttons, QStatusBar, QToolTip) so inner panels keep their existing
 *  styling until their own redesign. Every value comes from ArkenTheme tokens;
 *  no hard-coded colors, spacing or radii live in this file's output.
 *
 *  KDDockWidgets chrome (title bars, tab bars, separators) is styled where it
 *  is constructed (kddocksetup.cpp), because those widgets paint themselves
 *  and respond better to palette/font than to style sheets.
 */
namespace ArkenShell {

/** @brief Application-scoped style sheet for shell chrome, generated from Arken tokens. */
QString shellStyleSheet();
/** @brief Apply the Arken window palette and base fonts to @param window (menu/status bar included). */
void styleMainWindow(QMainWindow *window);
/** @brief Apply Arken spacing and font to a toolbar; visuals come from shellStyleSheet(). */
void styleToolBar(QToolBar *bar);

} // namespace ArkenShell
